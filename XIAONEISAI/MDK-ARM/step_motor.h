#ifndef __STEP_MOTOR_H
#define __STEP_MOTOR_H

#include "stm32f1xx_hal.h"
#include "tim.h"  // CubeMX生成的TIM8初始化头文件

// -------------------------- 硬件参数配置 --------------------------
#define STEP_ANGLE       1.8f    // 电机固有步距角（默认1.8°）
#define MICROSTEP        1       // 驱动器细分倍数（需与拨码一致）
#define LIFT_ANGLE       30.0f   // 每次接收数据时的升降角度（可按需修改）
// 方向控制GPIO（固定PF0~PF3）
#define DIR_G1M1_PORT    GPIOF   // 组1电机1方向
#define DIR_G1M1_PIN     GPIO_PIN_0
#define DIR_G2M1_PORT    GPIOF   // 组1电机2方向（与电机1同方向）
#define DIR_G2M1_PIN     GPIO_PIN_1
#define DIR_G2M2_PORT    GPIOF   // 组2电机1方向
#define DIR_G2M2_PIN     GPIO_PIN_2
#define DIR_G1M2_PORT    GPIOF   // 组2电机2方向（与电机1同方向）
#define DIR_G1M2_PIN     GPIO_PIN_3

// 控制数据类型（对应三种控制需求）
typedef enum {
    CTRL_DATA_GROUP1 = 1,  // 第一种数据：控制组1
    CTRL_DATA_GROUP2 = 2,  // 第二种数据：控制组2
    CTRL_DATA_ALL    = 3   // 第三种数据：控制所有电机
} CtrlDataType_TypeDef;

// 电机动作方向
typedef enum {
    MOTOR_DIR_UP = 1,
    MOTOR_DIR_DOWN = 0
} MotorDir_TypeDef;

// 电机运行状态
typedef enum {
    MOTOR_STATE_STOP = 0,
    MOTOR_STATE_RUNNING = 1
} MotorState_TypeDef;

// 电机组句柄（含方向记忆）
typedef struct {
    uint32_t target_pulse;   // 目标脉冲数
    uint32_t curr_pulse1;    // 组内电机1当前脉冲
    uint32_t curr_pulse2;    // 组内电机2当前脉冲
    MotorDir_TypeDef last_dir;// 上次动作方向（记忆用）
    MotorState_TypeDef state;// 运行状态
} MotorGroup_HandleTypeDef;

// 外部函数声明
void Motor_Init(void);  // 电机驱动初始化
// 核心功能：接收控制数据，执行对应电机组升降（再次接收时反向）
void Motor_HandleCtrlData(CtrlDataType_TypeDef ctrl_data);
MotorState_TypeDef Motor_GetGroupState(CtrlDataType_TypeDef ctrl_data);  // 获取状态
void Motor_ForceStopAll(void);  // 强制停止所有电机

// TIM8中断回调（内部使用，需在stm32f1xx_it.c中调用）
void Motor_TIM8_IRQCallback(void);

#endif // __STEP_MOTOR_H
