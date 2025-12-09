#include "pid.h"
#include "stdint.h"


// 4个电机的PID结构体（对应左前/右前/左后/右后）
PID_Info pid_LF = {2.0f, 0.1f, 0.05f, 0, 0, 0, 0, 0, 0, 100, -100};
PID_Info pid_RF = {2.0f, 0.1f, 0.05f, 0, 0, 0, 0, 0, 0, 100, -100};
PID_Info pid_LB = {2.0f, 0.1f, 0.05f, 0, 0, 0, 0, 0, 0, 100, -100};
PID_Info pid_RB = {2.0f, 0.1f, 0.05f, 0, 0, 0, 0, 0, 0, 100, -100};


// PID参数初始化（可以修改kp/ki/kd和限幅）
void PID_Init(void) {
    // 左前电机PID
    pid_LF.kp = 2.0f;
    pid_LF.ki = 0.1f;
    pid_LF.kd = 0.05f;
    pid_LF.max_out = 100;
    pid_LF.min_out = -100;
    
    // 右前电机PID
    pid_RF.kp = 2.0f;
    pid_RF.ki = 0.1f;
    pid_RF.kd = 0.05f;
    pid_RF.max_out = 100;
    pid_RF.min_out = -100;
    
    // 左后电机PID
    pid_LB.kp = 2.0f;
    pid_LB.ki = 0.1f;
    pid_LB.kd = 0.05f;
    pid_LB.max_out = 100;
    pid_LB.min_out = -100;
    
    // 右后电机PID
    pid_RB.kp = 2.0f;
    pid_RB.ki = 0.1f;
    pid_RB.kd = 0.05f;
    pid_RB.max_out = 100;
    pid_RB.min_out = -100;
}


// 单电机PID计算（输入当前值，输出控制量）
float PID_Calc(PID_Info *pid, float current) {
    // 1. 计算误差
    pid->error = pid->target - current;
    
    // 2. 计算积分（限制积分饱和）
    pid->integral += pid->error;
    if (pid->integral > pid->max_out / pid->ki) {
        pid->integral = pid->max_out / pid->ki;
    } else if (pid->integral < pid->min_out / pid->ki) {
        pid->integral = pid->min_out / pid->ki;
    }
    
    // 3. 计算微分
    pid->derivative = pid->error - pid->last_error;
    
    // 4. 计算PID输出
    pid->output = pid->kp * pid->error + pid->ki * pid->integral + pid->kd * pid->derivative;
    
    // 5. 输出限幅
    if (pid->output > pid->max_out) {
        pid->output = pid->max_out;
    } else if (pid->output < pid->min_out) {
        pid->output = pid->min_out;
    }
    
    // 6. 保存当前误差
    pid->last_error = pid->error;
    
    return pid->output;
}


// 设置电机目标速度
void PID_SetTarget(uint8_t motor_num, float target) {
    switch(motor_num) {
        case 1: pid_LF.target = target; break; // 左前
        case 2: pid_RF.target = target; break; // 右前
        case 3: pid_LB.target = target; break; // 左后
        case 4: pid_RB.target = target; break; // 右后
        default: break;
    }
}
