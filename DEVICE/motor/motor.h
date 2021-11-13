#ifndef __MOTOR_H
#define __MOTOR_H
#include "hardware.h" 

#define DUTY_MAX (999)

void motor_init(void);
void myMotorSetPWM(uint8_t channel, int16_t duty);

#endif
