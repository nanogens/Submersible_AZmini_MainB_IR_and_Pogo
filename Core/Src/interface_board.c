#include "main.h"
#include "interface_board.h"
#include <stdio.h>

//==============================================================================
// Implementation
//==============================================================================
#ifdef INTERFACE_BOARD_IMPLEMENTATION

// Read 3 bytes from a register (24-bit value)
uint8_t INTF_Read24Bit(uint8_t reg, int32_t *value)
{
    uint8_t tx[1] = {reg};
    uint8_t rx[3] = {0};
    HAL_StatusTypeDef status;

    // Send register address
    status = HAL_I2C_Master_Transmit(&hi2c1, INTF_BOARD_I2C_ADDR_HAL, tx, 1, 100);
    if (status != HAL_OK)
    {
#if DEBUG_I2C
        /* Diagnostic Log:
         * - status: HAL Status (1 = HAL_ERROR, 2 = HAL_BUSY, 3 = HAL_TIMEOUT)
         * - State: HAL I2C driver State (0 = RESET/Uninitialized, 0x20 = READY)
         * - ISR: Hardware EXTI and status register (Bit 15 is BUSY flag)
         */
        char dbg_msg[128];
        sprintf(dbg_msg, "[DEBUG] Master Tx Fail! status: %d, I2C State: %d, ISR: 0x%08lx\r\n", 
                status, hi2c1.State, hi2c1.Instance->ISR);
        SendString((uint8_t*)dbg_msg);
#endif
        return 0;
    }

    // Read 3 bytes
    status = HAL_I2C_Master_Receive(&hi2c1, INTF_BOARD_I2C_ADDR_HAL, rx, 3, 100);
    if (status != HAL_OK)
    {
#if DEBUG_I2C
        char dbg_msg[128];
        sprintf(dbg_msg, "[DEBUG] Master Rx Fail! status: %d, I2C State: %d, ISR: 0x%08lx\r\n", 
                status, hi2c1.State, hi2c1.Instance->ISR);
        SendString((uint8_t*)dbg_msg);
#endif
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
    char msg[128];
    HAL_StatusTypeDef status;

    // Send register address
    status = HAL_I2C_Master_Transmit(&hi2c1, INTF_BOARD_I2C_ADDR_HAL, tx, 1, 100);
    if (status != HAL_OK) {
#if DEBUG_I2C
        sprintf(msg, "[DEBUG] INTF_Init Tx Fail! status: %d, I2C State: %d, ISR: 0x%08lx\r\n",
                status, hi2c1.State, hi2c1.Instance->ISR);
        SendString((uint8_t*)msg);
#endif
        return 0;
    }

    // Read device ID
    status = HAL_I2C_Master_Receive(&hi2c1, INTF_BOARD_I2C_ADDR_HAL, rx, 1, 100);
    if (status != HAL_OK) {
#if DEBUG_I2C
        sprintf(msg, "[DEBUG] INTF_Init Rx Fail! status: %d, I2C State: %d, ISR: 0x%08lx\r\n",
                status, hi2c1.State, hi2c1.Instance->ISR);
        SendString((uint8_t*)msg);
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

