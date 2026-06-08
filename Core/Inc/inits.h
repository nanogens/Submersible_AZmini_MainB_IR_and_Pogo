/*
 * init.h
 *
 *  Created on: Sep 5, 2025
 *      Author: MT
 */

#ifndef INC_INITS_H_
#define INC_INITS_H_

//==============================================================================
// Function Prototypes
//==============================================================================
void Inits_Var(void);


void Inits_Var(void)
{
  Lpuart1.sent = 0;
  Lpuart1.crcsend = 0;
  Lpuart1.bufferrx[0] = 0;
  for(Counter.y0 = 0; Counter.y0 < MAX_LPUART1_ARRAY; Counter.y0++)
  {
    Lpuart1.payload[Counter.y0] = 0;
  }
  Lpuart1.status = CLEAR;
  Lpuart1.got = 0;
  Lpuart1.messagelength = 0;
  Lpuart1.messageidglobal = 0;
  Lpuart1.crcmsg = 0;
  Lpuart1.crcset = 0;
  Lpuart1.prevmessageidglobal = 0;

  Lpuart1shadow.messageid = 0;
  for(Counter.y0 = 0; Counter.y0 < MAX_LPUART1_ARRAY; Counter.y0++)
  {
    Lpuart1shadow.payload[Counter.y0] = 0;
  }

  Counter.yi = 0; // special interrupt counter, do NOT use in any place other than TimerX
  Counter.y0 = 0; // main counter, do not use in interrupt
  Counter.y1 = 0; // main counter, do not use in interrupt (e.g. its for 2D arrays)
  Counter.streaming = 0; // counter to switch between CTD stream and Data for the 6 boxes on First page
  Counter.totalvalidpacketcount = 0; // total count of all valid incoming packets
  Counter.reedswitchcount = 0; // to debounce the reed switch
  Counter.repeat = 0;

  // Version
  Version.reserved = 0;
  Version.boxselection = 0;
  for(Counter.y0 = 0; Counter.y0 < MAX_FWVERSION_ARRAY; Counter.y0++)
  {
    Version.fw_version[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < MAX_SWVERSION_ARRAY; Counter.y0++)
  {
    Version.sw_version[Counter.y0] = 0;
  }

  // Status
  Status.reserved = 0;
  Status.boxselection = 0;
  for(Counter.y0 = 0; Counter.y0 < MAX_STATUS_RES_ARRAY; Counter.y0++)
  {
    Status.res[Counter.y0] = 0;
  }

  // Instrument
  Instrument.reserved = 0;
  Instrument.boxselection = 0;

  Instrument.device = 0;
  for(Counter.y0 = 0; Counter.y0 < MAX_INSTRUMENT_SERIAL_ARRAY; Counter.y0++)
  {
    Instrument.serialnumber[Counter.y0] = 0;
  }
  Instrument.serialnumber[0] = 'X';
  Instrument.serialnumber[1] = 'X';
  Instrument.serialnumber[2] = 'X';
  Instrument.serialnumber[3] = '-';
  Instrument.serialnumber[4] = 'X';
  Instrument.serialnumber[5] = 'X';
  Instrument.serialnumber[6] = 'X';
  Instrument.serialnumber[7] = 'X';
  Instrument.serialnumber[8] = 'X';
  Instrument.serialnumber[9] = 'X';
  Instrument.serialnumber[10] = 'X';
  Instrument.serialnumber[11] = 'X';
  Instrument.serialnumber[12] = 'X';
  for(Counter.y0 = 0; Counter.y0 < MAX_INSTRUMENT_USAGE_ARRAY; Counter.y0++)
  {
    Instrument.usage[Counter.y0] = 0;
  }

  // Communications
  Communication.reserved0 = 0;
  Communication.boxselection = 0;
  Communication.connetion = 0;
  Communication.baudrate = 0;

  // Power
  Power.reserved0 = 0;
  Power.boxselection = 0;
  Power.batterytype = 0;
  for(Counter.y0 = 0; Counter.y0 < MAX_POWER_DURATION_ARRAY; Counter.y0++)
  {
    Power.duration[Counter.y0] = 0;
  }
  Power.powerremaining = 0;

  // Time structure
  Time.set_year = 0;
  Time.set_month = 0;
  Time.set_day = 0;
  Time.set_hour = 0;
  Time.set_minute = 0;
  Time.set_second = 0;
  Time.set_ampm = 0;
  Time.set_weekday = 0;

  Time.read_year = 0;       // 0-99 (00-99)
  Time.read_month = 0;      // 1-12
  Time.read_day = 0;        // 1-31
  Time.read_hour = 0;       // 0-23 (24-hour format)
  Time.read_minute = 0;     // 0-59
  Time.read_second = 0;     // 0-59
  Time.read_ampm = 0;       // 0=AM, 1=PM (optional, since we're using 24-hour)
  Time.read_full_year = 0;  // Full year (2025)
  Time.read_weekday = 0;    // 1=Monday, 7=Sunday

  Sampling.reserved0 = 0;
  Sampling.boxselection = 0;
  Sampling.mode = 0;
  Sampling.rate = 0;

  Activation.reserved0 = 0;
  Activation.boxselection = 0;
  Activation.start_year = 0;
  Activation.start_month = 0;
  Activation.start_day = 0;
  Activation.start_hour = 0;
  Activation.start_minute = 0;
  Activation.start_second = 0;
  Activation.start_ampm = 0;
  for(Counter.y0 = 0; Counter.y0 < MAX_ACTIVATE_START_FULLYEAR_ARRAY; Counter.y0++)
  {
    Activation.start_full_year[Counter.y0] = 0;
  }
  Activation.start_weekday = 0;

  Activation.end_year = 0;
  Activation.end_month = 0;
  Activation.end_day = 0;
  Activation.end_hour = 0;
  Activation.end_minute = 0;
  Activation.end_second = 0;
  Activation.end_ampm = 0;
  for(Counter.y0 = 0; Counter.y0 < MAX_ACTIVATE_END_FULLYEAR_ARRAY; Counter.y0++)
  {
	  Activation.end_full_year[Counter.y0] = 0;
  }
  Activation.end_weekday = 0;
  Activation.event = 0;
  Activation.eventtigger = 0;

  for(Counter.y0 = 0; Counter.y0 < MAX_NOTES_NOTE_ARRAY; Counter.y0++)
  {
    Notes.note[Counter.y0] = 0;
  }

  for(Counter.y0 = 0; Counter.y0 < MAX_CLOUD_IP_ARRAY; Counter.y0++)
  {
    Cloud.ip[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < MAX_CLOUD_LOGIN_ARRAY; Counter.y0++)
  {
    Cloud.login[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < MAX_CLOUD_PW_ARRAY; Counter.y0++)
  {
    Cloud.pw[Counter.y0] = 0;
  }

  Misc.stuff = 0;


  // First page - 6 boxes ------------------

  // Box 0
  // Uses the same variables used in Instruments above

  // Box 1
  for(Counter.y0 = 0; Counter.y0 < MAX_MEMORY_TOTAL_ARRAY; Counter.y0++)
  {
    Memory.total[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < MAX_MEMORY_USED_ARRAY; Counter.y0++)
  {
    Memory.used[Counter.y0] = 0;
  }

  // Box 2
  for(Counter.y0 = 0; Counter.y0 < MAX_SURFACE_PRESSURE_ARRAY; Counter.y0++)
  {
    Surface.pressure[Counter.y0] = 0;
  }

  // Box 3
  Battery.cell = 0;
  Battery.type = 0;
  for(Counter.y0 = 0; Counter.y0 < MAX_USAGE_HOURS_ARRAY; Counter.y0++)
  {
    Usage.hours[Counter.y0] = 0;
  }

  // Box 4
  for(Counter.y0 = 0; Counter.y0 < MAX_MESSAGES_RECEIVED_ARRAY; Counter.y0++)
  {
    Messages.received[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < MAX_MESSAGES_SENT_ARRAY; Counter.y0++)
  {
    Messages.sent[Counter.y0] = 0;
  }

  // Box 5
  TabletTime.year = 0;
  TabletTime.month = 0;
  TabletTime.day = 0;
  TabletTime.hour = 0;
  TabletTime.minute = 0;
  TabletTime.second = 0;
  TabletTime.ampm = 0;
  //--
  DeviceTime.year = 0;
  DeviceTime.month = 0;
  DeviceTime.day = 0;
  DeviceTime.hour = 0;
  DeviceTime.minute = 0;
  DeviceTime.second = 0;
  DeviceTime.ampm = 0;
  //--
  UpcomingRecordingTime.year = 0;
  UpcomingRecordingTime.month = 0;
  UpcomingRecordingTime.day = 0;
  UpcomingRecordingTime.hour = 0;
  UpcomingRecordingTime.minute = 0;
  UpcomingRecordingTime.second = 0;
  UpcomingRecordingTime.ampm = 0;


  CTD.depth_i = 0;
  CTD.temp_i = 0;
  CTD.cond_i = 0;

  ReedSwitch.state = DEACTIVATED;

  for(Counter.y0 = 0; Counter.y0 < EEP_TIME_ARRAY; Counter.y0++)
  {
    EEPROMRecord.time[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < EEP_PRESSURE_ARRAY; Counter.y0++)
  {
    EEPROMRecord.pressure[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < EEP_TEMPERATURE_ARRAY; Counter.y0++)
  {
    EEPROMRecord.temperature[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < EEP_CONDUCVITIVY_ARRAY; Counter.y0++)
  {
    EEPROMRecord.conductivity[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < EEP_RESERVED_ARRAY; Counter.y0++)
  {
    EEPROMRecord.reserved[Counter.y0] = 0;
  }

  ShowFiles.reserved = 0;
  ShowFiles.fileindex = 0;
  for(Counter.y0 = 0; Counter.y0 < SHOWFILES_FILECREATIONTIME_ARRAY; Counter.y0++)
  {
	  ShowFiles.filecreationtime[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < SHOWFILES_FILENAME_ARRAY; Counter.y0++)
  {
	  ShowFiles.filename[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < SHOWFILES_TOTALNUMBEROFRECORDS_ARRAY; Counter.y0++)
  {
	  ShowFiles.totalnumberofrecords[Counter.y0] = 0;
  }

  for(Counter.y0 = 0; Counter.y0 < PAGEDATA_W_ARRAY; Counter.y0++)
  {
    PageData.pagedata_w[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < PAGEDATA_R_ARRAY; Counter.y0++)
  {
	PageData.pagedata_r[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < LOGTRANSMITDATA_QUADRANTS_ARRAY; Counter.y0++)
  {
    for(Counter.y1 = 0; Counter.y1 < LOGTRANSMITDATA_QUADRANTBYTES_ARRAY; Counter.y1++)
    {
      PageData.pagedata_rq[Counter.y0][Counter.y1] = 0;
    }
  }

  RecordState.started = RECORDING_NOTSTARTED;
  RecordState.saverecord = RECORDSTATE_DONOTHING; // tells memory FILLPAGE routine what to do (WRITEPAGE, UPDATEPAGE) while recording.
  RecordState.pageaddress = 0;        //
  RecordState.savetime = LESSTHAN30S; // GREATERTHAN30S;
  RecordState.sector = 0;
  RecordState.page = 0;
  RecordState.startsector = 0;
  RecordState.endsector = 0;
  RecordState.fileslot = 0;
  RecordState.totalrecordcount = 0;

  RecordStatePrev.started = RecordState.started;
  RecordStatePrev.saverecord = RecordState.saverecord;
  RecordStatePrev.pageaddress = RecordState.pageaddress;
  RecordStatePrev.savetime = RecordState.savetime;
  RecordStatePrev.sector = RecordState.sector;
  RecordStatePrev.page = RecordState.page;

  RecordHeader.filenumber = 0;                              // 0 to 255
  RecordHeader.seqnumber = 0;                               // Increment with every page written
  RecordHeader.recordcount = 0;                             // Number of 32 byte records currently in this page
  RecordHeader.status = RECORDSTATUS_UNKNOWN;               // 0x02 (Active), 0x01 (Deleted), 0x00 (Unknown)
  RecordHeader.crc = 0;                                     // Header Checksum

  RecordHeaderPrev.filenumber = RecordHeader.filenumber;    // 0 to 255
  RecordHeaderPrev.seqnumber = RecordHeader.seqnumber;      // Increment with every page written
  RecordHeaderPrev.recordcount = RecordHeader.recordcount;  // Number of 32 byte records currently in this page
  RecordHeaderPrev.status = RecordHeader.status;            // 0x01 (Active), 0x00 (Deleted)
  RecordHeaderPrev.crc = RecordHeader.crc;                  // Header Checksum

  // Sensor - readings from sensor stored here before being committed to memory.
  Sensor.temperature = 0;
  Sensor.pressure = 0;
  Sensor.conductivity = 0;
  Sensor.reserved = 0;

  LogReadSpecificFile.whichfile = 0;
  LogReadSpecificFile.reserved = 0;

  // Set
  LogTransmitData.file = 0;
  for(Counter.y0 = 0; Counter.y0 < LOGTRANSMITDATA_SECTOR_ARRAY; Counter.y0++)
  {
    LogTransmitData.sector[Counter.y0];
  }
  for(Counter.y0 = 0; Counter.y0 < LOGTRANSMITDATA_RESERVED_ARRAY; Counter.y0++)
  {
    LogTransmitData.reserved[Counter.y0];
  }
  LogTransmitData.page = 0;
  LogTransmitData.quadrant = 0;

  /*
  for(Counter.y0 = 0; Counter.y0 < MAX_CTD_PRESS_ARRAY; Counter.y0++)
  {
    CTD.press_uc[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < MAX_CTD_TEMP_ARRAY; Counter.y0++)
  {
    CTD.temp_uc[Counter.y0] = 0;
  }
  for(Counter.y0 = 0; Counter.y0 < MAX_CTD_COND_ARRAY; Counter.y0++)
  {
    CTD.cond_uc[Counter.y0] = 0;
  }
  */


  /*
  for(Counter.y0 = 0; Counter.y0 < NUMBEROF_RECORDSBUFFERWRITE_ARRAY; Counter.y0++)
  {
	  for(Counter.y1 = 0; Counter.y1 < RECORDBUFFERWRITE_ARRAY; Counter.y1++)
	  {
          RecordBufferWrite.record[Counter.y0][Counter.y1] = 0;
	  }
  }
  */

  Temporary.temporary = 0;
}

#endif /* INC_INITS_H_ */
