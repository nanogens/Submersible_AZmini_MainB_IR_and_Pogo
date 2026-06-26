#include "main.h"
#include "interface_board.h"

//==============================================================================
// Implementation
//==============================================================================
#ifdef INTERFACE_BOARD_IMPLEMENTATION

// Read 3 bytes from a register (24-bit value)
uint8_t INTF_Read24Bit(uint8_t reg, int32_t *value)
{
    uint8_t tx[1] = {reg};
    uint8_t rx[3] = {0};

    // Send register address
    if (HAL_I2C_Master_Transmit(&hi2c1, INTF_BOARD_I2C_ADDR_HAL, tx, 1, 100) != HAL_OK)
    {
        return 0;
    }

    // Read 3 bytes
    if (HAL_I2C_Master_Receive(&hi2c1, INTF_BOARD_I2C_ADDR_HAL, rx, 3, 100) != HAL_OK)
    {
        return 0;
    }

    // Assemble 24-bit value
    *value = ((int32_t)rx[0] << 16) | ((int32_t)rx[1] << 8) | (int32_t)rx[2];

    return 1;
}

// Initialize Interface Board (check device ID)
uint8_t INTF_Init(void)
{
    uint8_t tx[1] = {INTF_REG_DEVICE_ID};
    uint8_t rx[1] = {0};
    char msg[64];

    // Send register address
    if (HAL_I2C_Master_Transmit(&hi2c1, INTF_BOARD_I2C_ADDR_HAL, tx, 1, 100) != HAL_OK) {
#if DEBUG_SENSOR
    	SendString((uint8_t*)"Transmit failed\r\n");
#endif
        return 0;
    }

    // Read device ID
    if (HAL_I2C_Master_Receive(&hi2c1, INTF_BOARD_I2C_ADDR_HAL, rx, 1, 100) != HAL_OK) {
#if DEBUG_SENSOR
        SendString((uint8_t*)"Receive failed\r\n");
#endif
        return 0;
    }

#if DEBUG_SENSOR
    sprintf(msg, "Received Device ID: 0x%02X\r\n", rx[0]);
    SendString((uint8_t*)msg);
#endif

    if (rx[0] == SLAVE_DEVICE_ID_VALUE) {
        return 1;
    }

    return 0;
}

// Read raw pressure ADC code (24-bit)
uint8_t INTF_ReadPressureRaw(int32_t *pressure_code)
{
    return INTF_Read24Bit(INTF_REG_PRESSURE_RAW, pressure_code);
}

// Read raw temperature ADC code (24-bit)
uint8_t INTF_ReadTemperatureRaw(int32_t *temp_code)
{
    return INTF_Read24Bit(INTF_REG_TEMP_RAW, temp_code);
}

// Read raw AVDD ADC code (24-bit)
uint8_t INTF_ReadAVDDRaw(int32_t *avdd_code)
{
    return INTF_Read24Bit(INTF_REG_AVDD_RAW, avdd_code);
}

// Read all three channels at once
uint8_t INTF_ReadAllRaw(int32_t *pressure, int32_t *temperature, int32_t *avdd)
{
    if (!INTF_ReadPressureRaw(pressure)) return 0;
    if (!INTF_ReadTemperatureRaw(temperature)) return 0;
    if (!INTF_ReadAVDDRaw(avdd)) return 0;
    return 1;
}

#endif /* INTERFACE_BOARD_IMPLEMENTATION */

