#include "stm32f10x.h"
#include "hardware.h"
#include "device.h"
#include "app.h"

#define laserLength 300

//u8 flag_gesture = 0,flag_mpu6050 = 0,flag_laser = 0;
u8 mpu6050_failed = 0,gesture_failed = 0,mpu6050_successful = 0;
float pitch,roll,yaw;

ActThreeVell globalVell;
uint8_t pak;
int main(void)
{
	uint16_t dir=0;
	uint32_t lastTikc500,lastTikc200,lastTikc20,lastTikc1,tickllll;
	uint8_t i;

	delay_init();

	motor_init();
	myEncoderInit();
	
	initValuePack(9600);
	usart3_init(115200);//USART3初始化 Laser
	
	MPU_Init();					//初始化MPU6050
	
	PidParameter_init();
	
	while(mpu_dmp_init())//MPU6050 ERROR
	{
		mpu6050_failed = 1;
	}
	mpu6050_failed = 0;
	

	while(!paj7620u2_init())//GESTURE ERROR
	{
	 	gesture_failed = 1;
	}
	gesture_failed = 0;

//	while(1)
//	{
//		delay_ms(200);
//		myLED=!myLED;
//		TIM_SetCompare1(TIM1,500);
//		TIM_SetCompare2(TIM1,500);
//		TIM_SetCompare3(TIM1,500);
//	}	
	while(1)
	{
		if(sysTick-lastTikc1>=1){
			lastTikc1=sysTick;
			mySpeedPIDProcess(globalVell);
		}

		if(sysTick-lastTikc20>=20){
			lastTikc20=sysTick;
			myGetSpeed(5);
			if(RC_Data.pak==0){
				GestureFrist = 1;
			}
			if(RC_Data.mod==1)
			{
				myMoveSpeedSetXYR(0,RC_Data.speed_x,RC_Data.speed_y,RC_Data.speed_r);
			}
//			else if(RC_Data.mod==0){
//				vell=myMoveSpeedSetXYR(0,0,0,0);
//			}
			//手势识别模式
			else if(RC_Data.pak)
			{
				Gesture_test();//PAJ7620U2传感器测试
			}
			//MPU6050姿态调整模式
			else if(RC_Data.MPU)
			{
				if(yaw<0){
					myMoveSpeedSetXYR(0,0,0,-0.02*yaw);
				}	else if(yaw>5){
					myMoveSpeedSetXYR(0,0,0,-0.02*yaw);
				} else {
					myMoveSpeedSetXYR(0,0,0,0);
				}
			}
			//激光板测距模式
			else if(RC_Data.laser)
			{
				if(Laser_distance[1]>laserLength+10){
					myMoveSpeedSetDir(0,90,10*(Laser_distance[1]-laserLength));
				}
				else if(Laser_distance[1]<laserLength){
					myMoveSpeedSetDir(0,90,20*(Laser_distance[1]-laserLength));
				}
				else{
					myMoveSpeedSetDir(0,0,0);
				}
			}
			else
			{
				myMoveSpeedSetXYR(0,0,0,0);
			}
			
				//pitch:俯仰角 精度:0.1°   范围:-90.0° <---> +90.0°
				//roll:横滚角  精度:0.1°   范围:-180.0°<---> +180.0°
				//yaw:航向角   精度:0.1°   范围:-180.0°<---> +180.0°
				//返回值:0,正常
				//    其他,失败
			if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
			{
				mpu6050_successful = 1;
			}
		}

		if(sysTick-lastTikc500>=500){
			lastTikc500=sysTick;
			myLED=!myLED;
		}

		if(sysTick-lastTikc200>=200){
			lastTikc200=sysTick;
			if(readValuePack()){
				ReadPack();
			}
			fillSendPack();
			sendValuePack();
		}

//		if(sysTick-tickllll>=2000*4){
//			tickllll=sysTick;
//			dir += 90;
//			dir = dir%360;
//			vell=myMoveSpeedSet(0,dir,1500);


	}
}

