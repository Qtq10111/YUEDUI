#include "PidMotor.h"
#include "main.h"
#include "tim.h"

int16_t motor_kp = 800;
int16_t motor_kd = 400;

/**
 * @简  述  电机PID控制函数
 * @参  数  spd_target:编码器速度目标值 ,范围（±250）
 *          spd_current: 编码器速度当前值
 * @返回值  电机PWM速度
 */
int16_t SPEED_PidCtlA(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias, bias_last;

	// 获得偏差值
	bias = spd_target - spd_current;

	// PID计算电机输出PWM值
	motor_pwm_out += motor_kp * bias + motor_kd * (bias - bias_last);

	// 记录上次偏差
	bias_last = bias;

	// 限制最大输出
	if (motor_pwm_out > 2000)
		motor_pwm_out = 2000;
	if (motor_pwm_out < -2000)
		motor_pwm_out = -2000;

	// 返回PWM控制值
	return motor_pwm_out;
}

/**
 * @简  述  电机PID控制函数
 * @参  数  spd_target:编码器速度目标值
 *          spd_target: 编码器速度当前值
 * @返回值  电机PWM速度
 */
int16_t SPEED_PidCtlB(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias, bias_last;

	// 获得偏差值
	bias = spd_target - spd_current;

	// PID计算电机输出PWM值
	motor_pwm_out += motor_kp * bias + motor_kd * (bias - bias_last);

	// 记录上次偏差
	bias_last = bias;

	// 限制最大输出
	if (motor_pwm_out > 2000)
		motor_pwm_out = 2000;
	if (motor_pwm_out < -2000)
		motor_pwm_out = -2000;

	// printf("@%d  ",motor_pwm_out);

	// 返回PWM控制值
	return motor_pwm_out;
}

/**
 * @简  述  电机PID控制函数
 * @参  数  spd_target:编码器速度目标值
 *          spd_target: 编码器速度当前值
 * @返回值  电机PWM速度
 */
int16_t SPEED_PidCtlC(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias, bias_last;

	// 获得偏差值
	bias = spd_target - spd_current;

	// PID计算电机输出PWM值
	motor_pwm_out += motor_kp * bias + motor_kd * (bias - bias_last);

	// 记录上次偏差
	bias_last = bias;

	// 限制最大输出
	if (motor_pwm_out > 2000)
		motor_pwm_out = 2000;
	if (motor_pwm_out < -2000)
		motor_pwm_out = -2000;

	// 返回PWM控制值
	return motor_pwm_out;
}

/**
 * @简  述  电机PID控制函数
 * @参  数  spd_target:编码器速度目标值
 *          spd_target: 编码器速度当前值
 * @返回值  电机PWM速度
 */
int16_t SPEED_PidCtlD(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias, bias_last;

	// 获得偏差值
	bias = spd_target - spd_current;

	// PID计算电机输出PWM值
	motor_pwm_out += motor_kp * bias + motor_kd * (bias - bias_last);

	// 记录上次偏差
	bias_last = bias;

	// 限制最大输出
	if (motor_pwm_out > 2000)
		motor_pwm_out = 2000;
	if (motor_pwm_out < -2000)
		motor_pwm_out = -2000;

	// 返回PWM控制值
	return motor_pwm_out;
}

/**
 * @简  述 电机PWM速度控制
 * @参  数 speed 电机转速数值，范围-2000~2000
 * @返回值 无
 */
void MOTOR_A_SetSpeed(int16_t speed)
{
	int16_t temp;

	temp = speed;

	if (temp > 2000)
		temp = 2000;
	if (temp < -2000)
		temp = -2000;

	if (temp > 0)
	{
		__HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_2,temp);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_2,temp+2000);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET);
	}
}

/**
 * @简  述 电机PWM速度控制
 * @参  数 speed 电机转速数值，范围-2000~2000
 * @返回值 无
 */
void MOTOR_B_SetSpeed(int16_t speed)
{
	int16_t temp;

	temp = speed;

	if (temp > 2000)
		temp = 2000;
	if (temp < -2000)
		temp = -2000;

	if (temp > 0)
	{
		__HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_4,temp);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_4,temp+2000);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
	}
}

/**
 * @简  述 电机PWM速度控制
 * @参  数 speed 电机转速数值，范围-2000~2000
 * @返回值 无
 */
void MOTOR_C_SetSpeed(int16_t speed)
{
	int16_t temp;

	temp = speed;

	if (temp > 2000)
		temp = 2000;
	if (temp < -2000)
		temp = -2000;

	if (temp > 0)
	{
		__HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_1,temp);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_1,temp+2000);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
	}
}

/**
 * @简  述 电机PWM速度控制
 * @参  数 speed 电机转速数值，范围-2000~2000
 * @返回值 无
 */
void MOTOR_D_SetSpeed(int16_t speed)
{
	int16_t temp;

	temp = speed;

	if (temp > 2000)
		temp = 2000;
	if (temp < -2000)
		temp = -2000;

	if (temp > 0)
	{
		__HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_3,temp);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_3,temp+2000);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);
	}
}
