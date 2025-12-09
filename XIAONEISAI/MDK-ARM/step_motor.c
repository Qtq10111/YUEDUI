#include "step_motor.h"

#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

// 全局电机组句柄（静态，记忆上次方向）
static MotorGroup_HandleTypeDef Group1 = {0, 0, 0, MOTOR_DIR_UP, MOTOR_STATE_STOP};  // 组1
static MotorGroup_HandleTypeDef Group2 = {0, 0, 0, MOTOR_DIR_UP, MOTOR_STATE_STOP};  // 组2

/**
  * @brief  计算升降角度对应的目标脉冲数
  * @param  angle：升降角度（单位°）
  * @retval 目标脉冲数
  */
static uint32_t Motor_CalcTargetPulse(float angle)
{
    if (angle <= 0.0f) return 0;
    return (uint32_t)((angle / STEP_ANGLE) * MICROSTEP + 0.5f);  // 四舍五入取整
}

/**
  * @brief  切换电机组方向（上次UP→本次DOWN，上次DOWN→本次UP）
  * @param  group：待切换方向的电机组指针
  * @retval 切换后的方向
  */
static MotorDir_TypeDef Motor_ToggleDir(MotorGroup_HandleTypeDef *group)
{
    group->last_dir = (group->last_dir == MOTOR_DIR_UP) ? MOTOR_DIR_DOWN : MOTOR_DIR_UP;
    return group->last_dir;
}

/**
  * @brief  设置电机方向GPIO电平
  * @param  ctrl_data：控制数据类型，dir：目标方向
  * @retval 无
  */
static void Motor_SetDirGpio(CtrlDataType_TypeDef ctrl_data, MotorDir_TypeDef dir)
{
    GPIO_PinState level = (dir == MOTOR_DIR_UP) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    switch(ctrl_data)
    {
        case CTRL_DATA_GROUP1:  // 仅设置组1方向
            HAL_GPIO_WritePin(DIR_G1M1_PORT, DIR_G1M1_PIN, level);
            HAL_GPIO_WritePin(DIR_G2M1_PORT, DIR_G2M1_PIN, level);
            break;
        case CTRL_DATA_GROUP2:  // 仅设置组2方向
            HAL_GPIO_WritePin(DIR_G2M2_PORT, DIR_G2M2_PIN, level);
            HAL_GPIO_WritePin(DIR_G1M2_PORT, DIR_G1M2_PIN, level);
            break;
        case CTRL_DATA_ALL:     // 设置所有电机方向
            HAL_GPIO_WritePin(DIR_G1M1_PORT, DIR_G1M1_PIN, level);
            HAL_GPIO_WritePin(DIR_G2M1_PORT, DIR_G2M1_PIN, level);
            HAL_GPIO_WritePin(DIR_G2M2_PORT, DIR_G2M2_PIN, level);
            HAL_GPIO_WritePin(DIR_G1M2_PORT, DIR_G1M2_PIN, level);
            break;
        default: break;
    }
}

/**
  * @brief  启动指定电机组的PWM输出
  * @param  ctrl_data：控制数据类型
  * @retval 无
  */
static void Motor_StartPWM(CtrlDataType_TypeDef ctrl_data)
{
    switch(ctrl_data)
    {
        case CTRL_DATA_GROUP1:
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);  // 组1电机1（CH1）
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);  // 组1电机2（CH2）
            break;
        case CTRL_DATA_GROUP2:
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);  // 组2电机1（CH3）
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);  // 组2电机2（CH4）
            break;
        case CTRL_DATA_ALL:
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
            HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
            break;
        default: break;
    }
}

/**
  * @brief  停止指定电机组的PWM输出
  * @param  ctrl_data：控制数据类型
  * @retval 无
  */
static void Motor_StopPWM(CtrlDataType_TypeDef ctrl_data)
{
    switch(ctrl_data)
    {
        case CTRL_DATA_GROUP1:
            HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);
            HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_2);
            Group1.state = MOTOR_STATE_STOP;
            break;
        case CTRL_DATA_GROUP2:
            HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_3);
            HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_4);
            Group2.state = MOTOR_STATE_STOP;
            break;
        case CTRL_DATA_ALL:
            HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_1);
            HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_2);
            HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_3);
            HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_4);
            Group1.state = MOTOR_STATE_STOP;
            Group2.state = MOTOR_STATE_STOP;
            break;
        default: break;
    }
}

/**
  * @brief  电机驱动初始化
  * @param  无
  * @retval 无
  */
void Motor_Init(void)
{
    // 初始化方向GPIO为低电平（默认下降方向）
    HAL_GPIO_WritePin(DIR_G1M1_PORT, DIR_G1M1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DIR_G2M1_PORT, DIR_G2M1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DIR_G2M2_PORT, DIR_G2M2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DIR_G1M2_PORT, DIR_G1M2_PIN, GPIO_PIN_RESET);
    
    // 1. 使能 TIM8 内部的 CC1~CC4 中断
    TIM8->DIER |= TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE;
    // 2. 使能 NVIC 层面的 TIM8 中断
    HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
    HAL_NVIC_SetPriority(TIM8_CC_IRQn, 2, 0);
}

