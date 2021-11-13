#include "paj7620u2.h"
#include "paj7620u2_cfg.h"
#include "paj7620u2_iic.h"
#include "move.h"

u8 index_mode = 0;

//选择PAJ7620U2 BANK区域
void paj7620u2_selectBank(bank_e bank)
{
	switch(bank)
	{
		case BANK0: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK0);break;//BANK0寄存器区域
		case BANK1: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK1);break;//BANK1寄存器区域
	}
			
}

//PAJ7620U2唤醒
u8 paj7620u2_wakeup(void)
{ 
	u8 data=0x0a;
	GS_WakeUp();//唤醒PAJ7620U2
	delay_ms(5);//唤醒时间>400us
	GS_WakeUp();//唤醒PAJ7620U2
	delay_ms(5);//唤醒时间>400us
	paj7620u2_selectBank(BANK0);//进入BANK0寄存器区域
	data = GS_Read_Byte(0x00);//读取状态
	if(data!=0x20) return 0; //唤醒失败
	
	return 1;
}

//PAJ7620U2初始化
//返回值：0:失败 1:成功
u8 paj7620u2_init(void)
{
	u8 i;
	u8 status;
	
	GS_i2c_init();//IIC初始化
    status = paj7620u2_wakeup();//唤醒PAJ7620U2
	if(!status) return 0;
	paj7620u2_selectBank(BANK0);//进入BANK0寄存器区域
	for(i=0;i<INIT_SIZE;i++)
	{
		GS_Write_Byte(init_Array[i][0],init_Array[i][1]);//初始化PAJ7620U2
	}
    paj7620u2_selectBank(BANK0);//切换回BANK0寄存器区域
	
	return 1;
}

uint8_t GestureFrist;
//手势识别测试
uint16_t Gesture_test(void)
{
	u8 i;
  u8 status;
	u8 data[2]={0x00};
	u16 gesture_data;

	if(GestureFrist==1)
	{
		paj7620u2_selectBank(BANK0);//进入BANK0寄存器区域
		for(i=0;i<GESTURE_SIZE;i++)
		{
			GS_Write_Byte(gesture_arry[i][0],gesture_arry[i][1]);//手势识别模式初始化
		}
		paj7620u2_selectBank(BANK0);//切换回BANK0寄存器区域
		GestureFrist=0;
	}

	status = GS_Read_nByte(PAJ_GET_INT_FLAG1,2,&data[0]);//读取手势状态			
	if(!status)
	{
		gesture_data =(u16)data[1]<<8 | data[0];
		if(gesture_data) 
		{
			//修改相应操作
			switch(gesture_data)
			{
				case GES_FORWARD:index_mode =1;break; //向上
				case GES_BACKWARD:index_mode =2;break; //向下

				case GES_LEFT:index_mode =3;myMoveSpeedSetDir(0,-60,2500);break; //向左
				case GES_RIGHT:index_mode =4;myMoveSpeedSetDir(0,120,2500);break; //向右
				case GES_DOWM:index_mode =5;myMoveSpeedSetDir(0,30,2500);break; //向前
				case GES_UP:index_mode =6;myMoveSpeedSetDir(0,-150,2500);break; //向后
//					case GES_LEFT:index_mode =3;myMoveSpeedSetXYR(0,-0.5,0,0);break; //向左
//					case GES_RIGHT:index_mode =4;myMoveSpeedSetXYR(0,0.5,0,0);break; //向右
//					case GES_DOWM:index_mode =5;myMoveSpeedSetXYR(0,0,0.5,0);break; //向前
//					case GES_UP:index_mode =6;myMoveSpeedSetXYR(0,0,-0.5,0);break; //向后

				case GES_CLOCKWISE:break; //顺时针
				case GES_COUNT_CLOCKWISE:break; //逆时针
				case GES_WAVE:break; //挥动
				default: break;
			}
		}
	}
}

