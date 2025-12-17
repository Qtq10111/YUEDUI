#include "step_motor.h"
#include "stdint.h"

// 全局变量：脉冲计数
static uint32_t g_pulse_cnt = 0;

// 引脚宏定义（可根据实际硬件修改）
#define LF_DIR_PORT   GPIOA
#define LF_EN_PORT    GPIOA

#define RF_DIR_PORT   GPIOA
#define RF_EN_PORT    GPIOA

#define LB_DIR_PORT   GPIOB
#define LB_EN_PORT    GPIOB

#define RB_DIR_PORT   GPIOB
#define RB_EN_PORT    GPIOB



/**
 * @brief  电机初始化：失能A4988，默认方向
 * @param  无
 * @retval 无
 */
void Motor_Init(void)
{
    // 1. 方向引脚默认低电平
    HAL_GPIO_WritePin(LF_DIR_PORT, LF_DIR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RF_DIR_PORT, RF_DIR_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LB_DIR_PORT, LB_DIR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RB_DIR_PORT, RB_DIR_Pin, GPIO_PIN_RESET);
    
    // 2. EN引脚高电平（A4988低电平使能，高电平失能）
    HAL_GPIO_WritePin(LF_EN_PORT, LF_EN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RF_EN_PORT, RF_EN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LB_EN_PORT, LB_EN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RB_EN_PORT, RB_EN_Pin, GPIO_PIN_SET);
    
    // 3. 停止定时器脉冲输出
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_ALL);
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
        __HAL_TIM_SET_AUTORELOAD(&htim1, arr_val);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, arr_val/2);
				__HAL_TIM_SET_AUTORELOAD(&htim1, arr_val);
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, arr_val/2);
    } else {
        __HAL_TIM_SET_AUTORELOAD(&htim1, arr_val);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, arr_val/2);
				__HAL_TIM_SET_AUTORELOAD(&htim1, arr_val);
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, arr_val/2);
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
            HAL_GPIO_WritePin(LF_DIR_PORT, LF_DIR_Pin, GPIO_PIN_SET);
						HAL_GPIO_WritePin(RF_DIR_PORT, RF_DIR_Pin, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(LB_DIR_PORT, LB_DIR_Pin, GPIO_PIN_SET);
						HAL_GPIO_WritePin(RB_DIR_PORT, RB_DIR_Pin, GPIO_PIN_SET);
        }
    } else {  // 逆时针
        if (group == MOTOR_GROUP1) {
            HAL_GPIO_WritePin(LF_DIR_PORT, LF_DIR_Pin, GPIO_PIN_RESET);
						HAL_GPIO_WritePin(RF_DIR_PORT, RF_DIR_Pin, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(LB_DIR_PORT, LB_DIR_Pin, GPIO_PIN_RESET);
						HAL_GPIO_WritePin(RB_DIR_PORT, RB_DIR_Pin, GPIO_PIN_RESET);
        }
    }

    // 3. 使能A4988（低电平有效）
    if (group == MOTOR_GROUP1) {
        HAL_GPIO_WritePin(LF_EN_PORT, LF_EN_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(RF_EN_PORT, RF_EN_Pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(LB_EN_PORT, LB_EN_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(RB_EN_PORT, RB_EN_Pin, GPIO_PIN_RESET);
    }

    // 4. 启动定时器PWM输出（STEP脉冲）
    if (group == MOTOR_GROUP1) {
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
				HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    } else {
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
				HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    }

    // 5. 轮询计数脉冲，达到目标后停止
    while (g_pulse_cnt < target_pulse) {
        if (group == MOTOR_GROUP1) {
            // 检测TIM3更新标志（ARR溢出=1个脉冲）
            if (__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) != RESET) {
                __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
                g_pulse_cnt++;
            }
        } else {
            if (__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) != RESET) {
                __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
                g_pulse_cnt++;
            }
        }
    }

    // 6. 停止脉冲输出，失能电机
    if (group == MOTOR_GROUP1) {
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
        HAL_GPIO_WritePin(LF_EN_PORT, LF_EN_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(RF_EN_PORT, RF_EN_Pin, GPIO_PIN_SET);
    } else {
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
			  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
        HAL_GPIO_WritePin(LB_EN_PORT, LB_EN_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(RB_EN_PORT, RB_EN_Pin, GPIO_PIN_SET);
    }
}
