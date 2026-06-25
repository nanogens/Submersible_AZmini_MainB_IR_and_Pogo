// ============================================
// Communication Interface Selection
// ============================================


// -----------------------------------------------------------------------------------------------------

// Choose ONLY ONE of the following:
//#define POGO_LPUART1   1   // Use LPUART1 (Wired connection also known as Pogo pins)
#define IR_USART2     1   // Use USART2 (IrDA)

// Configure the actual defines used in code
#ifdef IR_USART2
    #define IR_USART2_SEL 1
#elif defined(POGO_LPUART1)
    #define POGO_LPUART1_SEL 1
#else
    #error "Please select either POGO_LPUART1 or IR_USART2 in defines.h"
#endif
// ============================================


// Debug Print
#define DEBUG_SENSOR 1// 1 to show ascii readouts in console, 0 to not

#define DLE 0x10
#define STX 0x02
#define SOURCE 0x88 // must be opposite on receiving end
#define DEST 0x00

#define RLE 0x77
#define RTX 0x76

// Array sizes
// LPUART1
#define MAX_LPUART1_ARRAY 256

// Version
#define MAX_FWVERSION_ARRAY 2
#define MAX_SWVERSION_ARRAY 2

// Instrument
#define MAX_INSTRUMENT_SERIAL_ARRAY 13
#define MAX_INSTRUMENT_USAGE_ARRAY 2

// Status
#define MAX_STATUS_RES_ARRAY 4

// Power
#define MAX_POWER_DURATION_ARRAY 2

// Activate
#define MAX_ACTIVATE_START_FULLYEAR_ARRAY 2
#define MAX_ACTIVATE_END_FULLYEAR_ARRAY 2

// Note
#define MAX_NOTES_NAME_ARRAY 24
#define MAX_NOTES_LOCATION_ARRAY 24
#define MAX_NOTES_NOTE_ARRAY 128

// Cloud
#define MAX_CLOUD_IP_ARRAY 13
#define MAX_CLOUD_LOGIN_ARRAY 25
#define MAX_CLOUD_PW_ARRAY 25

// First Page - 6 boxes from 0 to 5
#define MAX_MEMORY_TOTAL_ARRAY 2
#define MAX_MEMORY_USED_ARRAY 2

#define MAX_SURFACE_PRESSURE_ARRAY 2
#define MAX_USAGE_HOURS_ARRAY 2

#define MAX_MESSAGES_RECEIVED_ARRAY 4
#define MAX_MESSAGES_SENT_ARRAY 4

// CTD
#define MAX_CTD_PRESS_ARRAY 2
#define MAX_CTD_TEMP_ARRAY 2
#define MAX_CTD_COND_ARRAY 2

// Record
#define PAGEDATA_W_ARRAY 512
#define PAGEDATA_R_ARRAY 512



// Log_TransmitData
#define LOGTRANSMITDATA_SECTOR_ARRAY 2

#define LOGTRANSMITDATA_QUADRANTS_ARRAY 4
#define LOGTRANSMITDATA_QUADRANTBYTES_ARRAY 128

#define LOGTRANSMITDATA_RESERVED_ARRAY 2

// Pressure sensor ROM
#define PERSSURESENSOR_ROM_ARRAY 7

// EEPROM
#define TOTAL_SECTORS 1024
#define PAGES_PER_SECTOR 8
#define BYTES_PER_SECTOR 4096
#define BYTES_PER_PAGE 512

// Records
#define RECORDS_PER_PAGE 16
#define BYTES_PER_RECORD 32

// File (Slot)
#define QUADRANTS 4
#define BYTES_PER_QUADRANT 128


#define FILLED 1
#define CLEAR 0

#define QUERY 0
#define SETTING 1


// Surface Pressure Value
#define SURFACE_PRESSURE 1013.25


// Pressure (mbar) to Depth conversion
#define FRESHWATER_DENSITY 1000.0f  // kg/m³
#define GRAVITY 9.80665f           // m/s²
#define MBAR_TO_PA 100.0f          // 1 mbar = 100 Pa

// Reed Switch Status
#define ACTIVATED 1
#define DEACTIVATED 0

// EEPROMRecord
#define EEP_TIME_ARRAY 8
#define EEP_PRESSURE_ARRAY 6
#define EEP_TEMPERATURE_ARRAY 6
#define EEP_CONDUCVITIVY_ARRAY 6
#define EEP_RESERVED_ARRAY 6

// FileCreationTime
#define SHOWFILES_FILENAME_ARRAY 8
#define SHOWFILES_FILECREATIONTIME_ARRAY EEP_TIME_ARRAY
#define SHOWFILES_TOTALNUMBEROFRECORDS_ARRAY 4

// RecordStatus
#define RECORDING_UNEXPECTED 4
#define RECORDING_NOSPACE 3
#define RECORDING_ONGOING 2
#define RECORDING_STARTED 1
#define RECORDING_NOTSTARTED 0

#define RECORDSTATE_WRITEPAGE 2
#define RECORDSTATE_UPDATEPAGE 1
#define RECORDSTATE_DONOTHING 0

