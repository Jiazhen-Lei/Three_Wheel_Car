#ifndef __ENCODER_H
#define __ENCODER_H
#include "hardware.h"

#define __TIM_IS_TIM_COUNTING_DOWN(__HANDLE__)    (((__HANDLE__)->CR1 &(TIM_CR1_DIR)) == (TIM_CR1_DIR))
extern uint16_t encoder_last[3],encoder_now[3];
extern float wheel_speed[3];
extern uint8_t dir[3];

void myGetSpeed(float gap);
void myGetEncoder(void);
void myEncoderInit(void);

#endif



