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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>

#include "inits.h"
#include <lpuart.h>
#include "interface_board.h"
#include "rtc.h"
#include "pressure.h"
#include "memory.h"
#include "misc.h"
#include "process.h"
#include "tim.h"

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
ADC_HandleTypeDef hadc;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

IRDA_HandleTypeDef hirda2;

/* USER CODE BEGIN PV */
uint8_t rx_buffer[256];
uint8_t tx_buffer[256];
volatile uint16_t rx_index;
volatile uint16_t tx_index;
volatile uint16_t tx_length;
volatile uint8_t tx_busy;

volatile uint8_t last_tx_usart2;
volatile uint8_t ignore_echo_usart2;

#ifdef IR_USART2_SEL
  IRDA_HandleTypeDef* pActiveIrda = &hirda2;
#else
  UART_HandleTypeDef* pActiveUart = &hlpuart1;
#endif

volatile uint32_t last_activity_time;
volatile bool is_sleeping = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_IRDA_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#ifdef INTERFACE_BOARD_IMPLEMENTATION

void I2C_Scan(void)
{
#if DEBUG_SENSOR
    SendString((uint8_t*)"I2C Scanning...\r\n");
#endif
    for (uint8_t addr = 1; addr < 128; addr++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 3, 50) == HAL_OK) {
            char msg[32];
            sprintf(msg, "Device found at 0x%02X\r\n", addr);
#if DEBUG_SENSOR
            SendString((uint8_t*)msg);
#endif
        }
    }
#if DEBUG_SENSOR
    SendString((uint8_t*)"I2C Scan complete\r\n");
#endif
}

// Test function to verify Interface Board communication
void Test_InterfaceBoard(void)
{
    int32_t pressure = 0, temperature = 0, avdd = 0;

#if DEBUG_SENSOR
    SendString((uint8_t*)"\r\n=== Testing Interface Board ===\r\n");
#endif

    if (INTF_Init()) {
#if DEBUG_SENSOR
        SendString((uint8_t*)"Interface Board detected (ID: 0x21)\r\n");
#endif
    } else {
#if DEBUG_SENSOR
        SendString((uint8_t*)"ERROR: Interface Board not detected!\r\n");
#endif
        return;
    }

    if (INTF_ReadAllRaw(&pressure, &temperature, &avdd)) {
        char msg[64];
        sprintf(msg, "Pressure: %ld (0x%06lX)\r\n", pressure, pressure);
#if DEBUG_SENSOR
        SendString((uint8_t*)msg);
#endif
        sprintf(msg, "Temperature: %ld (0x%06lX)\r\n", temperature, temperature);
#if DEBUG_SENSOR
        SendString((uint8_t*)msg);
#endif
        sprintf(msg, "AVDD: %ld (0x%06lX)\r\n", avdd, avdd);
#if DEBUG_SENSOR
        SendString((uint8_t*)msg);
#endif
    } else {
#if DEBUG_SENSOR
        SendString((uint8_t*)"ERROR: Failed to read ADC values!\r\n");
#endif
    }

#if DEBUG_SENSOR
    SendString((uint8_t*)"=== Test Complete ===\r\n\r\n");
#endif
}
#endif

// UART Operations
#ifdef IR_USART2_SEL
void HAL_IRDA_RxCpltCallback(IRDA_HandleTypeDef *hirda)
{
    if(hirda->Instance == USART2)
    {
        last_activity_time = HAL_GetTick();
        IncomingByteCheck();
        HAL_IRDA_Receive_IT(hirda, &rx_buffer[0], 1);
    }
}

void HAL_IRDA_TxCpltCallback(IRDA_HandleTypeDef *hirda)
{
    if(hirda->Instance == USART2)
    {
        tx_busy = 0;
    }
}

