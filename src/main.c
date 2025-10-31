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
#include <stdbool.h>

#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

typedef struct LED
{
	GPIO_TypeDef *port;
	uint16_t      pin;
} LED;

typedef enum State
{
	STATE_START,
	STATE_MOVE_LEFT,
	STATE_MOVE_RIGHT
} State;

typedef struct Press
{
	uint8_t pressing_this_frame;
	uint8_t pressing_last_frame;
	uint8_t just_pressed;
	uint8_t just_released;
} Press;

void update_press(Press *press, uint8_t pressing)
{
	press->pressing_last_frame = press->pressing_this_frame;
	press->pressing_this_frame = pressing;

	press->just_pressed  =  press->pressing_this_frame && !press->pressing_last_frame;
	press->just_released = !press->pressing_this_frame &&  press->pressing_last_frame;
}

uint8_t is_running   = 0;
uint8_t player1_turn = 0;

uint8_t player1_score = 0;
uint8_t player2_score = 0;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	State state = STATE_START;
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	LED LEDS[8] = {
		{ LED1_GPIO_Port, LED1_Pin },
		{ LED2_GPIO_Port, LED2_Pin },
		{ LED3_GPIO_Port, LED3_Pin },
		{ LED4_GPIO_Port, LED4_Pin },
		{ LED5_GPIO_Port, LED5_Pin },
		{ LED6_GPIO_Port, LED6_Pin },
		{ LED7_GPIO_Port, LED7_Pin },
		{ LED8_GPIO_Port, LED8_Pin }
	};

  /* USER CODE END 2 */

  Press l_press, r_press;

  uint16_t hit_timer = 200;
  uint16_t hit_timer_decrement = 50;

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

	uint16_t is_pressing_left  = !HAL_GPIO_ReadPin(L_button_GPIO_Port, L_button_Pin);
	uint16_t is_pressing_right = !HAL_GPIO_ReadPin(R_button_GPIO_Port, R_button_Pin);
	update_press(&l_press, is_pressing_left);
	update_press(&r_press, is_pressing_right);

	uint16_t ball_position;

	switch (state)
	{
		case STATE_START:
		{
			bool both_pressed = l_press.just_pressed && r_press.just_pressed;
			if (!both_pressed)
			{
				if (l_press.just_pressed)
				{
					state = STATE_MOVE_RIGHT;
					ball_position = 0;
				}
				else if (r_press.just_pressed)
				{
					state = STATE_MOVE_LEFT;
					ball_position = 7;
				}
			}
			break;
		}
		case STATE_MOVE_LEFT:
		{
			ball_position--;
			if (ball_position == 0)
			{
				state = STATE_MOVE_RIGHT;
				if (hit_timer > hit_timer_decrement) hit_timer -= hit_timer_decrement;
			}
			break;
		}
		case STATE_MOVE_RIGHT:
		{
			ball_position++;
			if (ball_position == 7)
			{
				state = STATE_MOVE_LEFT;
				if (hit_timer > hit_timer_decrement) hit_timer -= hit_timer_decrement;
			}
			break;
		}
	}

	if (state != STATE_START)
	{
		HAL_GPIO_WritePin(LEDS[ball_position].port, LEDS[ball_position].pin, GPIO_PIN_SET);
		HAL_Delay(hit_timer);
		HAL_GPIO_WritePin(LEDS[ball_position].port, LEDS[ball_position].pin, GPIO_PIN_RESET);
	}

	// for (int i = 0; i < 8; i++)
	// {
	// 	HAL_GPIO_WritePin(LEDS[i].port, LEDS[i].pin, GPIO_PIN_SET);
	// 	HAL_Delay(100);
	// 	HAL_GPIO_WritePin(LEDS[i].port, LEDS[i].pin, GPIO_PIN_RESET);
	// }

	// for (int i = 0; i < 8; i++)
	// {
	// 	HAL_GPIO_WritePin(LEDS[8 - i].port, LEDS[8 - i].pin, GPIO_PIN_SET);
	// 	HAL_Delay(100);
	// 	HAL_GPIO_WritePin(LEDS[8 - i].port, LEDS[8 - i].pin, GPIO_PIN_RESET);
	// }

	// HAL_GPIO_WritePin(LEDS[0].port, LEDS[0].pin, is_pressing_left  ? GPIO_PIN_RESET : GPIO_PIN_SET);
	// HAL_GPIO_WritePin(LEDS[7].port, LEDS[7].pin, ris_pressing_rightight ? GPIO_PIN_RESET : GPIO_PIN_SET);

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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
