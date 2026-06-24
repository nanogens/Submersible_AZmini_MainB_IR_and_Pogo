#ifndef INC_LPUART_H_
#define INC_LPUART_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32l0xx_hal.h"

//==============================================================================
// Function Prototypes
//==============================================================================
uint8_t Send_Data(uint8_t *data, uint16_t length);
void FalseHeader(void);
void SendHeader(uint8_t msg_length, uint8_t msg_id);
void IncomingByteCheck(void);
void SendByte(uint8_t data);
void ContinueSend(void);
bool Search_MsgID(uint8_t settingorquery, uint8_t messageidglobal);

// HANDLES =============================================

// VARIABLES ================================================
extern uint8_t rx_buffer[256];
extern uint8_t tx_buffer[256];
extern uint8_t mainrx_buffer[1];
extern volatile uint16_t rx_index;
extern volatile uint16_t tx_index;
extern volatile uint16_t tx_length;
extern volatile uint8_t tx_busy;

#ifdef IR_USART2_SEL
  extern IRDA_HandleTypeDef hirda2;
  extern IRDA_HandleTypeDef* pActiveIrda;
  #define pActiveUart NULL
#else
  extern UART_HandleTypeDef hlpuart1;
  extern UART_HandleTypeDef* pActiveUart;
  #define pActiveIrda NULL
#endif

#if IR_USART2_SEL // IR version
  #define MY_UART_HANDLE  hirda2
  #define MY_UART_INSTANCE USART2

  // Macros
  #define DE_RE_ENABLE()  HAL_GPIO_WritePin(USART2_SD_DE_GPIO_Port, USART2_SD_DE_Pin, GPIO_PIN_RESET)
  #define DE_RE_DISABLE() HAL_GPIO_WritePin(USART2_SD_DE_GPIO_Port, USART2_SD_DE_Pin, GPIO_PIN_RESET)

#else // POGO version
  #define MY_UART_HANDLE  hlpuart1
  #define MY_UART_INSTANCE LPUART1

  // Macros
  #define DE_RE_ENABLE()  HAL_GPIO_WritePin(LPUART1_SD_DE_GPIO_Port, LPUART1_SD_DE_Pin, GPIO_PIN_SET)
  #define DE_RE_DISABLE() HAL_GPIO_WritePin(LPUART1_SD_DE_GPIO_Port, LPUART1_SD_DE_Pin, GPIO_PIN_RESET)
#endif

#endif

