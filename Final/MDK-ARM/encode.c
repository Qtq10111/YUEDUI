#include "encode.h"
#include "tim.h"

/**
 * @简  述  编码器获取计数器数值
 * @参  数  无
 * @返回值  计数器当前值
 */
int16_t ENCODER_A_GetCounter(void)
{
	int16_t Encoder_TIM = 0;
	Encoder_TIM = (short)TIM3->CNT;
	TIM3->CNT=0;
	return Encoder_TIM;
}

/**
 * @简  述  编码器获取计数器数值
 * @参  数  无
 * @返回值  计数器当前值
 */
int16_t ENCODER_B_GetCounter(void)
{
	int16_t Encoder_TIM = 0;
	Encoder_TIM = (short)TIM4->CNT;
	TIM4->CNT=0;
	return Encoder_TIM;
}

/**
 * @简  述  编码器获取计数器数值
 * @参  数  无
 * @返回值  计数器当前值
 */
int16_t ENCODER_C_GetCounter(void)
{
	int16_t Encoder_TIM = 0;
	Encoder_TIM = (short)TIM5->CNT;
	TIM5->CNT=0;
	return Encoder_TIM;
}

/**
 * @简  述  编码器获取计数器数值
 * @参  数  无
 * @返回值  计数器当前值
 */
int16_t ENCODER_D_GetCounter(void)
{
	int16_t Encoder_TIM = 0;
	Encoder_TIM = (short)TIM2->CNT;
	TIM2->CNT=0;
	return Encoder_TIM;
}
