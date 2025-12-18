#ifndef __STEP_MOTOR_H
#define __STEP_MOTOR_H

#include "stdint.h"
#include "stm32f1xx_hal.h"  // 替换为实际MCU型号（如stm32f1xx_hal.h）
#include "stm32f1xx_hal_tim.h"

// 电机组枚举类型
typedef enum {
    MOTOR_GROUP1,  // 左前(LF)、右前(RF)电机（TIM1_CH1、TIM1_CH2）
    MOTOR_GROUP2   // 左后(LB)、右后(RB)电机（TIM1_CH3、TIM1_CH4）
} Motor_Group_t;

// 每圈总脉冲数（1.8°步距角42电机，根据细分调整：整步=200，4细分=800，8细分=1600）
#define TOTAL_STEP_PER_REV  200

// 函数声明
void Motor_Init(void);
void Motor_Set_Speed(Motor_Group_t group, uint16_t pulse_freq);
void Motor_Rotate_Angle(Motor_Group_t group, float target_angle);
void Motor_Stop(Motor_Group_t group); // 新增：紧急停止函数

#endif /* __STEP_MOTOR_H */
