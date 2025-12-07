#include "main.h"
#include "Catch.h"
#include "stdlib.h"
#include "stdint.h"
#include "usart.h"
#include "tim.h"

void Catch_hand(uint8_t pwm){ //抓取角度设置
	uint8_t pwm_input;
	pwm_input = 50+2*pwm;
	HAL_UART_Transmit_IT(&huart1,&pwm_input,sizeof(pwm_input));
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,pwm_input);
}

void Catch_move(uint8_t up,uint8_t down){ //抓取丝杆移动
	uint8_t Up = up;
	uint8_t Dn = down;
	
	if(Up==1){
		
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
	}else{
		if(Dn == 1){
			
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
		}else if(Dn != 1 && Up != 1){
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
		}
		
	}
	
}
