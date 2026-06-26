#include "main.h"
#include "string.h"
#include "stdio.h"
#include "memory.h"

// FUNCTION DEFINITIONS =====================================

// Basic SPI Functions
uint8_t SPI_ReadRegister(uint8_t reg)
{
    uint8_t rx_data[2] = {0};
    uint8_t tx_data[2] = {reg, 0xFF};

    MEM_CS_ENABLE();
    HAL_Delay(1);
    HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, SPI_TIMEOUT);
    HAL_Delay(1);
    MEM_CS_DISABLE();
    return rx_data[1];
}

void SPI_WriteRegister(uint8_t reg, uint8_t data)
{
    uint8_t tx_data[2] = {reg, data};
    MEM_CS_ENABLE();
    HAL_Delay(1);
    HAL_SPI_Transmit(&hspi1, tx_data, 2, SPI_TIMEOUT);
    HAL_Delay(1);
    MEM_CS_DISABLE();
}

// Memory Operation Functions
HAL_StatusTypeDef SPI_Memory_Write(uint32_t address, uint8_t* data, uint16_t length)
{
    if (data == NULL || length == 0 || address >= TOTAL_SIZE) {
        return HAL_ERROR;
    }

    // Prevents overwriting the start of the page due to internal rollover
    if ((address % PAGE_SIZE) + length > PAGE_SIZE) {
        return HAL_ERROR;
    }

    // 1. Enable Writing
    HAL_StatusTypeDef status = SPI_WriteEnable();
    if (status != HAL_OK) return status;

    // 2. Prepare Command + 24-bit Address
    uint8_t cmd[4] = {
    	CMD_PAGE_WRITE, // 0x02 (Auto-erase + Program)
        (uint8_t)(address >> 16),
        (uint8_t)(address >> 8),
        (uint8_t)(address)
    };

    MEM_CS_ENABLE();

    // 3. Send Command and Address
    status = HAL_SPI_Transmit(&hspi1, cmd, 4, SPI_TIMEOUT);

    if (status == HAL_OK) {
        // Send data payload. Chunking is removed for speed,
        // but CS stays LOW throughout to ensure validity.
        status = HAL_SPI_Transmit(&hspi1, data, length, SPI_TIMEOUT);
    }

    // 4. Rising edge of CS triggers the internal write cycle
    MEM_CS_DISABLE();

    if (status != HAL_OK) return status;

    // 5. Poll WIP bit until internal 2ms cycle is complete
    return SPI_WaitForWriteComplete();
}

HAL_StatusTypeDef SPI_Memory_Read(uint32_t address, uint8_t* data, uint16_t length)
{
    if (data == NULL || length == 0 || address >= TOTAL_SIZE) {
        return HAL_ERROR;
    }

    uint8_t cmd[4] = {
        CMD_READ_DATA, // 0x03
        (uint8_t)(address >> 16),
        (uint8_t)(address >> 8),
        (uint8_t)(address)
    };

    MEM_CS_ENABLE();

    // Send Command and Address
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, cmd, 4, SPI_TIMEOUT);

    if (status == HAL_OK) {
        // Read the entire data block in one burst.
        // The M95P32 increments the address automatically.
        status = HAL_SPI_Receive(&hspi1, data, length, SPI_TIMEOUT);
    }

    MEM_CS_DISABLE();

#if DEBUG_SENSOR
    if (status != HAL_OK) {
    	SendString((uint8_t*)"\nMemory read failed\r\n");
    }
#endif
    return status;
}

HAL_StatusTypeDef SPI_EraseSectorSafe(uint32_t address)
{
    if (address >= TOTAL_SIZE) return HAL_ERROR;

    address = address & ~(SECTOR_SIZE - 1);

    HAL_StatusTypeDef status = SPI_WriteEnable();
    if (status != HAL_OK) return status;

    uint8_t cmd[4] = {
        CMD_SECTOR_ERASE,
        (uint8_t)(address >> 16),
        (uint8_t)(address >> 8),
        (uint8_t)(address)
    };

    MEM_CS_ENABLE();
    status = HAL_SPI_Transmit(&hspi1, cmd, 4, SPI_TIMEOUT);
    MEM_CS_DISABLE();

    if (status != HAL_OK) return status;
    return SPI_WaitForWriteComplete();
}

HAL_StatusTypeDef SPI_UpdateData(uint32_t address, uint8_t* data, uint16_t length)
{
    // 1. Basic Parameter Validation
    if (data == NULL || length == 0 || address >= TOTAL_SIZE) {
        return HAL_ERROR;
    }

    // 2. Physical Page Boundary Check (M95P32 pages are 512 bytes)
    // Ensures the update is contained within a single physical page.
    uint32_t page_start = address & ~(PAGE_SIZE - 1);
    if ((address % PAGE_SIZE) + length > PAGE_SIZE) {
        return HAL_ERROR; // Update crosses a 512-byte boundary
    }

    // 3. Read the existing 512-byte page into a temporary buffer
    // This preserves the data in the page that we are not currently modifying.
    uint8_t page_buffer[PAGE_SIZE];
    HAL_StatusTypeDef status = SPI_Memory_Read(page_start, page_buffer, PAGE_SIZE);
    if (status != HAL_OK) {
        return status;
    }

    // 4. Modify the specific bytes within the local RAM buffer
    uint32_t offset = address - page_start;
    memcpy(&page_buffer[offset], data, length);

    /*
     * 5. Write the modified 512-byte buffer back to the chip.
     *
     * By calling your SPI_Memory_Write (which uses Opcode 02h):
     * - The chip performs an internal auto-erase of this specific page.
     * - The rest of the 4KB sector remains untouched.
     * - No manual Sector Erase command is required.
     */
    status = SPI_Memory_Write(page_start, page_buffer, PAGE_SIZE);

    return status;
}

