#include "main.h"
#include "string.h"
#include "lpuart.h"

// Send function implementation - Blocking
uint8_t Send_Data(uint8_t *data, uint16_t length)
{
    if (tx_busy) return 0;

    if (length > sizeof(tx_buffer)) return 0;

    memcpy(tx_buffer, data, length);
    tx_length = length;
    tx_index = 0;
    tx_busy = 1;

    DE_RE_ENABLE();  // MT Temporary

    // Send ALL bytes (not just one)
#ifdef IR_USART2_SEL
    // Disable receiver before transmitting
    USART2->CR1 &= ~USART_CR1_RE;

    for(uint16_t f = 0; f < length; f++)
    {
      // Use the index 'f' to stay at the correct offset
      HAL_IRDA_Transmit_IT(pActiveIrda, &tx_buffer[f], 1);
      // Wait for transmit to complete
      while(!(USART2->ISR & USART_ISR_TC));
      for (volatile int i = 0; i < 150; i++); // for 16 Mhz clock
      //for (volatile int i = 0; i < 300; i++); // for 32 Mhz clock
    }
    // Re-enable receiver after transmission
    USART2->CR1 |= USART_CR1_RE;
#else // for LPUART1 (Pogo pins)
    for (volatile int i = 0; i < 150; i++); // for 16 Mhz clock
    //for (volatile int i = 0; i < 300; i++); // for 32 Mhz clock
    HAL_UART_Transmit(pActiveUart, tx_buffer, length, HAL_MAX_DELAY);
#endif
    DE_RE_DISABLE();
    tx_busy = 0;
    return 1;
}

// Continue send function
void ContinueSend(void)
{
  tx_index++;
  if (tx_index < tx_length)
  {
#ifdef IR_USART2_SEL
    HAL_IRDA_Transmit_IT(pActiveIrda, &tx_buffer[tx_index], 1);
#else
    HAL_UART_Transmit_IT(pActiveUart, &tx_buffer[tx_index], 1);
#endif
  }
  else
  {
    // Wait for the last stop bit to be sent
#ifdef IR_USART2_SEL
    volatile uint32_t delay = (12 * 1000000) / pActiveIrda->Init.BaudRate;
#else
    volatile uint32_t delay = (12 * 1000000) / pActiveUart->Init.BaudRate;
#endif
    while(delay--);
    // Now safe to return to receive mode
    DE_RE_DISABLE();
    tx_busy = 0;
  }
}

// =========================================

void FalseHeader(void)
{
  if(rx_buffer[0] == DLE)
  {
	Lpuart1.got = 1;
  }
  else
  {
	Lpuart1.got = 0;
    Lpuart1.crcmsg = 0;
  }
}

// Wrapper function - uses LPUART_Send for single byte
void SendByte(uint8_t data)
{
  // Send single byte using the interrupt-based send
  Send_Data(&data, 1);  // Added hlpuart1 as first parameter

  // Wait for the byte to finish transmitting
  while(tx_busy);

  // Update CRC (keep your existing functionality)
  Lpuart1.crcsend += data;
}

void SendHeader(uint8_t msg_length, uint8_t msg_id)
{
  Lpuart1.crcsend = 0;
  SendByte(DLE);
  SendByte(STX);
  SendByte(SOURCE); // 0x88
  SendByte(DEST);   // 0x00
  SendByte(msg_length);
  SendByte(msg_id);
}

bool Search_MsgID(uint8_t settingorquery, uint8_t messageidglobal)
{
  if(settingorquery == QUERY)
  {
    if(
    	(messageidglobal == VERSION_QUERY_MSGID) ||
    	(messageidglobal == STATUS_QUERY_MSGID) ||
	    (messageidglobal == INSTRUMENT_QUERY_MSGID) ||
        (messageidglobal == COMMUNICATION_QUERY_MSGID) ||
        (messageidglobal == POWER_QUERY_MSGID) ||
        (messageidglobal == TIME_QUERY_MSGID) ||
        (messageidglobal == SAMPLING_QUERY_MSGID) ||
        (messageidglobal == ACTIVATION_QUERY_MSGID) ||
        (messageidglobal == NOTES_QUERY_MSGID) ||
        (messageidglobal == CLOUD_QUERY_MSGID) ||
		(messageidglobal == MISC_QUERY_MSGID) ||

		(messageidglobal == LOG_SHOWFILES_QUERY_MSGID) ||
		//(messageidglobal == LOG_READSPECIFICFILE_QUERY_MSGID) ||

		(messageidglobal == CTD_VARIABLES_QUERY_MSGID) ||
		(messageidglobal == CTD_READINGS_RAW_QUERY_MSGID) ||
		(messageidglobal == CTD_READINGS_PROCESSED_QUERY_MSGID)

	  )
    {
      return true;
    }
    return false;
  }
  else // SETTING
  {
    if(
    	(messageidglobal == STATUS_SET_MSGID) ||
	    (messageidglobal == INSTRUMENT_SET_MSGID) ||
        (messageidglobal == COMMUNICATION_SET_MSGID) ||
        (messageidglobal == POWER_SET_MSGID) ||
        (messageidglobal == TIME_SET_MSGID) ||
        (messageidglobal == SAMPLING_SET_MSGID) ||
        (messageidglobal == ACTIVATION_SET_MSGID) ||
        (messageidglobal == NOTES_SET_MSGID) ||
        (messageidglobal == CLOUD_SET_MSGID) ||
		(messageidglobal == MISC_SET_MSGID) ||

		(messageidglobal == LOG_READSPECIFICFILE_SET_MSGID) ||
		(messageidglobal == LOG_TRANSMITDATA_SET_MSGID) ||

		(messageidglobal == CTD_VARIABLES_SET_MSGID) ||

		(messageidglobal == BOOTLOADER_TRANSMITDATA_SET_MSGID) ||

		(messageidglobal == BOOTLOADER_FWUPDATE_SET_MSGID)

      )
	{
	  return true;
	}
	return false;
  }
}

