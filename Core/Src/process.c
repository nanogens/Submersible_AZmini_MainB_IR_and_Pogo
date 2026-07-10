/*
 * process.c
 *
 *  Created on: Jun 23, 2026
 *      Author: MT
 */
#include "main.h"
#include "string.h"
#include "process.h"
#include "rtc.h"
#include "memory.h"
#include "lpuart.h"
#include "tim.h"
#include "misc.h"
#include "pressure.h"


// Function definition
void ProcessMsg(void)
{
  if(Lpuart1.status == FILLED)
  {
    switch(Lpuart1shadow.messageid)
	{
      case VERSION_QUERY_MSGID:
    	        Lpuart1shadow.messageid = 0;
    	        break;
      case STATUS_QUERY_MSGID:
    	        Lpuart1shadow.messageid = 0;
    	        break;
      case INSTRUMENT_QUERY_MSGID:
    	        Instrument_Resp();
    	        Lpuart1shadow.messageid = 0;
    	        break;
      case COMMUNICATION_QUERY_MSGID:
    	        Lpuart1shadow.messageid = 0;
    	        break;
      case POWER_QUERY_MSGID:
    	        Lpuart1shadow.messageid = 0;
    	        break;
      case TIME_QUERY_MSGID:
    	        RTC_ReadTime();
    	        Time_Resp();
    	        Lpuart1shadow.messageid = 0;
    	        // Read current time
    	        //RTC_ReadTime();
    	        break;
      case SAMPLING_QUERY_MSGID:
    	        Sampling_Resp();
    	        Lpuart1shadow.messageid = 0;
    	        break;
      case ACTIVATION_QUERY_MSGID:
    	        Activation_Resp();
    	        Lpuart1shadow.messageid = 0;
    	        break;
      case NOTES_QUERY_MSGID:
    	        Notes_Resp();
    	        Lpuart1shadow.messageid = 0;
    	        break;
      case CLOUD_QUERY_MSGID:
    	        Lpuart1shadow.messageid = 0;
    	        break;
      case MISC_QUERY_MSGID:
    	        Lpuart1shadow.messageid = 0;
    	        break;
      case APPLYRECORDINGPLAN_QUERY_MSGID:
    	        ApplyRecordingPlan_Status();
    	        ApplyRecordingPlan_Resp();
	            Lpuart1shadow.messageid = 0;
	            break;

      // Set
      case INSTRUMENT_SET_MSGID:
    	        Instrument_StoreData();
    	        Instrument_Resp();
    	        Lpuart1shadow.messageid = 0;
    	        break;

      case TIME_SET_MSGID:
    	        Time_StoreData();
    	        RTC_ReadTime(); // read back the time data we just stored for the response
    	        Time_Resp();
                Lpuart1shadow.messageid = 0;
                break;

      case SAMPLING_SET_MSGID:
    	        Sampling_Set();
    	        Sampling_Resp();
                Lpuart1shadow.messageid = 0;
    	        break;

      case ACTIVATION_SET_MSGID:
    	        Activation_Set();
    	        Activation_Resp();
    	        break;

      case NOTES_SET_MSGID:
    	        Notes_Set();
    	        Notes_Resp();
    	        break;

      case APPLYRECORDINGPLAN_SET_MSGID:
    	  	   ApplyRecordingPlan_Set();
    	  	   ApplyRecordingPlan_Resp();
    	  	   break;

      // Logging
      case LOG_SHOWFILES_QUERY_MSGID:  // Retrieve basic info on all files
    	        SaveRetrievedPagetoMemory(SHOWFILES_SECTOR, SHOWFILES_PAGE, BREAK_INTO_QUADRANT_YES); // sector 1, page 0
    	        Log_ShowFiles_Resp();
    	        break;

      case LOG_READSPECIFICFILE_SET_MSGID:
    	        Log_ReadSpecificFile_Set();
    	        SaveRetrievedPagetoMemory(READSPECIFICFILE_SECTOR, LogReadSpecificFile.whichfile, BREAK_INTO_QUADRANT_YES); // sector 2, page 0,1,2,3
    	        Log_ReadSpecificFile_Resp();
    	        break;

      case LOG_TRANSMITDATA_SET_MSGID:
    	        Log_TransmitData_Set();
    	        if(LogTransmitData.quadrant == 0)  // breakup of page into 4 quadrants needs to be done only once (on the 0th quadrant)
    	        {
    	          SaveRetrievedPagetoMemory((LogTransmitData.sector[1] << 8) + LogTransmitData.sector[0], LogTransmitData.page, BREAK_INTO_QUADRANT_YES); // sector 3 onwards, page 0 to 7 (in the case of the first file slot for instance)
    	        }
    	        Log_TransmitData_Resp();
    	        break;

      case CTD_READINGS_TARE_SENSOR_SET_MSGID:
    	        CTD_Readings_Tare_Sensor_Set();
    	        CTD_Readings_Tare();
    	        CTD_Readings_Tare_Sensor_Resp();
    	        break;

      // Streaming
      case CTD_READINGS_PROCESSED_QUERY_MSGID:
    	        //HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
    	        DebugSensor();
    	        if(Counter.streaming == 0)
    	        {
      	  	      Submersible_Info_Resp(); //
    	        }
    	        else
    	        {
          	  	  CTD_Reading_Processed_Resp();  // Sends back current Depth, Temperate, Conductivity Readings once every 10 cycles (see if statement below for exact number)
    	        }
    	        // This streams the data for the 6 boxes on the first page
    	        // once every 10 cycles.
    	        // The remaining 9 cycles are dedicated to streaming data
    	        // of the CTD guages.
                Counter.streaming++;
                if(Counter.streaming >= 10)
                {
                  Counter.streaming = 0;
                }
    	  	    Lpuart1shadow.messageid = 0;
    	        break;

      // BOOTLOADER ------------------------------------------------------------
      case BOOTLOADER_FWUPDATE_SET_MSGID:
    	      Bootloader_FWUpdate_Resp();
          	  Bootloader_JumpToBootloader();
          	  break;

      default :
    	      Lpuart1shadow.messageid = 0;
		      break;
	}
    Lpuart1.status = CLEAR;
    Counter.totalvalidpacketcount++; // Increment the valid packet count by 1
  }
}

