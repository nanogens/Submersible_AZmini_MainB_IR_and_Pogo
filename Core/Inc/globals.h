/*
 * globals.h
 *
 *  Created on: Sep 3, 2025
 *      Author: MT
 */

#ifndef INC_GLOBALS_H_
#define INC_GLOBALS_H_

#include <stdint.h>
#include "defines.h"

struct Lpuartl1 {
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
};
extern struct Lpuartl1 Lpuart1;

struct Lpuartshadow11 {
  uint8_t messageid;
  uint8_t payload[MAX_LPUART1_ARRAY];
};
extern struct Lpuartshadow11 Lpuart1shadow;

struct Counter1 {
  uint32_t yi;
  uint32_t y0;
  uint32_t y1;
  uint8_t streaming;
  uint32_t totalvalidpacketcount;
  uint32_t reedswitchcount;
  int repeat;
};
extern struct Counter1 Counter;

struct Version1 {
    uint8_t reserved;
    uint8_t boxselection;
    uint8_t fw_version[MAX_FWVERSION_ARRAY];
    uint8_t sw_version[MAX_SWVERSION_ARRAY];
};
extern struct Version1 Version;

struct Status1 {
  uint8_t reserved;
  uint8_t boxselection;
  uint8_t res[4];
};
extern struct Status1 Status;

struct Instrument1 {
  uint8_t reserved;
  uint8_t boxselection;
  uint8_t device;
  uint8_t serialnumber[MAX_INSTRUMENT_SERIAL_ARRAY];
  uint8_t usage[MAX_INSTRUMENT_USAGE_ARRAY];
};
extern struct Instrument1 Instrument;

struct Communication1 {
  uint8_t reserved0;
  uint8_t boxselection;
  uint8_t connetion;
  uint8_t baudrate;
};
extern struct Communication1 Communication;

struct Power1 {
  uint8_t reserved0;
  uint8_t boxselection;
  uint8_t batterytype;
  uint8_t duration[MAX_POWER_DURATION_ARRAY];
  uint8_t powerremaining;
};
extern struct Power1 Power;

struct Time1 {
  uint8_t reserved0;
  uint8_t boxselection;
  uint8_t set_year;
  uint8_t set_month;
  uint8_t set_day;
  uint8_t set_hour;
  uint8_t set_minute;
  uint8_t set_second;
  uint8_t set_ampm;
  uint8_t set_weekday;
  uint8_t read_year;
  uint8_t read_month;
  uint8_t read_day;
  uint8_t read_hour;
  uint8_t read_minute;
  uint8_t read_second;
  uint8_t read_centisecond;
  uint8_t read_ampm;
  uint16_t read_full_year;
  uint8_t read_weekday;
};
extern struct Time1 Time;

struct Sampling1 {
  uint8_t boxselection;
  uint8_t reserved0;
  uint8_t mode;
  uint8_t rate;
  uint8_t reserved1;
  uint8_t reserved2;
};
extern struct Sampling1 Sampling;

struct Activation1 {
  uint8_t boxselection;
  uint8_t reserved0;
  uint8_t start_year;
  uint8_t start_month;
  uint8_t start_day;
  uint8_t start_hour;
  uint8_t start_minute;
  uint8_t start_second;
  uint8_t start_ampm;
  uint16_t start_full_year[MAX_ACTIVATE_START_FULLYEAR_ARRAY];
  uint8_t start_weekday;
  uint8_t end_year;
  uint8_t end_month;
  uint8_t end_day;
  uint8_t end_hour;
  uint8_t end_minute;
  uint8_t end_second;
  uint8_t end_ampm;
  uint8_t end_full_year[MAX_ACTIVATE_END_FULLYEAR_ARRAY];
  uint8_t end_weekday;
  uint8_t eventtrigger;
  uint8_t eventthreshold;
};
extern struct Activation1 Activation;

struct Notes1 {
  uint8_t boxselection;
  uint8_t reserved0;
  uint8_t name[MAX_NOTES_NAME_ARRAY];
  uint8_t location[MAX_NOTES_LOCATION_ARRAY];
  uint8_t note[MAX_NOTES_NOTE_ARRAY];
};
extern struct Notes1 Notes;

struct Cloud1 {
  uint8_t boxselection;
  uint8_t reserved0;
  uint8_t ip[MAX_CLOUD_IP_ARRAY];
  uint8_t login[MAX_CLOUD_LOGIN_ARRAY];
  uint8_t pw[MAX_CLOUD_PW_ARRAY];
};
extern struct Cloud1 Cloud;

struct Misc1 {
  uint8_t boxselection;
  uint8_t reserved0;
  uint8_t stuff;
};
extern struct Misc1 Misc;

struct ApplyRecordingPlan1 {
  uint8_t boxselection;
  uint8_t reserved0;
  uint8_t run;
  uint8_t started;
  uint8_t status_a;
  uint8_t status_b;
};
extern struct ApplyRecordingPlan1 ApplyRecordingPlan;
extern volatile uint8_t is_timer_triggered;
extern volatile uint8_t recording_timer_expired;
extern uint8_t time_is_set;
extern uint8_t sampling_is_set;


