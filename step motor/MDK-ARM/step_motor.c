#include "step_motor.h"
#include "stdint.h"

// 全局变量：脉冲计数
static uint32_t g_pulse_cnt = 0;

// 引脚宏定义（可根据实际硬件修改）
#define GROUP1_DIR_PIN    GPIO_PIN_10
#define GROUP1_DIR_PORT   GPIOA
#define GROUP1_EN_PIN     GPIO_PIN_9
#define GROUP1_EN_PORT    GPIOA

#define GROUP2_DIR_PIN    GPIO_PIN_12
#define GROUP2_DIR_PORT   GPIOA
#define GROUP2_EN_PIN     GPIO_PIN_11
#define GROUP2_EN_PORT    GPIOA

/**
 * @brief  电机初始化：失能A4988，默认方向
 * @param  无
 * @retval 无
 */
void Motor_Init(void)
{
    // 1. 方向引脚默认低电平
    HAL_GPIO_WritePin(GROUP1_DIR_PORT, GROUP1_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GROUP2_DIR_PORT, GROUP2_DIR_PIN, GPIO_PIN_RESET);
    
    // 2. EN引脚高电平（A4988低电平使能，高电平失能）
    HAL_GPIO_WritePin(GROUP1_EN_PORT, GROUP1_EN_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GROUP2_EN_PORT, GROUP2_EN_PIN, GPIO_PIN_SET);
    
    // 3. 停止定时器脉冲输出
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
}

/**
 * @brief  调整电机转速（修改定时器ARR值，改变脉冲频率）
 * @param  group：电机组
 * @param  pulse_freq：目标脉冲频率（Hz，范围100~20000）
 * @retval 无
 */
void Motor_Set_Speed(Motor_Group_t group, uint16_t pulse_freq)
{
    uint16_t arr_val = 1000000 / pulse_freq;  // 1MHz定时器时钟 → ARR=1e6/频率
    if (arr_val < 1) arr_val = 1;             // 防止溢出
    
    if (group == MOTOR_GROUP1) {
        __HAL_TIM_SET_AUTORELOAD(&htim3, arr_val);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, arr_val/2);  // 占空比50%
    } else {
        __HAL_TIM_SET_AUTORELOAD(&htim4, arr_val);
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, arr_val/2);
    }
}

/**
 * @brief  控制电机组旋转固定角度（适配1.8°步距角42电机）
 * @param  group：电机组（MOTOR_GROUP1/MOTOR_GROUP2）
 * @param  target_angle：目标角度（正=顺时针，负=逆时针）
 * @retval 无
 */
void Motor_Rotate_Angle(Motor_Group_t group, float target_angle)
{
    // 1. 计算目标脉冲数（四舍五入，角度绝对值/360 * 每圈总脉冲数）
    uint32_t target_pulse = (uint32_t)(fabs(target_angle) / 360.0f * TOTAL_STEP_PER_REV + 0.5f);
    g_pulse_cnt = 0;

    // 2. 设置旋转方向
    if (target_angle > 0) {  // 顺时针
        if (group == MOTOR_GROUP1) {
            HAL_GPIO_WritePin(GROUP1_DIR_PORT, GROUP1_DIR_PIN, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GROUP2_DIR_PORT, GROUP2_DIR_PIN, GPIO_PIN_SET);
        }
    } else {  // 逆时针
        if (group == MOTOR_GROUP1) {
            HAL_GPIO_WritePin(GROUP1_DIR_PORT, GROUP1_DIR_PIN, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(GROUP2_DIR_PORT, GROUP2_DIR_PIN, GPIO_PIN_RESET);
        }
    }

    // 3. 使能A4988（低电平有效）
    if (group == MOTOR_GROUP1) {
        HAL_GPIO_WritePin(GROUP1_EN_PORT, GROUP1_EN_PIN, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(GROUP2_EN_PORT, GROUP2_EN_PIN, GPIO_PIN_RESET);
    }

    // 4. 启动定时器PWM输出（STEP脉冲）
    if (group == MOTOR_GROUP1) {
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    } else {
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    }

    // 5. 轮询计数脉冲，达到目标后停止
    while (g_pulse_cnt < target_pulse) {
        if (group == MOTOR_GROUP1) {
            // 检测TIM3更新标志（ARR溢出=1个脉冲）
            if (__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_UPDATE) != RESET) {
                __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
                g_pulse_cnt++;
            }
        } else {
            if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE) != RESET) {
                __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
                g_pulse_cnt++;
            }
        }
    }

    // 6. 停止脉冲输出，失能电机
    if (group == MOTOR_GROUP1) {
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
        HAL_GPIO_WritePin(GROUP1_EN_PORT, GROUP1_EN_PIN, GPIO_PIN_SET);
    } else {
        HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
        HAL_GPIO_WritePin(GROUP2_EN_PORT, GROUP2_EN_PIN, GPIO_PIN_SET);
    }
}
