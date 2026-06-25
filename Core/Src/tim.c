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
#include <stdio.h>
#include "memory.h"
#include "misc.h"


/* USER CODE BEGIN 0 */
volatile uint32_t software_overflow_count = 0;
volatile uint32_t current_overflow_target = 0;
volatile uint32_t initial_remainder_arr = 0; // Tracks the initial step value
/* USER CODE END 0 */

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */


/* USER CODE BEGIN 1 */
void Blinky(void)
{
  HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);
  //HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
}

void Set_TIM2_Interval(uint32_t value, TimeUnit_t unit)
{
    uint64_t total_ms = 0;

    // 1. Convert all inputs into total milliseconds
    switch (unit) {
        case TIME_UNIT_MS:   total_ms = value; break;
        case TIME_UNIT_SEC:  total_ms = (uint64_t)value * 1000; break;
        case TIME_UNIT_MIN:  total_ms = (uint64_t)value * 60000; break;
        case TIME_UNIT_HOUR: total_ms = (uint64_t)value * 3600000; break;
    }

    // 2. Safety First: Turn off the timer to avoid race conditions
    HAL_TIM_Base_Stop_IT(&htim2);

    // 3. Reset all software and hardware registers to a true zero state
    software_overflow_count = 0;
    __HAL_TIM_SET_COUNTER(&htim2, 0);

    // 4. Set up the target configurations
    if (total_ms <= 65535) {
        current_overflow_target = 0;
        initial_remainder_arr = (uint32_t)(total_ms - 1);
        __HAL_TIM_SET_AUTORELOAD(&htim2, initial_remainder_arr);
    }
    else {
        current_overflow_target = total_ms / 50000;
        uint32_t remainder = total_ms % 50000;

        if (remainder == 0) {
            initial_remainder_arr = 50000 - 1;
        } else {
            initial_remainder_arr = remainder - 1;
        }

        // Load the initial step value into the hardware ARR
        __HAL_TIM_SET_AUTORELOAD(&htim2, initial_remainder_arr);
    }

    // 5. Force update event register to commit ARR configurations instantly
    htim2.Instance->EGR = TIM_EGR_UG;

    // Clear any accidental interrupt flags triggered by forcing the update event
    __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);

    // 6. Start back up cleanly
    HAL_TIM_Base_Start_IT(&htim2);
}

void StartRecordingTimer(void)
{
    uint32_t interval = 1;
    TimeUnit_t unit = TIME_UNIT_SEC;

    switch (Sampling.rate) {
        case 0:  interval = 100; unit = TIME_UNIT_MS;   break; // 0.1 sec
        case 1:  interval = 500; unit = TIME_UNIT_MS;   break; // 0.5 sec
        case 2:  interval = 1;   unit = TIME_UNIT_SEC;  break; // 1 sec
        case 3:  interval = 5;   unit = TIME_UNIT_SEC;  break; // 5 sec
        case 4:  interval = 10;  unit = TIME_UNIT_SEC;  break; // 10 sec
        case 5:  interval = 30;  unit = TIME_UNIT_SEC;  break; // 30 sec
        case 6:  interval = 1;   unit = TIME_UNIT_MIN;  break; // 1 min
        case 7:  interval = 5;   unit = TIME_UNIT_MIN;  break; // 5 min
        case 8:  interval = 10;  unit = TIME_UNIT_MIN;  break; // 10 min
        case 9:  interval = 30;  unit = TIME_UNIT_MIN;  break; // 30 min
        case 10: interval = 1;   unit = TIME_UNIT_HOUR; break; // 1 hour
        default: interval = 1;   unit = TIME_UNIT_SEC;  break;
    }

    Set_TIM2_Interval(interval, unit);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        uint8_t is_timer_expired = 0;

        if (current_overflow_target > 0) {
            software_overflow_count++;

            // Step 1: After the unique remainder block completes, switch to standard 50s blocks
            if (software_overflow_count == 1) {
                __HAL_TIM_SET_AUTORELOAD(&htim2, 50000 - 1);
            }

            // Step 2: Check if the total required blocks have elapsed
            if (software_overflow_count > current_overflow_target)
            {
                software_overflow_count = 0;

                // Reload the unique remainder block to seamlessly start the next cycle
                __HAL_TIM_SET_AUTORELOAD(&htim2, initial_remainder_arr);

                is_timer_expired = 1;
            }
        }
        else
        {
            // Short-duration mode automatically loops in hardware
            is_timer_expired = 1;
        }

        // =======================================================
        // PUT YOUR CONTINUOUS CODE HERE
        // =======================================================
        if (is_timer_expired)
        {
            Blinky(); // Toggle LED_A every second to verify timer interrupts are working

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
                else // TIME_DONOTLOOP
                {
                    // Mode 1 (Do Not Loop) logs indefinitely; Mode 0 logs until end time is reached
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
        // =======================================================
    }
}
/* USER CODE END 1 */
