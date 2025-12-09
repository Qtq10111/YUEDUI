#ifndef PID_H
#define PID_H

#include "stdint.h"

// 外部声明PID结构体（供其他文件调用）
typedef struct {
    float kp;
    float ki;
    float kd;
    float target;
    float error;
    float last_error;
    float integral;
    float derivative;
    float output;
    float max_out;
    float min_out;
} PID_Info;

// 函数声明
void PID_Init(void);
float PID_Calc(PID_Info *pid, float current);
void PID_SetTarget(uint8_t motor_num, float target);

extern PID_Info pid_LF;
extern PID_Info pid_RF;
extern PID_Info pid_LB;
extern PID_Info pid_RB;

#endif
