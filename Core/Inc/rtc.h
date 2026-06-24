#ifndef INC_RTC_H
#define INC_RTC_H

// Function declarations
void RTC_Init(void);
extern void RTC_SetTime(void);
extern void RTC_ReadTime(void);
uint8_t RTC_IsTimeSet(void);
void RTC_SendTimeViaUART(void);
const char* GetWeekdayString(uint8_t weekday);


extern RTC_HandleTypeDef hrtc;

#endif /* INC_RTC_H_ */
