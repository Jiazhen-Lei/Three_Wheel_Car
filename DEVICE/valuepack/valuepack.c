#include "valuepack.h"

#include <stdio.h>

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
    USART2->DR = (u8) ch;      
	return ch;
}



// 发送数据包的字节长度
const unsigned short  TXPACK_BYTE_SIZE = ((TX_BOOL_NUM+7)>>3)+TX_BYTE_NUM+(TX_SHORT_NUM<<1)+(TX_INT_NUM<<2)+(TX_FLOAT_NUM<<2);

// 接收数据包的字节长度
const unsigned short  RXPACK_BYTE_SIZE = ((RX_BOOL_NUM+7)>>3)+RX_BYTE_NUM+(RX_SHORT_NUM<<1)+(RX_INT_NUM<<2)+(RX_FLOAT_NUM<<2);

// 接收数据包的原数据加上包头、校验和包尾 之后的字节长度
unsigned short rx_pack_length = RXPACK_BYTE_SIZE+3;

// 接收计数-记录当前的数据接收进度
// 接收计数每次随串口的接收中断后 +1
long rxIndex=0;

// 读取计数-记录当前的数据包读取进度，读取计数会一直落后于接收计数，当读取计数与接收计数之间距离超过一个接收数据包的长度时，会启动一次数据包的读取。
// 读取计数每次在读取数据包后增加 +(数据包长度)
long rdIndex=0;

// 用于环形缓冲区的数组，环形缓冲区的大小可以在.h文件中定义VALUEPACK_BUFFER_SIZE
unsigned char vp_rxbuff[VALUEPACK_BUFFER_SIZE];

// 用于暂存发送数据的数组
unsigned char vp_txbuff[TXPACK_BYTE_SIZE+3];

// 全局的发送数据结构体，只需要一个
TxPack tx_pack_ptr;

// 全局的接收数据结构体，只需要一个
RxPack rx_pack_ptr;

//全局的遥控数据结构体，只需要一个
RC_DATA RC_Data;

// 初始化数据包  使用的是USART2 波特率可配置
void initValuePack(int baudrate)
{

	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// 时钟初始化

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	// 引脚初始化

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* PA3 USART2_Rx  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 串口初始化

  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART2, ENABLE);
}


// 数据包环形缓冲区计数
unsigned int vp_circle_rx_index;

// 串口接收中断 服务函数， 每次接收到数据后将字节存入环形缓冲区中，从头存到尾。所谓的环形缓冲区就是当接收环形缓冲区计数大于等于缓冲区的大小时（即数据到达缓冲区的尾部时）
// 数据会在缓冲区的头部继续存储，覆盖掉最早的数据。
void USART2_IRQHandler(void)
{
	// 判断是否是USART2接收了数据
	if(USART_GetITStatus(USART2, USART_IT_RXNE))
	{
	        // 读取数据到缓冲区中
		vp_rxbuff[vp_circle_rx_index] = USART2->DR;

		// 将环形缓冲接收计数加一
		vp_circle_rx_index++;
	        // 数据到达缓冲区尾部后，转移到头部
		if(vp_circle_rx_index>=VALUEPACK_BUFFER_SIZE)
			vp_circle_rx_index=0;

		// 将全局接收计数加一
		rxIndex++;
	}
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
}


// 数据读取涉及到的变量
unsigned short rdi,rdii,idl,idi,bool_index,bool_bit;
// 变量地址
uint32_t  idc;
// 记录读取的错误字节的次数
unsigned int err=0;
// 用于和校验
unsigned char sum=0;

// 存放数据包读取的结果
unsigned char isok;

unsigned short fail_num=0;

//------------------------------------------------------------------------------------------------------------------------
// unsigned char readValuePack(RxPack *rx_pack_ptr)
// 尝试从缓冲区中读取数据包
// 参数   - 传入接收数据结构体的指针，从环形缓冲区中读取出数据包，并将各类数据存储到rx_pack_ptr中
// 返回值 - 如果成功读取到数据包，则返回1，否则返回0
//