void HAL_IRDA_ErrorCallback(IRDA_HandleTypeDef *hirda)
{
    if(hirda->Instance == USART2)
    {
        volatile uint32_t temp;
        temp = USART2->ISR;
        temp = USART2->RDR;
        (void)temp;
        HAL_IRDA_Receive_IT(&hirda2, rx_buffer, 1);
    }
}
#else

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *hlpuart1)
{
    if(hlpuart1->Instance == LPUART1)
    {
        last_activity_time = HAL_GetTick();
        IncomingByteCheck();
        HAL_UART_Receive_IT(hlpuart1, &rx_buffer[0], 1);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == LPUART1)
    {
        ContinueSend();
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == LPUART1)
    {
        last_activity_time = HAL_GetTick();
        DE_RE_DISABLE();
        tx_busy = 0;
        tx_index = 0;
        tx_length = 0;
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
        HAL_UART_DeInit(&hlpuart1);
        MX_LPUART1_UART_Init();
        HAL_UART_Receive_IT(&hlpuart1, rx_buffer, 1);
    }
}
#endif

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  uint8_t msg = 0;
  uint8_t test_write[] = {0xDE, 0xAD, 0xBE, 0xEF};
  uint8_t test_read[4] = {0};
  uint8_t test_cmd = 0x9F;
  uint8_t spi_response[3] = {0};
  uint8_t tx_buf[3] = {0x9F, 0, 0};
  float temp, pressure;

  Inits_Var();
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
  MX_ADC_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_USART2_IRDA_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_NVIC_DisableIRQ(EXTI4_15_IRQn); // Disable wakeup EXTI until we explicitly enter Stop Mode
  Set_REC_START_Pin_As_Input();       // Configure REC_START (PB13) pin as standard input pin when awake

  // Now all peripherals are initialized -- safe to enable interrupts
  __enable_irq();

#ifdef IR_USART2_SEL
    pActiveIrda = &hirda2;
#else
    pActiveUart = &hlpuart1;
#endif

  HAL_GPIO_WritePin(LED_EN_GPIO_Port, LED_EN_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(PWRDIST_GEN_PWR_EN_GPIO_Port, PWRDIST_GEN_PWR_EN_Pin, GPIO_PIN_SET);
  HAL_Delay(100);

  HAL_GPIO_WritePin(CS_MEM_0_BAR_GPIO_Port, CS_MEM_0_BAR_Pin, GPIO_PIN_SET);

  for (int i=0; i<20; i++) {
      HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_SET);
      HAL_Delay(50);
      HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_RESET);
      HAL_Delay(50);
  }

  DE_RE_DISABLE();
  rx_index = 0;
#ifdef IR_USART2_SEL
  HAL_IRDA_Receive_IT(&hirda2, &rx_buffer[rx_index], 1);
#else
  HAL_UART_Receive_IT(&hlpuart1, &rx_buffer[rx_index], 1);
#endif

  RTC_Init();
  //HAL_TIM_Base_Start_IT(&htim2);
  HAL_Delay(3000);

  // Memory wipeout and ReedSwitch activation (disabled at startup, handled dynamically by start-time trigger)
  // SendString((uint8_t*)"\n\nMemory Wiped...\r");
  // ClearShowFilesAndMetadata();  // WIpes out all memory
  // ReedSwitch.state = ACTIVATED;  // temporary to test recording
  // SendString((uint8_t*)"\nReed Switch Activated...\r");
  // Set_TIM2_Interval(1, TIME_UNIT_SEC);


#ifdef INTERFACE_BOARD_IMPLEMENTATION
#if DEBUG_SENSOR
  SendString((uint8_t*)"\r\n=== Starting I2C Test ===\r\n");
#endif
  HAL_Delay(1000);
  I2C_Scan();
  Test_InterfaceBoard();
