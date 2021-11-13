/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONTROL_H__
#define __CONTROL_H__

/* Includes ------------------------------------------------------------------*/
#include "hardware.h" 
#include "device.h"

//三轮
typedef struct
{
    float v1;
    float v2;
    float v3;
}ActThreeVell;

// PID算法的数据结构
typedef struct PID {
  float P; //参数
  float I;
  float D;
  float Error;    //比例项
  float Integral; //积分项
  float Differ;   //微分项
  float PreError;
  float PrePreError;
  float Ilimit;
  float Irang;
  float Pout;
  float Iout;
  float Dout;
  float OutPut;
  uint8_t Ilimit_flag; //积分分离
} PID_TYPE;

extern PID_TYPE PID_POS[3],PID_SPEED[3];

void PID_Postion_Cal(PID_TYPE *PID, float target, float measure);
void PidParameter_init(void);
void mySpeedPIDProcess(ActThreeVell vell);
ActThreeVell ThreeWheelVellControl(float Vx, float Vy, float angularVell,float theta);

#endif