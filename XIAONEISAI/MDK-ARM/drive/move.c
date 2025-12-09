#include "move.h"
#include "tim.h"
#include "GPIO.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_uart.h"
#include "main.h"

void LFMotor(int8_t speed)     //å·¦å‰è½®
{
	if(speed > 0)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
	}
	else if(speed == 0)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, -speed);
	}
}
void RFMotor(int8_t speed)    //å³å‰è½®
{
	if(speed > 0)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
	}
	else if(speed == 0)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, -speed);
	}
}
void LBMotor(int8_t speed)          //å·¦åŽè½®
{
	if(speed > 0)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, speed);
	}
	else if(speed == 0)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, speed);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, -speed);
	}
}
void RBMotor(int8_t speed)          //å³åŽè½®
{
	if(speed > 0)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, speed);
	}
	else if(speed == 0)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, speed);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, -speed);
	}
}

void FM()           //ÏòÇ°
{
	LFMotor(50);
	RFMotor(50);
	LBMotor(50);
	RBMotor(50);
}
void BM()          //Ïòºó
{
	LFMotor(-50);
	RFMotor(-50);
	LBMotor(-50);
	RBMotor(-50);
}
void LM()         //Ïò×ó
{
	LFMotor(-50);
	RFMotor(50);
	LBMotor(50);
	RBMotor(-50);
}
void RM()         //ÏòÓÒ    
{
	LFMotor(50);
	RFMotor(-50);
	LBMotor(-50);
	RBMotor(50);
}
void LR()         //×óÐý
{
	LFMotor(-50);
	RFMotor(50);
	LBMotor(-50);
	RBMotor(50);
}
void RR()           //ÓÒÐý
{
	LFMotor(50);
	RFMotor(-50);
	LBMotor(50);
	RBMotor(-50);
}
void Brake()           //É²³µ
{
	LFMotor(0);
	RFMotor(0);
	LBMotor(0);
	RBMotor(0);
}
