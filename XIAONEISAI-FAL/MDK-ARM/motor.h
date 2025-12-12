#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"

#define PI					3.14159265358979f

#define PID_RATE 72 // PID频率，(进入PID函数的频率)

// 电机编码器分辨率
#define WHEEL_RESOLUTION 2340.0f // 26极磁编码器分辨率,开关霍尔：13*2*90（减速比）= 2340

// 小车参数
#define MEC_WHEEL_BASE 0.287f													// 轮距，左右轮的距离
#define MEC_ACLE_BASE 0.242f													// 轴距，前后轮的距离
#define MEC_WHEEL_DIAMETER 0.08f												// 轮子直径
#define MEC_WHEEL_SCALE (PI * MEC_WHEEL_DIAMETER * PID_RATE / WHEEL_RESOLUTION) // 轮子速度m/s与编码器转换系数

// 机器人速度限制
#define R_VX_LIMIT 1500 // X轴速度限值 m/s*1000
#define R_VY_LIMIT 1200 // Y轴速度限值 m/s*1000
#define R_VW_LIMIT 6280 // W旋转角速度限值 rad/s*1000

//void app_motor_init(void);
void app_motor_run(void);

void motor_speed_set(float A,float B,float C,float D);

#endif