/**
  * @brief  接收控制数据，执行电机升降（再次接收反向）
  * @param  ctrl_data：控制数据类型（1=组1，2=组2，3=所有）
  * @retval 无
  */
void Motor_HandleCtrlData(CtrlDataType_TypeDef ctrl_data)
{
    uint32_t target_pulse = Motor_CalcTargetPulse(LIFT_ANGLE);
    if (target_pulse == 0) return;
    
    // 先停止当前电机组（防止运行中切换）
    Motor_StopPWM(ctrl_data);
    
    // 切换方向并设置GPIO
    MotorDir_TypeDef curr_dir;
    switch(ctrl_data)
    {
        case CTRL_DATA_GROUP1:
            curr_dir = Motor_ToggleDir(&Group1);  // 切换组1方向
            Group1.target_pulse = target_pulse;
            Group1.curr_pulse1 = 0;
            Group1.curr_pulse2 = 0;
            Group1.state = MOTOR_STATE_RUNNING;
            break;
        case CTRL_DATA_GROUP2:
            curr_dir = Motor_ToggleDir(&Group2);  // 切换组2方向
            Group2.target_pulse = target_pulse;
            Group2.curr_pulse1 = 0;
            Group2.curr_pulse2 = 0;
            Group2.state = MOTOR_STATE_RUNNING;
            break;
        case CTRL_DATA_ALL:
            // 所有电机：组1和组2同步切换方向（保持一致）
            curr_dir = Motor_ToggleDir(&Group1);
            Group2.last_dir = curr_dir;  // 组2跟随组1方向
            // 统一设置两组参数
            Group1.target_pulse = target_pulse;
            Group1.curr_pulse1 = 0;
            Group1.curr_pulse2 = 0;
            Group1.state = MOTOR_STATE_RUNNING;
            
            Group2.target_pulse = target_pulse;
            Group2.curr_pulse1 = 0;
            Group2.curr_pulse2 = 0;
            Group2.state = MOTOR_STATE_RUNNING;
            break;
        default: return;
    }
    
    // 设置方向GPIO并启动PWM
    Motor_SetDirGpio(ctrl_data, curr_dir);
    Motor_StartPWM(ctrl_data);
}

/**
  * @brief  获取指定控制数据对应的电机状态
  * @param  ctrl_data：控制数据类型
  * @retval 电机状态（停止/运行）
  */
MotorState_TypeDef Motor_GetGroupState(CtrlDataType_TypeDef ctrl_data)
{
    switch(ctrl_data)
    {
        case CTRL_DATA_GROUP1: return Group1.state;
        case CTRL_DATA_GROUP2: return Group2.state;
        case CTRL_DATA_ALL:    // 所有电机：两组均停止才判定为停止
            return (Group1.state == MOTOR_STATE_STOP && Group2.state == MOTOR_STATE_STOP) ? MOTOR_STATE_STOP : MOTOR_STATE_RUNNING;
        default: return MOTOR_STATE_STOP;
    }
}

/**
  * @brief  强制停止所有电机
  * @param  无
  * @retval 无
  */
void Motor_ForceStopAll(void)
{
    Motor_StopPWM(CTRL_DATA_ALL);
}

/**
  * @brief  TIM8中断回调（脉冲计数+同步停止）
  * @param  无
  * @retval 无
  */
void Motor_TIM8_IRQCallback(void)
{
    // -------------------------- 组1（CH1+CH2）脉冲处理 --------------------------
    if (__HAL_TIM_GET_FLAG(&htim8, TIM_FLAG_CC1) && Group1.state == MOTOR_STATE_RUNNING)
    {
        __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_CC1);
        Group1.curr_pulse1++;
        // 组1双电机均达目标脉冲→停止
        if (Group1.curr_pulse1 >= Group1.target_pulse && Group1.curr_pulse2 >= Group1.target_pulse)
        {
            Motor_StopPWM(CTRL_DATA_GROUP1);
        }
    }
    if (__HAL_TIM_GET_FLAG(&htim8, TIM_FLAG_CC2) && Group1.state == MOTOR_STATE_RUNNING)
    {
        __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_CC2);
        Group1.curr_pulse2++;
    }

    // -------------------------- 组2（CH3+CH4）脉冲处理 --------------------------
    if (__HAL_TIM_GET_FLAG(&htim8, TIM_FLAG_CC3) && Group2.state == MOTOR_STATE_RUNNING)
    {
        __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_CC3);
        Group2.curr_pulse1++;
        // 组2双电机均达目标脉冲→停止
        if (Group2.curr_pulse1 >= Group2.target_pulse && Group2.curr_pulse2 >= Group2.target_pulse)
        {
            Motor_StopPWM(CTRL_DATA_GROUP2);
        }
    }
    if (__HAL_TIM_GET_FLAG(&htim8, TIM_FLAG_CC4) && Group2.state == MOTOR_STATE_RUNNING)
    {
        __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_CC4);
        Group2.curr_pulse2++;
    }
}
