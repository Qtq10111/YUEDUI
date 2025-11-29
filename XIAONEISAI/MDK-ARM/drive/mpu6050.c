#include "mpu6050.h"

// -------------------------- 静态工具函数（内部使用） --------------------------
/**
 * @brief  向MPU6050单个寄存器写数据
 * @param  hi2c：I2C外设句柄（如&hi2c1）
 * @param  reg_addr：寄存器地址
 * @param  data：待写入的字节数据
 * @retval HAL_StatusTypeDef：HAL_OK=成功，其他=失败
 */
static HAL_StatusTypeDef MPU6050_WriteSingleReg(I2C_HandleTypeDef *hi2c, uint8_t reg_addr, uint8_t data) {
    uint8_t tx_buf[2] = {reg_addr, data};
    return HAL_I2C_Master_Transmit(hi2c, MPU6050_I2C_ADDR, tx_buf, 2, 100); // 100ms超时
}

/**
 * @brief  从MPU6050单个寄存器读数据
 * @param  hi2c：I2C外设句柄
 * @param  reg_addr：寄存器地址
 * @param  data：存储读取数据的指针
 * @retval HAL_StatusTypeDef：HAL_OK=成功，其他=失败
 */
static HAL_StatusTypeDef MPU6050_ReadSingleReg(I2C_HandleTypeDef *hi2c, uint8_t reg_addr, uint8_t *data) {
    // 先发送寄存器地址，再接收数据
    if (HAL_I2C_Master_Transmit(hi2c, MPU6050_I2C_ADDR, &reg_addr, 1, 100) != HAL_OK) {
        return HAL_ERROR;
    }
    return HAL_I2C_Master_Receive(hi2c, MPU6050_I2C_ADDR, data, 1, 100);
}

/**
 * @brief  获取陀螺仪转换系数（原始数据→°/s）
 * @param  range：陀螺仪量程（MPU6050_GyroRange枚举）
 * @retval float：转换系数（原始值 ÷ 系数 = 实际角速度）
 */
static float MPU6050_GetGyroScale(MPU6050_GyroRange range) {
    switch (range) {
        case GYRO_RANGE_250DPS:  return 131.0f;
        case GYRO_RANGE_500DPS:  return 65.5f;
        case GYRO_RANGE_1000DPS: return 32.8f;
        case GYRO_RANGE_2000DPS: return 16.4f;
        default: return 131.0f; // 默认±250DPS
    }
}

/**
 * @brief  获取加速度计转换系数（原始数据→g）
 * @param  range：加速度计量程（MPU6050_AccelRange枚举）
 * @retval float：转换系数（原始值 ÷ 系数 = 实际加速度）
 */
static float MPU6050_GetAccelScale(MPU6050_AccelRange range) {
    switch (range) {
        case ACCEL_RANGE_2G:  return 16384.0f;
        case ACCEL_RANGE_4G:  return 8192.0f;
        case ACCEL_RANGE_8G:  return 4096.0f;
        case ACCEL_RANGE_16G: return 2048.0f;
        default: return 16384.0f; // 默认±2g
    }
}

// -------------------------- 外部接口函数（与头文件声明对应） --------------------------
/**
 * @brief  MPU6050初始化（默认：±250DPS陀螺仪，±2g加速度计，1000Hz采样率）
 * @param  hi2c：I2C外设句柄
 * @param  calib：校准参数结构体指针（存储初始化后的量程）
 * @retval HAL_StatusTypeDef：HAL_OK=成功，其他=失败
 */
