#include "encoder.h"
uint16_t encoder_last[3],encoder_now[3];
float wheel_speed[3]={0.0};
uint8_t dir[3];

void myEncoderInit(){
	Encoder_Init_TIM2();
	Encoder_Init_TIM3();
	Encoder_Init_TIM4();
}

void myGetEncoder(){
	encoder_now[0] = TIM_GetCounter(TIM2);
	encoder_now[1] = TIM_GetCounter(TIM3);
	encoder_now[2] = TIM_GetCounter(TIM4);
	dir[0] = __TIM_IS_TIM_COUNTING_DOWN(TIM2);//returns 0 and 1
	dir[1] = __TIM_IS_TIM_COUNTING_DOWN(TIM3);
	dir[2] = __TIM_IS_TIM_COUNTING_DOWN(TIM4);
}

//返回值为脉冲每秒 TODO：改成转每秒，需要知道减速箱的减速比
//需要严格的每gap ms调用一次，否则会有偏差
void myGetSpeed(float gap){
	uint8_t i;
	myGetEncoder();
	for(i = 0;i<3;i++){
		wheel_speed[i]=encoder_now[i]-encoder_last[i];
		wheel_speed[i]=wheel_speed[i]*1000.0/gap;
		encoder_last[i]=encoder_now[i];
	}
}
