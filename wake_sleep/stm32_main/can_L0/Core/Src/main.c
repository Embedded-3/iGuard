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
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CANSPI.h"
#include "MCP2515.h"
#include "stdio.h"
#include "string.h"
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

volatile uCAN_MSG rxMessage;
extern ADC_HandleTypeDef hadc;
volatile uint32_t adcValue;
volatile float Vout;
volatile static float pressure = 0.0f;

volatile static uint8_t bReadHCSR501 = 0;
volatile static uint32_t IC_Val1 = 0;
volatile static uint32_t IC_Val2 = 0;
volatile static uint32_t Difference = 0;
volatile static uint8_t Is_First_Captured = 0;  // is the first value captured 
volatile static uint8_t Distance  = 0;
volatile uint8_t DistanceReady = 0;
volatile uint16_t no_activity_counter = 0;
volatile uint8_t tim3_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void delay (uint16_t time);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HCSR04_Read(void);
uint8_t FSR_Check (void);
uint8_t Ultrasonic_Check(void);
void check_and_send(void);
void enter_sleep_mode(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_SPI1_Init();
  MX_USART2_Init();
  MX_ADC_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  CANSPI_Initialize();	
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
  HAL_TIM_Base_Start_IT(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    if (tim3_flag) {
      tim3_flag = 0;
      check_and_send();  
    }
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_3;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void delay (uint16_t time)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    while (__HAL_TIM_GET_COUNTER (&htim2) < time);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == htim3.Instance){
    tim3_flag= 1;
  }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) // 초음파 센서 거리 계산 비동기 로직
{ // INT when detected Ultrasound sig
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)  // if the interrupt source is channel1
    {
        if (Is_First_Captured==0) // if the first value is not captured
        {
            IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3); // read the first value
            Is_First_Captured = 1; 
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else if (Is_First_Captured==1)   // if the first is already captured
        {
            IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);  // read second value
            __HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

            if (IC_Val2 > IC_Val1)
            {
                Difference = IC_Val2-IC_Val1;
            }
            else if (IC_Val1 > IC_Val2)
            {
                Difference = (0xffff - IC_Val1) + IC_Val2;
            }

            Distance = Difference * .034/2;
            DistanceReady = 1;
            Is_First_Captured = 0; 
						
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
            __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC3);
        }
    }
}


void enter_sleep_mode(void) {
  HAL_SuspendTick(); 
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  HAL_ResumeTick();   
}

void HCSR04_Read(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	delay(10);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);

	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC3);
}


uint8_t Ultrasonic_Check(void) {
  DistanceReady = 0;
  HCSR04_Read();
  // 변화량으로 할지 말지 결정해야함.
  uint32_t timeout = 0; 
  while (!DistanceReady && timeout < 200) {
      HAL_Delay(1);
      timeout++;
  }

  return (Distance < 100) ? 1 : 0;
}

uint8_t PIR_Check(void) {
  GPIO_PinState state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);  // PIR 센서 핀
  return (state == GPIO_PIN_SET) ? 1 : 0;
}

uint8_t FSR_Check(void){
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000);
	adcValue = HAL_ADC_GetValue(&hadc); // ADC
	HAL_ADC_Stop(&hadc);
	
	Vout = (adcValue / 4095.0f) * 5.0f;    // VOUT  (5V )
	if (Vout < 1.0f) {
			pressure = 0.0f; 
	} else if (Vout < 2.0f) {
			pressure = 200.0f;  // VOUT 2V 200g
	} else if (Vout < 3.0f) {
			pressure = 600.0f;  // VOUT 3V 600g
	} else {
			pressure = 800.0f;  // VOUT 5V 400g 
	}
	
  return (pressure > 200) ? 1 : 0;
}

void check_and_send(void) {
  uint8_t fsr = FSR_Check();
  uint8_t pir = PIR_Check();
  uint8_t ultrasonic = Ultrasonic_Check();

  uint8_t result = fsr | pir | ultrasonic; // Sensor Logic

  if(result) {
      uCAN_MSG txMessage;
      txMessage.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
      txMessage.frame.id = 0x100;
      txMessage.frame.dlc = 8;
      txMessage.frame.data0 = 'W';
      txMessage.frame.data1 = 'A';
      txMessage.frame.data2 = 'K';
      txMessage.frame.data3 = 'E';
      txMessage.frame.data4 = 0;
      txMessage.frame.data5 = 0;
      txMessage.frame.data6 = 0;
      txMessage.frame.data7 = 0;

      CANSPI_Transmit(&txMessage);
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
      no_activity_counter = 0;
  }else {
    no_activity_counter++;
  }

  if (no_activity_counter >= 500) {
    no_activity_counter = 0;
    enter_sleep_mode();
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