// Helper Functions
HAL_StatusTypeDef SPI_WriteEnable(void)
{
    uint8_t cmd = CMD_WRITE_ENABLE;
    MEM_CS_ENABLE();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, &cmd, 1, SPI_TIMEOUT);
    MEM_CS_DISABLE();

    HAL_Delay(1);
    uint8_t status_reg = SPI_ReadStatusRegister1();
    if (!(status_reg & STATUS_WEL)) {
#if DEBUG_SENSOR
    	SendString((uint8_t*)"\nWrite enable failed - WEL not set\r\n");
#endif
        return HAL_ERROR;
    }
    return status;
}

HAL_StatusTypeDef SPI_WaitForWriteComplete(void)
{
  uint32_t start_time = HAL_GetTick();
  while ((HAL_GetTick() - start_time) < 100) // 100ms timeout is very safe
  {
    if (!(SPI_ReadStatusRegister1() & STATUS_WIP))
    {
      return HAL_OK;
    }
    // No delay needed here; just loop until bit 0 is cleared
  }
#if DEBUG_SENSOR
  SendString((uint8_t*)"\nWrite complete timeout\r\n");
#endif
  return HAL_TIMEOUT;
}

void SPI_WriteDisable(void)
{
    uint8_t cmd = CMD_WRITE_DISABLE;
    MEM_CS_ENABLE();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, SPI_TIMEOUT);
    MEM_CS_DISABLE();
}

uint8_t SPI_ReadStatusRegister1(void)
{
    uint8_t cmd = CMD_READ_STATUS_REG1;
    uint8_t status = 0;
    MEM_CS_ENABLE();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, SPI_TIMEOUT);
    HAL_SPI_Receive(&hspi1, &status, 1, SPI_TIMEOUT);
    MEM_CS_DISABLE();
    return status;
}

// UART Helper Functions
void ByteToHex(uint8_t byte, uint8_t* hexStr)
{
    const uint8_t hex_chars[] = "0123456789ABCDEF";
    hexStr[0] = hex_chars[(byte >> 4) & 0x0F];
    hexStr[1] = hex_chars[byte & 0x0F];
    hexStr[2] = '\0';
}

void SendString(uint8_t* str)
{
#if DEBUG_SENSOR
    uint16_t len = 0;
    while(str[len] != '\0') len++;

#ifdef POGO_LPUART1_SEL  // Pogo version
      Send_Data(str, len);
#else // IR version
      for(uint16_t f = 0; f < len; f++)
      {
        // Use the index 'f' to stay at the correct offset
    	Send_Data(&str[f], 1);
      }
#endif
    // Remove HAL_Delay(10)
#else
    (void)str;
#endif
}

void Uint32ToHexString(uint32_t value, uint8_t* output)
{
    // Format as 8-digit hex (e.g., "0123ABCD")
    const char hex_digits[] = "0123456789ABCDEF";
    for(int i = 7; i >= 0; i--) {
        output[i] = hex_digits[value & 0x0F];
        value >>= 4;
    }
    output[8] = '\0';  // Null terminate
}

// Debug Memory Function
void DebugMemory(void)
{
#if DEBUG_SENSOR
    uint32_t test_address = 0x1000;
    uint8_t original_data[2] = {0xAA, 0x55};
    uint8_t updated_data[2] = {0xBB, 0x66};
    uint8_t read_back[2] = {0};

    SendString((uint8_t*)"\n=== Memory Debug Test ===\r\n");

    // 1. Erase sector first
    SendString((uint8_t*)"\nErasing sector 0x");
    uint8_t hex_buf[14];
    ByteToHex((test_address >> 16) & 0xFF, &hex_buf[0]);
    ByteToHex((test_address >> 8) & 0xFF, &hex_buf[2]);
    ByteToHex(test_address & 0xFF, &hex_buf[4]);
    hex_buf[6] = '-';
    ByteToHex(((test_address + SECTOR_SIZE - 1) >> 16) & 0xFF, &hex_buf[7]);
    ByteToHex(((test_address + SECTOR_SIZE - 1) >> 8) & 0xFF, &hex_buf[9]);
    ByteToHex((test_address + SECTOR_SIZE - 1) & 0xFF, &hex_buf[11]);
    hex_buf[13] = '\0';
    SendString(hex_buf);
    SendString((uint8_t*)"\r\n");

    if (SPI_EraseSectorSafe(test_address) != HAL_OK) {
    	SendString((uint8_t*)"\nErase FAILED!\r\n");
        return;
    }
    SendString((uint8_t*)"\nErase successful!\r\n");

    // 2. Write original data
    SendString((uint8_t*)"Writing bytes: ");
    ByteToHex(original_data[0], hex_buf); SendString(hex_buf); SendString((uint8_t*)" ");
    ByteToHex(original_data[1], hex_buf); SendString(hex_buf);
    SendString((uint8_t*)" to address 0x");
    ByteToHex((test_address >> 16) & 0xFF, &hex_buf[0]);
    ByteToHex((test_address >> 8) & 0xFF, &hex_buf[2]);
    ByteToHex(test_address & 0xFF, &hex_buf[4]);
    hex_buf[6] = '\0';
    SendString(hex_buf);
    SendString((uint8_t*)"\r\n");

    if (SPI_Memory_Write(test_address, original_data, 2) != HAL_OK) {
    	SendString((uint8_t*)"\nWrite FAILED!\r\n");
        return;
    }
    SendString((uint8_t*)"\nWrite successful!\r\n");

    // 3. Read back
    if (SPI_Memory_Read(test_address, read_back, 2) != HAL_OK) {
    	SendString((uint8_t*)"\nRead FAILED!\r\n");
        return;
    }
    SendString((uint8_t*)"\nRead back: ");
    ByteToHex(read_back[0], hex_buf); SendString(hex_buf); SendString((uint8_t*)" ");
    ByteToHex(read_back[1], hex_buf); SendString(hex_buf);
    SendString((uint8_t*)" - ");
    SendString((memcmp(original_data, read_back, 2) == 0) ?
               (uint8_t*)"MATCH!\r\n" : (uint8_t*)"MISMATCH!\r\n");

    // 4. Update data
    SendString((uint8_t*)"Updating to: ");
    ByteToHex(updated_data[0], hex_buf); SendString(hex_buf); SendString((uint8_t*)" ");
    ByteToHex(updated_data[1], hex_buf); SendString(hex_buf);
    SendString((uint8_t*)"\r\n");

    if (SPI_UpdateData(test_address, updated_data, 2) != HAL_OK) {
    	SendString((uint8_t*)"Update FAILED!\r\n");
        return;
    }
    SendString((uint8_t*)"Update successful!\r\n");

    // 5. Read final result
    if (SPI_Memory_Read(test_address, read_back, 2) != HAL_OK) {
    	SendString((uint8_t*)"Final read FAILED!\r\n");
        return;
    }
    SendString((uint8_t*)"Final read: ");
    ByteToHex(read_back[0], hex_buf); SendString(hex_buf); SendString((uint8_t*)" ");
    ByteToHex(read_back[1], hex_buf); SendString(hex_buf);
    SendString((uint8_t*)" - ");
    SendString((memcmp(updated_data, read_back, 2) == 0) ?
               (uint8_t*)"MATCH!\r\n" : (uint8_t*)"MISMATCH!\r\n");

    SendString((uint8_t*)"=== Test Complete ===\r\n\r\n");
#endif
}

