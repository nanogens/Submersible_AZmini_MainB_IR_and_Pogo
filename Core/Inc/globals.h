/*
 * globals.h
 *
 *  Created on: Sep 3, 2025
 *      Author: MT
 */

#ifndef INC_GLOBALS_H_
#define INC_GLOBALS_H_

static struct Lpuartl1
{
  uint8_t sent;
  uint8_t crcsend;
  uint8_t bufferrx[1];
  uint8_t payload[MAX_LPUART1_ARRAY];
  uint8_t status;
  uint8_t got;
  uint8_t messagelength;
  uint8_t messageidglobal;
  uint8_t crcmsg;
  uint8_t crcset;
  uint8_t prevmessageidglobal;
}Lpuart1;

static struct Lpuartshadow11
{
  uint8_t messageid;
  uint8_t payload[MAX_LPUART1_ARRAY];
}Lpuart1shadow;

static struct Counter1
{
  uint32_t yi; // interrupt counter, do not use in main

  uint32_t y0; // main counter, do not use in interrupt
  uint32_t y1; // main counter, do not use in interrupt

  uint8_t streaming;
  uint32_t totalvalidpacketcount;
  uint32_t reedswitchcount;

  int repeat; // temporary counter used for testing in main loop
}Counter;

// ========= Data from MCU to MainB =========
static struct Version1
{
    uint8_t reserved;
    uint8_t boxselection;

    uint8_t fw_version[MAX_FWVERSION_ARRAY];
    uint8_t sw_version[MAX_SWVERSION_ARRAY];
}Version;

static struct Status1
{
  uint8_t reserved;
  uint8_t boxselection;

  uint8_t res[4];
}Status;

// Second Page of Viewer Application (Spectraware)
static struct Instrument1
{
  uint8_t reserved;
  uint8_t boxselection;

  uint8_t device;
  uint8_t serialnumber[MAX_INSTRUMENT_SERIAL_ARRAY];
  uint8_t usage[MAX_INSTRUMENT_USAGE_ARRAY];
}Instrument;

static struct Communication1
{
  uint8_t reserved0;
  uint8_t boxselection;

  uint8_t connetion;
  uint8_t baudrate;
}Communication;

static struct Power1
{
  uint8_t reserved0;
  uint8_t boxselection;

  uint8_t batterytype;
  uint8_t duration[MAX_POWER_DURATION_ARRAY];
  uint8_t powerremaining;
}Power;

static struct Time1
{
  // used to set time either manually in firmware for testing
  // or from the viewer's SETTINGS page after setting the time (via sync to computer or manually)
  // and then clicking on Write Instrument button
  uint8_t reserved;
  uint8_t boxselection;
  uint8_t set_year;         // 0-99 (00-99)
  uint8_t set_month;      // 1-12
  uint8_t set_day;          // 1-31
  uint8_t set_hour;        // 0-23 (24-hour format)
  uint8_t set_minute;     // 0-59
  uint8_t set_second;     // 0-59
  uint8_t set_ampm;      // 0=AM, 1=PM (optional, since we're using 24-hour)
  uint8_t set_weekday;  // 1=Monday, 7=Sunday

  // for storing data from the MCU's RTC when we read from it
  uint8_t read_year;    // 0-99 (00-99)
  uint8_t read_month;   // 1-12
  uint8_t read_day;     // 1-31
  uint8_t read_hour;    // 0-23 (24-hour format)
  uint8_t read_minute;  // 0-59
  uint8_t read_second;  // 0-59
  uint8_t read_ampm;    // 0=AM, 1=PM (optional, since we're using 24-hour)
  uint16_t read_full_year; // Full year (2025)
  uint8_t read_weekday;    // 1=Monday, 7=Sunday
}Time;

static struct Sampling1
{
  uint8_t boxselection;
  uint8_t reserved0;
  uint8_t mode; // 0 = Time - Continuous, 1 = Time - Do not Loop, 2 = Event Triggered
  uint8_t rate;    // 0 = 0.1 sec, 1 = 0.5 sec, 2 = 1 sec, 3 = 5 sec, 4 = 10 sec, 5 = 30 sec, 6 = 1 min, 7 = 5 min, 8 = 10 min, 9 = 30 min, 10 = 1 hour
  uint8_t reserved1;
  uint8_t reserved2;
}Sampling;