void CTD_Reading_Processed_Resp(void)
{
  SendHeader(CTD_READINGS_PROCESSED_RESP_MSGLGT, CTD_READINGS_PROCESSED_RESP_MSGID);
  SendByte(CTD_READINGS_PROCESSED_RESP_MSGID); // Box Selection
  SendByte(0x00); // Reserved 0

  //SendByte(CTD.press_uc[0]);
  //SendByte(CTD.press_uc[1]);

  /*
  // This is just temporary to see the Depth guage advance.
  // Enable the block above and disable this block when done testing.
  // Also remove the Temporary structure from globals.h
  Temporary.temporary += 10;
  if(Temporary.temporary > 150)
  {
	Temporary.temporary = 0;
  }
  SendByte((Temporary.temporary >> 8) & 0xFF);
  SendByte((Temporary.temporary) & 0xFF);
  */
  SendByte((CTD.depth_i >> 8) & 0xFF);
  SendByte((CTD.depth_i) & 0xFF);

  SendByte((CTD.temp_i >> 8) & 0xFF);
  SendByte((CTD.temp_i) & 0xFF);

  SendByte((CTD.cond_i >> 8) & 0xFF);
  SendByte((CTD.cond_i) & 0xFF);

  SendByte(0x00); // Reserved 1 (possibly alerts)

  //ReedSwitch.state = ACTIVATED;  // MT temproary
  SendByte(ReedSwitch.state); // Reserved 2 (possibly alerts)

  SendByte(Lpuart1.crcsend);
}

