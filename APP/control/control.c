#include "control.h"
#include <stdint.h>
#include <math.h>
#include "app.h"

PID_TYPE PID_POS[3], PID_SPEED[3];
ActThreeVell vell;


#define AFA (60)
#define L (10)

ActThreeVell ThreeWheelVellControl(float Vx, float Vy, float angularVell,float theta){
	vell.v1 = (float)(-cos((AFA + theta) / 180.0f*3.1415926f) * Vx - sin((theta + AFA) / 180.0f*3.1415926f) * Vy + L * angularVell);
	vell.v2 = (float)(cos(theta / 180.0f*3.1415926f) * Vx + sin(theta /180.0f*3.1415926f) * Vy      + L * angularVell);
	vell.v3 = (float)(-cos((AFA - theta) / 180.0f * 3.1415926f) * Vx + sin((AFA - theta) / 180.0f*3.1415926f) * Vy + L * angularVell);

	return vell;
}

void mySpeedPIDProcess(ActThreeVell vell){	
	PID_Postion_Cal(&PID_SPEED[0],vell.v1,wheel_speed[0]);
	PID_Postion_Cal(&PID_SPEED[1],vell.v2,wheel_speed[1]);
	PID_Postion_Cal(&PID_SPEED[2],vell.v3,wheel_speed[2]);

	myMotorSetPWM(1, PID_SPEED[0].OutPut);
	myMotorSetPWM(2, PID_SPEED[1].OutPut);
	myMotorSetPWM(3, PID_SPEED[2].OutPut);
}

/*****************************************************************************
 * 函  数：void PID_Postion_Cal(PID_TYPE*PID,float target,float measure)
 * 功  能：位置式PID算法
 * 参  数：PID: 算法P I D参数的结构体
 *         target: 目标值
 *         measure: 测量值
 * 返回值：无
 * 备  注: 角度环和角速度环共用此函数
 *****************************************************************************/
void PID_Postion_Cal(PID_TYPE *PID, float target, float measure) {
  PID->Error = target - measure;            //误差
  PID->Differ = PID->Error - PID->PreError; //微分量

  PID->Pout = PID->P * PID->Error;                       //比例控制
  PID->Iout = PID->Ilimit_flag * PID->I * PID->Integral; //积分控制
  PID->Dout = PID->D * PID->Differ;                      //微分控制

  PID->OutPut = PID->Pout + PID->Iout + PID->Dout; //比例 + 积分 + 微分总控制

  if (measure > (PID->Ilimit) || measure < -PID->Ilimit) //积分分离
  {
    PID->Ilimit_flag = 0;
  } else {
    PID->Ilimit_flag =
        1; //加入积分(只有测量值在-PID->Ilimit~PID->Ilimit 范围内时才加入积分)
    PID->Integral += PID->Error;    //对误差进行积分
    if (PID->Integral > PID->Irang) //积分限幅
      PID->Integral = PID->Irang;
    if (PID->Integral < -PID->Irang) //积分限幅
      PID->Integral = -PID->Irang;
  }
  PID->PreError = PID->Error; //前一个误差值
}

void PidParameter_init(void) {
  // ROLL轴
  uint8_t i = 0;
  for (i = 0; i < 3; i++) {
    PID_POS[i].Ilimit_flag = 0; // Roll轴角速度积分的分离标志
    PID_POS[i].Ilimit = 150;    // Roll轴角速度积分范围
    PID_POS[i].Irang =
        1200; // Roll轴角速度积分限幅度（由于电机输出有限，所以积分输出也是有限的）
    PID_POS[i].P = 40;  // Roll轴角速度比例参数
    PID_POS[i].I = 0; // Roll轴角速度积分参数
    PID_POS[i].D = 0;  // Roll轴角速度微分参数

    PID_SPEED[i].Ilimit_flag = 0; // 积分的分离标志
    PID_SPEED[i].Ilimit = 1000;     // 积分范围
    PID_SPEED[i].Irang =
        300; // 积分限幅度（由于电机输出有限，所以积分输出也是有限的）
    PID_SPEED[i].P = 1.2;   // 比例参数
    PID_SPEED[i].I = 0.1;     // 积分参数
    PID_SPEED[i].D = 0.1;     // 微分参数
  }
}
