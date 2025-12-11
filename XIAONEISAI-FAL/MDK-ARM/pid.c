#include "pid.h"
#include "move.h"
#include "main.h"
#include "gpio.h"
#include "stdio.h"
#include "tim.h"

int16_t motor_kp = 800;
int16_t motor_kd = 400;

typedef struct
{
	float target_Speed;
	float current_speed;
	int16_t pwm_out;
}Wheel;

Wheel W_A,W_B,W_C,W_D;

int16_t SPEED_PidCtlA(float spd_target,float spd_current){
	static int16_t motor_pwm_out;
	static float bias,bias_last;
	
	bias = spd_target - spd_target;
	motor_pwm_out += motor_kp * bias + motor_kd*(bias - bias_last);
	bias_last = bias;
	
	if(motor_pwm_out > 2000 )motor_pwm_out = 2000;
	if(motor_pwm_out < -2000)motor_pwm_out = -2000;
	
	return motor_pwm_out;
	
}

void SPEED_SET(int16_t speed){
	int16_t temp = speed;
	
	if(temp > 2000)temp = 2000;
	if(temp < -2000)temp = -2000;
	
	if(temp > 0){
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,temp);
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,temp);
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_3,temp);
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_4,temp);
	}else{
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,temp+2000);
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,temp+2000);
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_3,temp+2000);
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_4,temp+2000);
	}
	
}
