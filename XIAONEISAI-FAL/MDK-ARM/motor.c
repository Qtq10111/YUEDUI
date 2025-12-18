#include "motor.h"
#include "main.h"
#include "pid.h"
#include "stdio.h"



void motor_speed_set(float A,float B,float C,float D)
{
	SPEED_ASET(A);
  SPEED_BSET(B);
  SPEED_CSET(C);
  SPEED_DSET(D);
}