// MT ----------------------------------------------
uint16_t Mem_CalculateSector(uint32_t address)
{
  return (address / 512);  // returns the sector which corresponds to the
}

void Mem_ClearSector(uint32_t address)
{
  if (SPI_EraseSectorSafe(address) != HAL_OK)
  {
#if DEBUG_SENSOR
	SendString((uint8_t*)"Erase FAILED!\r\n");
#endif
    return;
  }
#if DEBUG_SENSOR
  SendString((uint8_t*)"Erase successful!\r\n");
#endif
}

void Mem_WriteData(uint32_t address, uint8_t *data, uint16_t length)
{
  if (SPI_Memory_Write(address, data, length) != HAL_OK)
  {
#if DEBUG_SENSOR
	SendString((uint8_t*)"Write FAILED!\r\n");
#endif
    return;
  }
#if DEBUG_SENSOR
  SendString((uint8_t*)"Write successful!\r\n");
#endif
}

void Mem_ReadData(uint32_t address, uint8_t *data, uint16_t length)
{
  if (SPI_Memory_Read(address, data, length) != HAL_OK)
  {
    //SendString(&hlpuart1, (uint8_t*)"Read FAILED!\r\n");  // We will need to re-enable this later
    return;
  }
  //SendString(&hlpuart1, (uint8_t*)"Read successful!\r\n"); // We will need to re-enable this later
}

void Mem_UpdateData(uint32_t address, uint8_t *data, uint16_t length)
{
  if (SPI_UpdateData(address, data, length) != HAL_OK)
  {
#if DEBUG_SENSOR
	SendString((uint8_t*)"Update FAILED!\r\n");
#endif
    return;
  }
#if DEBUG_SENSOR
  SendString((uint8_t*)"Update successful!\r\n");
#endif
}

void DebugMemory2(void)
{
#if DEBUG_SENSOR
  uint32_t test_address = 0x1000;
  uint8_t original_data[2] = {0xAA, 0x55};
  uint8_t updated_data[2] = {0xBB, 0x66};
  uint8_t read_back[2] = {0};
  uint8_t hex_buf[9];

  Mem_ClearSector(0x00001000);

  Mem_WriteData(test_address, original_data, 2);

  Mem_ReadData(test_address, read_back, 2);
  SendString((uint8_t*)"Read back: ");
  ByteToHex(read_back[0], hex_buf); SendString(hex_buf); SendString((uint8_t*)" ");
  ByteToHex(read_back[1], hex_buf); SendString(hex_buf);
  SendString((uint8_t*)" - ");
  SendString((memcmp(original_data, read_back, 2) == 0) ?
             (uint8_t*)"MATCH!\r\n" : (uint8_t*)"MISMATCH!\r\n");




  Mem_UpdateData(test_address, updated_data, 2);

  Mem_ReadData(test_address, read_back, 2);
  SendString((uint8_t*)"Final read: ");
  ByteToHex(read_back[0], hex_buf); SendString(hex_buf); SendString((uint8_t*)" ");
  ByteToHex(read_back[1], hex_buf); SendString(hex_buf);
  SendString((uint8_t*)" - ");
  SendString((memcmp(updated_data, read_back, 2) == 0) ?
             (uint8_t*)"MATCH!\r\n" : (uint8_t*)"MISMATCH!\r\n");

  SendString((uint8_t*)"=== Test Complete ===\r\n\r\n\n");
#endif
}

void DebugMemory3(void)
{
#if DEBUG_SENSOR
  uint32_t test_address = 0x0000;
  uint8_t original_data[2] = {0xAA, 0x55};
  uint8_t updated_data[2] = {0xBB, 0x66};
  uint8_t read_back[2] = {0};
  uint8_t hex_buf[9];
  uint8_t pagedata_w[512];
  uint8_t pagedata_r[512];



  for(int i=0; i < 512; i++)
  {
	pagedata_w[i] = 0;
  }

  // Device - 123
  pagedata_w[0] = 1;
  pagedata_w[1] = 2;
  pagedata_w[2] = 3;

  // Serial # - 567
  pagedata_w[3] = 5;
  pagedata_w[4] = 6;
  pagedata_w[5] = 7;

  // Usage
  pagedata_w[6] = 8;
  pagedata_w[7] = 9;


  // Clear the sector
  Mem_ClearSector(0x00000000);

  // Fill Device Info - Address : 0x00000000
  Mem_WriteData(test_address, pagedata_w, 8);

  // Fill File List - 1 File (File A)


  // File Data - File A Data


  // Read back the data saved above
  Mem_ReadData(test_address, pagedata_r, 8);
  SendString((uint8_t*)"Final read: ");

  for(int i=0; i < 8; i++)
  {
    ByteToHex(pagedata_r[i], hex_buf); SendString(hex_buf); SendString((uint8_t*)" ");
  }
  SendString((memcmp(pagedata_w, pagedata_r, 8) == 0) ?
             (uint8_t*)"MATCH!\r\n" : (uint8_t*)"MISMATCH!\r\n");

  SendString((uint8_t*)"=== Test Complete ===\r\n\r\n\n");
#endif
}

