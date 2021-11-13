#ifndef __GPIO_H
#define __GPIO_H
#include "sys.h"

////////////////////////////////////////////////////////////////////////////////// 
#define MOTOR_DIR1 PBout(13)	// PB13
#define MOTOR_DIR2 PBout(14)	// PB13
#define MOTOR_DIR3 PBout(15)	// PB13
#define myLED PCout(13)	// PB13

void myGPIO_Init(void);
		 				    
#endif
