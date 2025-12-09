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
#include "pid.h"
#include "step_motor.h"
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
float current_yaw = 0.0f;
float target_speed_forward = 0;  // 前进/后退速度
float target_speed_left = 0;     // 左移/右移速度
float target_speed_rotate = 0;   // 旋转速度
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
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
		if (HAL_GetTick() - timeout > 100)
    {
        Brake();
        // 超时后把PID目标速度设为0，电机停止
        target_speed_forward = 0;
        target_speed_left = 0;
        target_speed_rotate = 0;
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
    if (huart == &huart1)
    {
			  if(huart->gState == HAL_UART_STATE_READY)
				{
		         HAL_UART_Transmit_IT(&huart2,recieveData,sizeof(recieveData));
	      }
        switch(recieveData[4])
        {
            // 前进（W，ASCII 87）
            case 87:
                current_move = 87;
                initial_yaw = current_yaw;
                timeout = HAL_GetTick();
                // 设置PID目标速度：前进50脉冲/ms
                target_speed_forward = 50.0f;
                target_speed_left = 0;
                target_speed_rotate = 0;
                break;

            // 后退（S，ASCII 83）
            case 83:
                current_move = 83;
                initial_yaw = current_yaw;
                timeout = HAL_GetTick();
                // 设置PID目标速度：后退-50脉冲/ms
                target_speed_forward = -50.0f;
                target_speed_left = 0;
                target_speed_rotate = 0;
                break;

            // 左移（A，ASCII 65）
            case 65:
                current_move = 65;
                initial_yaw = current_yaw;
                timeout = HAL_GetTick();
                // 设置PID目标速度：左移50脉冲/ms
                target_speed_forward = 0;
                target_speed_left = 50.0f;
                target_speed_rotate = 0;
                break;

            // 右移（D，ASCII 68）
            case 68:
                current_move = 68;
                initial_yaw = current_yaw;
                timeout = HAL_GetTick();
                // 设置PID目标速度：右移-50脉冲/ms
                target_speed_forward = 0;
                target_speed_left = -50.0f;
                target_speed_rotate = 0;
                break;

            // （可以加旋转指令，比如Q/E）
            case 81: // 左旋转（Q）
                current_move = 81;
                initial_yaw = current_yaw;
                timeout = HAL_GetTick();
                target_speed_forward = 0;
                target_speed_left = 0;
                target_speed_rotate = 30.0f;
                break;

            case 69: // 右旋转（E）
                current_move = 69;
                initial_yaw = current_yaw;
                timeout = HAL_GetTick();
                target_speed_forward = 0;
                target_speed_left = 0;
                target_speed_rotate = -30.0f;
                break;
						case 'Z'://第一组步进抬升
							  Motor_HandleCtrlData(CTRL_DATA_GROUP1);
						case 'X'://第二组步进抬升
							  Motor_HandleCtrlData(CTRL_DATA_GROUP2);
						case 'C'://第三组步进抬升
							  Motor_HandleCtrlData(CTRL_DATA_ALL);
        }

        // 重新开启UART接收中断
        HAL_UART_Receive_IT(&huart1, recieveData, sizeof(recieveData));
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim6)
    {
        // 1. 计算编码器速度
        Encoder_CalcSpeed(&htim1, 10);  // 左前
        Encoder_CalcSpeed(&htim3, 10);  // 右前
        Encoder_CalcSpeed(&htim4, 10);  // 左后
        Encoder_CalcSpeed(&htim5, 10);  // 右后

        // 2. 速度分解：把“前进/左移/旋转”分解到4个电机
        // 左前电机：前进 - 左移 - 旋转
        float target_LF = target_speed_forward - target_speed_left - target_speed_rotate;
        // 右前电机：前进 + 左移 + 旋转
        float target_RF = target_speed_forward + target_speed_left + target_speed_rotate;
        // 左后电机：前进 + 左移 - 旋转
        float target_LB = target_speed_forward + target_speed_left - target_speed_rotate;
        // 右后电机：前进 - 左移 + 旋转
        float target_RB = target_speed_forward - target_speed_left + target_speed_rotate;

        // 3. 设置PID目标
        PID_SetTarget(1, target_LF);
        PID_SetTarget(2, target_RF);
        PID_SetTarget(3, target_LB);
        PID_SetTarget(4, target_RB);

        // 4. 读取当前速度
        float speed_LF = Encoder_GetSpeed(&htim1);
        float speed_RF = Encoder_GetSpeed(&htim3);
        float speed_LB = Encoder_GetSpeed(&htim4);
        float speed_RB = Encoder_GetSpeed(&htim5);

        // 5. PID计算
        float out_LF = PID_Calc(&pid_LF, speed_LF);
        float out_RF = PID_Calc(&pid_RF, speed_RF);
        float out_LB = PID_Calc(&pid_LB, speed_LB);
        float out_RB = PID_Calc(&pid_RB, speed_RB);

        // 6. 控制电机
        LFMotor((int8_t)out_LF);
        RFMotor((int8_t)out_RF);
        LBMotor((int8_t)out_LB);
        RBMotor((int8_t)out_RB);
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
