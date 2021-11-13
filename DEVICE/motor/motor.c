#include "motor.h"

void motor_init(){
	myGPIO_Init();
	TIM1_PWM_Init();
	myMotorSetPWM(1,0);
	myMotorSetPWM(2,0);
	myMotorSetPWM(3,0);
}

void myMotorSetPWM(uint8_t channel, int16_t duty) {
	duty = -duty;
	
  int16_t duty_limited = duty;
  uint8_t dir;
  if (duty_limited > DUTY_MAX) {
    duty_limited = DUTY_MAX;
  } else if (duty_limited < -DUTY_MAX) {
    duty_limited = -DUTY_MAX;
  }
  if (duty < 0) {
    dir = 1;
		duty_limited=-duty_limited;
  } else if(duty==0){
		dir = 1;
	}	else {
    dir = 0;
    duty_limited = DUTY_MAX - duty_limited;
  }
  switch (channel) {
  case 1:
		MOTOR_DIR1=dir;
		TIM_SetCompare1(TIM1,duty_limited);
    break;
  case 2:
		MOTOR_DIR2=dir;
		TIM_SetCompare2(TIM1,duty_limited);
    break;
  case 3:
		MOTOR_DIR3=dir;
		TIM_SetCompare3(TIM1,duty_limited);
    break;
  }
}