static struct Activation1
{
  uint8_t boxselection;
  uint8_t reserved0;

  // Start Date
  uint8_t start_year;    // 0-99 (00-99)
  uint8_t start_month;  // 1-12
  uint8_t start_day;     // 1-31
  uint8_t start_hour;    // 0-23 (24-hour format)
  uint8_t start_minute;  // 0-59
  uint8_t start_second;  // 0-59
  uint8_t start_ampm;    // 0=AM, 1=PM (optional, since we're using 24-hour)
  // Additional variables for full year and weekday
  uint16_t start_full_year[MAX_ACTIVATE_START_FULLYEAR_ARRAY]; // Full year (2025)
  uint8_t start_weekday;    // 1=Monday, 7=Sunday

  // End Date
  uint8_t end_year;    // 0-99 (00-99)
  uint8_t end_month;  // 1-12
  uint8_t end_day;     // 1-31
  uint8_t end_hour;    // 0-23 (24-hour format)
  uint8_t end_minute;  // 0-59
  uint8_t end_second;  // 0-59
  uint8_t end_ampm;    // 0=AM, 1=PM (optional, since we're using 24-hour)
  // Additional variables for full year and weekday
  uint8_t end_full_year[MAX_ACTIVATE_END_FULLYEAR_ARRAY]; // Full year (2025)
  uint8_t end_weekday;    // 1=Monday, 7=Sunday

  uint8_t eventtrigger;
  uint8_t eventthreshold;
}Activation;

static struct Notes1
{
  uint8_t boxselection;
  uint8_t reserved0;
  uint8_t name[MAX_NOTES_NAME_ARRAY];
  uint8_t location[MAX_NOTES_LOCATION_ARRAY];
  uint8_t note[MAX_NOTES_NOTE_ARRAY];
}Notes;

static struct Cloud1
{
  uint8_t ip[MAX_CLOUD_IP_ARRAY];
  uint8_t login[MAX_CLOUD_LOGIN_ARRAY];
  uint8_t pw[MAX_CLOUD_PW_ARRAY];
}Cloud;

static struct Misc1
{
  uint8_t stuff;
}Misc;

// ----
// First page of viewer (Spectraware) -- Variables for 6 boxes at left and right flanks

// Box 0
// we use the serialnumber variable from Instrument structure of the 2nd page for our first box on left

// Box 1
static struct Memory1
{
  uint8_t total[MAX_MEMORY_TOTAL_ARRAY];
  uint8_t used[MAX_MEMORY_USED_ARRAY];
}Memory;

// Box 2
static struct Surface1
{
  uint8_t pressure[MAX_SURFACE_PRESSURE_ARRAY];
}Surface;

// Box 3 -- right hand side
static struct Battery1
{
  uint8_t cell;
  uint8_t type;
}Battery;

static struct Usage1
{
  uint8_t hours[MAX_USAGE_HOURS_ARRAY];
}Usage;

// Box 4
static struct Messages1
{
  uint8_t received[MAX_MESSAGES_RECEIVED_ARRAY];
  uint8_t sent[MAX_MESSAGES_SENT_ARRAY];
}Messages;

// Box 5
static struct TabletTime1
{
  uint8_t year;    // 0-99 (00-99)
  uint8_t month;  // 1-12
  uint8_t day;     // 1-31
  uint8_t hour;    // 0-23 (24-hour format)
  uint8_t minute;  // 0-59
  uint8_t second;  // 0-59
  uint8_t ampm;    // 0=AM, 1=PM (optional, since we're using 24-hour)
}TabletTime;

static struct DeviceTime1
{
  uint8_t year;    // 0-99 (00-99)
  uint8_t month;  // 1-12
  uint8_t day;     // 1-31
  uint8_t hour;    // 0-23 (24-hour format)
  uint8_t minute;  // 0-59
  uint8_t second;  // 0-59
  uint8_t ampm;    // 0=AM, 1=PM (optional, since we're using 24-hour)
}DeviceTime;

