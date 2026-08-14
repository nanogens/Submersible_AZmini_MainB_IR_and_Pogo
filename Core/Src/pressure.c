/*
 * pressure.c
 *
 *  Created on: Jun 23, 2026
 *      Author: MT
 */

#include "main.h"
#include "stdio.h"
#include "pressure.h"
#ifdef INTERFACE_BOARD_IMPLEMENTATION
#include "interface_board.h"
#endif

float surface_pressure = SURFACE_PRESSURE;

// Debug print wrapper
void DebugPrint(const uint8_t* message)
{
    #if DEBUG_SENSOR
    SendString((uint8_t*)message);
    #endif
}

void MS5837_Reset() {
    uint8_t cmd = CMD_RESET;
    HAL_I2C_Master_Transmit(&hi2c1, MS5837_ADDR, &cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(10); // Wait for reset
}

uint8_t MS5837_CRC4(uint16_t n_prom[])
{
    uint16_t n_rem = 0;
    uint8_t n_bit;

    n_prom[0] = ((n_prom[0]) & 0x0FFF);
    n_prom[7] = 0;

    for (uint8_t cnt = 0; cnt < 16; cnt++) {
        if (cnt % 2 == 1) n_rem ^= (uint16_t)((n_prom[cnt>>1]) & 0x00FF);
        else n_rem ^= (uint16_t)(n_prom[cnt>>1] >> 8);

        for (n_bit = 8; n_bit > 0; n_bit--) {
            if (n_rem & 0x8000) n_rem = (n_rem << 1) ^ 0x3000;
            else n_rem = (n_rem << 1);
        }
    }

    n_rem = ((n_rem >> 12) & 0x000F);
    return (n_rem ^ 0x00);
}

uint8_t MS5837_ReadPROM() {
    for (uint8_t i = 0; i < 7; i++) {  // Changed from 8 to 7
        uint8_t cmd = CMD_PROM_READ + (i * 2);
        uint8_t buf[2];

        if (HAL_I2C_Master_Transmit(&hi2c1, MS5837_ADDR, &cmd, 1, 100) != HAL_OK) {
            char err_msg[32];
            snprintf(err_msg, sizeof(err_msg), "I2C Transmit failed at i=%d\r\n", i);
            DebugPrint((uint8_t*)err_msg);
            return 0;
        }
        if (HAL_I2C_Master_Receive(&hi2c1, MS5837_ADDR, buf, 2, 100) != HAL_OK) {
            char err_msg[32];
            snprintf(err_msg, sizeof(err_msg), "I2C Receive failed at i=%d\r\n", i);
            DebugPrint((uint8_t*)err_msg);
            return 0;
        }

        C[i] = (buf[0] << 8) | buf[1];

        // Print each PROM value for debugging
        char msg[32];
        snprintf(msg, sizeof(msg), "C[%d]: 0x%04X\r\n", i, C[i]);
        DebugPrint((uint8_t*)msg);
    }

    // Verify CRC
    uint8_t crc_read = (C[0] >> 12) & 0xF;
    uint8_t crc_calc = MS5837_CRC4(C);

    char crc_msg[64];
    snprintf(crc_msg, sizeof(crc_msg), "CRC: Read=0x%X, Calc=0x%X\r\n", crc_read, crc_calc);
    DebugPrint((uint8_t*)crc_msg);

    if (crc_read != crc_calc) {
    	DebugPrint((uint8_t*)"CRC error\r\n");
        return 0;
    }

    DebugPrint((uint8_t*)"PROM read successful\r\n");
    return 1;
}

/*
void MS5837_ReadPROM() {
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t cmd = CMD_PROM_READ + (i * 2);
        uint8_t buf[2];
        HAL_I2C_Master_Transmit(&hi2c1, MS5837_ADDR, &cmd, 1, HAL_MAX_DELAY);
        HAL_I2C_Master_Receive(&hi2c1, MS5837_ADDR, buf, 2, HAL_MAX_DELAY);
        C[i] = (buf[0] << 8) | buf[1];
    }
}
*/

/*
uint32_t MS5837_ReadADC(uint8_t cmd) {
    uint8_t buf[3];

    // Start conversion
    HAL_I2C_Master_Transmit(&hi2c1, MS5837_ADDR, &cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(20); // Max conversion time at OSR=4096 is ~10ms

    // Read ADC result
    cmd = CMD_ADC_READ;
    HAL_I2C_Master_Transmit(&hi2c1, MS5837_ADDR, &cmd, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, MS5837_ADDR, buf, 3, HAL_MAX_DELAY);

    return ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];
}
*/

uint32_t MS5837_ReadADC(uint8_t cmd)
{
    uint8_t buf[3];

    // Start conversion
    if (HAL_I2C_Master_Transmit(&hi2c1, MS5837_ADDR, &cmd, 1, 100) != HAL_OK)
        return 0xFFFFFFFF;

    // Increase delay to be safe
    HAL_Delay(20); // Increased from 10ms

    // Read ADC result
    cmd = CMD_ADC_READ;
    if (HAL_I2C_Master_Transmit(&hi2c1, MS5837_ADDR, &cmd, 1, 100) != HAL_OK)
        return 0xFFFFFFFF;
    if (HAL_I2C_Master_Receive(&hi2c1, MS5837_ADDR, buf, 3, 100) != HAL_OK)
        return 0xFFFFFFFF;

    return ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];
}

