#include "encoder.h"
#include "tim.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"

// 编码器结构体
typedef struct {
    int32_t count;       // 总计数
    int32_t last_count;  // 上一次计数
    float speed;         // 速度（脉冲/ms）
} Encoder_Info;

Encoder_Info encoder1 = {0, 0, 0};  // TIM1
Encoder_Info encoder3 = {0, 0, 0};  // TIM3
Encoder_Info encoder4 = {0, 0, 0};  // TIM4
Encoder_Info encoder5 = {0, 0, 0};  // TIM5


// 编码器初始化
void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
}


// 读取计数值
int32_t Encoder_GetCount(TIM_HandleTypeDef *htim)
{
    if (htim == &htim1) {
        encoder1.count = (int32_t)__HAL_TIM_GET_COUNTER(&htim1);
        return encoder1.count;
    }
    else if (htim == &htim3) {
        encoder3.count = (int32_t)__HAL_TIM_GET_COUNTER(&htim3);
        return encoder3.count;
    }
    else if (htim == &htim4) {
        encoder4.count = (int32_t)__HAL_TIM_GET_COUNTER(&htim4);
        return encoder4.count;
    }
    else if (htim == &htim5) {
        encoder5.count = (int32_t)__HAL_TIM_GET_COUNTER(&htim5);
        return encoder5.count;
    }
    return 0;
}


// 计算速度（定时调用，比如10ms一次）
void Encoder_CalcSpeed(TIM_HandleTypeDef *htim, uint32_t interval_ms)
{
    int32_t current_count;
    int32_t diff;
    
    if (htim == &htim1) {
        current_count = (int32_t)__HAL_TIM_GET_COUNTER(&htim1);
        diff = current_count - encoder1.last_count;
        encoder1.speed = (float)diff / interval_ms;
        encoder1.last_count = current_count;
    }
    else if (htim == &htim3) {
        current_count = (int32_t)__HAL_TIM_GET_COUNTER(&htim3);
        diff = current_count - encoder3.last_count;
        encoder3.speed = (float)diff / interval_ms;
        encoder3.last_count = current_count;
    }
    else if (htim == &htim4) {
        current_count = (int32_t)__HAL_TIM_GET_COUNTER(&htim4);
        diff = current_count - encoder4.last_count;
        encoder4.speed = (float)diff / interval_ms;
        encoder4.last_count = current_count;
    }
    else if (htim == &htim5) {
        current_count = (int32_t)__HAL_TIM_GET_COUNTER(&htim5);
        diff = current_count - encoder5.last_count;
        encoder5.speed = (float)diff / interval_ms;
        encoder5.last_count = current_count;
    }
}


// 获取速度
float Encoder_GetSpeed(TIM_HandleTypeDef *htim)
{
    if (htim == &htim1) return encoder1.speed;
    else if (htim == &htim3) return encoder3.speed;
    else if (htim == &htim4) return encoder4.speed;
    else if (htim == &htim5) return encoder5.speed;
    return 0.0f;
}