void Submersible_Info_Resp(void)
{
  SendHeader(SUBMERSIBLE_INFO_RESP_MSGLGT, SUBMERSIBLE_INFO_RESP_MSGID);
  SendByte(SUBMERSIBLE_INFO_RESP_MSGID); // Box Selection
  SendByte(0x00); // Reserved 0

  SendByte(Instrument.device); // device

  SendByte(Instrument.serialnumber[0]);
  SendByte(Instrument.serialnumber[1]);
  SendByte(Instrument.serialnumber[2]);
  SendByte(Instrument.serialnumber[3]);
  SendByte(Instrument.serialnumber[4]);
  SendByte(Instrument.serialnumber[5]);
  SendByte(Instrument.serialnumber[6]);
  SendByte(Instrument.serialnumber[7]);
  SendByte(Instrument.serialnumber[8]);
  SendByte(Instrument.serialnumber[9]);
  SendByte(Instrument.serialnumber[10]);
  SendByte(Instrument.serialnumber[11]);
  SendByte(Instrument.serialnumber[12]);

  SendByte(Instrument.usage[0]);
  SendByte(Instrument.usage[1]);

  SendByte(Memory.total[0]);
  SendByte(Memory.total[1]);
  SendByte(Memory.used[0]);
  SendByte(Memory.used[1]);

  SendByte(0x00); // Reserved
  SendByte(0x00); // Reserved

  SendByte(Surface.pressure[0]);
  SendByte(Surface.pressure[1]);

  SendByte(0x00); // Reserved
  SendByte(0x00); // Reserved

  SendByte(Battery.cell);
  SendByte(Battery.type);
  SendByte(Usage.hours[0]);
  SendByte(Usage.hours[1]);

  SendByte(0x00); // Reserved
  SendByte(0x00); // Reserved

  Messages.received[0] = (Counter.totalvalidpacketcount >> 24) & 0xFF;
  Messages.received[1] = (Counter.totalvalidpacketcount >> 16) & 0xFF;
  Messages.received[2] = (Counter.totalvalidpacketcount >> 8) & 0xFF;
  Messages.received[3] = (Counter.totalvalidpacketcount) & 0xFF;

  SendByte(Messages.received[0]);
  SendByte(Messages.received[1]);
  SendByte(Messages.received[2]);
  SendByte(Messages.received[3]);

  SendByte(Messages.sent[0]);
  SendByte(Messages.sent[1]);
  SendByte(Messages.sent[2]);
  SendByte(Messages.sent[3]);

  SendByte(0x00); // Reserved
  SendByte(0x00); // Reserved

  SendByte(TabletTime.year);
  SendByte(TabletTime.month);
  SendByte(TabletTime.day);
  SendByte(TabletTime.hour);
  SendByte(TabletTime.minute);
  SendByte(TabletTime.second);
  SendByte(TabletTime.ampm);
  SendByte(DeviceTime.year);
  SendByte(DeviceTime.month);
  SendByte(DeviceTime.day);
  SendByte(DeviceTime.hour);
  SendByte(DeviceTime.minute);
  SendByte(DeviceTime.second);
  SendByte(DeviceTime.ampm);
  SendByte(UpcomingRecordingTime.year);
  SendByte(UpcomingRecordingTime.month);
  SendByte(UpcomingRecordingTime.day);
  SendByte(UpcomingRecordingTime.hour);
  SendByte(UpcomingRecordingTime.minute);
  SendByte(UpcomingRecordingTime.second);
  SendByte(UpcomingRecordingTime.ampm);

  SendByte(0x00); // Reserved
  SendByte(0x00); // Reserved

  SendByte(Lpuart1.crcsend);
}

