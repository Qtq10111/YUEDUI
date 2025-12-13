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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "move.h"
#include <stdio.h>
#include "encounter.h"
#include "pid.h"
#include "motor.h"

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
void BRAKE(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t recieveData[7];
uint32_t time_out=0;   //超时定时
uint8_t current_move;   //运动or停止状态
uint8_t move_state;   //运动方向状态
float target_speed_forward;   //三方向速度
float target_speed_left;
float target_speed_rotate;
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
HAL_TIM_Encoder_Start(&htim1,TIM_CHANNEL_ALL);
HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);
HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_ALL);
BRAKE();
time_out=HAL_GetTick();
HAL_UART_Receive_IT(&huart1,recieveData,sizeof(recieveData));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(HAL_GetTick() - time_out > 100)   //超时强制刹车
		{
			BRAKE();
			current_move=0;
				move_state=0;
				target_speed_forward=0;
				target_speed_left=0;
				target_speed_rotate=0;
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
	if(huart == &huart1)
	{
		HAL_UART_Transmit(&huart2,recieveData,sizeof(recieveData),HAL_MAX_DELAY);//板间通信
		switch(recieveData[4])
		{
			case 87:
			{
				current_move = 1;
				move_state=1;
				target_speed_forward=50.0f;
				target_speed_left=0;
				target_speed_rotate=0;
				time_out=HAL_GetTick();//重置超时定时器
				break;
			}
			case 83:
			{
				current_move = 1;
				move_state=2;
				target_speed_forward=-50.0f;
				target_speed_left=0;
				target_speed_rotate=0;
				time_out=HAL_GetTick();
				break;
			}
			case 65:
			{
				current_move = 1;
				move_state=3;
				target_speed_forward=0;
				target_speed_left=50.0;
				target_speed_rotate=0;
				time_out=HAL_GetTick();
				break;
			}
			case 68:
			{
				current_move = 1;
				move_state=4;
				target_speed_forward=0;
				target_speed_left=-50.0f;
				target_speed_rotate=0;
				time_out=HAL_GetTick();
				break;
			}
			case 81:
			{
				current_move =1;
				move_state=5;
				target_speed_forward=0;
				target_speed_left=0;
				target_speed_rotate=30.0f;
				time_out=HAL_GetTick();
				break;
			}
			case 69:
			{
				current_move=1;
				move_state=6;
				target_speed_forward=0;
				target_speed_left=0;
				target_speed_rotate=-30.0f;
				time_out=HAL_GetTick();
				break;
			}
			default:
			{
				current_move=0;
				move_state=0;
				target_speed_forward=0;
				target_speed_left=0;
				target_speed_rotate=0;
			}
		}
	}
	HAL_UART_Receive_IT(&huart1,recieveData,sizeof(recieveData));
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim6)
	{
		if(move_state != 0)
		{
			float target_LF = target_speed_forward-target_speed_left-target_speed_rotate;//四个电机的速度解算
			float target_RF = target_speed_forward+target_speed_left+target_speed_rotate;
			float target_LB = target_speed_forward+target_speed_left-target_speed_rotate;
			float target_RB = target_speed_forward-target_speed_left+target_speed_rotate;
			motor_speed_set(target_LF,target_RF,target_LB,target_RB);
			app_motor_run();
			switch(move_state)
			{
				case 1:
				{
					FORWARD_MOVE();
				}
				case 2:
				{
					BACK_MOVE();
				}
				case 3:
				{
					LEFT_MOVE();
				}
				case 4:
				{
					RIGHT_MOVE();
				}
				case 5:
				{
					LEFT_ROTAY();
				}
				case 6:
				{
					RIGHT_ROTAY();
				}
			}
		}
		else//若运动状态为停止，跳过PID强制刹车
		{
			BRAKE();
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

#ifdef  USE_FULL_ASSERT
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