#endif

  last_activity_time = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (Lpuart1.status == FILLED)
    {
        last_activity_time = HAL_GetTick(); // Reset watchdog on main thread when a valid packet is ready
    }

    ProcessMsg();

    // Sleep Watchdog
    if (RecordState.started == RECORDING_NOTSTARTED && ApplyRecordingPlan.run == PLAN_RUN_NO)
    {
        if ((int32_t)(HAL_GetTick() - last_activity_time) > 60000) // 1 minute (60000 ms)
        {
#if DEBUG_SENSOR
            SendString((uint8_t*)"Inactivity timeout reached. Entering STOP mode...\r\n");
            HAL_Delay(50); // Let the UART transmission complete
#endif
            Enter_Deep_Sleep();
        }
    }

    if (!is_timer_triggered && ApplyRecordingPlan.run == PLAN_RUN_YES)
    {
        if (IsTargetTimeReached())
        {
            // Only start if we are in Mode 1 (Do Not Loop) OR if we haven't reached the end time yet
            if (Sampling.mode == TIME_DONOTLOOP || !IsEndTimeReached())
            {
#if DEBUG_SENSOR
                SendString((uint8_t*)"\nTarget time reached. Clearing EEPROM memory...\r\n");
#endif
                ClearShowFilesAndMetadata();
#if DEBUG_SENSOR
                SendString((uint8_t*)"\nEEPROM cleared. Starting recording session...\r\n");
#endif

                ReedSwitch.state = ACTIVATED;
                is_timer_triggered = 1;
                StartRecordingTimer();
            }
            else
            {
                // Schedule has already passed; auto-deactivate
                ApplyRecordingPlan.run = PLAN_RUN_NO;
                is_timer_triggered = 0;
            }
        }
    }

    if (recording_timer_expired)
    {
        recording_timer_expired = 0;

        // Try to start a recording if the switch is ON (ReedSwitch.state == ACTIVATED)
        RecordingStart();

        if (RecordState.started == RECORDING_ONGOING)
        {
            // Check if we are in Time - Continuous Loop mode and end time is reached
            if (Sampling.mode == TIME_CONTINUOUSLOOP && IsEndTimeReached())
            {
                // Execute stop recording sequence
                ReedSwitch.state = DEACTIVATED;
                DebugMemory4(); // Write final records and stop

                RecordState.started = RECORDING_NOTSTARTED;
                RecordState.sector = 0;
                Counter.repeat = 0;

                is_timer_triggered = 0;
                ApplyRecordingPlan.run = PLAN_RUN_NO;
                HAL_TIM_Base_Stop_IT(&htim2);
                HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_RESET);  // turn off LED_A
            }
            else // TIME_DONOTLOOP (or continuous loop mode before end time)
            {
                DebugMemory4();

                if (RecordState.started == RECORDING_NOTSTARTED)
                {
                    // Stop recording sequence
                    RecordState.sector = 0;
                    Counter.repeat = 0;
                    is_timer_triggered = 0;
                    ApplyRecordingPlan.run = PLAN_RUN_NO;
                    HAL_TIM_Base_Stop_IT(&htim2);
                    HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_RESET);  // turn off LED_A
                }
                else
                {
                    Counter.repeat++;
                }
            }
        }
    }

    //TestingRuns();

    //SendByte(0x55);
    //HAL_Delay(1000);


    //HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);
    //HAL_Delay(1000);

    //Test_InterfaceBoard();
    //HAL_Delay(1000);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSEState = RCC_LSE_BYPASS;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV32;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerFrequencyMode = ENABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x6000C7FF;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 15999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_IRDA_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  hirda2.Instance = USART2;
  hirda2.Init.BaudRate = 115200;
  hirda2.Init.WordLength = IRDA_WORDLENGTH_8B;
  hirda2.Init.Parity = IRDA_PARITY_NONE;
  hirda2.Init.Mode = IRDA_MODE_TX_RX;
  hirda2.Init.Prescaler = 1;
  hirda2.Init.PowerMode = IRDA_POWERMODE_NORMAL;
  if (HAL_IRDA_Init(&hirda2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, PWRDIST_GEN_PWR_EN_Pin|USART2_SD_DE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CS_MEM_0_BAR_Pin|LED_A_Pin|LED_B_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, READBATT_Pin|LED_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PWRDIST_GEN_PWR_EN_Pin */
  GPIO_InitStruct.Pin = PWRDIST_GEN_PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PWRDIST_GEN_PWR_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USART2_SD_DE_Pin */
  GPIO_InitStruct.Pin = USART2_SD_DE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(USART2_SD_DE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_MEM_0_BAR_Pin LED_A_Pin LED_B_Pin */
  GPIO_InitStruct.Pin = CS_MEM_0_BAR_Pin|LED_A_Pin|LED_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : READBATT_Pin LED_EN_Pin */
  GPIO_InitStruct.Pin = READBATT_Pin|LED_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : REC_START_Pin */
  GPIO_InitStruct.Pin = REC_START_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(REC_START_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