void Instrument_StoreData(void)
{
  // Saving the stuff FW received from Viewer (as a SET)
  Instrument.boxselection = Lpuart1shadow.payload[0];
  Instrument.reserved = Lpuart1shadow.payload[1];
  Instrument.device = Lpuart1shadow.payload[2];
  Instrument.serialnumber[0] = Lpuart1shadow.payload[3];
  Instrument.serialnumber[1] = Lpuart1shadow.payload[4];
  Instrument.serialnumber[2] = Lpuart1shadow.payload[5];
  Instrument.serialnumber[3] = Lpuart1shadow.payload[6];
  Instrument.serialnumber[4] = Lpuart1shadow.payload[7];
  Instrument.serialnumber[5] = Lpuart1shadow.payload[8];
  Instrument.serialnumber[6] = Lpuart1shadow.payload[9];
  Instrument.serialnumber[7] = Lpuart1shadow.payload[10];
  Instrument.serialnumber[8] = Lpuart1shadow.payload[11];
  Instrument.serialnumber[9] = Lpuart1shadow.payload[12];
  Instrument.serialnumber[10] = Lpuart1shadow.payload[13];
  Instrument.serialnumber[11] = Lpuart1shadow.payload[14];
  Instrument.serialnumber[12] = Lpuart1shadow.payload[15];
  Instrument.usage[0] = 0; // Lpuart1shadow.payload[16];
  Instrument.usage[1] = 5; // Lpuart1shadow.payload[17];
}

void Instrument_Resp(void)
{
  // Now transmitting from FW as a RESP to Viewer
  SendHeader(INSTRUMENT_RESP_MSGLGT, INSTRUMENT_RESP_MSGID);
  SendByte(Instrument.boxselection); // boxselection
  SendByte(Instrument.reserved); // reserved
  SendByte(Instrument.device); // device
  SendByte(Instrument.serialnumber[0]); // serialnumber[0]
  SendByte(Instrument.serialnumber[1]); // serialnumber[1]
  SendByte(Instrument.serialnumber[2]); // serialnumber[2]
  SendByte(Instrument.serialnumber[3]); // serialnumber[3]
  SendByte(Instrument.serialnumber[4]); // serialnumber[4]
  SendByte(Instrument.serialnumber[5]); // serialnumber[5]
  SendByte(Instrument.serialnumber[6]); // serialnumber[6]
  SendByte(Instrument.serialnumber[7]); // serialnumber[7]
  SendByte(Instrument.serialnumber[8]); // serialnumber[8]
  SendByte(Instrument.serialnumber[9]); // serialnumber[9]
  SendByte(Instrument.serialnumber[10]); // serialnumber[10]
  SendByte(Instrument.serialnumber[11]); // serialnumber[11]
  SendByte(Instrument.serialnumber[12]); // serialnumber[12]
  SendByte(Instrument.usage[0]); // serialnumber[0]
  SendByte(Instrument.usage[1]); // serialnumber[1]
  SendByte(Lpuart1.crcsend);
}

void Time_StoreData(void)
{
  // Saving the stuff FW received from Viewer (as a SET)
  Time.boxselection = Lpuart1shadow.payload[0];
  Time.reserved0 = Lpuart1shadow.payload[1];

  Time.set_year = Lpuart1shadow.payload[2];         // 0-99 (00-99)
  Time.set_month  = Lpuart1shadow.payload[3];      // 1-12
  Time.set_day  = Lpuart1shadow.payload[4];          // 1-31
  Time.set_hour  = Lpuart1shadow.payload[5];        // 1-12 (12-hour format)
  Time.set_minute  = Lpuart1shadow.payload[6];     // 0-59
  Time.set_second  = Lpuart1shadow.payload[7];     // 0-59
  Time.set_ampm  = Lpuart1shadow.payload[8];      // 0=AM, 1=PM (optional, since we're using 24-hour)
  Time.set_weekday  = Lpuart1shadow.payload[9];  // 1=Monday, 7=Sunday

  // Set the new time based on what we received above
  RTC_SetTime();
  time_is_set = 1;
}

