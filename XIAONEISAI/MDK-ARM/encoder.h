#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f1xx_hal.h"

void Encoder_Init(void);
int32_t Encoder_GetCount(TIM_HandleTypeDef *htim);
void Encoder_CalcSpeed(TIM_HandleTypeDef *htim, uint32_t interval_ms);
float Encoder_GetSpeed(TIM_HandleTypeDef *htim);

#endif
