// DEFINE STATEMENTS ========================================
#ifndef INC_PRESSURE_H
#define INC_PRESSURE_H

// I2C -- PressB Sensor (MS583730BA01-50)
#define MS5837_ADDR       (0x76 << 1)
#define CMD_RESET         0x1E
#define CMD_PROM_READ     0xA0
#define CMD_CONVERT_D1    0x48  // OSR=4096
#define CMD_CONVERT_D2    0x58
#define CMD_ADC_READ      0x00

// PROTOTYPE STATEMENTS =====================================
// I2C - PressB Sensor (MS583730BA01-50)
void DebugPrint(const uint8_t* message);
uint8_t MS5837_ReadPT(float *pressure_mbar, float *temp_c);
uint8_t MS5837_ReadPT_FirstOrder(float *pressure_mbar, float *temp_c);
uint32_t MS5837_ReadADC(uint8_t cmd);
void MS5837_Reset();

uint8_t MS5837_CRC4(uint16_t n_prom[]);
uint8_t MS5837_ReadPROM();

extern void DebugSensor(void);
int32_t Pressure_to_Depth_cm(float pressure_mbar, float surface_pressure_mbar);

uint8_t MS5837_InitCalibration(void);
uint8_t Acquire_RawPressureReading_TEIntegrated(void);
uint8_t Acquire_RawPressureReading_InterfaceBoard(void);
extern uint8_t Acquire_RawPressureReading(void);

void I2C_Recovery(void);

// Forward Declaration
extern void SendString(uint8_t* str);


// HANDLES ==================================================

extern I2C_HandleTypeDef hi2c1;

// VARIABLES ================================================
extern float surface_pressure;

#endif /* INC_PRESSURE_H */