void Time_Resp(void)
{
  SendHeader(TIME_RESP_MSGLGT, TIME_RESP_MSGID);
  SendByte(Time.boxselection);     // Box selection
  SendByte(Time.reserved0);         // Reserved0
  SendByte(Time.read_year);         // 0-99 (00-99)
  SendByte(Time.read_month);     // 1-12
  SendByte(Time.read_day);          // 1-31
  SendByte(Time.read_hour);        // 0-59
  SendByte(Time.read_minute);    // 0-59
  SendByte(Time.read_second);    // 0-59
  SendByte(Time.read_ampm);     // 0=AM, 1=PM (optional, since we're using 24-hour)
  SendByte(Time.read_weekday);  // 1=Monday, 7=Sunday
  SendByte(Lpuart1.crcsend);
}

void Sampling_Resp(void)
{
  SendHeader(SAMPLING_RESP_MSGLGT, SAMPLING_RESP_MSGID);
  SendByte(Sampling.boxselection);    // Box selection
  SendByte(Sampling.reserved0);         // Reserved 0
  SendByte(Sampling.mode);               // 0 = Time - Continuous, 1 = Time - Do not Loop, 2 = Event Triggered
  SendByte(Sampling.rate);                  // 0 = 0.1 sec, 1 = 0.5 sec, 2 = 1 sec, 3 = 5 sec, 4 = 10 sec, 5 = 30 sec, 6 = 1 min, 7 = 5 min, 8 = 10 min, 9 = 30 min, 10 = 1 hour
  SendByte(Sampling.reserved1);         // Reserved 1
  SendByte(Sampling.reserved2);         // Reserved 2
  SendByte(Lpuart1.crcsend);
}

void Sampling_Set(void)
{
  Sampling.boxselection = Lpuart1shadow.payload[0];
  Sampling.reserved0 = Lpuart1shadow.payload[1];
  Sampling.mode = Lpuart1shadow.payload[2];
  Sampling.rate = Lpuart1shadow.payload[3];
  Sampling.reserved1 = Lpuart1shadow.payload[4];
  Sampling.reserved2 = Lpuart1shadow.payload[5];
  sampling_is_set = 1;
}

void Activation_Resp(void)
{
  SendHeader(ACTIVATION_RESP_MSGLGT, ACTIVATION_RESP_MSGID);
  SendByte(Activation.boxselection);
  SendByte(Activation.reserved0);

  SendByte(Activation.start_year);
  SendByte(Activation.start_month);
  SendByte(Activation.start_day);
  SendByte(Activation.start_hour);
  SendByte(Activation.start_minute);
  SendByte(Activation.start_second);
  SendByte(Activation.start_ampm);
  SendByte(Activation.start_full_year[0]);
  SendByte(Activation.start_full_year[1]);
  SendByte(Activation.start_weekday);

  SendByte(Activation.end_year);
  SendByte(Activation.end_month);
  SendByte(Activation.end_day);
  SendByte(Activation.end_hour);
  SendByte(Activation.end_minute);
  SendByte(Activation.end_second);
  SendByte(Activation.end_ampm);
  SendByte(Activation.end_full_year[0]);
  SendByte(Activation.end_full_year[1]);
  SendByte(Activation.end_weekday);

  SendByte(Activation.eventtrigger);
  SendByte(Activation.eventthreshold);
  SendByte(Lpuart1.crcsend);
}

void Activation_Set(void)
{
  // General info
  Activation.boxselection = Lpuart1shadow.payload[0];
  Activation.reserved0 = Lpuart1shadow.payload[1];
  // Start Time
  Activation.start_year = Lpuart1shadow.payload[2];
  Activation.start_month = Lpuart1shadow.payload[3];
  Activation.start_day = Lpuart1shadow.payload[4];
  Activation.start_hour = Lpuart1shadow.payload[5];
  Activation.start_minute = Lpuart1shadow.payload[6];
  Activation.start_second = Lpuart1shadow.payload[7];
  Activation.start_ampm = Lpuart1shadow.payload[8];
  Activation.start_full_year[0] = Lpuart1shadow.payload[9];
  Activation.start_full_year[1] = Lpuart1shadow.payload[10];
  Activation.start_weekday = Lpuart1shadow.payload[11];
  // End Time
  Activation.end_year = Lpuart1shadow.payload[12];
  Activation.end_month = Lpuart1shadow.payload[13];
  Activation.end_day = Lpuart1shadow.payload[14];
  Activation.end_hour = Lpuart1shadow.payload[15];
  Activation.end_minute = Lpuart1shadow.payload[16];
  Activation.end_second = Lpuart1shadow.payload[17];
  Activation.end_ampm = Lpuart1shadow.payload[18];
  Activation.end_full_year[0] = Lpuart1shadow.payload[19];
  Activation.end_full_year[1] = Lpuart1shadow.payload[20];
  Activation.end_weekday = Lpuart1shadow.payload[21];
  // Event related info
  Activation.eventtrigger = Lpuart1shadow.payload[22];
  Activation.eventthreshold = Lpuart1shadow.payload[23];
}

