#include "motor.h"
#include "main.h"
#include "stdio.h"
#include "tim.h"


void motor_speed_set(float A,float B,float C,float D)
{
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,A);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,B);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,C);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,D);
}