void IncomingByteCheck(void)
{
  /*
  // MT Temporary -- remove later -- testing IR
  if(rx_buffer[0] == 0x52)
  {
	HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);
  }
  */

  if(Lpuart1.status != FILLED)
  {
	if(Lpuart1.got == 0)
	{
	  FalseHeader();
    }

	else if(Lpuart1.got == 1)
	{
      if(rx_buffer[0] == STX)
	  {
    	Lpuart1.got = 2;
	  }
	  else
	  {
	    FalseHeader();
	  }
	}
    else if(Lpuart1.got == 2)
    {
	  if(rx_buffer[0] == DEST) // Source as defined by Viewer
	  {
		Lpuart1.got = 3;
      }
	  else
	  {
        FalseHeader();
      }
    }
    else if(Lpuart1.got == 3)
    {
	  if(rx_buffer[0] == SOURCE) // Source as defined by Viewer
	  {
		Lpuart1.got = 4;
      }
	  else
	  {
        FalseHeader();
      }
    }
	else if(Lpuart1.got == 4)  // message length
	{
	  // Make a function to check message length
	  Lpuart1.messagelength = rx_buffer[0];
	  Lpuart1.got = 5;
	}
    else if(Lpuart1.got == 5)
    {
  	  // Make a function to check message id
      Lpuart1.messageidglobal = rx_buffer[0];
      Lpuart1.got = 6;
	}


	// Query
	else if((Lpuart1.got == 6) && (Search_MsgID(QUERY, Lpuart1.messageidglobal) == true))
	{
	  Lpuart1.crcmsg = rx_buffer[0];

	  if(Lpuart1.crcmsg == (DLE + STX + DEST + SOURCE + Lpuart1.messagelength + Lpuart1.messageidglobal) % 256)
	  {
		Lpuart1.status = FILLED; // immediately block it from re-entering this receive interrupt until present request is processed in main loop
		Lpuart1shadow.messageid = Lpuart1.messageidglobal;

		//HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
	    if(Lpuart1.messageidglobal == BOOTLOADER_FWUPDATE_SET_MSGID)
	    {
	    	  HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);
	    }
      }
	  Lpuart1.got = 0;
	  Lpuart1.crcmsg = 0;
	}


	// Setting - next 2 blocks
	else if(
		     (Search_MsgID(SETTING, Lpuart1.messageidglobal) == true) &&
			 ((Lpuart1.got >= 6) && (Lpuart1.got < (Lpuart1.messagelength - 1)))
		   )
	{
      if(((Lpuart1.got - 6) < MAX_LPUART1_ARRAY) && ((Lpuart1.got - 6) >= 0)) // protection
	  {
    	/*
        if((rx_buffer[0] == 0x06) && (Lpuart1.got == 11))
        {
      	  // Temporary -- REMOVE
      	  HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);
      	  Lpuart1.status = CLEAR;
          Lpuart1.got = 0;
          return;
        }
        */
    	Lpuart1.payload[Lpuart1.got-6] = rx_buffer[0];
    	Lpuart1.got++;
	  }
	  else
	  {
		Lpuart1.got = 0;
	  }
	}
	else if(
			(Lpuart1.got <= (Lpuart1.messagelength - 1)) &&
			(Search_MsgID(SETTING, Lpuart1.messageidglobal) == true)
		   )
	{

	  Lpuart1.crcmsg = rx_buffer[0]; // the crc at the end of Set Working Parameters
	  // it is DEST + SOURCE and not SOURCE + DEST because dest & source values are in relation to what we send not receive.
	  Lpuart1.crcset = DLE + STX + DEST + SOURCE + Lpuart1.messagelength + Lpuart1.messageidglobal;

	  if (Lpuart1.messagelength >= 7)
	  {
	    for(Counter.yi=0;Counter.yi < (Lpuart1.messagelength - 7);Counter.yi++)  // add up the incoming bytes in the incomingbuffer0 to check it against the crc
	    {
	      if(Counter.yi < MAX_LPUART1_ARRAY) // protection
	      {
	        Lpuart1.crcset += Lpuart1.payload[Counter.yi];
	      }
	    }
	  }

	  //HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);

	  if(Lpuart1.crcset == Lpuart1.crcmsg) //Uart1.crcmsg)  // if it equals the crc of the message packet
	  {
		Lpuart1.status = FILLED; // immediately block it from reentering uart0 rx until request is processed in main loop
			                          // makes us reply host without making any setting if its outside range
		Lpuart1.got = 0;

		for(Counter.yi=0;Counter.yi < MAX_LPUART1_ARRAY;Counter.yi++)
	    {
		  Lpuart1shadow.payload[Counter.yi] = Lpuart1.payload[Counter.yi];
	    }
		Lpuart1shadow.messageid = Lpuart1.messageidglobal;

    	//HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);
  	    //HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
	  }
	  else
	  {
		Lpuart1.got = 0;
	  }
	}
    else
    {
      Lpuart1.got = 0;
      Lpuart1.crcmsg = 0;
    }
  }
  else
  {
	Lpuart1.got = 0;
	Lpuart1.crcmsg = 0;
  }
}

