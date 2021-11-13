#ifndef _TIMER_H_
#define _TIMER_H_

#include "hardware.h"

extern uint32_t sysTick;

void TIM1_PWM_Init(void);

void Encoder_Init_TIM2(void);
void Encoder_Init_TIM3(void);
void Encoder_Init_TIM4(void);

#endif
