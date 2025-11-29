#include "move.h"
#include "tim.h"
#include "GPIO.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_uart.h"
#include "main.h"

void LFMotor(int8_t speed)     //左前电机
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
void RFMotor(int8_t speed)    //右前电机
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
void LBMotor(int8_t speed)          //左后电机
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
void RBMotor(int8_t speed)          //右后电机
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

void FM()           //向前
{
	LFMotor(50);
	RFMotor(50);
	LBMotor(50);
	RBMotor(50);
}
void BM()          //向后
{
	LFMotor(-50);
	RFMotor(-50);
	LBMotor(-50);
	RBMotor(-50);
}
void LM()         //向左
{
	LFMotor(-50);
	RFMotor(50);
	LBMotor(50);
	RBMotor(-50);
}
void RM()         //向右    
{
	LFMotor(50);
	RFMotor(-50);
	LBMotor(-50);
	RBMotor(50);
}
void LR()         //左旋
{
	LFMotor(-50);
	RFMotor(50);
	LBMotor(-50);
	RBMotor(50);
}
void RR()           //右旋
{
	LFMotor(50);
	RFMotor(-50);
	LBMotor(50);
	RBMotor(-50);
}
void Brake()           //刹车
{
	LFMotor(0);
	RFMotor(0);
	LBMotor(0);
	RBMotor(0);
}