static struct UpcomingRecordingTime1
{
  uint8_t year;    // 0-99 (00-99)
  uint8_t month;  // 1-12
  uint8_t day;     // 1-31
  uint8_t hour;    // 0-23 (24-hour format)
  uint8_t minute;  // 0-59
  uint8_t second;  // 0-59
  uint8_t ampm;    // 0=AM, 1=PM (optional, since we're using 24-hour)
}UpcomingRecordingTime;


// ----
// First page of viewer (Spectraware) -- Streaming variables for 3 Gauges
static struct CTD1
{
  uint16_t depth_i;
  uint16_t temp_i;
  uint16_t cond_i;
}CTD;

static struct ReedSwitch1
{
  uint8_t state;
}ReedSwitch;

static struct Temporary1
{
  uint16_t temporary;
}Temporary;


// ----------

// Record
static struct EEPROMRecord1
{
  uint8_t time[EEP_TIME_ARRAY];                 // 8
  uint8_t pressure[EEP_PRESSURE_ARRAY];         // 6
  uint8_t temperature[EEP_TEMPERATURE_ARRAY];   // 6
  uint8_t conductivity[EEP_CONDUCVITIVY_ARRAY]; // 6
  uint8_t reserved[EEP_RESERVED_ARRAY];         // 6
}EEPROMRecord;

static struct ShowFiles1
{
  uint8_t reserved;
  uint8_t fileindex;
  uint8_t filename	[SHOWFILES_FILENAME_ARRAY];
  uint8_t filecreationtime[SHOWFILES_FILECREATIONTIME_ARRAY];
  uint8_t totalnumberofrecords[SHOWFILES_TOTALNUMBEROFRECORDS_ARRAY];
}ShowFiles;

static struct RecordState1
{
  uint8_t started; // flag to indicate recording has just been started. // 0 = NOTSTARTED, 1 = STARTED, 2 = ONGOING
  uint8_t saverecord;
  uint32_t pageaddress;
  uint8_t savetime;
  uint32_t sector;
  uint8_t page;

  uint32_t startsector;
  uint32_t endsector;

  uint8_t fileslot;

  uint32_t totalrecordcount;
}RecordState;

static struct RecordStatePrev1
{
  uint8_t started; // flag to indicate recording has just been started. // 0 = NOTSTARTED, 1 = STARTED, 2 = ONGOING
  uint8_t saverecord;
  uint32_t pageaddress;
  uint8_t savetime;
  uint32_t sector;
  uint8_t page;
}RecordStatePrev;

static struct RecordHeader1
{
  uint8_t filenumber;
  uint16_t seqnumber;
  uint8_t recordcount;
  uint8_t status;
  uint8_t crc;
}RecordHeader;

static struct RecordHeaderPrev1
{
  uint8_t filenumber;
  uint16_t seqnumber;
  uint8_t recordcount;
  uint8_t status;
  uint8_t crc;
}RecordHeaderPrev;

static struct PageData1
{
  uint8_t pagedata_w[PAGEDATA_W_ARRAY];
  uint8_t pagedata_r[PAGEDATA_R_ARRAY];
  uint8_t pagedata_rq[LOGTRANSMITDATA_QUADRANTS_ARRAY][LOGTRANSMITDATA_QUADRANTBYTES_ARRAY];  // 4 x 128 bytes
}PageData;

static struct Sensor1
{
  uint32_t temperature;
  uint32_t pressure;
  uint32_t conductivity;
  uint32_t reserved;
}Sensor;

// Log / Graph related structures ---
static struct LogReadSpecificFile1
{
  // Set
  uint8_t whichfile;
  // Resp
  uint8_t reserved;
  uint8_t unknown[36]; // Temporary
}LogReadSpecificFile;

static struct LogTransmitData1
{
  // Set
  uint8_t file;
  uint8_t sector[LOGTRANSMITDATA_SECTOR_ARRAY];
  uint8_t page;
  uint8_t reserved[LOGTRANSMITDATA_RESERVED_ARRAY];
  uint8_t quadrant;

  // Resp
  // we use the 4 quadrant array in PageData structure
}LogTransmitData;


// MS5837 Variables
static uint16_t C[PERSSURESENSOR_ROM_ARRAY]; // Calibration coefficients C0..C6 (7 total)
static uint32_t D1; // raw Digital Pressure Value from pressure sensor
static uint32_t D2; // raw Digital Temperature Value from pressure sensor

#endif /* INC_GLOBALS_H_ */