void FillPage(void)
{
  // for debug printing
  uint8_t testbyte[9] = {0};  // Need 9 bytes for 8 hex digits + null terminator
  uint32_t rec[1] = {0};

  uint8_t hi[1] = {0};
  uint8_t lo[1] = {0};

  // Calculate record offset in the page (i.e. start of existing page)
  uint16_t recordoffset = (BYTES_PER_RECORD * RecordHeader.recordcount);

  // protection - final record must have a starting address of 480 and no higher
  //            - if its higher we go to zero
  if(recordoffset > (PAGEDATA_W_ARRAY - BYTES_PER_RECORD))  // 512 - 32 = 480
  {
    recordoffset = 0;
  }

  RTC_ReadTime();
  // ToDo: Insert the error into an errorcode variable stored in Page 0, Record 0 (bytes X to XX) of a given recording
  uint8_t error = Acquire_RawPressureReading();  // Fetch raw digital pressure (D1) and raw digital temperature data (D2) from sensor

  if(RecordHeader.recordcount != 0) // on every record that is not the first
  {
	// Time (8 bytes)
	EEPROMRecord.time[0] = Time.read_year;          // 0-99 (00-99)
	EEPROMRecord.time[1] = Time.read_month;      // 1-12
	EEPROMRecord.time[2] = Time.read_day;           // 1-31
	EEPROMRecord.time[3] = Time.read_hour;         // 0-23 (24-hour format)
	EEPROMRecord.time[4] = Time.read_minute;     // 0-59
	EEPROMRecord.time[5] = Time.read_second;     // 0-59
	EEPROMRecord.time[6] = Time.read_ampm;      // 0=AM, 1=PM (optional, since we're using 24-hour)
	EEPROMRecord.time[7] = Time.read_weekday;   // 1=Monday, 7=Sunday
    PageData.pagedata_w[recordoffset + 0] = EEPROMRecord.time[0];
    PageData.pagedata_w[recordoffset + 1] = EEPROMRecord.time[1];
    PageData.pagedata_w[recordoffset + 2] = EEPROMRecord.time[2];
    PageData.pagedata_w[recordoffset + 3] = EEPROMRecord.time[3];
    PageData.pagedata_w[recordoffset + 4] = EEPROMRecord.time[4];
    PageData.pagedata_w[recordoffset + 5] = EEPROMRecord.time[5];
    PageData.pagedata_w[recordoffset + 6] = EEPROMRecord.time[6];
    PageData.pagedata_w[recordoffset + 7] = EEPROMRecord.time[7];

    // Pressure (6 bytes) - raw value, conversion on software side.
    EEPROMRecord.pressure[0] = (D1 >> 24) & 0xFF;
    EEPROMRecord.pressure[1] = (D1 >> 16) & 0xFF;
    EEPROMRecord.pressure[2] = (D1 >> 8) & 0xFF;
    EEPROMRecord.pressure[3] = (D1) & 0xFF;
    EEPROMRecord.pressure[4] = 0;
    EEPROMRecord.pressure[5] = 0;
    PageData.pagedata_w[recordoffset + 8] = EEPROMRecord.pressure[0];
    PageData.pagedata_w[recordoffset + 9] = EEPROMRecord.pressure[1];
    PageData.pagedata_w[recordoffset + 10] = EEPROMRecord.pressure[2];
    PageData.pagedata_w[recordoffset + 11] = EEPROMRecord.pressure[3];
    PageData.pagedata_w[recordoffset + 12] = EEPROMRecord.pressure[4];
    PageData.pagedata_w[recordoffset + 13] = EEPROMRecord.pressure[5];
    // Temperature  (6 bytes)  - raw value, conversion on software side.
    EEPROMRecord.temperature[0] = (D2 >> 24) & 0xFF;
    EEPROMRecord.temperature[1] = (D2 >> 16) & 0xFF;
    EEPROMRecord.temperature[2] = (D2 >> 8) & 0xFF;
    EEPROMRecord.temperature[3] = (D2) & 0xFF;
    EEPROMRecord.temperature[4] = 0;
    EEPROMRecord.temperature[5] = 0;
    PageData.pagedata_w[recordoffset + 14] = EEPROMRecord.temperature[0];
    PageData.pagedata_w[recordoffset + 15] = EEPROMRecord.temperature[1];
    PageData.pagedata_w[recordoffset + 16] = EEPROMRecord.temperature[2];
    PageData.pagedata_w[recordoffset + 17] = EEPROMRecord.temperature[3];
    PageData.pagedata_w[recordoffset + 18] = EEPROMRecord.temperature[4];
    PageData.pagedata_w[recordoffset + 19] = EEPROMRecord.temperature[5];
    // Conductivity  (6 bytes) -- replace what's below with what needs to be replaced later
    EEPROMRecord.conductivity[0] = (Sensor.conductivity >> 24) & 0xFF;
    EEPROMRecord.conductivity[1] = (Sensor.conductivity >> 16) & 0xFF;
    EEPROMRecord.conductivity[2] = (Sensor.conductivity >> 8) & 0xFF;
    EEPROMRecord.conductivity[3] = (Sensor.conductivity) & 0xFF;
    EEPROMRecord.conductivity[4] = 0;
    EEPROMRecord.conductivity[5] = 0;
    PageData.pagedata_w[recordoffset + 20] = EEPROMRecord.conductivity[0];
    PageData.pagedata_w[recordoffset + 21] = EEPROMRecord.conductivity[1];
    PageData.pagedata_w[recordoffset + 22] = EEPROMRecord.conductivity[2];
    PageData.pagedata_w[recordoffset + 23] = EEPROMRecord.conductivity[3];
    PageData.pagedata_w[recordoffset + 24] = EEPROMRecord.conductivity[4];
    PageData.pagedata_w[recordoffset + 25] = EEPROMRecord.conductivity[5];
    // Reserved  (6 bytes)
    EEPROMRecord.reserved[0] = (Sensor.reserved >> 24) & 0xFF;
    EEPROMRecord.reserved[1] = (Sensor.reserved >> 16) & 0xFF;
    EEPROMRecord.reserved[2] = (Sensor.reserved >> 8) & 0xFF;
    EEPROMRecord.reserved[3] = (Sensor.reserved) & 0xFF;
    EEPROMRecord.reserved[4] = 0;
    EEPROMRecord.reserved[5] = 0;
    PageData.pagedata_w[recordoffset + 26] = EEPROMRecord.reserved[0];
    PageData.pagedata_w[recordoffset + 27] = EEPROMRecord.reserved[1];
    PageData.pagedata_w[recordoffset + 28] = EEPROMRecord.reserved[2];
    PageData.pagedata_w[recordoffset + 29] = EEPROMRecord.reserved[3];
    PageData.pagedata_w[recordoffset + 30] = EEPROMRecord.reserved[4];
    PageData.pagedata_w[recordoffset + 31] = EEPROMRecord.reserved[5];

    //HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);

    // Debug printouts to Terminal program.  DISABLE when using.
    SendString((uint8_t*)" One Record Accumulated in PWB!\r\n");
  }
  else // Record Header (in the position of the first record of every page)
  {
	// Clear the array that is to hold the pagedata_w values to be written first
	Clear_PagedataWrite_Array();

	RecordHeader.recordcount = 0;

	// Initialize all bytes of Record 0 (32 bytes) with 0's.
	for(uint8_t i = 0; i < BYTES_PER_RECORD; i++)
	{
      PageData.pagedata_w[i] = 0;
	}

    // Update for the next header
	// Note: These updates have to be performed somewhere in the code - likely individually
	PageData.pagedata_w[0] = RLE;                                   // Magic Number: High
	PageData.pagedata_w[1] = RTX;                                   // Magic Number: Low
	PageData.pagedata_w[2] = RecordHeader.filenumber;               // 0 to 3
	PageData.pagedata_w[3] = (RecordHeader.seqnumber >> 8) & 0xFF;  // Increment with every page written (hi)
	PageData.pagedata_w[4] = (RecordHeader.seqnumber) & 0xFF;       // Increment with every page written (lo)
	PageData.pagedata_w[5] = RecordHeader.recordcount;              // Records currently in this page
	PageData.pagedata_w[6] = RecordHeader.status;
	RecordHeader.crc = (RLE + RTX + RecordHeader.filenumber + RecordHeader.seqnumber + RecordHeader.recordcount + RecordHeader.status); // Header Checksum
	PageData.pagedata_w[7] = RecordHeader.crc;                      // Header Checksum

	// Store the MS5837 pressure sensor ROM variables of the sensor in the initial page meta data header of the recording
	if(RecordState.totalrecordcount == 0)
	{
		// Create the record for the file slot we are using in SHOWFILES_SECTOR, SHOWFILES_PAGE (Sector 1, 0)
		Update_ShowFilesQuadrant();

		// The seven C[] calibration values from the sensor
	    for (uint8_t i = 0; i < 7; i++) {
	        PageData.pagedata_w[16 + (2 * i)]     = (uint8_t)(C[i] >> 8);
	        PageData.pagedata_w[16 + (2 * i) + 1] = (uint8_t)(C[i] & 0xFF);
	    }
	}

	//HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
	// Debug printouts to Terminal program.  DISABLE when using.
	SendString((uint8_t*)"\n-=Page Header Accumulated in PWB!=-\r\n");
  }

  // Debug printouts to Terminal program.  DISABLE when using.
  hi[0] = ((recordoffset + 32) >> 8) & 0xFF;
  lo[0] = (recordoffset + 32) & 0xFF;

  SendString((uint8_t*)"recordoffset: ");

  ByteToHex(hi[0], testbyte);
  SendString(testbyte);
  SendString((uint8_t*)" ");

  ByteToHex(lo[0], testbyte);
  SendString(testbyte);
  SendString((uint8_t*)" ");

  rec[0] = RecordHeader.recordcount;
  ByteToHex(rec[0], testbyte);
  SendString(testbyte);

  // Increment the count of records in the page
  RecordHeader.recordcount = RecordHeader.recordcount + 1;
  // update the number of records being written to the Page header (i.e. record position 0)
  PageData.pagedata_w[5] = RecordHeader.recordcount;   // Update the number of records in the page
  // update the total count of committed records in the file slot
  RecordState.totalrecordcount = RecordState.totalrecordcount + 1;

  //if((recordoffset + 32) == PAGEDATA_W_ARRAY)  // 512 == 512 meaning page buffer is filled, ready to write to page
  if(RecordHeader.recordcount == RECORDS_PER_PAGE)
  {
	// Debug printouts to Terminal program.  DISABLE when using.
	// Page is now fully prepared and ready to be added to the memory.
	SendString((uint8_t*)"\nGetting ready to Commit the Page !!!\r\n");

	// Increment the count of sequences (i.e. page number)
	// Note: This has to be reset to 0 somewhere - same place the file number is to be updated.
	RecordHeader.seqnumber = RecordHeader.seqnumber + 1;

    // 1. Set write to page flag.
	RecordState.saverecord = RECORDSTATE_WRITEPAGE;

	CommitPage();
	//HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);

	// Debug printouts to Terminal program.  DISABLE when using.
	SendString((uint8_t*)"-=Page Committed LESSTHAN30S aka Occasional!=-\r\n");
	SendString((uint8_t*)"\n-------------------------------\r\n\n");

	// if -- if Time Stop flag is active in Timer mode (or Ring Switch Off flag if in Ring Switch mode)
	// RecordState.saverecord set to NOTSTARTED (i.e. stop)
	if(ReedSwitch.state == DEACTIVATED) // we use the reed switch state for now instead of timer based
	{
	  HaltRecording();
    }
  }
  else
  {
    if(RecordState.savetime == GREATERTHAN30S)
    {
      // 1. Set update to page flag. Midway update of records
      RecordState.saverecord = RECORDSTATE_WRITEPAGE; // was RECORDSTATE_UPDATEPAGE;
                                                                                          // we should be able to use writepage instead of update
                                                                                          // since we have the entire pagedata_w array in tact
  	  CommitPage();
  	  //HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);

  	  // Debug printouts to Terminal program.  DISABLE when using.
  	  SendString((uint8_t*)"-=Page Committed GREATERTHAN30S aka Frequent!=-\r\n");

  	  // if -- if Time Stop flag is active in Timer mode (or Ring Switch Off flag if in Ring Switch mode)
  	  // RecordState.saverecord set to NOTSTARTED (i.e. stop)
  	  if(ReedSwitch.state == DEACTIVATED) // we use the reed switch state for now instead of timer based
  	  {
  	    HaltRecording();
  	  }
    }
    // else if -- if Time Stop flag is active in Timer mode (or Ring Switch Off flag if in Ring Switch mode)
    // update number of records in page (record position 0)
    // set RecordState.saverecord = RECORDSTATE_UPDATEPAGE
    // and Commit records you have to the EEPROM page and set an OKtoStop flag = 1
    else if(ReedSwitch.state == DEACTIVATED) // we use the reed switch state for now instead of timer based
    {
      // 1. Set update to page flag. Midway update of records
      //RecordState.saverecord = RECORDSTATE_UPDATEPAGE;
      RecordState.saverecord = RECORDSTATE_WRITEPAGE;  // we should be able to use writepage instead of update
                                                                                           // since we have the entire pagedata_w array in tact
      CommitPage();
      HaltRecording();
    }
  }
  // Protection
  // Should have already been set to zero on entry into if loop above.
  // But if not, we have this as a protection here.
  RecordHeader.recordcount = (RecordHeader.recordcount % RECORDS_PER_PAGE);
}

