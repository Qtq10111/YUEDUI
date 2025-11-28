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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

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
uint8_t recieveDeta[1];
uint32_t timeout = 0;
char current_move = 'O';
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
  /* USER CODE BEGIN 2 */
	MPU6050_CalibData mpu_calib;
	if(MPU6050_Init(&hi2c1, &mpu_calib) != HAL_OK)
	{
		while(1);
	}
	HAL_Delay(1000);
	MPU6050_Calibrate(&hi2c1, &mpu_calib);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	Brake();
	HAL_UART_Receive_IT(&huart1, recieveDeta, 1);
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
	switch(recieveDeta[0])
		{
			case 'W':
				current_move = 'W';
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			case 'S':
				current_move = 'S';
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			case 'A':
				current_move = 'A';
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			case 'D':
				current_move = 'D';
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			case 'Q':
				current_move = 'Q';
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			case 'E':
				current_move = 'E';
				initial_yaw = current_yaw;
				timeout = HAL_GetTick();
				break;
			default:
				current_move = 'O';
				Brake();
		}
		HAL_UART_Receive_IT(&huart1, recieveDeta, 1);
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


    if (current_move != '0') 
		{
      float target_yaw = initial_yaw;
      float pid_comp = Yaw_PID(target_yaw, current_yaw);
      int left_speed = base_speed + pid_comp;
      int right_speed = base_speed - pid_comp;


      switch(current_move) 
			{
        case 'W':
          LFMotor(left_speed); RFMotor(right_speed);
          LBMotor(left_speed); RBMotor(right_speed);
          break;
        case 'S':
          LFMotor(-left_speed); RFMotor(-right_speed);
          LBMotor(-left_speed); RBMotor(-right_speed);
          break;
        case 'A':
          LFMotor(-left_speed); RFMotor(right_speed);
          LBMotor(left_speed); RBMotor(-right_speed);
          break;
        case 'D':
          LFMotor(left_speed); RFMotor(-right_speed);
          LBMotor(-left_speed); RBMotor(right_speed);
          break;
        case 'Q':
          LFMotor(-left_speed); RFMotor(left_speed);
          LBMotor(-left_speed); RBMotor(left_speed);
          break;
        case 'E':
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
