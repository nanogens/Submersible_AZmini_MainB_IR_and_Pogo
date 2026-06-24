/*
 * rtc.c
 *
 *  Created on: Jun 23, 2026
 *      Author: MT
 */

#include "main.h"
#include "stdio.h"
#include "rtc.h"
#include "lpuart.h"

void RTC_Init(void)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  // Check if RTC is already initialized
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2)
  {
    // Initialize RTC only if not already done

    // Configure the Time
    sTime.Hours = 0;
    sTime.Minutes = 0;
    sTime.Seconds = 0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
      Error_Handler();
    }

    // Configure the Date
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 1;
    sDate.Year = 0;

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
    {
      Error_Handler();
    }

    // Mark RTC as initialized
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2);
  }

  // Set initial time to September 22, 2025, 16:36:00
  Time.set_year = 25;      // 2025
  Time.set_month = 9;      // September
  Time.set_day = 23;       // 23rd
  Time.set_hour = 15;      // 3 PM (24-hour format)
  Time.set_minute = 26;    // 26 minutes
  Time.set_second = 0;     // 0 seconds
  Time.set_ampm = 1;       // PM

  // Set the initial time
  RTC_SetTime();
}

void RTC_SetTime(void)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  // Convert incoming 12-hour + AM/PM to 24-hour format
  uint8_t hour24 = Time.set_hour;  // Time.set_hour is 1-12

  if (Time.set_ampm == 1) { // PM
      if (hour24 != 12) {
          hour24 += 12;  // 1 PM -> 13, 2 PM -> 14, ... 11 PM -> 23
      }
      // 12 PM stays 12
  } else { // AM
      if (hour24 == 12) {
          hour24 = 0;  // 12 AM -> 0
      }
      // 1 AM -> 1, 2 AM -> 2, ... 11 AM -> 11
  }

  // Convert to BCD format using the 24-hour value
  sTime.Hours = ((hour24 / 10) << 4) | (hour24 % 10);
  sTime.Minutes = ((Time.set_minute / 10) << 4) | (Time.set_minute % 10);
  sTime.Seconds = ((Time.set_second / 10) << 4) | (Time.set_second % 10);
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;

  // Map weekday from your format (1=Monday, 7=Sunday) to STM32 RTC defines
  sDate.WeekDay = Time.set_weekday;

  sDate.Month = ((Time.set_month / 10) << 4) | (Time.set_month % 10);
  sDate.Date = ((Time.set_day / 10) << 4) | (Time.set_day % 10);
  sDate.Year = ((Time.set_year / 10) << 4) | (Time.set_year % 10);

  // Disable write protection
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2);

  // Set date and time
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
}

void RTC_ReadTime(void)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  // Get time and date from RTC
  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  // sTime.Hours is in 24-hour format (0-23)
  uint8_t hour24 = sTime.Hours;

  // Convert to 12-hour format and set AM/PM
  Time.read_ampm = (hour24 >= 12) ? 1 : 0;

  uint8_t hour12 = hour24 % 12;
  if (hour12 == 0) hour12 = 12;  // 0 or 24 becomes 12

  Time.read_hour = hour12;
  Time.read_minute = sTime.Minutes;
  Time.read_second = sTime.Seconds;

  // Date fields
  Time.read_year = sDate.Year;
  Time.read_month = sDate.Month;
  Time.read_day = sDate.Date;
  Time.read_full_year = 2000 + Time.read_year;
  Time.read_weekday = sDate.WeekDay;
}

uint8_t RTC_IsTimeSet(void)
{
  return (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) == 0x32F2);
}

void RTC_SendTimeViaUART(void)
{
  uint8_t buffer[128];
  uint16_t length;

  // Read current time from RTC
  RTC_ReadTime();

  // Format the date/time string
  // Format: "2025-09-22 16:36:45 PM Monday\r\n"
  length = snprintf((char *)buffer, sizeof(buffer),
                   "%04d-%02d-%02d %02d:%02d:%02d %s %s\r\n",
                   Time.read_full_year,
                   Time.read_month,
                   Time.read_day,
                   Time.read_hour,
                   Time.read_minute,
                   Time.read_second,
                   Time.read_ampm ? "PM" : "AM",
                   GetWeekdayString(Time.read_weekday));

  if (length > 0 && length < sizeof(buffer))
  {
    // Enable RS-485 transmitter
	DE_RE_ENABLE();

    // Small delay to ensure transceiver is ready (optional but recommended)
    HAL_Delay(1);

#ifdef IR_USART2_SEL
    for(uint16_t f = 0; f < length; f++)
    {
      // Use the index 'f' to stay at the correct offset
      HAL_IRDA_Transmit_IT(pActiveUart, &buffer[f], 1);
    }
#else
    // Send data via LPUART1
    HAL_UART_Transmit(pActiveUart, (uint8_t*)buffer, length, HAL_MAX_DELAY);
#endif
    // Small delay to ensure all data is sent before disabling transmitter
    HAL_Delay(1);

    // Disable RS-485 transmitter (return to receive mode)
	DE_RE_DISABLE();
  }
}

const char* GetWeekdayString(uint8_t weekday)
{
  switch(weekday)
  {
    case 1: return "Monday";
    case 2: return "Tuesday";
    case 3: return "Wednesday";
    case 4: return "Thursday";
    case 5: return "Friday";
    case 6: return "Saturday";
    case 7: return "Sunday";
    default: return "Unknown";
  }
}

