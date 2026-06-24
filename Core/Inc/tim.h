/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim2;



/* USER CODE BEGIN Private defines */
typedef enum {
    TIME_UNIT_MS,
    TIME_UNIT_SEC,
    TIME_UNIT_MIN,
    TIME_UNIT_HOUR
} TimeUnit_t;


/* USER CODE END Private defines */


/* USER CODE BEGIN Prototypes */
void Blinky(void);
void Set_TIM2_Interval(uint32_t value, TimeUnit_t unit);

void DebugMemory4(void);
void RecordingStart(void);
void SendByte(uint8_t data);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */


