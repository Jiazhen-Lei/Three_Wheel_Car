#ifndef __PAJ7620U2_IIC_H
#define __PAJ7620U2_IIC_H

#include "hardware.h"




#define GS_SDA_IN()  {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=8<<4;}
#define GS_SDA_OUT() {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=3<<4;}

//IO²Ù×÷º¯Êý	 
#define GS_IIC_SCL    PBout(0) 		//SCL
#define GS_IIC_SDA    PBout(1) 		//SDA	 
#define GS_READ_SDA   PBin(1) 		    //ÊäÈëSDA 

u8 GS_Write_Byte(u8 REG_Address,u8 REG_data);
u8 GS_Read_Byte(u8 REG_Address);
u8 GS_Read_nByte(u8 REG_Address,u16 len,u8 *buf);
void GS_i2c_init(void);
void GS_WakeUp(void);

#endif