unsigned char readValuePack(void)
{

	isok = 0;

	// 确保读取计数和接收计数之间的距离小于2个数据包的长度
	while(rdIndex<(rxIndex-((rx_pack_length)*2)))
          rdIndex+=rx_pack_length;

	// 如果读取计数落后于接收计数超过 1个 数据包的长度，则尝试读取
	while(rdIndex<=(rxIndex-rx_pack_length))
	{
		rdi = rdIndex % VALUEPACK_BUFFER_SIZE;
		rdii=rdi+1;
		if( vp_rxbuff[rdi]==PACK_HEAD) // 比较包头
		{
			if(vp_rxbuff[(rdi+RXPACK_BYTE_SIZE+2)%VALUEPACK_BUFFER_SIZE]==PACK_TAIL) // 比较包尾 确定包尾后，再计算校验和
			{
				//  计算校验和
				sum=0;
			  for(short s=0;s<RXPACK_BYTE_SIZE;s++)
				{
					rdi++;
					if(rdi>=VALUEPACK_BUFFER_SIZE)
					  rdi -= VALUEPACK_BUFFER_SIZE;
					sum += vp_rxbuff[rdi];
				}
						rdi++;
					if(rdi>=VALUEPACK_BUFFER_SIZE)
					  rdi -= VALUEPACK_BUFFER_SIZE;

        if(sum==vp_rxbuff[rdi]) // 校验和正确，则开始将缓冲区中的数据读取出来
				{
					//  提取数据包数据 一共有五步， bool byte short int float
					// 1. bool
					#if  RX_BOOL_NUM>0

					  idc = (uint32_t)rx_pack_ptr.bools;
					  idl = (RX_BOOL_NUM+7)>>3;

					bool_bit = 0;
					for(bool_index=0;bool_index<RX_BOOL_NUM;bool_index++)
					{
						*((unsigned char *)(idc+bool_index)) = (vp_rxbuff[rdii]&(0x01<<bool_bit))?1:0;
						bool_bit++;
						if(bool_bit>=8)
						{
						  bool_bit = 0;
							rdii ++;
						}
					}
					if(bool_bit)
						rdii ++;

				        #endif
					// 2.byte
					#if RX_BYTE_NUM>0
						idc = (uint32_t)(rx_pack_ptr.bytes);
					  idl = RX_BYTE_NUM;
					  for(idi=0;idi<idl;idi++)
					  {
					    if(rdii>=VALUEPACK_BUFFER_SIZE)
					      rdii -= VALUEPACK_BUFFER_SIZE;
					    (*((unsigned char *)idc))= vp_rxbuff[rdii];
							rdii++;
							idc++;
					  }
				        #endif
					// 3.short
					#if RX_SHORT_NUM>0
						idc = (uint32_t)(rx_pack_ptr.shorts);
					  idl = RX_SHORT_NUM<<1;
					  for(idi=0;idi<idl;idi++)
					  {
					    if(rdii>=VALUEPACK_BUFFER_SIZE)
					      rdii -= VALUEPACK_BUFFER_SIZE;
					    (*((unsigned char *)idc))= vp_rxbuff[rdii];
							rdii++;
							idc++;
					  }
				        #endif
					// 4.int
					#if RX_INT_NUM>0
						idc = (uint32_t)(&(rx_pack_ptr.integers[0]));
					  idl = RX_INT_NUM<<2;
					  for(idi=0;idi<idl;idi++)
					  {
					    if(rdii>=VALUEPACK_BUFFER_SIZE)
					      rdii -= VALUEPACK_BUFFER_SIZE;
					    (*((unsigned char *)idc))= vp_rxbuff[rdii];
							rdii++;
							idc++;
					  }
				        #endif
					// 5.float
					#if RX_FLOAT_NUM>0
						idc = (uint32_t)(&(rx_pack_ptr.floats[0]));
					  idl = RX_FLOAT_NUM<<2;
					  for(idi=0;idi<idl;idi++)
					  {
					    if(rdii>=VALUEPACK_BUFFER_SIZE)
					      rdii -= VALUEPACK_BUFFER_SIZE;
					    (*((unsigned char *)idc))= vp_rxbuff[rdii];
							rdii++;
							idc++;
					  }
				  #endif

				        // 更新读取计数
					rdIndex+=rx_pack_length;
					isok = 1;
				}
				else
				{
				// 校验值错误 则 err+1 且 更新读取计数
				  rdIndex++;
			    err++;
				}
			}
			else
			{
				// 包尾错误 则 err+1 且 更新读取计数
				rdIndex++;
				err++;
			}
		}
		else
		{
			// 包头错误 则 err+1 且 更新读取计数
			rdIndex++;
			err++;
		}
	}
	return isok;
}

//-------------------------------------------------------------------------------------------------------------------------
// void sendBuffer(unsigned char *p,unsigned short length)
// 发送数据包
// 传入指针 和 字节长度

void sendBuffer(unsigned char *p,unsigned short length)
{
	  for(int i=0;i<length;i++)
   {
      USART_SendData(USART2, *p++);
      while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
      {}
    }
}

// 数据包发送涉及的变量
unsigned short loop;
unsigned char valuepack_tx_bit_index;
unsigned char valuepack_tx_index;

//---------------------------------------------------------------------------------------------------------
//  void sendValuePack(void)
//  将发送数据结构体中的变量打包，并发送出去
//  传入参数- 无 只有一个tx_pack_ptr
//
//  先将待发送数据包结构体的变量转移到“发送数据缓冲区”中，然后将发送数据缓冲区中的数据发送
//

