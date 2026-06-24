#ifndef INC_MEMORY_H
#define INC_MEMORY_H

// SPI Chip Select control
#define MEM_CS_ENABLE() HAL_GPIO_WritePin(CS_MEM_0_BAR_GPIO_Port, CS_MEM_0_BAR_Pin, GPIO_PIN_RESET)
#define MEM_CS_DISABLE() HAL_GPIO_WritePin(CS_MEM_0_BAR_GPIO_Port, CS_MEM_0_BAR_Pin, GPIO_PIN_SET)
// SPI Timeout
#define SPI_TIMEOUT 500

// Memory chip commands
#define CMD_PAGE_WRITE       0x02  // Auto-erase + Program (Use this for your logic)
#define CMD_WRITE_ENABLE         0x06
#define CMD_WRITE_DISABLE        0x04
#define CMD_READ_STATUS_REG1     0x05
#define CMD_READ_STATUS_REG2     0x07
#define CMD_WRITE_STATUS_REG     0x01
#define CMD_READ_DATA            0x03
#define CMD_PAGE_PROGRAM     0x0A  // Program only
#define CMD_SECTOR_ERASE         0x20
#define CMD_BLOCK_ERASE_32K      0x52
#define CMD_BLOCK_ERASE_64K      0xD8
#define CMD_CHIP_ERASE       0xC7  // Standard for this chip
#define CMD_READ_ID              0x9F
#define CMD_DEEP_POWER_DOWN      0xB9
#define CMD_RELEASE_DPD          0xAB
#define CMD_PAGE_ERASE           0xDB

// EEPROM Status register bits
#define STATUS_WIP     0x01
#define STATUS_WEL     0x02

#define PAGE_SIZE         512   // Changed from 256 to 512
#define SECTOR_SIZE     4096  // Correct (4KB)
#define TOTAL_PAGES    8192  // Changed from 16384 to 8192
#define TOTAL_SIZE        (TOTAL_PAGES * PAGE_SIZE)  // Still 4MB

// PROTOTYPE STATEMENTS =====================================

// Forward declarations for functions defined in misc.h (included after memory.h in main.c)
extern void Clear_PagedataWrite_Array(void);
extern void Clear_PagedataRead_Array(void);


// SPI Basic Functions
uint8_t SPI_ReadRegister(uint8_t reg);
void SPI_WriteRegister(uint8_t reg, uint8_t data);
void SPI_EraseSector(uint32_t address);
//uint8_t ReadManufacturerID(void);

// Memory Operations
HAL_StatusTypeDef SPI_Memory_Write(uint32_t address, uint8_t* data, uint16_t length);
HAL_StatusTypeDef SPI_Memory_Read(uint32_t address, uint8_t* data, uint16_t length);
HAL_StatusTypeDef SPI_EraseSectorSafe(uint32_t address);
//HAL_StatusTypeDef SPI_ErasePageSafe(uint32_t address);
HAL_StatusTypeDef SPI_UpdateData(uint32_t address, uint8_t* data, uint16_t length);

// Helper Functions
HAL_StatusTypeDef SPI_WriteEnable(void);
void SPI_WriteDisable(void);
HAL_StatusTypeDef SPI_WaitForWriteComplete(void);
uint8_t SPI_ReadStatusRegister1(void);

// UART Functions
extern void ByteToHex(uint8_t byte, uint8_t* hexStr);
extern void SendString(uint8_t* str);
extern void Uint32ToHexString(uint32_t value, uint8_t* output) ;
//void SendManufacturerID(uint8_t id);

// Debug Functions
void DebugMemory(void);
void DebugMemory2(void);
void DebugMemory3(void);
extern void DebugMemory4(void);
//void SPISendDebugInfo(uint8_t manufacturer_id);
//void SPIReadWriteTest(void);

// MT - Memory Operations - Function Prototypes
uint16_t Mem_CalculateSector(uint32_t address);
void Mem_ClearSector(uint32_t address);
extern void Mem_WriteData(uint32_t address, uint8_t *data, uint16_t length);
extern void Mem_ReadData(uint32_t address, uint8_t *data, uint16_t length);
void Mem_UpdateData(uint32_t address, uint8_t *data, uint16_t length);
void Mem_Retrieve_GlobalInfo(uint8_t whichchip);

// Fill page with data
void ClearShowFilesAndMetadata(void);
void FillPage(void);
void CommitPage();
void DeterminePageAddress(void);
void IncrementPageSector(void);
void HaltRecording(void);

// Debug
void PrintVar(uint8_t var);
extern void PrintOutRecord(uint32_t sector, uint8_t page);
extern void SaveRetrievedPagetoMemory(uint32_t sector, uint8_t page, uint8_t breakup);

// Record Count Ledger
void Update_ShowFilesQuadrant(void);

void RTC_ReadTime(void);
uint8_t Send_Data(uint8_t *data, uint16_t length);
uint8_t Acquire_RawPressureReading(void);
// HANDLES ==================================================
extern SPI_HandleTypeDef hspi1;

#endif /* INC_MEMORY_H */
