#ifndef __PID_H
#define __PID_H

#include "main.h"
void SPEED_ASET(int16_t speed);
void SPEED_BSET(int16_t speed);
void SPEED_CSET(int16_t speed);
void SPEED_DSET(int16_t speed);
int16_t SPEED_PidCtlA(float spd_target,float spd_current);
int16_t SPEED_PidCtlB(float spd_target,float spd_current);
int16_t SPEED_PidCtlC(float spd_target,float spd_current);
int16_t SPEED_PidCtlD(float spd_target,float spd_current);

typedef struct
{
	short CNT_RT;
	float TG;
	short PWM;
	double RT;
}Wheel;

extern Wheel W_A,W_B,W_C,W_D;

#endif