// Returns 1 = success, 0 = failure
uint8_t MS5837_ReadPT_FirstOrder(float *pressure_mbar, float *temp_c)
{
    if (PRESSURE_SENSOR_CURRENTLY_IN_USE == SENSOR_ID_MPM281II_2BAR) {
#ifdef INTERFACE_BOARD_IMPLEMENTATION
        if (!Acquire_RawPressureReading_InterfaceBoard()) {
            return 0;
        }
        // D1 has raw pressure code, D2 has raw temp code
        int32_t raw_pressure = (int32_t)((uint32_t)D1 << 8) >> 8; // sign extend 24-bit
        float voltage_mv_press = ((float)(raw_pressure - 0x800000) * 3000.0f) / (16.0f * 8388608.0f);

        uint32_t raw_temp = D2 & 0x00FFFFFF;
        float voltage_mv_temp = ((float)raw_temp * 3000.0f) / 16777216.0f;

        Sensor.temperature = (int32_t)(voltage_mv_temp * 100.0f);
        Sensor.pressure = (int32_t)(voltage_mv_press * 10.0f);

        *pressure_mbar = voltage_mv_press;
        *temp_c = voltage_mv_temp;
        return 1;
#else
        *pressure_mbar = 0.0f;
        *temp_c = 0.0f;
        return 0;
#endif
    }

    // Read raw ADC values
    D1 = MS5837_ReadADC(CMD_CONVERT_D1);
    if (D1 == 0xFFFFFFFF) return 0;
    D2 = MS5837_ReadADC(CMD_CONVERT_D2);
    if (D2 == 0xFFFFFFFF) return 0;

    if (PRESSURE_SENSOR_CURRENTLY_IN_USE == SENSOR_ID_MS5837_2BAR) {
        // Apply MS5837-02BA (2 bar) formulas using 64-bit integers
        int64_t dT = D2 - ((int64_t)C[5] * 256);
        int64_t TEMP = 2000 + (dT * C[6]) / 8388608LL;

        int64_t SENS = (int64_t)C[1] * 65536 + (C[3] * dT) / 128;
        int64_t OFF = (int64_t)C[2] * 131072 + (C[4] * dT) / 64;

        // Second order temperature compensation for 02BA
        int64_t T2 = 0;
        int64_t OFF2_corr = 0;
        int64_t SENS2_corr = 0;

        if (TEMP < 2000) { // Low temperature compensation (below 20°C)
            T2 = 11 * (dT * dT) / 34359738368LL;
            OFF2_corr = 31 * ((TEMP - 2000) * (TEMP - 2000)) / 8;
            SENS2_corr = 63 * ((TEMP - 2000) * (TEMP - 2000)) / 32;
        }

        TEMP = TEMP - T2;
        int64_t OFF2 = OFF - OFF2_corr;
        int64_t SENS2 = SENS - SENS2_corr;

        int64_t P = (((D1 * SENS2) / 2097152LL) - OFF2) / 32768LL;

        Sensor.temperature = TEMP;
        Sensor.pressure = P / 10; // Store in 0.1 mbar units for firmware consistency

        *temp_c = TEMP / 100.0f;           // °C
        *pressure_mbar = P / 100.0f;       // mbar (resolution is 0.01 mbar, so divide by 100)
    }
    else if (PRESSURE_SENSOR_CURRENTLY_IN_USE == SENSOR_ID_MS5837_30BAR) {
        // Default to MS5837-30BA (30 bar) formulas
        int64_t dT = D2 - ((int64_t)C[5] << 8);
        int64_t TEMP = 2000 + (dT * C[6]) / 8388608LL;

        int64_t OFF = ((int64_t)C[2] << 16) + (C[4] * dT) / 128;
        int64_t SENS = ((int64_t)C[1] << 15) + (C[3] * dT) / 256;

        // Second order temperature compensation for 30BA
        int64_t T2 = 0;
        int64_t OFF2_corr = 0;
        int64_t SENS2_corr = 0;

        if (TEMP < 2000) { // Low temperature compensation (below 20°C)
            T2 = 3 * (dT * dT) / 8589934592LL;
            OFF2_corr = 3 * ((TEMP - 2000) * (TEMP - 2000)) / 2;
            SENS2_corr = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 8;

            if (TEMP < -1500) { // Very low temperature compensation (below -15°C)
                OFF2_corr = OFF2_corr + 7 * ((TEMP + 1500) * (TEMP + 1500));
                SENS2_corr = SENS2_corr + 11 * ((TEMP + 1500) * (TEMP + 1500)) / 2;
            }
        }

        TEMP = TEMP - T2;
        int64_t OFF2 = OFF - OFF2_corr;
        int64_t SENS2 = SENS - SENS2_corr;

        int64_t P = (((D1 * SENS2) / 2097152LL) - OFF2) / 8192LL;

        Sensor.temperature = TEMP;
        Sensor.pressure = P; // Store in 0.1 mbar units

        *temp_c = TEMP / 100.0f;           // °C
        *pressure_mbar = P / 10.0f;        // mbar (resolution is 0.1 mbar, so divide by 10)
    }
    else {
        // Placeholder for future SENSOR_ID_MPM281II_2BAR
        Sensor.temperature = 0;
        Sensor.pressure = 0;
        *temp_c = 0.0f;
        *pressure_mbar = 0.0f;
    }

    return 1;
}