void Notes_Resp(void)
{
  SendHeader(NOTES_RESP_MSGLGT, NOTES_RESP_MSGID);
  SendByte(Notes.boxselection);
  SendByte(Notes.reserved0);
  // Name - 24 bytes
  for(uint8_t x = 0; x < MAX_NOTES_NAME_ARRAY; x++)
  {
    SendByte(Notes.name[x]);
  }
  // Location - 24 bytes
  for(uint8_t y = 0; y < MAX_NOTES_LOCATION_ARRAY; y++)
  {
    SendByte(Notes.location[y]);
  }
  // Note - 128 bytes
  for(uint8_t z = 0; z < MAX_NOTES_NOTE_ARRAY; z++)
  {
    SendByte(Notes.note[z]);
  }
  SendByte(Lpuart1.crcsend);
}

void Notes_Set(void)
{
  // General info
  Notes.boxselection = Lpuart1shadow.payload[0];
  Notes.reserved0 = Lpuart1shadow.payload[1];
  // Name - 24 bytes
  for(uint8_t x = 0; x < MAX_NOTES_NAME_ARRAY;x++)
  {
    Notes.name[x] = Lpuart1shadow.payload[2 + x];
  }
  // Location - 24 bytes
  for(uint8_t y = 0; y < MAX_NOTES_LOCATION_ARRAY;y++)
  {
    Notes.location[y] = Lpuart1shadow.payload[(2 + MAX_NOTES_NAME_ARRAY) + y];
  }
  // Note - 128 bytes
  for(uint8_t z = 0; z < MAX_NOTES_NOTE_ARRAY;z++)
  {
    Notes.note[z] = Lpuart1shadow.payload[(2 + MAX_NOTES_NAME_ARRAY + MAX_NOTES_LOCATION_ARRAY) + z];
  }
}

void ApplyRecordingPlan_Status(void)
{
  uint8_t time_status = time_is_set ? 1 : 0;
  uint8_t sampling_status = sampling_is_set ? 1 : 0;
  uint8_t activation_status = (Activation.start_year > 0) ? 1 : 0;
  uint8_t notes_status = (Notes.name[0] != 0 || Notes.location[0] != 0) ? 1 : 0;

  ApplyRecordingPlan.status_a = (time_status & 0x03) |
                                ((sampling_status & 0x03) << 2) |
                                ((activation_status & 0x03) << 4) |
                                ((notes_status & 0x03) << 6);
  ApplyRecordingPlan.status_b = 0; // Reserved for future categories
}

void ApplyRecordingPlan_Resp(void)
{
  ApplyRecordingPlan_Status();
  SendHeader(APPLYRECORDINGPLAN_RESP_MSGLGT, APPLYRECORDINGPLAN_RESP_MSGID);
  SendByte(ApplyRecordingPlan.boxselection);
  SendByte(ApplyRecordingPlan.reserved0);
  SendByte(ApplyRecordingPlan.run);
  SendByte(ApplyRecordingPlan.started);
  SendByte(ApplyRecordingPlan.status_a);
  SendByte(ApplyRecordingPlan.status_b);
  SendByte(Lpuart1.crcsend);
}

