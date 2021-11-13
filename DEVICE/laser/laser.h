#ifndef __LASER_H
#define __LASER_H
#include "hardware.h"


#define   LaserRCVSTAHEADER1  0xff
#define   LaserRCVSTAHEADER2  0xff

extern u16 Laser_distance[5];

//���� ״̬��
typedef enum tagLaserRCVSTATE
{
	LaserRCVSTA_HEADER1,   //0xff 
	LaserRCVSTA_HEADER2,   //0xff
	LaserRCVSTA_ID,        //ID
	LaserRCVSTA_Length,
	LaserRCVSTA_error,     //����λ
	LaserRCVSTA_L,         //�Ͱ�λ
	LaserRCVSTA_H,         //�߰�λ
	LaserRCVSTA_CHECK      //У��λ
}LaserRCVSTATE;

typedef enum Laser_INST
{
	Laser_READ         = 0x02,
	Laser_NONE     	   = 0xff,
}LaserINST;


typedef struct Laser_tagDATAPACK
{
	u8 id;
	LaserINST instruction;
	u8 length;
	u8 param[156];	
}Laser_DATAPACK, *Laser_PDATAPACK;


void Laser_StateIterator(u8 data);
u8 Get_distance(u8 direction);


#endif



