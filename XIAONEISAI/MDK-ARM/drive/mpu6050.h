#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f1xx_hal.h"  // 适配STM32F1系列HAL库

// -------------------------- 硬件基础配置 --------------------------
#define MPU6050_I2C_ADDR    0xD0  // I2C设备地址（AD0接GND=0xD0，接VCC=0xD2）
#define MPU6050_DEVICE_ID   0x68  // 芯片固定ID，用于验证连接

// -------------------------- 核心寄存器地址 --------------------------
#define MPU6050_REG_PWR_MGMT1  0x6B  // 电源管理1（控制睡眠/唤醒）
#define MPU6050_REG_SMPLRT_DIV 0x19  // 采样率分频器
#define MPU6050_REG_CONFIG     0x1A  // 低通滤波器配置
#define MPU6050_REG_GYRO_CONFIG 0x1B // 陀螺仪量程配置
#define MPU6050_REG_ACCEL_CONFIG 0x1C// 加速度计量程配置
#define MPU6050_REG_ACCEL_X_H   0x3B // 加速度计X轴高位数据
#define MPU6050_REG_GYRO_X_H    0x43 // 陀螺仪X轴高位数据

// -------------------------- 量程枚举（与驱动逻辑对应） --------------------------
// 陀螺仪量程（决定原始数据→°/s的转换系数）
typedef enum {
    GYRO_RANGE_250DPS = 0x00,  // ±250°/s（系数131.0）
    GYRO_RANGE_500DPS = 0x08,  // ±500°/s（系数65.5）
    GYRO_RANGE_1000DPS = 0x10, // ±1000°/s（系数32.8）
    GYRO_RANGE_2000DPS = 0x18  // ±2000°/s（系数16.4）
} MPU6050_GyroRange;

// 加速度计量程（决定原始数据→g的转换系数）
typedef enum {
    ACCEL_RANGE_2G = 0x00,  // ±2g（系数16384.0）
    ACCEL_RANGE_4G = 0x08,  // ±4g（系数8192.0）
    ACCEL_RANGE_8G = 0x10,  // ±8g（系数4096.0）
    ACCEL_RANGE_16G = 0x18  // ±16g（系数2048.0）
} MPU6050_AccelRange;

// -------------------------- 数据结构体（驱动直接调用） --------------------------
// 校准参数结构体（存储零偏，消除温漂）
typedef struct {
    int16_t accel_x_offset;  // 加速度计X轴零偏
    int16_t accel_y_offset;  // 加速度计Y轴零偏
    int16_t accel_z_offset;  // 加速度计Z轴零偏
    int16_t gyro_x_offset;   // 陀螺仪X轴零偏
    int16_t gyro_y_offset;   // 陀螺仪Y轴零偏
    int16_t gyro_z_offset;   // 陀螺仪Z轴零偏
    MPU6050_GyroRange gyro_range;  // 当前陀螺仪量程
    MPU6050_AccelRange accel_range;// 当前加速度计量程
} MPU6050_CalibData;

// 输出数据结构体（物理单位：加速度g，角速度°/s）
typedef struct {
    float accel_x;  // X轴加速度（g）
    float accel_y;  // Y轴加速度（g）
    float accel_z;  // Z轴加速度（g）
    float gyro_x;   // X轴角速度（°/s）
    float gyro_y;   // Y轴角速度（°/s）
    float gyro_z;   // Z轴角速度（°/s）
} MPU6050_SensorData;

// -------------------------- 函数声明（驱动完全实现） --------------------------
/**
 * @brief  MPU6050初始化（默认：±250DPS陀螺仪，±2g加速度计，1000Hz采样率）
 * @param  hi2c：I2C外设句柄（如&hi2c1）
 * @param  calib：校准参数结构体指针（存储初始化后的量程）
 * @retval HAL_StatusTypeDef：HAL_OK=成功，其他=失败
 */
HAL_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c, MPU6050_CalibData *calib);

/**
 * @brief  MPU6050校准（需水平静止放置，计算零偏）
 * @param  hi2c：I2C外设句柄
 * @param  calib：校准参数结构体指针（存储计算出的零偏）
 */
void MPU6050_Calibrate(I2C_HandleTypeDef *hi2c, MPU6050_CalibData *calib);

/**
 * @brief  读取MPU6050传感器数据（已校准，转换为物理单位）
 * @param  hi2c：I2C外设句柄
 * @param  calib：校准参数结构体指针（用于零偏补偿）
 * @param  sensor_data：输出数据结构体指针（存储最终测量值）
 * @retval HAL_StatusTypeDef：HAL_OK=成功，其他=失败
 */
HAL_StatusTypeDef MPU6050_ReadSensorData(I2C_HandleTypeDef *hi2c, MPU6050_CalibData *calib, MPU6050_SensorData *sensor_data);

#endif
