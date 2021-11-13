#include "laser.h"

LaserRCVSTATE LaserSta;

u32 Laser_check = 0;

u16 Laser_distance[5]={0,0,0,0,0};

Laser_DATAPACK Laser_datapack = 
{
	0,   //id
    Laser_NONE,//指令
	0,  //长度
   	{0}	 //参数
};

void Laser_StateIterator(u8 data)
{
	switch(LaserSta)
	{
		case LaserRCVSTA_HEADER1:
					if(data == LaserRCVSTAHEADER1)
					{
						Laser_check = 0;
						LaserSta = LaserRCVSTA_HEADER2;				
					}
					break;
				
		case LaserRCVSTA_HEADER2:
					  if(data == LaserRCVSTAHEADER2)
					  {
						Laser_check = 0;
						Laser_datapack.id=0;
						Laser_datapack.instruction=Laser_NONE;
						Laser_datapack.length=0;
						Laser_datapack.param[0]=0;
						Laser_datapack.param[1]=0;
						LaserSta = LaserRCVSTA_ID;				
					  }
					  else 
					  {
						LaserSta=LaserRCVSTA_HEADER1;
					  }  
						break;	

		case LaserRCVSTA_ID: 
		
				  if(data==0x41||data==0x42||data==0x43||data==0x44)
				  {
					Laser_datapack.id=data;
					Laser_check+=data;
					  
					LaserSta=LaserRCVSTA_Length;
				  }
				  else 
				  {
					LaserSta=LaserRCVSTA_HEADER1;
				  }  
				  break;	
			
		case LaserRCVSTA_Length:
					if(data==0x04)
					{
						Laser_datapack.length=data;
						Laser_check+=data;
						LaserSta=LaserRCVSTA_error;
					}
					else 
					{
						LaserSta=LaserRCVSTA_HEADER1;
					}  
					break;
		case LaserRCVSTA_error:
					if(data==0)
					{
						Laser_check+=data;
						LaserSta=LaserRCVSTA_L;
					}
					else 
				   {
					LaserSta=LaserRCVSTA_HEADER1;
				   }  
					break;
			
		case LaserRCVSTA_L:
		          Laser_check+=data;  
		          Laser_datapack.param[0]=data;
		          LaserSta=LaserRCVSTA_H;
		          break;
		case LaserRCVSTA_H:
			      Laser_check+=data;
		          Laser_datapack.param[1]=data;
			      LaserSta=LaserRCVSTA_CHECK;
		          break;
	    
		case LaserRCVSTA_CHECK:
			      Laser_check=(~Laser_check) & 0xff;
					if(Laser_check==data)
					{
						switch(Laser_datapack.id)
						{
							case 0x41:Laser_distance[1]= (short)(Laser_datapack.param[0]|( Laser_datapack.param[1]<<8));break;
							case 0x42:Laser_distance[2]= (short)(Laser_datapack.param[0]|( Laser_datapack.param[1]<<8));break;
							case 0x43:Laser_distance[3]= (short)(Laser_datapack.param[0]|( Laser_datapack.param[1]<<8));break;
							case 0x44:Laser_distance[4]= (short)(Laser_datapack.param[0]|( Laser_datapack.param[1]<<8));break;
							default: break;	
						
						}
					Laser_check=0;
					}
					LaserSta=LaserRCVSTA_HEADER1;
                    break;
				  	
		default: 
				LaserSta=LaserRCVSTA_HEADER1;
		        break;
			
	}
	
}