// Sent to New Orleans (Lousiana) show with this function version enabled
uint8_t  MS5837_ReadPT(float *pressure_mbar, float *temp_c)
{
    return MS5837_ReadPT_FirstOrder(pressure_mbar, temp_c);
}


/*
void DebugSensor()
{
  static uint8_t initialized = 0;
  if (!initialized)
  {
    MS5837_Reset();
    MS5837_ReadPROM();
    initialized = 1;
  }

  float press, temp;
  MS5837_ReadPT(&press, &temp);

  // conversion from float to int

  //CTD.press_i = (uint16_t) (press * 10.0f);
  //CTD.temp_i = (uint16_t) (temp * 10.0f);

  CTD.press_i += 10;  // temporary
  CTD.temp_i += 10;
  CTD.cond_i += 10;

  if(CTD.press_i > 150) // temporary - based on minValue / maxvalue in QML
  {
    CTD.press_i = 0;
  }
  if(CTD.temp_i > 70)
  {
    CTD.temp_i = 0;
  }
  if(CTD.cond_i > 2000)
  {
    CTD.cond_i = 0;
  }

  CTD.press_uc[0] = CTD.press_i & 0xFF;     // Low byte
  CTD.press_uc[1] = (CTD.press_i >> 8) & 0xFF;  // High byte

  CTD.temp_uc[0] = CTD.temp_i & 0xFF;     // Low byte
  CTD.temp_uc[1] = (CTD.temp_i >> 8) & 0xFF;  // High byte

  CTD.cond_uc[0] = CTD.cond_i & 0xFF;     // Low byte
  CTD.cond_uc[1] = (CTD.cond_i >> 8) & 0xFF;  // High byte

  // MT : was disabled, temporarily activated
  char msg[64];
  snprintf(msg, sizeof(msg), "P: %.2f mbar, T: %.2f C\r\n", press, temp);
  SendString(&hlpuart1, (uint8_t*)msg);
}
*/

