#include "encounter.h"
#include "main.h"
#include "gpio.h"
#include "stdio.h"
#include "tim.h"

int16_t ENCODE_1_GETCOUNT(void){
	int16_t count = (int16_t)__HAL_TIM_GET_COUNTER(&htim1);
	__HAL_TIM_SET_COUNTER(&htim1,0);
	return count;
}

int16_t ENCODE_2_GETCOUNT(void){
	int16_t count = (int16_t)__HAL_TIM_GET_COUNTER(&htim2);
	__HAL_TIM_SET_COUNTER(&htim2,0);
	return count;
}

int16_t ENCODE_3_GETCOUNT(void){
	int16_t count = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
	__HAL_TIM_SET_COUNTER(&htim3,0);
	return count;
}

int16_t ENCODE_4_GETCOUNT(void){
	int16_t count = (int16_t)__HAL_TIM_GetCounter(&htim4);
	__HAL_TIM_SET_COUNTER(&htim4,0);
	return count;
}