void HaltRecording(void)
{
	RecordState.started = RECORDING_NOTSTARTED; // Stop recording, we exit the recording in the main loop
	SendString((uint8_t*)"-=Final page of record written.=-\r\n");
}

void CommitPage(void)
{
  // for debug printing
  uint8_t testbyte[9] = {0};  // Need 9 bytes for 8 hex digits + null terminator
  uint32_t rec[1] = {0};

  if(RecordState.saverecord != RECORDSTATE_DONOTHING)
  {
	// Calculates where to save the page.  Needs the (next avaialble) free sector.
  	DeterminePageAddress();
  	// Update
    if(RecordState.saverecord == RECORDSTATE_UPDATEPAGE)  // eliminate this
    {
	  // 2. Update page
	  Mem_UpdateData(RecordState.pageaddress, PageData.pagedata_w, PAGEDATA_W_ARRAY);
#if DEBUG_SENSOR
	  SendString((uint8_t*)"* Page : Updated.\r\n");
#endif
	  // Update Record Ledger in ShowFiles Page in its respective Quadrant (that corresponds to its File Slot)
	  Update_ShowFilesQuadrant();
    }
    // Write (whole page)
    else if(RecordState.saverecord == RECORDSTATE_WRITEPAGE)
    {
	  // 2. Write to page
	  // The PGWR (0x02) command will erase and write the page
	  // so we do not need to erase.
	  Mem_WriteData(RecordState.pageaddress, PageData.pagedata_w, PAGEDATA_W_ARRAY);
#if DEBUG_SENSOR
	  SendString((uint8_t*)"* Page : Write.\r\n");
#endif
	  // Update Record Ledger in ShowFiles Page in its respective Quadrant (that corresponds to its File Slot)
	  Update_ShowFilesQuadrant();
    }

	// Read and print out the contents of the page.
	PrintOutRecord(RecordState.sector, RecordState.page);

	// Advance the page, and the sector if need be.
	IncrementPageSector();
  }
}

