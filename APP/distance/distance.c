#include "distance.h"
#include "laser.h"

extern u16 Laser_distance[5];

void Distance_warning(void)
{
	if(Laser_distance[1]<MIN_LIMIT)
	{
		//ֹͣ
		//stop()
		//����
		//usart2_send("Distance Warning!!!")
	}
}
