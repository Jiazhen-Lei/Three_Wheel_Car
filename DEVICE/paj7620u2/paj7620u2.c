#include "paj7620u2.h"
#include "paj7620u2_cfg.h"
#include "paj7620u2_iic.h"
#include "move.h"

u8 index_mode = 0;

//ѡ��PAJ7620U2 BANK����
void paj7620u2_selectBank(bank_e bank)
{
	switch(bank)
	{
		case BANK0: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK0);break;//BANK0�Ĵ�������
		case BANK1: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK1);break;//BANK1�Ĵ�������
	}
			
}

//PAJ7620U2����
u8 paj7620u2_wakeup(void)
{ 
	u8 data=0x0a;
	GS_WakeUp();//����PAJ7620U2
	delay_ms(5);//����ʱ��>400us
	GS_WakeUp();//����PAJ7620U2
	delay_ms(5);//����ʱ��>400us
	paj7620u2_selectBank(BANK0);//����BANK0�Ĵ�������
	data = GS_Read_Byte(0x00);//��ȡ״̬
	if(data!=0x20) return 0; //����ʧ��
	
	return 1;
}

//PAJ7620U2��ʼ��
//����ֵ��0:ʧ�� 1:�ɹ�
u8 paj7620u2_init(void)
{
	u8 i;
	u8 status;
	
	GS_i2c_init();//IIC��ʼ��
    status = paj7620u2_wakeup();//����PAJ7620U2
	if(!status) return 0;
	paj7620u2_selectBank(BANK0);//����BANK0�Ĵ�������
	for(i=0;i<INIT_SIZE;i++)
	{
		GS_Write_Byte(init_Array[i][0],init_Array[i][1]);//��ʼ��PAJ7620U2
	}
    paj7620u2_selectBank(BANK0);//�л���BANK0�Ĵ�������
	
	return 1;
}

uint8_t GestureFrist;
//����ʶ�����
uint16_t Gesture_test(void)
{
	u8 i;
  u8 status;
	u8 data[2]={0x00};
	u16 gesture_data;

	if(GestureFrist==1)
	{
		paj7620u2_selectBank(BANK0);//����BANK0�Ĵ�������
		for(i=0;i<GESTURE_SIZE;i++)
		{
			GS_Write_Byte(gesture_arry[i][0],gesture_arry[i][1]);//����ʶ��ģʽ��ʼ��
		}
		paj7620u2_selectBank(BANK0);//�л���BANK0�Ĵ�������
		GestureFrist=0;
	}

	status = GS_Read_nByte(PAJ_GET_INT_FLAG1,2,&data[0]);//��ȡ����״̬			
	if(!status)
	{
		gesture_data =(u16)data[1]<<8 | data[0];
		if(gesture_data) 
		{
			//�޸���Ӧ����
			switch(gesture_data)
			{
				case GES_FORWARD:index_mode =1;break; //����
				case GES_BACKWARD:index_mode =2;break; //����

				case GES_LEFT:index_mode =3;myMoveSpeedSetDir(0,-60,2500);break; //����
				case GES_RIGHT:index_mode =4;myMoveSpeedSetDir(0,120,2500);break; //����
				case GES_DOWM:index_mode =5;myMoveSpeedSetDir(0,30,2500);break; //��ǰ
				case GES_UP:index_mode =6;myMoveSpeedSetDir(0,-150,2500);break; //���
//					case GES_LEFT:index_mode =3;myMoveSpeedSetXYR(0,-0.5,0,0);break; //����
//					case GES_RIGHT:index_mode =4;myMoveSpeedSetXYR(0,0.5,0,0);break; //����
//					case GES_DOWM:index_mode =5;myMoveSpeedSetXYR(0,0,0.5,0);break; //��ǰ
//					case GES_UP:index_mode =6;myMoveSpeedSetXYR(0,0,-0.5,0);break; //���

				case GES_CLOCKWISE:break; //˳ʱ��
				case GES_COUNT_CLOCKWISE:break; //��ʱ��
				case GES_WAVE:break; //�Ӷ�
				default: break;
			}
		}
	}
}