void ApplyRecordingPlan_Set(void)
{
  // General info
  ApplyRecordingPlan.boxselection = Lpuart1shadow.payload[0];
  ApplyRecordingPlan.reserved0 = Lpuart1shadow.payload[1];

  ApplyRecordingPlan.run = Lpuart1shadow.payload[2];

  if (ApplyRecordingPlan.run == PLAN_RUN_NO)
  {
      ApplyRecordingPlan.started = PLAN_NOTSTARTED;
  }

  // If changed to PLAN_RUN_NO while recording is active, abort immediately
  if (ApplyRecordingPlan.run == PLAN_RUN_NO && RecordState.started == RECORDING_ONGOING)
  {
      ReedSwitch.state = DEACTIVATED;
      DebugMemory4(); // Write final records and stop

      HaltRecording();
      RecordState.sector = 0;
      Counter.repeat = 0;

      is_timer_triggered = 0;
      HAL_TIM_Base_Stop_IT(&htim2);
      HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_RESET);  // turn off LED_A
  }
}

void Log_ShowFiles_Resp(void)
{
  int i = 0;
  SendHeader(LOG_SHOWFILES_RESP_MSGLGT, LOG_SHOWFILES_RESP_MSGID);
  for(i = 0; i < LOGTRANSMITDATA_QUADRANTS_ARRAY; i++)
  {
    SendByte(PageData.pagedata_rq[i][0]); // Reserved
    SendByte(PageData.pagedata_rq[i][1]); // File Index

    SendByte(PageData.pagedata_rq[i][2]); // File Name 0
    SendByte(PageData.pagedata_rq[i][3]); // File Name 1
    SendByte(PageData.pagedata_rq[i][4]); // File Name 2
    SendByte(PageData.pagedata_rq[i][5]); // File Name 3
    SendByte(PageData.pagedata_rq[i][6]); // File Name 4
    SendByte(PageData.pagedata_rq[i][7]); // File Name 5
    SendByte(PageData.pagedata_rq[i][8]); // File Name 6
    SendByte(PageData.pagedata_rq[i][9]); // File Name 7

    SendByte(PageData.pagedata_rq[i][10]); // File Length 0
    SendByte(PageData.pagedata_rq[i][11]); // File Length 1
    SendByte(PageData.pagedata_rq[i][12]); // File Length 2
    SendByte(PageData.pagedata_rq[i][13]); // File Length 3

    SendByte(PageData.pagedata_rq[i][14]); // File Date 0
    SendByte(PageData.pagedata_rq[i][15]); // File Date 1
    SendByte(PageData.pagedata_rq[i][16]); // File Date 2
    SendByte(PageData.pagedata_rq[i][17]); // File Date 3
    SendByte(PageData.pagedata_rq[i][18]); // File Date 4
    SendByte(PageData.pagedata_rq[i][19]); // File Date 5
    SendByte(PageData.pagedata_rq[i][20]); // File Date 6
    SendByte(PageData.pagedata_rq[i][21]); // File Date 7
  }
  SendByte(Lpuart1.crcsend);
}

void Log_ReadSpecificFile_Set(void)
{
  if(Lpuart1shadow.payload[0] < READSPECIFICFILE_FILESLOTS)
  {
    LogReadSpecificFile.whichfile = Lpuart1shadow.payload[0];
  }
  else  //   Error: LogReadSpecificFile.whichfile exceeds array boundary!
  {
#if DEBUG_SENSOR
	SendString((uint8_t*)"\nError: LogReadSpecificFile.whichfile exceeds array boundary!");
#endif
  }
}