struct Memory1 {
  uint8_t total[MAX_MEMORY_TOTAL_ARRAY];
  uint8_t used[MAX_MEMORY_USED_ARRAY];
};
extern struct Memory1 Memory;

struct Surface1 {
  uint8_t pressure[MAX_SURFACE_PRESSURE_ARRAY];
};
extern struct Surface1 Surface;

struct Battery1 {
  uint8_t cell;
  uint8_t type;
};
extern struct Battery1 Battery;

struct Usage1 {
  uint8_t hours[MAX_USAGE_HOURS_ARRAY];
};
extern struct Usage1 Usage;

struct Messages1 {
  uint8_t received[MAX_MESSAGES_RECEIVED_ARRAY];
  uint8_t sent[MAX_MESSAGES_SENT_ARRAY];
};
extern struct Messages1 Messages;

struct TabletTime1 {
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t ampm;
};
extern struct TabletTime1 TabletTime;

struct DeviceTime1 {
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t ampm;
};
extern struct DeviceTime1 DeviceTime;

struct UpcomingRecordingTime1 {
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t ampm;
};
extern struct UpcomingRecordingTime1 UpcomingRecordingTime;

struct CTD1 {
  uint16_t depth_i;
  uint16_t temp_i;
  uint16_t cond_i;
};
extern struct CTD1 CTD;

struct ReedSwitch1 {
  volatile uint8_t state;
};
extern struct ReedSwitch1 ReedSwitch;

struct Temporary1 {
  uint16_t temporary;
};
extern struct Temporary1 Temporary;

struct EEPROMRecord1 {
  uint8_t time[EEP_TIME_ARRAY];
  uint8_t pressure[EEP_PRESSURE_ARRAY];
  uint8_t temperature[EEP_TEMPERATURE_ARRAY];
  uint8_t conductivity[EEP_CONDUCVITIVY_ARRAY];
  uint8_t reserved[EEP_RESERVED_ARRAY];
};
extern struct EEPROMRecord1 EEPROMRecord;

struct ShowFiles1 {
  uint8_t reserved;
  uint8_t fileindex;
  uint8_t filename[SHOWFILES_FILENAME_ARRAY];
  uint8_t filecreationtime[SHOWFILES_FILECREATIONTIME_ARRAY];
  uint8_t totalnumberofrecords[SHOWFILES_TOTALNUMBEROFRECORDS_ARRAY];
};
extern struct ShowFiles1 ShowFiles;

struct RecordState1 {
  volatile uint8_t started;
  uint8_t saverecord;
  uint32_t pageaddress;
  uint8_t savetime;
  uint32_t sector;
  uint8_t page;
  uint32_t startsector;
  uint32_t endsector;
  uint8_t fileslot;
  uint32_t totalrecordcount;
};
extern struct RecordState1 RecordState;

struct RecordStatePrev1 {
  uint8_t started;
  uint8_t saverecord;
  uint32_t pageaddress;
  uint8_t savetime;
  uint32_t sector;
  uint8_t page;
};
extern struct RecordStatePrev1 RecordStatePrev;

struct RecordHeader1 {
  uint8_t filenumber;
  uint16_t seqnumber;
  uint8_t recordcount;
  uint8_t status;
  uint8_t crc;
};
extern struct RecordHeader1 RecordHeader;

struct RecordHeaderPrev1 {
  uint8_t filenumber;
  uint16_t seqnumber;
  uint8_t recordcount;
  uint8_t status;
  uint8_t crc;
};
extern struct RecordHeaderPrev1 RecordHeaderPrev;

struct PageData1 {
  uint8_t pagedata_w[PAGEDATA_W_ARRAY];
  uint8_t pagedata_r[PAGEDATA_R_ARRAY];
  uint8_t pagedata_rq[LOGTRANSMITDATA_QUADRANTS_ARRAY][LOGTRANSMITDATA_QUADRANTBYTES_ARRAY];
};
extern struct PageData1 PageData;

struct Sensor1 {
  uint32_t temperature;
  uint32_t pressure;
  uint32_t conductivity;
  uint32_t reserved;
};
extern struct Sensor1 Sensor;

struct LogReadSpecificFile1 {
  uint8_t whichfile;
  uint8_t reserved;
  uint8_t unknown[36];
};
extern struct LogReadSpecificFile1 LogReadSpecificFile;

struct LogTransmitData1 {
  uint8_t file;
  uint8_t sector[LOGTRANSMITDATA_SECTOR_ARRAY];
  uint8_t page;
  uint8_t reserved[LOGTRANSMITDATA_RESERVED_ARRAY];
  uint8_t quadrant;
};
extern struct LogTransmitData1 LogTransmitData;

struct CTDReadingsTareSensor1 {
  uint8_t tare;
};
extern struct CTDReadingsTareSensor1 CTDReadingsTareSensor;

extern uint16_t C[PERSSURESENSOR_ROM_ARRAY];
extern uint32_t D1;
extern uint32_t D2;

#endif
