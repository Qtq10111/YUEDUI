#ifndef __STEP_MOTOR_H
#define __STEP_MOTOR_H

#include "main.h"
#include "tim.h"
#include <math.h>

// 电机组定义
typedef enum {
    MOTOR_GROUP1 = 0,  // 第1组（TIM3驱动）
    MOTOR_GROUP2 = 1   // 第2组（TIM4驱动）
} Motor_Group_t;

// 42步进电机核心参数（1.8°步距角）
#define BASE_STEP_PER_REV  200        // 基础步距角1.8° → 每圈200步
#define MICRO_STEP         1          // A4988细分（1/2/4/8/16，需硬件拨码匹配）
#define TOTAL_STEP_PER_REV (BASE_STEP_PER_REV * MICRO_STEP)  // 每圈总脉冲数

// 函数声明
void Motor_Init(void);                  // 电机初始化
void Motor_Rotate_Angle(Motor_Group_t group, float target_angle);  // 旋转指定角度
void Motor_Set_Speed(Motor_Group_t group, uint16_t pulse_freq);    // 调整转速（脉冲频率）

#endif