void Update_ShowFilesQuadrant(void)
{
    // 1. Calculate linear physical address (M95P32 pages are exactly 512 bytes)
    uint32_t page_address = (BYTES_PER_SECTOR * SHOWFILES_SECTOR) + (BYTES_PER_PAGE * SHOWFILES_PAGE);

    // 2. Calculate offset (assuming 4 slots of 128 bytes fit into one 512-byte page)
    uint16_t offset = (RecordState.fileslot * BYTES_PER_QUADRANT);

    // Safety: Ensure we don't write past the end of the 512-byte pagedata_r array
    if (offset + 13 >= BYTES_PER_PAGE) {
#if DEBUG_SENSOR
    	SendString((uint8_t*)"\nError: Fileslot index out of bounds for page\r\n");
#endif
        return;
    }

    // 3. READ: Retrieve the full 512-byte page into RAM
    // Ensure this function fills PageData.pagedata_r with 512 bytes
    SaveRetrievedPagetoMemory(SHOWFILES_SECTOR, SHOWFILES_PAGE, BREAK_INTO_QUADRANT_NO);

    // 4. MODIFY: Update the Total Record Count bytes in the RAM buffer
    PageData.pagedata_r[offset + 10] = (uint8_t)(RecordState.totalrecordcount >> 24);
    PageData.pagedata_r[offset + 11] = (uint8_t)(RecordState.totalrecordcount >> 16);
    PageData.pagedata_r[offset + 12] = (uint8_t)(RecordState.totalrecordcount >> 8);
    PageData.pagedata_r[offset + 13] = (uint8_t)(RecordState.totalrecordcount);

    // 5. MODIFY: Update these in the RAM buffer once (on the first page header) as it never changes for the recording
    if(RecordState.totalrecordcount == 0) // first time in
    {
    	// Reserved
    	PageData.pagedata_r[offset + 0] = FILE_SLOT_OCCUPIED; // Reserved 0 - Temporary (fixed)
    	// File Index
    	PageData.pagedata_r[offset + 1] = 0x00;
    	// File name
        PageData.pagedata_r[offset + 2] = 0x46;  // F
        PageData.pagedata_r[offset + 3] = 0x49;  // I
        PageData.pagedata_r[offset + 4] = 0x4C;  // L
        PageData.pagedata_r[offset + 5] = 0x45;  // E
        PageData.pagedata_r[offset + 6] = 0x5F;  // -
        PageData.pagedata_r[offset + 7] = 0x30;  // 0
        PageData.pagedata_r[offset + 8] = 0x30;  // 0
        PageData.pagedata_r[offset + 9] = 0x30 + RecordState.fileslot;  // the  file slot to which we are recording

        ShowFiles.filecreationtime[0] = Time.read_year;          // 0-99 (00-99)
        ShowFiles.filecreationtime[1] = Time.read_month;      // 1-12
        ShowFiles.filecreationtime[2] = Time.read_day;           // 1-31
        ShowFiles.filecreationtime[3] = Time.read_hour;         // 0-23 (24-hour format)
        ShowFiles.filecreationtime[4] = Time.read_minute;     // 0-59
        ShowFiles.filecreationtime[5] = Time.read_second;     // 0-59
        ShowFiles.filecreationtime[6] = Time.read_ampm;      // 0=AM, 1=PM (optional, since we're using 24-hour)
        ShowFiles.filecreationtime[7] = Time.read_weekday;   // 1=Monday, 7=Sunday
        for(uint8_t e=0; e < SHOWFILES_FILECREATIONTIME_ARRAY; e++)
        {
        	PageData.pagedata_r[offset + 14 + e] = ShowFiles.filecreationtime[e];
        }
    }

    // 6. WRITE: Send the modified 512-byte page back to the EEPROM
    // This utilizes your SPI_Memory_Write (Opcode 02h) which handles the auto-erase.
    Mem_WriteData(page_address, PageData.pagedata_r, PAGEDATA_R_ARRAY);

#if DEBUG_SENSOR
    // Debug output
    char msg[64];
    sprintf(msg, "DEBUG: Writing RC=%lu to slot %d at offset+10-13\r\n",
            RecordState.totalrecordcount, RecordState.fileslot);
    SendString((uint8_t*)msg);

    // Also print the actual bytes
    sprintf(msg, "DEBUG: Bytes: %02X %02X %02X %02X\r\n",
            PageData.pagedata_r[offset + 10],
            PageData.pagedata_r[offset + 11],
            PageData.pagedata_r[offset + 12],
            PageData.pagedata_r[offset + 13]);
    SendString((uint8_t*)msg);
#endif
}