void sendValuePack(void)
{
  int i;
  vp_txbuff[0]=0xa5;
  sum=0;
  //  由于结构体中不同类型的变量在内存空间的排布不是严格对齐的，中间嵌有无效字节，因此需要特殊处理
  valuepack_tx_bit_index = 0;
  valuepack_tx_index = 1;

	#if TX_BOOL_NUM>0
	  for(loop=0;loop<TX_BOOL_NUM;loop++)
	  {
		  if(tx_pack_ptr.bools[loop])
			vp_txbuff[valuepack_tx_index] |= 0x01<<valuepack_tx_bit_index;
		  else
			vp_txbuff[valuepack_tx_index] &= ~(0x01<<valuepack_tx_bit_index);
		  valuepack_tx_bit_index++;

		  if(valuepack_tx_bit_index>=8)
		  {
			  valuepack_tx_bit_index = 0;
			  valuepack_tx_index++;
		  }
	  }
	  if(valuepack_tx_bit_index!=0)
		  valuepack_tx_index++;
	#endif
	#if TX_BYTE_NUM>0

	  for(loop=0;loop<TX_BYTE_NUM;loop++)
	  {
		  vp_txbuff[valuepack_tx_index] = tx_pack_ptr.bytes[loop];
		  valuepack_tx_index++;
	  }
	#endif

	#if TX_SHORT_NUM>0
	  for(loop=0;loop<TX_SHORT_NUM;loop++)
	  {
		  vp_txbuff[valuepack_tx_index] = tx_pack_ptr.shorts[loop]&0xff;
		  vp_txbuff[valuepack_tx_index+1] = tx_pack_ptr.shorts[loop]>>8;
		  valuepack_tx_index+=2;
	  }
	#endif

	#if TX_INT_NUM>0
	  for(loop=0;loop<TX_INT_NUM;loop++)
	  {
		  i = tx_pack_ptr.integers[loop];
		  vp_txbuff[valuepack_tx_index] = i&0xff;
		  vp_txbuff[valuepack_tx_index+1] = (i>>8)&0xff;
		  vp_txbuff[valuepack_tx_index+2] =(i>>16)&0xff;
		  vp_txbuff[valuepack_tx_index+3] = (i>>24)&0xff;
		  valuepack_tx_index+=4;
		}
	#endif

	#if TX_FLOAT_NUM>0
	  for(loop=0;loop<TX_FLOAT_NUM;loop++)
	  {
		  i = *(int *)(&(tx_pack_ptr.floats[loop]));
		  vp_txbuff[valuepack_tx_index] = i&0xff;
		  vp_txbuff[valuepack_tx_index+1] = (i>>8)&0xff;
		  vp_txbuff[valuepack_tx_index+2] =(i>>16)&0xff;
		  vp_txbuff[valuepack_tx_index+3] = (i>>24)&0xff;
		  valuepack_tx_index+=4;
	  }
	#endif

	for(unsigned short d=1;d<=TXPACK_BYTE_SIZE;d++)
		sum+=vp_txbuff[d];

	vp_txbuff[TXPACK_BYTE_SIZE+1] = sum;
	vp_txbuff[TXPACK_BYTE_SIZE+2] = 0x5a;
	sendBuffer(vp_txbuff,TXPACK_BYTE_SIZE+3);
}

//---------------------------------------------------------------------------------------------------------
//  void fillSendPack(void)
//  将数据结构体中的数据填入到发送结构体中的变量，准备发送出去
//  传入参数- 无 只有一个数据结构体

//  先将待发送数据包结构体的变量转移到“发送数据缓冲区”中，然后将发送数据缓冲区中的数据发送

extern float pitch,roll,yaw;
extern u16 Laser_distance[5];

void fillSendPack(void)
{
	//将陀螺仪数据填入
	tx_pack_ptr.integers[0]=(int)yaw;
	tx_pack_ptr.integers[1]=(int)Laser_distance[1];
}

//---------------------------------------------------------------------------------------------------------
//  void ReadPack(void)
//  将接收结构体中的数据填入到遥控数据结构体中的变量，给下一步使用
//  传入参数- 无 只有一个数据结构体
//
//  先将待发送数据包结构体的变量转移到“发送数据缓冲区”中，然后将发送数据缓冲区中的数据发送
//

void ReadPack(void)
{
	RC_Data.mod = rx_pack_ptr.bytes[0];
	RC_Data.MPU = rx_pack_ptr.bytes[1];
	RC_Data.laser = rx_pack_ptr.bytes[2];
	RC_Data.pak = rx_pack_ptr.bytes[3];
	RC_Data.speed_x = rx_pack_ptr.floats[0];
	RC_Data.speed_y = rx_pack_ptr.floats[1];
	RC_Data.speed_r = rx_pack_ptr.floats[2];
}