#define GREATERTHAN30S 1 // the memory page will be UPDATED every time a record is complete (16 records per page currently).
#define LESSTHAN30S 0    // the memory page will be (erased and) WRITTEN ONLY when 512 bytes (16 records) have been accumulated.

// RecordStatus.status
#define RECORDSTATUS_ACTIVE 2
#define RECORDSTATUS_DELETED 1
#define RECORDSTATUS_UNKNOWN 0

// ShowFiles - fixed location of (basic) info on files in memory.
// Sector 1, Page 0
#define SHOWFILES_SECTOR 1
#define SHOWFILES_PAGE 0
// ReadSpecificFile - fixed location of file metadata in memory
// Sector 2, Page X
#define READSPECIFICFILE_SECTOR 2
#define READSPECIFICFILE_FILESLOTS 1 // THIS CONTROLS THE NUMBERO OF FILES (IMPORTANT: Max value 4)

// Determines if read page array is to be broken up into quadrants or kept whole
#define BREAK_INTO_QUADRANT_YES 1
#define BREAK_INTO_QUADRANT_NO 0

#define FILE_SLOT_EMPTY   0xFF
#define FILE_SLOT_OCCUPIED 0x99

// Sensor
#define SENSOR_TEMPERATURE_ARRAY 8
#define SENSOR_PRESSURE_ARRAY 6
#define SENSOR_CONDUCTIVITY_ARRAY 6
#define SENSOR_RESERVED_ARRAY 6

// ============================================
// Sensor Type Selection (Hard coded for now)
// ============================================
#define SENSOR_TYPE_TE_INTEGRATED    1
#define SENSOR_TYPE_INTERFACE_BOARD  2

// Select the active sensor (change this to switch)
#define SENSOR_TYPE  SENSOR_TYPE_TE_INTEGRATED // SENSOR_TYPE_INTERFACE_BOARD

// Enable Interface Board implementation (comment out to disable)
//#define INTERFACE_BOARD_IMPLEMENTATION

// SamplingMode
#define TIME_CONTINUOUSLOOP 0
#define TIME_DONOTLOOP 1
#define EVENT_TRIGGERED 2

// ActivateRecordingPlan
#define RUN_PLAN_NO 0
#define RUN_PLAN_YES 1


// Messages ========================================================
// Version
#define VERSION_QUERY_MSGLGT 0x07
#define VERSION_QUERY_MSGID 0x01

#define VERSION_RESP_MSGLGT 0x0D
#define VERSION_RESP_MSGID 0x02

// Status
#define STATUS_QUERY_MSGLGT 0x07
#define STATUS_QUERY_MSGID 0x03

#define STATUS_RESP_MSGLGT 0x0B
#define STATUS_RESP_MSGID 0x04

#define STATUS_SET_MSGLGT 0x0B
#define STATUS_SET_MSGID 0x05

// Instrument
#define INSTRUMENT_QUERY_MSGLGT 0x07
#define INSTRUMENT_QUERY_MSGID 0x06

#define INSTRUMENT_RESP_MSGLGT 0x19
#define INSTRUMENT_RESP_MSGID 0x07

#define INSTRUMENT_SET_MSGLGT 0x19
#define INSTRUMENT_SET_MSGID 0x08

// Communication
#define COMMUNICATION_QUERY_MSGLGT 0x07
#define COMMUNICATION_QUERY_MSGID 0x09

#define COMMUNICATION_RESP_MSGLGT 0x0B
#define COMMUNICATION_RESP_MSGID 0x0A

#define COMMUNICATION_SET_MSGLGT 0x0B
#define COMMUNICATION_SET_MSGID 0x0B

// Power
#define POWER_QUERY_MSGLGT 0x07
#define POWER_QUERY_MSGID 0x0C

#define POWER_RESP_MSGLGT 0x0F
#define POWER_RESP_MSGID 0x0D

#define POWER_SET_MSGLGT 0x0F
#define POWER_SET_MSGID 0x0E

// Time
#define TIME_QUERY_MSGLGT 0x07
#define TIME_QUERY_MSGID 0x0F

#define TIME_RESP_MSGLGT 0x11
#define TIME_RESP_MSGID 0x10

#define TIME_SET_MSGLGT 0x11
#define TIME_SET_MSGID 0x11

// Sampling
#define SAMPLING_QUERY_MSGLGT 0x07
#define SAMPLING_QUERY_MSGID 0x12

#define SAMPLING_RESP_MSGLGT 0x0D
#define SAMPLING_RESP_MSGID 0x13

#define SAMPLING_SET_MSGLGT 0x0D
#define SAMPLING_SET_MSGID 0x14

// Activation
#define ACTIVATION_QUERY_MSGLGT 0x07
#define ACTIVATION_QUERY_MSGID 0x15

#define ACTIVATION_RESP_MSGLGT 0x1F
#define ACTIVATION_RESP_MSGID 0x16

#define ACTIVATION_SET_MSGLGT 0x1F
#define ACTIVATION_SET_MSGID 0x17

// Notes
#define NOTES_QUERY_MSGLGT 0x07
#define NOTES_QUERY_MSGID 0x18