void DeterminePageAddress(void)
{
  // for debug printing
  uint8_t testbyte[9] = {0};  // Need 9 bytes for 8 hex digits + null terminator
  uint32_t rec[1] = {0};

#if DEBUG_SENSOR
  SendString((uint8_t*)"\n\nDP_RecordState.sector: \r");
  rec[0] = RecordState.sector;
  Uint32ToHexString(rec[0], (uint8_t*)testbyte);
  SendString((uint8_t*)testbyte);
  SendString((uint8_t*)"\r\n");

  SendString((uint8_t*)"\nDP_RecordState.page: \r");
  rec[0] = RecordState.page;
  ByteToHex((uint8_t)rec[0], testbyte);
  SendString(testbyte);

  SendString((uint8_t*)"\n");
#endif

  // since sector and page are likely unsigned integers (uint32_t or uint8_t), they can never be less than 0.
  // so that's taken care of.
  if ((RecordState.sector < TOTAL_SECTORS) && (RecordState.page < PAGES_PER_SECTOR))
  {
    RecordState.pageaddress = (BYTES_PER_SECTOR * RecordState.sector) + (BYTES_PER_PAGE * RecordState.page);
  }
  else
  {
#if DEBUG_SENSOR
	SendString((uint8_t*)"\nPageAddress Boundary Error !\n");
#endif
    // RecordState.pageaddress will thus remain unchanged.  i.e. over-writing the same page.
  }
}

void PrintOutRecord(uint32_t sector, uint8_t page)
{
#if DEBUG_SENSOR
  uint8_t hex_buf[QUADRANTS]; // Buffer for "XX " + null terminator

  // Populate pagedata_rq[4][128]
  SaveRetrievedPagetoMemory(sector, page, BREAK_INTO_QUADRANT_YES);

  SendString((uint8_t*)"\r\n--- Read back (32 bytes per record) ---\r\n");

  for(int q = 0; q < QUADRANTS; q++) // 4 Quadrants
  {
    for(int i = 0; i < BYTES_PER_QUADRANT; i++) // 128 Bytes per quadrant
    {
      ByteToHex(PageData.pagedata_rq[q][i], hex_buf);
      SendString(hex_buf);
      SendString((uint8_t*)" ");
      // Check if we have printed 32 bytes (one record)
      // Since i is local to the 128-byte loop, we check (i + 1) % 32
      if ((i + 1) % 32 == 0)
      {
    	SendString((uint8_t*)"\r\n"); // Newline after each 32-byte record
        HAL_Delay(5); // Give the UART buffer time to clear between records
      }
    }
  }
  SendString((uint8_t*)"---------------------------------------\r\n");
#else
  (void)sector;
  (void)page;
#endif
}

