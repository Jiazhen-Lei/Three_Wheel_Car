#include "move.h"

#define HEAD_OFFSET 0

extern ActThreeVell globalVell;

#define SPEED_MAX_XY 5000
#define SPEED_MAX_R 400

void myMoveSpeedSetDir(uint8_t headless,float dir,float speed){
	float Vx = speed*cos(dir/ 180.0f*3.1415926f),Vy = speed*sin(dir / 180.0f*3.1415926f);
	float theta;
	if(headless){
		theta = theta + HEAD_OFFSET;
	}else{
		theta = 0 + HEAD_OFFSET;
	}
	globalVell = ThreeWheelVellControl(Vx,Vy,0,theta);
}

void myMoveSpeedSetXYR(uint8_t headless,float Vx,float Vy,float Vr){
	float theta;
	if(headless){
		theta = theta + HEAD_OFFSET;
	}else{
		theta = 0 + HEAD_OFFSET;
	}
	globalVell = ThreeWheelVellControl(Vx*SPEED_MAX_XY,Vy*SPEED_MAX_XY,Vr*SPEED_MAX_R,theta);
}