void Log_ReadSpecificFile_Resp(void)
{
    if(LogReadSpecificFile.whichfile < READSPECIFICFILE_FILESLOTS)
    {
        for(uint8_t quadrant = 0; quadrant < 4; quadrant++)
        {
            uint8_t txBuffer[LOG_READSPECIFICFILE_RESP_MSGLGT];
            uint8_t idx = 0;

            txBuffer[idx++] = DLE;
            txBuffer[idx++] = STX;
            txBuffer[idx++] = SOURCE;
            txBuffer[idx++] = DEST;
            txBuffer[idx++] = LOG_READSPECIFICFILE_RESP_MSGLGT;
            txBuffer[idx++] = LOG_READSPECIFICFILE_RESP_MSGID;
            txBuffer[idx++] = LogReadSpecificFile.whichfile;
            txBuffer[idx++] = quadrant;
            memcpy(&txBuffer[idx], PageData.pagedata_rq[quadrant], 128);
            idx += 128;

            // Calculate CRC
            uint8_t crc = 0;
            for (int i = 0; i < idx; i++) {
                crc += txBuffer[i];
            }
            txBuffer[idx++] = crc;

            Send_Data(txBuffer, idx);
            // Remove HAL_Delay(10) - not needed if LPUART_Send waits for completion
        }
    }
}

void Log_TransmitData_Set(void)
{
  LogTransmitData.file = Lpuart1shadow.payload[0];
  LogTransmitData.sector[1] = Lpuart1shadow.payload[1];
  LogTransmitData.sector[0] = Lpuart1shadow.payload[2];
  LogTransmitData.page = Lpuart1shadow.payload[3];
  LogTransmitData.reserved[1] = Lpuart1shadow.payload[4];
  LogTransmitData.reserved[0] = Lpuart1shadow.payload[5];
  
  if (Lpuart1shadow.payload[6] < LOGTRANSMITDATA_QUADRANTS_ARRAY)
  {
    LogTransmitData.quadrant = Lpuart1shadow.payload[6];
  }
  else
  {
    LogTransmitData.quadrant = 0; // Default to 0 if out-of-bounds
  }
}

void Log_TransmitData_Resp(void)
{
    // Small delay to ensure the page data is stable in memory
    // This is especially important for IrDA where timing can be critical
    HAL_Delay(2);

    SendHeader(LOG_TRANSMITDATA_RESP_MSGLGT, LOG_TRANSMITDATA_RESP_MSGID);
    SendByte(LogTransmitData.file);
    SendByte(LogTransmitData.sector[1]);
    SendByte(LogTransmitData.sector[0]);
    SendByte(LogTransmitData.page);
    SendByte(LogTransmitData.reserved[1]);
    SendByte(LogTransmitData.reserved[0]);
    SendByte(LogTransmitData.quadrant);
    for(uint8_t i = 0; i < LOGTRANSMITDATA_QUADRANTBYTES_ARRAY; i++)  // 128 bytes
    {
        SendByte(PageData.pagedata_rq[LogTransmitData.quadrant][i]);
    }
    SendByte(Lpuart1.crcsend);
}

void CTD_Readings_Tare_Sensor_Set(void)
{
    CTDReadingsTareSensor.tare = Lpuart1shadow.payload[0];
}

void CTD_Readings_Tare_Sensor_Resp(void)
{
    SendHeader(CTD_READINGS_TARE_SENSOR_RESP_MSGLGT, CTD_READINGS_TARE_SENSOR_RESP_MSGID);
    SendByte(CTDReadingsTareSensor.tare);
    SendByte(Lpuart1.crcsend);
}

void CTD_Readings_Tare(void)
{
    float press = 0.0f;
    float temp = 0.0f;
    if (MS5837_ReadPT_FirstOrder(&press, &temp)) {
        surface_pressure = press;
    }
}

void Bootloader_FWUpdate_Resp(void)
{
    SendHeader(BOOTLOADER_FWUPDATE_RESP_MSGLGT, BOOTLOADER_FWUPDATE_RESP_MSGID);
    SendByte(Lpuart1shadow.payload[0]);
    SendByte(Lpuart1.crcsend);
}

void Bootloader_JumpToBootloader(void)
{
  // 1. Disable all interrupts to prevent any code execution during reset trigger
  __disable_irq();

  // 2. Trigger a Software Reset
  // This sets the RCC_FLAG_SFTRST flag that the Bootloader checks above
  NVIC_SystemReset();

  // The MCU resets here. It will start execution at the Bootloader's Reset_Handler.
}