HAL_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c, MPU6050_CalibData *calib) {
    uint8_t device_id;

    // 1. 唤醒MPU6050（默认上电为睡眠模式，写0x00取消睡眠）
    if (MPU6050_WriteSingleReg(hi2c, MPU6050_REG_PWR_MGMT1, 0x00) != HAL_OK) {
        return HAL_ERROR;
    }
    HAL_Delay(10); // 等待模块稳定

    // 2. 验证设备ID（确保硬件连接正确，ID固定为0x68）
    if (MPU6050_ReadSingleReg(hi2c, 0x75, &device_id) != HAL_OK) {
        return HAL_ERROR;
    }
    if (device_id != MPU6050_DEVICE_ID) {
        return HAL_ERROR; // ID不匹配，硬件异常
    }

    // 3. 配置采样率：采样率 = 1000Hz / (1 + 分频值)，分频0→1000Hz
    if (MPU6050_WriteSingleReg(hi2c, MPU6050_REG_SMPLRT_DIV, 0x00) != HAL_OK) {
        return HAL_ERROR;
    }

    // 4. 配置低通滤波器：带宽256Hz（减少噪声，不影响响应速度）
    if (MPU6050_WriteSingleReg(hi2c, MPU6050_REG_CONFIG, 0x00) != HAL_OK) {
        return HAL_ERROR;
    }

    // 5. 配置陀螺仪量程（默认±250DPS，存入校准结构体）
    calib->gyro_range = GYRO_RANGE_250DPS;
    if (MPU6050_WriteSingleReg(hi2c, MPU6050_REG_GYRO_CONFIG, calib->gyro_range) != HAL_OK) {
        return HAL_ERROR;
    }

    // 6. 配置加速度计量程（默认±2g，存入校准结构体）
    calib->accel_range = ACCEL_RANGE_2G;
    if (MPU6050_WriteSingleReg(hi2c, MPU6050_REG_ACCEL_CONFIG, calib->accel_range) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/**
 * @brief  MPU6050校准（需水平静止放置，计算零偏以消除温漂）
 * @param  hi2c：I2C外设句柄
 * @param  calib：校准参数结构体指针（存储计算出的零偏值）
 */
void MPU6050_Calibrate(I2C_HandleTypeDef *hi2c, MPU6050_CalibData *calib) {
    int32_t accel_x_sum = 0, accel_y_sum = 0, accel_z_sum = 0;
    int32_t gyro_x_sum = 0, gyro_y_sum = 0, gyro_z_sum = 0;
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;
    const uint16_t CALIB_SAMPLE_NUM = 1000; // 校准采样次数（越多越精准）
    uint8_t rx_buf[14]; // 存储14个寄存器数据（6轴×2字节 + 温度×2字节）

    // 循环采样，累加原始数据（消除随机噪声）
    for (uint16_t i = 0; i < CALIB_SAMPLE_NUM; i++) {
        // 发送起始寄存器地址（0x3B，加速度计X轴高位）
				uint8_t reg_addr = MPU6050_REG_ACCEL_X_H;
        if (HAL_I2C_Master_Transmit(hi2c, MPU6050_I2C_ADDR, (uint8_t*)&reg_addr, 1, 100) != HAL_OK) {
            return;
        }
        // 读取14个字节数据（0x3B~0x48）
        if (HAL_I2C_Master_Receive(hi2c, MPU6050_I2C_ADDR, rx_buf, 14, 100) != HAL_OK) {
            return;
        }

        // 转换加速度计原始数据（16位有符号数：高8位<<8 + 低8位）
        accel_x = (rx_buf[0] << 8) | rx_buf[1];
        accel_y = (rx_buf[2] << 8) | rx_buf[3];
        accel_z = (rx_buf[4] << 8) | rx_buf[5];

        // 转换陀螺仪原始数据（跳过温度数据：rx_buf[6]~rx_buf[7]）
        gyro_x = (rx_buf[8] << 8) | rx_buf[9];
        gyro_y = (rx_buf[10] << 8) | rx_buf[11];
        gyro_z = (rx_buf[12] << 8) | rx_buf[13];

        // 累加数据
        accel_x_sum += accel_x;
        accel_y_sum += accel_y;
        accel_z_sum += accel_z;
        gyro_x_sum += gyro_x;
        gyro_y_sum += gyro_y;
        gyro_z_sum += gyro_z;

        HAL_Delay(1); // 间隔1ms，避免数据读取重叠
    }

    // 计算零偏值（平均值 = 总累加值 / 采样次数）
    calib->accel_x_offset = accel_x_sum / CALIB_SAMPLE_NUM;
    calib->accel_y_offset = accel_y_sum / CALIB_SAMPLE_NUM;
    // Z轴静止时受重力（约1g），需减去1g对应的原始值（16384）
    calib->accel_z_offset = accel_z_sum / CALIB_SAMPLE_NUM - 16384;
    calib->gyro_x_offset = gyro_x_sum / CALIB_SAMPLE_NUM;
    calib->gyro_y_offset = gyro_y_sum / CALIB_SAMPLE_NUM;
    calib->gyro_z_offset = gyro_z_sum / CALIB_SAMPLE_NUM;
}

/**
 * @brief  读取MPU6050传感器数据（已校准，转换为物理单位）
 * @param  hi2c：I2C外设句柄
 * @param  calib：校准参数结构体指针（用于零偏补偿）
 * @param  sensor_data：输出数据结构体指针（存储最终测量值）
 * @retval HAL_StatusTypeDef：HAL_OK=成功，其他=失败
 */
HAL_StatusTypeDef MPU6050_ReadSensorData(I2C_HandleTypeDef *hi2c, MPU6050_CalibData *calib, MPU6050_SensorData *sensor_data) {
    uint8_t rx_buf[14];
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;
    // 从校准结构体获取当前量程的转换系数
    float accel_scale = MPU6050_GetAccelScale(calib->accel_range);
    float gyro_scale = MPU6050_GetGyroScale(calib->gyro_range);

    // 1. 读取14个字节数据（0x3B~0x48）
		uint8_t reg_addr = MPU6050_REG_ACCEL_X_H;
    if (HAL_I2C_Master_Transmit(hi2c, MPU6050_I2C_ADDR, (uint8_t*)&reg_addr, 1, 100) != HAL_OK) {
        return HAL_ERROR;
    }
    if (HAL_I2C_Master_Receive(hi2c, MPU6050_I2C_ADDR, rx_buf, 14, 100) != HAL_OK) {
        return HAL_ERROR;
    }

    // 2. 转换原始数据（16位有符号数）
    accel_x = (rx_buf[0] << 8) | rx_buf[1];
    accel_y = (rx_buf[2] << 8) | rx_buf[3];
    accel_z = (rx_buf[4] << 8) | rx_buf[5];
    gyro_x = (rx_buf[8] << 8) | rx_buf[9];
    gyro_y = (rx_buf[10] << 8) | rx_buf[11];
    gyro_z = (rx_buf[12] << 8) | rx_buf[13];

    // 3. 零偏补偿 + 转换为物理单位（加速度：g，角速度：°/s）
    sensor_data->accel_x = (accel_x - calib->accel_x_offset) / accel_scale;
    sensor_data->accel_y = (accel_y - calib->accel_y_offset) / accel_scale;
    sensor_data->accel_z = (accel_z - calib->accel_z_offset) / accel_scale;
    sensor_data->gyro_x = (gyro_x - calib->gyro_x_offset) / gyro_scale;
    sensor_data->gyro_y = (gyro_y - calib->gyro_y_offset) / gyro_scale;
    sensor_data->gyro_z = (gyro_z - calib->gyro_z_offset) / gyro_scale;

    return HAL_OK;
}