void DebugSensor(void)
{
    static uint8_t initialized = 0;
    static uint8_t error_count = 0;
    static uint32_t last_reset_time = 0;

    if (!initialized)
    {
        if (PRESSURE_SENSOR_CURRENTLY_IN_USE != SENSOR_ID_MPM281II_2BAR)
        {
            MS5837_Reset();
            if (MS5837_ReadPROM()) {
                initialized = 1;
                error_count = 0;
            } else {
                // Handle initialization failure
                DebugPrint((uint8_t*)"Sensor init failed\r\n");

                // Try bus recovery after multiple failures
                if(error_count > 5) {
                    DebugPrint((uint8_t*)"Attempting I2C bus recovery\r\n");
                    I2C_Recovery();
                    error_count = 0;
                }
                error_count++;
                return;
            }
        }
        else
        {
#ifdef INTERFACE_BOARD_IMPLEMENTATION
            extern volatile uint32_t last_power_on_time;
            
            /* Wakeup Timing Guard:
             * Ensures the Interface Board has had at least 750 ms to power up and stabilize SCL/SDA
             * lines before the Main Board tries to communicate with it.
             */
            uint32_t elapsed = HAL_GetTick() - last_power_on_time;
            if (elapsed < 750)
            {
                HAL_Delay(750 - elapsed);
            }
            
            /* Driver State Guard:
             * If I2C was de-initialized for sleep (putting it in RESET state), force-initialize it
             * here now that SCL/SDA are stabilized and pulled high.
             */
            if (hi2c1.State == HAL_I2C_STATE_RESET)
            {
#if DEBUG_I2C
                SendString((uint8_t*)"[DEBUG] DebugSensor: Force initializing hi2c1\r\n");
#endif
                HAL_I2C_Init(&hi2c1);
            }

            if (INTF_Init()) {
                initialized = 1;
                error_count = 0;
                DebugPrint((uint8_t*)"Interface Board detected\r\n");
            } else {
                DebugPrint((uint8_t*)"Interface Board not detected\r\n");
                error_count++;
                return;
            }
#else
            initialized = 1;
#endif
        }
    }

    float press, temp;
    if(MS5837_ReadPT_FirstOrder(&press, &temp) == 0) {  // Modify ReadPT to return success/fail
        error_count++;
        DebugPrint((uint8_t*)"ReadPT failed\r\n");

        if (error_count > 3)
        {
            initialized = 0;
            error_count = 0;
            if (PRESSURE_SENSOR_CURRENTLY_IN_USE != SENSOR_ID_MPM281II_2BAR)
            {
                // Reset sensor after multiple errors
                MS5837_Reset();
                HAL_Delay(100);
                DebugPrint((uint8_t*)"Sensor reset due to errors\r\n");

                // Also try bus recovery if reset doesn't help
                if(HAL_GetTick() - last_reset_time < 30000)
                { // If reset recently didn't help
                    I2C_Recovery();
                }
                last_reset_time = HAL_GetTick();
            }
            else
            {
                DebugPrint((uint8_t*)"Interface Board read failed multiple times\r\n");
            }
        }
        return;
    }

    // Check for reasonable values (only for MS5837)
    if (PRESSURE_SENSOR_CURRENTLY_IN_USE != SENSOR_ID_MPM281II_2BAR)
    {
        if (press < 0.0f || press > 50000.0f || temp < -40.0f || temp > 85.0f)
        {
            error_count++;
            if (error_count > 3)
            {
                initialized = 0;
                error_count = 0;
                MS5837_Reset();
                HAL_Delay(100);
                DebugPrint((uint8_t*)"Sensor reset due to invalid values\r\n");
            }
            return;
        }
    }

    error_count = 0; // Reset error counter on good reading

    // Pressure to Depth (in cm) conversion, Temperature, Conductivity -- all integers
    if (PRESSURE_SENSOR_CURRENTLY_IN_USE == SENSOR_ID_MPM281II_2BAR)
    {
        // Auto-tare on first good read so depth starts at 0.00m
        static uint8_t first_read = 1;
        if (first_read)
        {
            surface_pressure = press;
            first_read = 0;
        }

        // For MPM281II (Interface Board), press and temp contain millivolt values.
        // We scale depth by 100 so 1 mV of pressure delta displays as 1.00 meter on PC GUI.
        float delta_mv = press - surface_pressure;
        if (delta_mv < 0.0f) delta_mv = 0.0f;
        CTD.depth_i = (uint16_t)(delta_mv * 100.0f);
        CTD.temp_i = (uint16_t)(temp * 1.0f); // Temp mV scaled by 1.0 so 405 mV shows as 40.5 C on PC
    }
    else
    {
        if(press > surface_pressure)
        {
          CTD.depth_i = Pressure_to_Depth_cm(press, surface_pressure);  // should be CTD.depth_i
        }
        else
        {
          CTD.depth_i = 0;	// should be CTD.depth_i
        }
        CTD.temp_i = temp * 10;
    }
    CTD.cond_i = 0;

    char msg[64];
    snprintf(msg, sizeof(msg), "P: %.2f mV, T: %.2f mV\r\n", press, temp);
    DebugPrint((uint8_t*)msg);
}

