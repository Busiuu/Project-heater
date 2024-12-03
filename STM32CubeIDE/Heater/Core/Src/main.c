/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "BMPXX80.h"
#include <stdlib.h>
#include <math.h>
//#include "heater_config.c"
//#include "bmp2_config.c"
//#include "btn_config.c"
//#include "pwm.c"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_BUFFER_SIZE 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float err = 0.0f;
int duty = 0;
float temp = 0.0f;
float temp_zad = 38.0f;
float Ts = 0.01f;
float wyjscie = 0.0f;
float integrator = 0.0f;
float Kp = 5000.0f;
float Ki = 400.420;
int licznik_wiadomosc = 0;
int licznik = 0;
char uart_buffer[UART_BUFFER_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART3){
	  size_t len = strlen(uart_buffer);
		// Używamy strtof (string to float) do konwersji na zmienną typu float
		char *endptr;
		temp_zad = strtof(uart_buffer, &endptr);

		memset(uart_buffer, 0, sizeof(uart_buffer));
      // Ponowne uruchomienie odbioru przez USART w trybie przerwań
		HAL_UART_Receive_IT(&huart3, (uint8_t*)uart_buffer, UART_BUFFER_SIZE);
  }
}



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
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_SPI1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
//  BMP2_Init(&bmp2dev_1);
//  HEATER_PWM_Init(&hheater);
//  HAL_UART_Receive_IT(&huart3, pwm_duty_msg, strlen(pwm_duty_msg));
  BMP280_Init(&hspi1, BMP280_TEMPERATURE_16BIT, BMP280_STANDARD, BMP280_FORCEDMODE);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  HAL_UART_Receive_IT(&huart3, (uint8_t*)uart_buffer, UART_BUFFER_SIZE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	licznik = __HAL_TIM_GET_COUNTER(&htim4);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2,ceil(licznik*256.4));
    temp = BMP280_ReadTemperature(); // pomiar
    if (temp_zad > 65){ // zabezpieczenie
    	temp_zad = 65.0f;
    } else if (temp_zad < 20) {
    	temp_zad = 20.0f;
    }
    err = temp_zad - temp; // uchyb
    integrator += err; // integrator
    if (integrator > 2299){ // anty wind-up
            integrator = 2300;
    } else if (integrator < -2299){
            integrator = -2300;
    }
    wyjscie = Kp * err + Ki * Ts * integrator; // wyjscie
    if(wyjscie > 9999){
    	duty = 9999;
    }
    if(wyjscie < 0){
        duty = 0;
    }
    if(wyjscie >= 0 && wyjscie <= 9999){
    	duty = ceil(wyjscie);
    }
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,duty);
    licznik_wiadomosc++;
    if (licznik_wiadomosc >= 5){
    	licznik_wiadomosc = 0;
		const char msg[150] = { 0, };
		int msg_len = sprintf(msg, "Wypelnienie=%d.%02d;Pomiar_temperatury=%d.%03d;Temperatura_zadana=%d.%02d;Poziom_zaklocenia=%d\r\n",
				(int)(duty*0.01),(int)(duty%100), (int)temp, (int)(1000.0f*temp)%1000, (int)temp_zad, (int)(100.0f*temp_zad)%100, (int)(ceil(licznik*2.564)));
		HAL_UART_Transmit(&huart3, (uint8_t*)msg, msg_len,HAL_MAX_DELAY);
    }
    HAL_Delay(10);
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
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
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//  if (htim == &htim5) {
//    HAL_GPIO_TogglePin(FAN_GPIO_Port, FAN_Pin);
//
//  }
//}
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
