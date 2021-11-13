#ifndef __FORWARD_H
#define __FORWARD_H

#include "hardware.h" 
#include "app.h"
#include "control.h"
#include <math.h>

void myMoveSpeedSetDir(uint8_t headless,float dir,float speed);
void myMoveSpeedSetXYR(uint8_t headless,float Vx,float Vy,float Vr);

#endif


