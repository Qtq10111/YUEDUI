#include "pid.h"
#include "move.h"
#include "main.h"
#include "gpio.h"
#include "stdio.h"
#include "tim.h"

int16_t motor_kp = 800;
int16_t motor_kd = 400;

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

int16_t SPEED_PidCtlB(float spd_target,float spd_current){
	static int16_t motor_pwm_out;
	static float bias,bias_last;
	
	bias = spd_target - spd_target;
	motor_pwm_out += motor_kp * bias + motor_kd*(bias - bias_last);
	bias_last = bias;
	
	if(motor_pwm_out > 2000 )motor_pwm_out = 2000;
	if(motor_pwm_out < -2000)motor_pwm_out = -2000;
	
	return motor_pwm_out;
}

int16_t SPEED_PidCtlC(float spd_target,float spd_current){
	static int16_t motor_pwm_out;
	static float bias,bias_last;
	
	bias = spd_target - spd_target;
	motor_pwm_out += motor_kp * bias + motor_kd*(bias - bias_last);
	bias_last = bias;
	
	if(motor_pwm_out > 2000 )motor_pwm_out = 2000;
	if(motor_pwm_out < -2000)motor_pwm_out = -2000;
	
	return motor_pwm_out;
}

int16_t SPEED_PidCtlD(float spd_target,float spd_current){
	static int16_t motor_pwm_out;
	static float bias,bias_last;
	
	bias = spd_target - spd_target;
	motor_pwm_out += motor_kp * bias + motor_kd*(bias - bias_last);
	bias_last = bias;
	
	if(motor_pwm_out > 2000 )motor_pwm_out = 2000;
	if(motor_pwm_out < -2000)motor_pwm_out = -2000;
	
	return motor_pwm_out;
}


void SPEED_ASET(int16_t speed){//主函数pwm输入的标准值
	int16_t temp = speed;
	
	if(temp > 2000)temp = 2000;
	if(temp < -2000)temp = -2000;
	
	if(temp > 0){
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,temp);
	}else{
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,temp+2000);
	}

}

void SPEED_BSET(int16_t speed){//主函数pwm输入的标准值
	int16_t temp = speed;
	
	if(temp > 2000)temp = 2000;
	if(temp < -2000)temp = -2000;
	
	if(temp > 0){
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,temp);
	}else{
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,temp+2000);
	}

}

void SPEED_CSET(int16_t speed){//主函数pwm输入的标准值
	int16_t temp = speed;
	
	if(temp > 2000)temp = 2000;
	if(temp < -2000)temp = -2000;
	
	if(temp > 0){
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,temp);
	}else{
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,temp+2000);
	}

}

void SPEED_DSET(int16_t speed){//主函数pwm输入的标准值
	int16_t temp = speed;
	
	if(temp > 2000)temp = 2000;
	if(temp < -2000)temp = -2000;
	
	if(temp > 0){
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,temp);
	}else{
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,temp+2000);
	}

}


