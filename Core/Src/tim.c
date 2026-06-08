/*
 * tim.c
 *
 *  Created on: Feb 27, 2026
 *      Author: MT
 */

/* USER CODE BEGIN Header */

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */


/* USER CODE BEGIN 1 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    //Blinky();
  }
}
/* USER CODE END 1 */