void I2C_Recovery(void)
{
    // Toggle SCL to try to free stuck devices
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Get current I2C GPIO configuration (SCL: PA9, SDA: PA10)
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;  // SCL & SDA pins
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Generate clock pulses
    for(int i = 0; i < 10; i++) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);  // SCL low
        HAL_Delay(1);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);    // SCL high
        HAL_Delay(1);
    }

    // Send STOP condition
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);  // SDA low
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);     // SCL high
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);    // SDA high
    HAL_Delay(1);

    // Reinitialize I2C
    HAL_I2C_DeInit(&hi2c1);
    HAL_Delay(10);
    HAL_I2C_Init(&hi2c1);
}

int32_t Pressure_to_Depth_cm(float pressure_mbar, float surface_pressure_mbar)
{
    float water_pressure_pa = (pressure_mbar - surface_pressure_mbar) * MBAR_TO_PA;
    float depth_m = water_pressure_pa / (FRESHWATER_DENSITY * GRAVITY);
    int32_t depth_cm_x100 = (int32_t)(depth_m * 10000.0f);  // MT changed from 10000.0f to 1.0f for now  // meters × 10000 = cm × 100
    return depth_cm_x100;
}

// Call this once at system startup or before first recording
uint8_t MS5837_InitCalibration(void)
{
    MS5837_Reset();
    if (MS5837_ReadPROM()) {
        // C[] array is now populated with calibration coefficients
        return 1;  // Success
    }
    return 0;  // Failure
}

//==============================================================================
// TE Integrated Sensor (MS5837)
//==============================================================================
uint8_t Acquire_RawPressureReading_TEIntegrated(void)
{
    /* Driver State Guard:
     * If I2C was de-initialized for sleep (putting it in RESET state), force-initialize it
     * here now that SCL/SDA are stabilized and pulled high.
     */
    if (hi2c1.State == HAL_I2C_STATE_RESET)
    {
#if DEBUG_I2C
        SendString((uint8_t*)"[DEBUG] Acquire_TE: Force initializing hi2c1\r\n");
#endif
        HAL_I2C_Init(&hi2c1);
    }

    // Initialize calibration once or if cleared (C[1] == 0)
    if (C[1] == 0) {
        if (MS5837_InitCalibration()) {
#if DEBUG_SENSOR
            // Debug: Print calibration coefficients
            for (int i = 0; i < 7; i++) {
                char msg[64];
                sprintf(msg, "C[%d] = %d (0x%04X)\r\n", i, C[i], C[i]);
                SendString((uint8_t*)msg);
            }
#endif
        } else {
#if DEBUG_SENSOR
            SendString((uint8_t*)"ERROR: Failed to read calibration\r\n");
#endif
            return 0;
        }
    }

    // Read raw ADC values
    D1 = MS5837_ReadADC(CMD_CONVERT_D1);
    if (D1 == 0xFFFFFFFF) return 0;
    D2 = MS5837_ReadADC(CMD_CONVERT_D2);
    if (D2 == 0xFFFFFFFF) return 0;

#if DEBUG_SENSOR
    char msg[128];
    sprintf(msg, "TE Sensor - D1=%lu, D2=%lu\r\n", D1, D2);
    SendString((uint8_t*)msg);
#endif

    return 1;
}