void IncrementPageSector(void)
{
  SendString((uint8_t*)"\n\n============================================\n");
  // if a record is being committed,
  if ((RecordState.saverecord == RECORDSTATE_WRITEPAGE) || (RecordState.saverecord == RECORDSTATE_UPDATEPAGE))
  {
    // Read FAT into structure
	// Temporary - calculates page and sector iterations
	RecordState.page++; // Increment page count
	if(RecordState.page >= 8)
	{
	  RecordState.page = 0;
      RecordState.sector++;

      // Currently set to Round Robin mode.
      // Default should be to end the recording (fixed length) once the final sector in the file slot is reached.
      // RecordState.endsector and RecordState.startsector hold the start and end boundaries
	  if(RecordState.sector >= RecordState.endsector)  // Round Robin, data starts at selected file slot sector
 	  {
        if (Sampling.mode == TIME_CONTINUOUSLOOP)
        {
          RecordState.sector = RecordState.startsector; // Return to beginning of file slot sector
        }
        else // TIME_DONOTLOOP (or other modes)
        {
          HaltRecording();
        }
	  }
    }
	// 3. Clear commit to page flag.
	RecordState.saverecord = RECORDSTATE_DONOTHING;
  }
}

void SaveRetrievedPagetoMemory(uint32_t sector, uint8_t page, uint8_t breakup)
{
  if ((sector < TOTAL_SECTORS) && (page < PAGES_PER_SECTOR)) // protection
  {
    uint32_t test_address = (BYTES_PER_SECTOR * sector) + (PAGEDATA_R_ARRAY * page);

    // Clear the array using a local counter
    Clear_PagedataRead_Array();

    Mem_ReadData(test_address, PageData.pagedata_r, PAGEDATA_R_ARRAY);

    if(breakup == BREAK_INTO_QUADRANT_YES)
    {
      // Break the page up into 128 byte quadrants using local counters
      // q = quadrant index (0-3), b = byte index within quadrant (0-127)
      for(int q = 0; q < LOGTRANSMITDATA_QUADRANTS_ARRAY; q++)
      {
        for(int b = 0; b < LOGTRANSMITDATA_QUADRANTBYTES_ARRAY; b++)
        {
          PageData.pagedata_rq[q][b] = PageData.pagedata_r[(q * BYTES_PER_QUADRANT) + b];
        }
      }
    }
  }
  else
  {
#if DEBUG_SENSOR
	SendString((uint8_t*)"\nSaveRetrievedPagetoMemory: sector or page value exceeds limits!");
#endif
  }
}

// Used for printing variables for debugging
void PrintVar(uint8_t var)
{
#if DEBUG_SENSOR
  uint8_t testbyte[1] = {0};
  uint8_t rec[1] = {0};

  rec[0] = var;
  ByteToHex(rec[0], testbyte);
  SendString(testbyte);
#else
  (void)var;
#endif
}

void DebugMemory4(void)
{
  // If start of record, file header should have already been written to first page
  uint32_t test_address = 0x0000;
  uint8_t hex_buf[9];

  // First sector (8 x 512 bytes = 4096 bytes) is reserved for GLOBAL INFO
  // Second sector is reserved for PLAN
  // Third sector is reserved for FAT

  FillPage(); // Gathers record, writes/updates to page as per frequency, auto-increments after commiting to a page

#if DEBUG_SENSOR
  SendString((uint8_t*)"\nWritten...");
#endif

  /*
  test_address = (4096 * RecordState.sector) + (512 * 0);
  Mem_ReadData(test_address, PageData.pagedata_r, 8);



  SendString(&hlpuart1, (uint8_t*)"Written read: ");
  for(int i=0; i < 8; i++)
  {
    ByteToHex(PageData.pagedata_r[i], hex_buf); SendString(&hlpuart1, hex_buf); SendString(&hlpuart1, (uint8_t*)" ");
  }
  SendString(&hlpuart1, (memcmp(PageData.pagedata_w, PageData.pagedata_r, 8) == 0) ?
          (uint8_t*)"MATCH!\r\n" : (uint8_t*)"MISMATCH!\r\n");
  */
}



// Possibly Garbage -- Check
void Mem_Retrieve_GlobalInfo(uint8_t whichchip)
{
  uint32_t test_address = 0x0000;
  uint8_t page_buf[BYTES_PER_PAGE] = {0};

  uint32_t sector = 0;

  // clear PageData.pagedata_r

  test_address = (BYTES_PER_SECTOR * sector) + (BYTES_PER_PAGE * 0);
  // adjust depending on whichchip
  Mem_ReadData(test_address, PageData.pagedata_r, 8);

#if DEBUG_SENSOR
  SendString((uint8_t*)"Written read: ");
  for(int i=0; i < 8; i++)
  {
    ByteToHex(PageData.pagedata_r[i], page_buf); SendString(page_buf); SendString((uint8_t*)" ");
  }
#endif
}

void ClearShowFilesAndMetadata(void)
{
    Clear_PagedataWrite_Array(); // All 0xFF

    // Clear Sector 1 (SHOWFILES)
    for (uint32_t page = 0; page < PAGES_PER_SECTOR; page++) {
        uint32_t address = (BYTES_PER_SECTOR * 1) + (BYTES_PER_PAGE * page);
        Mem_WriteData(address, PageData.pagedata_w, PAGEDATA_W_ARRAY);
    }

    // Clear Sector 2 (Metadata for file slots)
    for (uint32_t page = 0; page < READSPECIFICFILE_FILESLOTS; page++) {
        uint32_t address = (BYTES_PER_SECTOR * 2) + (BYTES_PER_PAGE * page);
        Mem_WriteData(address, PageData.pagedata_w, PAGEDATA_W_ARRAY);
    }

#if DEBUG_SENSOR
    SendString((uint8_t*)"SHOWFILES and Metadata cleared\r\n");
#endif
}

