/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "move.h"
#include "mpu6050.h"
#include "Catch.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CATCH_MOVE 500

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LFMotor(int8_t speed);
void RFMotor(int8_t speed);
void LBMotor(int8_t speed);
void RBMotor(int8_t speed);
void Brake(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t recieveData[7];
uint8_t Catchsg90 ;
uint32_t timeout = 0;
uint8_t current_move ;
float initial_yaw = 0.0f;
int base_speed = 50;
MPU6050_CalibData mpu_calib;
float current_yaw = 0.0f;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	MPU6050_CalibData mpu_calib;
	if(MPU6050_Init(&hi2c1, &mpu_calib) != HAL_OK)
	{
		while(1);
	}
	HAL_Delay(1000);
	MPU6050_Calibrate(&hi2c1, &mpu_calib);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//时钟二pwm初始化（底盘）
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//时钟一pwm初始化（舵机）
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);//时钟四pwm初始化（丝杆）
	Brake();//设置初始状态为刹停
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,CATCH_MOVE);
	__HAL_TIM_SET_AUTORELOAD(&htim4,2000-1);
	HAL_UART_Receive_IT(&huart1, recieveData, sizeof(recieveData));
	timeout = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(HAL_GetTick() - timeout > 100)
		{
			Brake();
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	Catch_hand(recieveData[1]); //抓取
	
	Catch_move(recieveData[2],recieveData[3]); //爪移动
	
	switch(recieveData[4]) //整车移动
		{
			case 87:
				current_move = 87;
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			case 83:
				current_move = 83;
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			case 65:
				current_move = 65;
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			case 68:
				current_move = 68;
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			case 81:
				current_move = 81;
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			case 69:
				current_move = 69;
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			default:
				current_move = 0;
				Brake();
		}
		HAL_UART_Receive_IT(&huart1, recieveData, sizeof(recieveData));
}

float Yaw_PID(float target_yaw, float current_yaw) 
{
  static float last_error = 0, integral = 0;
  float error = target_yaw - current_yaw;
  
  if (error > 180) error -= 360;
  if (error < -180) error += 360;
  
  float P = 2.5f * error;
  integral += error * 0.01f;
  integral = (integral > 50) ? 50 : integral;
  integral = (integral < -50) ? -50 : integral;
  float I = 0.05f * integral;
  float D = 0.6f * (error - last_error) / 0.01f;
  last_error = error;
  
  return P + I + D;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM3) 
	{
    MPU6050_SensorData mpu_data;
    MPU6050_ReadSensorData(&hi2c1, &mpu_calib, &mpu_data);
    
    static float yaw = 0;
    yaw += mpu_data.gyro_z * 0.01f;
    current_yaw = yaw;


    if (current_move != 0) 
		{
      float target_yaw = initial_yaw;
      float pid_comp = Yaw_PID(target_yaw, current_yaw);
      int left_speed = base_speed + pid_comp;
      int right_speed = base_speed - pid_comp;


      switch(current_move) 
			{
        case 87:
          LFMotor(left_speed); RFMotor(right_speed);
          LBMotor(left_speed); RBMotor(right_speed);
          break;
        case 83:
          LFMotor(-left_speed); RFMotor(-right_speed);
          LBMotor(-left_speed); RBMotor(-right_speed);
          break;
        case 65:
          LFMotor(-left_speed); RFMotor(right_speed);
          LBMotor(left_speed); RBMotor(-right_speed);
          break;
        case 68:
          LFMotor(left_speed); RFMotor(-right_speed);
          LBMotor(-left_speed); RBMotor(right_speed);
          break;
        case 81:
          LFMotor(-left_speed); RFMotor(left_speed);
          LBMotor(-left_speed); RBMotor(left_speed);
          break;
        case 69:
          LFMotor(left_speed); RFMotor(-left_speed);
          LBMotor(left_speed); RBMotor(-left_speed);
          break;
      }
    }
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
