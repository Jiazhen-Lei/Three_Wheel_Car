#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "hardware.h"

 	   		   //1000-->1000 0000 0000 0000 0000 0000
//IO��������
#define MPU_SDA_IN()  {GPIOC->CRH&=0X0FFFFFFF;GPIOC->CRH|=8<<28;}
#define MPU_SDA_OUT() {GPIOC->CRH&=0X0FFFFFFF;GPIOC->CRH|=3<<28;}

//IO��������	 
#define MPU_IIC_SCL    PCout(14) 		//SCL
#define MPU_IIC_SDA    PCout(15) 		//SDA	 
#define MPU_READ_SDA   PCin(15) 		//����SDA 

//IIC���в�������
void MPU_IIC_Delay(void);				//MPU IIC��ʱ����
void MPU_IIC_Init(void);                //��ʼ��IIC��IO��				 
void MPU_IIC_Start(void);				//����IIC��ʼ�ź�
void MPU_IIC_Stop(void);	  			//����IICֹͣ�ź�
void MPU_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 MPU_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 MPU_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void MPU_IIC_Ack(void);					//IIC����ACK�ź�
void MPU_IIC_NAck(void);				//IIC������ACK�ź�

void IMPU_IC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 MPU_IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