//==============================================================================
// Interface Board (Traditional Pressure Transducer)
//==============================================================================
#ifdef INTERFACE_BOARD_IMPLEMENTATION
uint8_t Acquire_RawPressureReading_InterfaceBoard(void)
{
    extern volatile uint32_t last_power_on_time;

    static uint8_t initialized = 0;
    static uint8_t detected = 0;
    int32_t pressure_code = 0;
    int32_t temp_code = 0;
    int32_t avdd_code = 0;
    uint32_t elapsed;

    // Ensure the Interface Board has had adequate stabilization time since the load switch was enabled.
    elapsed = HAL_GetTick() - last_power_on_time;

#if DEBUG_I2C
    char dbg_msg[128];
    sprintf(dbg_msg, "[DEBUG] Acquire start: elapsed=%lu, State=%d\r\n", elapsed, hi2c1.State);
    SendString((uint8_t*)dbg_msg);
#endif

    if (elapsed < 750)
    {
        HAL_Delay(750 - elapsed);
    }

    /* Force re-initialization if the driver was left de-initialized during sleep.
     * This prevents HAL_BUSY errors caused by SCL/SDA analog state on wakeup.
     */
    if (hi2c1.State == HAL_I2C_STATE_RESET)
    {
#if DEBUG_I2C
        SendString((uint8_t*)"[DEBUG] Guard triggered: calling HAL_I2C_Init\r\n");
#endif
        HAL_I2C_Init(&hi2c1);
    }

    // Initialize Interface Board once
    if (!initialized) {
        if (INTF_Init()) {
            initialized = 1;
            detected = 1;
            SendString((uint8_t*)"Interface Board detected\r\n");
        } else {
            SendString((uint8_t*)"ERROR: Interface Board not detected\r\n");
            return 0;
        }
    }

    if (!detected) return 0;

    // Read all three channels
    if (!INTF_ReadAllRaw(&pressure_code, &temp_code, &avdd_code)) {
        SendString((uint8_t*)"ERROR: Failed to read Interface Board\r\n");
        return 0;
    }

    // Store raw values in D1 and D2 (for EEPROM compatibility)
    // D1 = Pressure raw (24-bit -> 32-bit)
    // D2 = Temperature raw (24-bit -> 32-bit)
    D1 = (uint32_t)pressure_code;
    D2 = (uint32_t)temp_code;

    char msg[128];
    sprintf(msg, "INTF Board - P=%lu, T=%lu, AVDD=%lu\r\n", D1, D2, (uint32_t)avdd_code);
    SendString((uint8_t*)msg);

        return 1;
}
#endif /* INTERFACE_BOARD_IMPLEMENTATION */

// Wrapper function that calls the appropriate sensor function based on SENSOR_TYPE
uint8_t Acquire_RawPressureReading(void)
{
#if SENSOR_TYPE == SENSOR_TYPE_TE_INTEGRATED
    return Acquire_RawPressureReading_TEIntegrated();
#elif SENSOR_TYPE == SENSOR_TYPE_INTERFACE_BOARD
    return Acquire_RawPressureReading_InterfaceBoard();
#else
    #error "Invalid SENSOR_TYPE selected in defines.h"
#endif
}

