#include "step_motor.h"
#include <math.h>

extern TIM_HandleTypeDef htim1;  // 定时器1句柄（CubeMX生成）

#define LF_DIR_PIN    GPIO_PIN_0
#define LF_DIR_PORT   GPIOA
#define LF_EN_PIN     GPIO_PIN_2
#define LF_EN_PORT    GPIOA

#define RF_DIR_PIN    GPIO_PIN_1
#define RF_DIR_PORT   GPIOA
#define RF_EN_PIN     GPIO_PIN_3
#define RF_EN_PORT    GPIOA

#define LB_DIR_PIN    GPIO_PIN_0
#define LB_DIR_PORT   GPIOB
#define LB_EN_PIN     GPIO_PIN_2
#define LB_EN_PORT    GPIOB

#define RB_DIR_PIN    GPIO_PIN_1
#define RB_DIR_PORT   GPIOB
#define RB_EN_PIN     GPIO_PIN_3
#define RB_EN_PORT    GPIOB

// 全局变量：两组电机的脉冲计数（避免冲突）
static uint32_t g_pulse_cnt1 = 0;
static uint32_t g_pulse_cnt2 = 0;
// 全局变量：目标脉冲数和当前电机组（中断中使用）
static uint32_t g_target_pulse = 0;
static Motor_Group_t g_current_group = MOTOR_GROUP1;

/**
 * @brief  电机初始化：失能A4988，默认方向，开启定时器中断
 * @param  无
 * @retval 无
 */
void Motor_Init(void)
{
    // 1. 方向引脚默认低电平（逆时针）
    HAL_GPIO_WritePin(LF_DIR_PORT, LF_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RF_DIR_PORT, RF_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LB_DIR_PORT, LB_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RB_DIR_PORT, RB_DIR_PIN, GPIO_PIN_RESET);
    
    // 2. EN引脚高电平（A4988低电平使能，高电平失能）
    HAL_GPIO_WritePin(LF_EN_PORT, LF_EN_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RF_EN_PORT, RF_EN_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LB_EN_PORT, LB_EN_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RB_EN_PORT, RB_EN_PIN, GPIO_PIN_SET);
    
    // 3. 停止定时器PWM输出
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_ALL);
    // 4. 开启定时器更新中断（用于脉冲计数）
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
    // 5. 启动定时器（仅计数，不输出PWM）
    HAL_TIM_Base_Start_IT(&htim1);
}

/**
 * @brief  调整电机转速（修改定时器ARR值，改变脉冲频率）
 * @param  group：电机组
 * @param  pulse_freq：目标脉冲频率（Hz，范围100~20000）
 * @retval 无
 */
void Motor_Set_Speed(Motor_Group_t group, uint16_t pulse_freq)
{
    // 定时器时钟为1MHz（CubeMX中PSC=71，72MHz/(71+1)=1MHz）
    uint16_t arr_val = 1000000 / pulse_freq;
    if (arr_val < 1) arr_val = 1; // 防止溢出
    
    // 停止PWM输出后修改参数（避免脉冲异常）
    Motor_Stop(group);
    // 设置ARR值（脉冲频率=1MHz/(ARR+1)，此处简化为1MHz/ARR）
    __HAL_TIM_SET_AUTORELOAD(&htim1, arr_val - 1);
    // 设置PWM占空比为50%（STEP脉冲推荐占空比）
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, arr_val / 2);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, arr_val / 2);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, arr_val / 2);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, arr_val / 2);
    // 重置定时器计数器
    __HAL_TIM_SET_COUNTER(&htim1, 0);
}

/**
 * @brief  紧急停止电机
 * @param  group：电机组
 * @retval 无
 */
void Motor_Stop(Motor_Group_t group)
{
    // 停止对应通道的PWM输出
    if (group == MOTOR_GROUP1) {
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1 | TIM_CHANNEL_2);
        HAL_GPIO_WritePin(LF_EN_PORT, LF_EN_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(RF_EN_PORT, RF_EN_PIN, GPIO_PIN_SET);
    } else {
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3 | TIM_CHANNEL_4);
        HAL_GPIO_WritePin(LB_EN_PORT, LB_EN_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(RB_EN_PORT, RB_EN_PIN, GPIO_PIN_SET);
    }
    // 重置计数
    g_pulse_cnt1 = 0;
    g_pulse_cnt2 = 0;
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
    g_target_pulse = (uint32_t)(fabs(target_angle) / 360.0f * TOTAL_STEP_PER_REV + 0.5f);
    g_current_group = group;
    // 重置计数
    if (group == MOTOR_GROUP1) {
        g_pulse_cnt1 = 0;
    } else {
        g_pulse_cnt2 = 0;
    }

    // 2. 设置旋转方向（对应升降的正反向）
    if (target_angle > 0) {  // 顺时针（升）
        if (group == MOTOR_GROUP1) {
            HAL_GPIO_WritePin(LF_DIR_PORT, LF_DIR_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(RF_DIR_PORT, RF_DIR_PIN, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(LB_DIR_PORT, LB_DIR_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(RB_DIR_PORT, RB_DIR_PIN, GPIO_PIN_SET);
        }
    } else {  // 逆时针（降）
        if (group == MOTOR_GROUP1) {
            HAL_GPIO_WritePin(LF_DIR_PORT, LF_DIR_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(RF_DIR_PORT, RF_DIR_PIN, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(LB_DIR_PORT, LB_DIR_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(RB_DIR_PORT, RB_DIR_PIN, GPIO_PIN_RESET);
        }
    }

    // 3. 使能A4988（低电平有效）
    if (group == MOTOR_GROUP1) {
        HAL_GPIO_WritePin(LF_EN_PORT, LF_EN_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(RF_EN_PORT, RF_EN_PIN, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(LB_EN_PORT, LB_EN_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(RB_EN_PORT, RB_EN_PIN, GPIO_PIN_RESET);
    }

    // 4. 启动定时器PWM输出（STEP脉冲）
    if (group == MOTOR_GROUP1) {
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    } else {
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    }
}

/**
 * @brief  定时器更新中断回调函数（脉冲计数核心逻辑）
 * @param  htim：定时器句柄
 * @retval 无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1) {
        // 根据当前电机组累加计数
        if (g_current_group == MOTOR_GROUP1) {
            g_pulse_cnt1++;
            // 达到目标脉冲数，停止电机
            if (g_pulse_cnt1 >= g_target_pulse) {
                Motor_Stop(MOTOR_GROUP1);
            }
        } else {
            g_pulse_cnt2++;
            if (g_pulse_cnt2 >= g_target_pulse) {
                Motor_Stop(MOTOR_GROUP2);
            }
        }
        // 清除中断标志（HAL库会自动清除，此处可选）
        __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    }
}
