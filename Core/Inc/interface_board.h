/*
 * interface_board.h
 */

#ifndef INC_INTERFACE_BOARD_H
#define INC_INTERFACE_BOARD_H

//==============================================================================
// I2C Address Definitions
//==============================================================================
#define INTF_BOARD_I2C_ADDR      0x7F   // 7-bit address
#define INTF_BOARD_I2C_ADDR_HAL (INTF_BOARD_I2C_ADDR << 1)  // 8-bit address = 0xFE

//==============================================================================
// Register Addresses (matches Interface Board's i2c_slave.h)
//==============================================================================
#define INTF_REG_PRESSURE_RAW  0x00   // 3 bytes
#define INTF_REG_TEMP_RAW      0x03   // 3 bytes
#define INTF_REG_AVDD_RAW      0x06   // 3 bytes
#define INTF_REG_STATUS        0x09   // 1 byte
#define INTF_REG_DEVICE_ID     0x0A   // 1 byte

//==============================================================================
// Device ID Expected Value
//==============================================================================
#define SLAVE_DEVICE_ID_VALUE  0x21

// Forward declaration
extern void SendString(uint8_t* str);

//==============================================================================
// Extern declarations
//==============================================================================
extern I2C_HandleTypeDef hi2c1;

//==============================================================================
// Function Prototypes
//==============================================================================
uint8_t INTF_Init(void);
uint8_t INTF_ReadPressureRaw(int32_t *pressure_code);
uint8_t INTF_ReadTemperatureRaw(int32_t *temp_code);
uint8_t INTF_ReadAVDDRaw(int32_t *avdd_code);
uint8_t INTF_ReadAllRaw(int32_t *pressure, int32_t *temperature, int32_t *avdd);

#endif /* INC_INTERFACE_BOARD_H */