#define NOTES_RESP_MSGLGT 0xB9
#define NOTES_RESP_MSGID 0x19

#define NOTES_SET_MSGLGT 0xB9
#define NOTES_SET_MSGID 0x1A

// Cloud
#define CLOUD_QUERY_MSGLGT 0x07
#define CLOUD_QUERY_MSGID 0x1B

#define CLOUD_RESP_MSGLGT 0x07
#define CLOUD_RESP_MSGID 0x1C

#define CLOUD_SET_MSGLGT 0x07
#define CLOUD_SET_MSGID 0x1D

// Misc
#define MISC_QUERY_MSGLGT 0x07
#define MISC_QUERY_MSGID 0x1E

#define MISC_RESP_MSGLGT 0x0D
#define MISC_RESP_MSGID 0x1F

#define MISC_SET_MSGLGT 0x0D
#define MISC_SET_MSGID 0x20

// Activation
#define APPLYRECORDINGPLAN_QUERY_MSGLGT 0x07
#define APPLYRECORDINGPLAN_QUERY_MSGID 0x90

#define APPLYRECORDINGPLAN_RESP_MSGLGT 0x0D
#define APPLYRECORDINGPLAN_RESP_MSGID 0x91

#define APPLYRECORDINGPLAN_SET_MSGLGT 0x0D
#define APPLYRECORDINGPLAN_SET_MSGID 0x92

#define PLAN_RUN_NO   0
#define PLAN_RUN_YES  1


// ----------------------------

// Re-check this -- just using some bogus values for now
#define CTD_VARIABLES_QUERY_MSGLGT 0x07
#define CTD_VARIABLES_QUERY_MSGID 0x21

#define CTD_VARIABLES_RESP_MSGLGT 0x07
#define CTD_VARIABLES_RESP_MSGID 0x22

#define CTD_VARIABLES_SET_MSGLGT 0x07
#define CTD_VARIABLES_SET_MSGID 0x23

// Re-check this -- just using some bogus values for now
#define CTD_READINGS_RAW_QUERY_MSGLGT 0x07
#define CTD_READINGS_RAW_QUERY_MSGID 0x24

#define CTD_READINGS_RAW_RESP_MSGLGT 0x07
#define CTD_READINGS_RAW_RESP_MSGID 0x25

// Re-check this -- just using some bogus values for now
#define CTD_READINGS_PROCESSED_QUERY_MSGLGT 0x07
#define CTD_READINGS_PROCESSED_QUERY_MSGID 0x26

#define CTD_READINGS_PROCESSED_RESP_MSGLGT 0x11
#define CTD_READINGS_PROCESSED_RESP_MSGID 0x27

// Re-check this -- just using some bogus values for now
#define SUBMERSIBLE_INFO_QUERY_MSGLGT 0x07
#define SUBMERSIBLE_INFO_QUERY_MSGID 0x28

#define SUBMERSIBLE_INFO_RESP_MSGLGT 0x4A
#define SUBMERSIBLE_INFO_RESP_MSGID 0x29

// Logging
#define LOG_SHOWFILES_QUERY_MSGID 0x50
#define LOG_SHOWFILES_QUERY_MSGLGT 0x07

#define LOG_SHOWFILES_RESP_MSGID 0x51
#define LOG_SHOWFILES_RESP_MSGLGT 0x5F

#define LOG_READSPECIFICFILE_SET_MSGID 0x52
#define LOG_READSPECIFICFILE_SET_MSGLGT 0x08

#define LOG_READSPECIFICFILE_RESP_MSGID 0x53
#define LOG_READSPECIFICFILE_RESP_MSGLGT 0x89

#define LOG_TRANSMITDATA_SET_MSGID 0x54
#define LOG_TRANSMITDATA_SET_MSGLGT 0x0E

#define LOG_TRANSMITDATA_RESP_MSGID 0x55
#define LOG_TRANSMITDATA_RESP_MSGLGT 0x8E

// BOOT LOADER ==================================================
// Note: Messages below are used exclusively by the Bootloader, so do NOT use them here in the Application firmware.

#define BOOTLOADER_VERSION_QUERY_MSGID 0x60
#define BOOTLOADER_VERSION_QUERY_MSGLGT 0x07

#define BOOTLOADER_VERSION_RESP_MSGID 0x61
#define BOOTLOADER_VERSION_RESP_MSGLGT 0x0B

#define BOOTLOADER_FWUPDATE_SET_MSGID 0x62
#define BOOTLOADER_FWUPDATE_SET_MSGLGT 0x08

#define BOOTLOADER_FWUPDATE_RESP_MSGID 0x63
#define BOOTLOADER_FWUPDATE_RESP_MSGLGT 0x08

#define BOOTLOADER_TRANSMITDATA_SET_MSGID   0x64
#define BOOTLOADER_TRANSMITDATA_SET_MSGLGT  0x8E

#define BOOTLOADER_TRANSMITDATA_RESP_MSGID   0x65
#define BOOTLOADER_TRANSMITDATA_RESP_MSGLGT  0x0E


