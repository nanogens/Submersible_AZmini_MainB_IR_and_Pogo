#include "main.h"
#include <stdbool.h>
#include <stdio.h>
#include "misc.h"
#include "memory.h"

// Desc: Detects the reed switch state
//       Does a 32000 continuous AND to de-bounce the switch
void ReedSwitch_Detect(void)
{
  // Check if the button on PA0 is pressed (assumes pull-up, so pressed = LOW)
  if(HAL_GPIO_ReadPin(REC_START_GPIO_Port, REC_START_Pin) == GPIO_PIN_RESET)
  {
	if(Counter.reedswitchcount < 65535) // for debouncing the reed switch
	{
	  Counter.reedswitchcount++;
	}
	else
	{
	  // Turn on the LED_A
	  HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_SET);
	  ReedSwitch.state = ACTIVATED;
	}
  }
  else
  {
	// Turn off the LED_A
	HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_RESET);
	Counter.reedswitchcount = 0;

	ReedSwitch.state = DEACTIVATED;
  }
}

void Clear_PagedataRead_Array(void)
{
  for(int i = 0;i < PAGEDATA_R_ARRAY;i++)
  {
    PageData.pagedata_r[i] = 0xFF;
  }
}

void Clear_PagedataWrite_Array(void)
{
  for(uint16_t i = 0;i < PAGEDATA_W_ARRAY;i++)
  {
    PageData.pagedata_w[i] = 0xFF;
  }
}

void FileHeader_TestFill(void)
{
  uint16_t offset = 0;

  // FILE-000 ------------------------------------------------
  offset = (0 * 128);  // Page 1

  // Reserved
  PageData.pagedata_w[offset + 0] = 0x09; // Reserved 0 - Temporary (fixed)
  // File Index
  PageData.pagedata_w[offset + 1] = 0x00;
  // File Name
  PageData.pagedata_w[offset + 2] = 0x46;  // F
  PageData.pagedata_w[offset + 3] = 0x49;  // I
  PageData.pagedata_w[offset + 4] = 0x4C;  // L
  PageData.pagedata_w[offset + 5] = 0x45;  // E
  PageData.pagedata_w[offset + 6] = 0x5F;  // -
  PageData.pagedata_w[offset + 7] = 0x30;  // 0
  PageData.pagedata_w[offset + 8] = 0x30;  // 0
  PageData.pagedata_w[offset + 9] = 0x30;  // 0
  // Total Number of Records - Number of Records Written Thus Far
  PageData.pagedata_w[offset + 10] = 0x00;
  PageData.pagedata_w[offset + 11] = 0x00;
  PageData.pagedata_w[offset + 12] = 0x00;
  PageData.pagedata_w[offset + 13] = 0x00;
  // File Date
  PageData.pagedata_w[offset + 14] = 0x1A;  // YY
  PageData.pagedata_w[offset + 15] = 0x02;  // MM
  PageData.pagedata_w[offset + 16] = 0x1B;  // DD
  PageData.pagedata_w[offset + 17] = 0x18;  // HH
  PageData.pagedata_w[offset + 18] = 0x0D; // MM
  PageData.pagedata_w[offset + 19] = 0x1E;  // SS
  PageData.pagedata_w[offset + 20] = 0x01;  // AM/PM
  PageData.pagedata_w[offset + 21] = 0x00;  // RESERVED

  // FILE-001 ------------------------------------------------
  offset = (1 * 128);  // Page 1

  // Reserved
  PageData.pagedata_w[offset + 0] = 0x09; // Reserved 1 - Temporary (fixed)
  // File Index
  PageData.pagedata_w[offset + 1] = 0x01;
  // File Name
  PageData.pagedata_w[offset + 2] = 0x46;  // F
  PageData.pagedata_w[offset + 3] = 0x49;  // I
  PageData.pagedata_w[offset + 4] = 0x4C;  // L
  PageData.pagedata_w[offset + 5] = 0x45;  // E
  PageData.pagedata_w[offset + 6] = 0x5F;  // -
  PageData.pagedata_w[offset + 7] = 0x30;  // 0
  PageData.pagedata_w[offset + 8] = 0x30;  // 0
  PageData.pagedata_w[offset + 9] = 0x31;  // 1
  // Total Number of Records - Number of Records Written Thus Far
  PageData.pagedata_w[offset + 10] = 0x00;
  PageData.pagedata_w[offset + 11] = 0x00;
  PageData.pagedata_w[offset + 12] = 0x00;
  PageData.pagedata_w[offset + 13] = 0x00;
  // File Date
  PageData.pagedata_w[offset + 14] = 0x1A;  // YY
  PageData.pagedata_w[offset + 15] = 0x02;  // MM
  PageData.pagedata_w[offset + 16] = 0x1B;  // DD
  PageData.pagedata_w[offset + 17] = 0x18;  // HH
  PageData.pagedata_w[offset + 18] = 0x0D; // MM
  PageData.pagedata_w[offset + 19] = 0x1F;  // SS
  PageData.pagedata_w[offset + 20] = 0x01;  // AM/PM
  PageData.pagedata_w[offset + 21] = 0x00;  // RESERVED

  // FILE-002 ------------------------------------------------
  offset = (2 * 128);  // Page 2

  // Reserved
  PageData.pagedata_w[offset + 0] = 0x09; // Reserved 2 - Temporary (fixed)
  // File Index
  PageData.pagedata_w[offset + 1] = 0x02;
  // File Name
  PageData.pagedata_w[offset + 2] = 0x46;  // F
  PageData.pagedata_w[offset + 3] = 0x49;  // I
  PageData.pagedata_w[offset + 4] = 0x4C;  // L
  PageData.pagedata_w[offset + 5] = 0x45;  // E
  PageData.pagedata_w[offset + 6] = 0x5F;  // -
  PageData.pagedata_w[offset + 7] = 0x30;  // 0
  PageData.pagedata_w[offset + 8] = 0x30;  // 0
  PageData.pagedata_w[offset + 9] = 0x32;  // 2
  // Total Number of Records - Number of Records Written Thus Far
  PageData.pagedata_w[offset + 10] = 0x00;
  PageData.pagedata_w[offset + 11] = 0x00;
  PageData.pagedata_w[offset + 12] = 0x00;
  PageData.pagedata_w[offset + 13] = 0x00;
  // File Date
  PageData.pagedata_w[offset + 14] = 0x1A;  // YY
  PageData.pagedata_w[offset + 15] = 0x02;  // MM
  PageData.pagedata_w[offset + 16] = 0x1B;  // DD
  PageData.pagedata_w[offset + 17] = 0x18;  // HH
  PageData.pagedata_w[offset + 18] = 0x0D; // MM
  PageData.pagedata_w[offset + 19] = 0x20;  // SS
  PageData.pagedata_w[offset + 20] = 0x01;  // AM/PM
  PageData.pagedata_w[offset + 21] = 0x00;  // RESERVED

  // FILE-003 ------------------------------------------------
  offset = (3 * 128);  // Page 3

  // Reserved
  PageData.pagedata_w[offset + 0] = 0x09; // Reserved 3 - Temporary (fixed)
  // File Index
  PageData.pagedata_w[offset + 1] = 0x03;
  // File Name
  PageData.pagedata_w[offset + 2] = 0x46;  // F
  PageData.pagedata_w[offset + 3] = 0x49;  // I
  PageData.pagedata_w[offset + 4] = 0x4C;  // L
  PageData.pagedata_w[offset + 5] = 0x45;  // E
  PageData.pagedata_w[offset + 6] = 0x5F;  // -
  PageData.pagedata_w[offset + 7] = 0x30;  // 0
  PageData.pagedata_w[offset + 8] = 0x30;  // 0
  PageData.pagedata_w[offset + 9] = 0x33;  // 3
  // Total Number of Records - Number of Records Written Thus Far
  PageData.pagedata_w[offset + 10] = 0x00;
  PageData.pagedata_w[offset + 11] = 0x00;
  PageData.pagedata_w[offset + 12] = 0x00;
  PageData.pagedata_w[offset + 13] = 0x00;
  // File Date
  PageData.pagedata_w[offset + 14] = 0x1A;  // YY
  PageData.pagedata_w[offset + 15] = 0x02;  // MM
  PageData.pagedata_w[offset + 16] = 0x1B;  // DD
  PageData.pagedata_w[offset + 17] = 0x18;  // HH
  PageData.pagedata_w[offset + 18] = 0x0D; // MM
  PageData.pagedata_w[offset + 19] = 0x21;  // SS
  PageData.pagedata_w[offset + 20] = 0x01;  // AM/PM
  PageData.pagedata_w[offset + 21] = 0x00;  // RESERVED

  // Enter sector and page and calculate address
  RecordState.pageaddress = (4096 * SHOWFILES_SECTOR) + (512 * SHOWFILES_PAGE);   // start at Sector 1, Page 0

  Mem_WriteData(RecordState.pageaddress, PageData.pagedata_w, PAGEDATA_W_ARRAY);
}

void SpecificFileHeader_TestFill(void)
{

}

// Clears Sector 2, first 4 of 8 pages to 0xFF
void FileMeta_ClearSector(void)
{
  uint32_t test_address = 0;

  Clear_PagedataWrite_Array();

  for(uint8_t index=0; index < READSPECIFICFILE_FILESLOTS; index++)
  {
    test_address = (BYTES_PER_SECTOR * READSPECIFICFILE_SECTOR) + (PAGEDATA_W_ARRAY * index);
	Mem_WriteData(test_address, PageData.pagedata_w, PAGEDATA_W_ARRAY);
  }
}

void TestingRuns(void)
{
  HAL_Delay(2000);

  // 1. Handle the "Timeline" of the Reed Switch
  if(Counter.repeat == 10 || Counter.repeat == 35 || Counter.repeat == 50 || Counter.repeat == 65 || Counter.repeat == 75)
  {
     ReedSwitch.state = ACTIVATED;
  }

  // 2. Try to start a recording if the switch is ON
  // If memory is full, this sets state to RECORDING_NOSPACE and stops trying.
  RecordingStart();

  // 3. ONLY execute memory operations if a slot was successfully opened
  if(RecordState.started == RECORDING_ONGOING)
  {
    DebugMemory4();

    // Deactivation points
    if(Counter.repeat == 29 || Counter.repeat == 42 || Counter.repeat == 58 || Counter.repeat == 70 || Counter.repeat == 79)
    {
       ReedSwitch.state = DEACTIVATED;
       DebugMemory4(); // Write final records and stop

       // Reset state so we can start a NEW file on the next ACTIVATED marker
       RecordState.started = RECORDING_NOTSTARTED;
       RecordState.sector = 0;
    }
  }

  // 4. Advance the test counter
  if(Counter.repeat < 85)
  {
    Counter.repeat++;
  }
}

// Extracts and analyzes the contents of the EEPROM
void AnalyzeEEPROM(void)
{

}

uint8_t Write_FileSettings(void)
{
  // for debug printing
  uint8_t testbyte[9] = {0};  // Need 9 bytes for 8 hex digits + null terminator
  uint32_t rec[1] = {0};

  // Find first available empty slot in the 4 file slots
  for(uint8_t i=0; i < READSPECIFICFILE_FILESLOTS; i++) // careful not to use Counter.y0 because its used below
  {


#if DEBUG_SENSOR
  	SendString((uint8_t*)"\n\nIn File Slot Finder For Loop...\r");
#endif

    uint32_t test_address = (BYTES_PER_SECTOR * READSPECIFICFILE_SECTOR) + (PAGEDATA_R_ARRAY * i); // careful not to use Counter.y0 because its used below
    // Clear the array that is to hold the pagedata values to be read first
#if DEBUG_SENSOR
  	SendString((uint8_t*)"\n\nClearing pagedata_r array to 0...\r");
#endif
    for(uint16_t j = 0; j < PAGEDATA_R_ARRAY; j++)
    {
	  PageData.pagedata_r[j] = 0;
    }

#if DEBUG_SENSOR
  	SendString((uint8_t*)"\n\nReading Sector 2, Page X...\r");
#endif
    Mem_ReadData(test_address, PageData.pagedata_r, PAGEDATA_R_ARRAY);

    // Debugging
#if DEBUG_SENSOR
    SendString((uint8_t*)"Value at [0]: ");
    ByteToHex(PageData.pagedata_r[0], testbyte);
    SendString(testbyte);
#endif

    if(PageData.pagedata_r[0] == FILE_SLOT_OCCUPIED)
    {
      // Page has a record!
#if DEBUG_SENSOR
      SendString((uint8_t*)"\nPage is Occupied DLE...\n");
#endif
    }
    else if(PageData.pagedata_r[0] == FILE_SLOT_EMPTY)
    {
      // Page is free!
      // Set where the starting sector to which data is to be written based on the free file slot we have found (in sector 2)
      // Calculate sector and page based on file index and page number
      // For file 0: sectors 3 – 258 (256 sectors total)
      // For file 1: sectors 259 – 514 (256 sectors)
      // For file 2: sectors 515 – 770 (256 sectors)
      // For file 3: sectors 771 – 1023 (256 sectors)
      RecordState.sector = 3 + ((TOTAL_SECTORS / READSPECIFICFILE_FILESLOTS) * i);  // Calculation needed to offset to correct data sector to which to write
      RecordState.page = 0;

      RecordState.fileslot = i;
      RecordState.startsector = RecordState.sector;
      RecordState.endsector = RecordState.sector + (TOTAL_SECTORS / READSPECIFICFILE_FILESLOTS);

#if DEBUG_SENSOR
      SendString((uint8_t*)"\n\nPage is Empty 0xFF...\r");
#endif
      // 1. check structure to see all settings are there (should be done before we even enter this function!)
    	  // abort if not
      // 2. write occupied flag and settings to sector 2, (page number = file number)
    	 // Clear the array that is to hold the pagedata values to be read first
      Clear_PagedataWrite_Array(); // sets pagedata_w to 0xFF (not 0x00)
      PageData.pagedata_w[0] = FILE_SLOT_OCCUPIED;  // shows its now occupied
      PageData.pagedata_w[1] = Sampling.mode;
      PageData.pagedata_w[2] = Sampling.rate;
      
      PageData.pagedata_w[3] = Activation.start_year;
      PageData.pagedata_w[4] = Activation.start_month;
      PageData.pagedata_w[5] = Activation.start_day;
      PageData.pagedata_w[6] = Activation.start_hour;
      PageData.pagedata_w[7] = Activation.start_minute;
      PageData.pagedata_w[8] = Activation.start_second;
      PageData.pagedata_w[9] = Activation.start_ampm;
      
      PageData.pagedata_w[10] = Activation.end_year;
      PageData.pagedata_w[11] = Activation.end_month;
      PageData.pagedata_w[12] = Activation.end_day;
      PageData.pagedata_w[13] = Activation.end_hour;
      PageData.pagedata_w[14] = Activation.end_minute;
      PageData.pagedata_w[15] = Activation.end_second;
      PageData.pagedata_w[16] = Activation.end_ampm;
      
      PageData.pagedata_w[17] = Activation.eventtrigger;
      PageData.pagedata_w[18] = Activation.eventthreshold;
      
      for (uint8_t x = 0; x < MAX_NOTES_NAME_ARRAY; x++) {
          PageData.pagedata_w[19 + x] = Notes.name[x];
      }
      for (uint8_t y = 0; y < MAX_NOTES_LOCATION_ARRAY; y++) {
          PageData.pagedata_w[43 + y] = Notes.location[y];
      }
      for (uint8_t z = 0; z < MAX_NOTES_NOTE_ARRAY; z++) {
          PageData.pagedata_w[67 + z] = Notes.note[z];
      }

      // Write the meta data to the sector & page specified
      test_address = (BYTES_PER_SECTOR * READSPECIFICFILE_SECTOR) + (PAGEDATA_W_ARRAY * i);
      Mem_WriteData(test_address, PageData.pagedata_w, PAGEDATA_W_ARRAY);

      // Test printout -- temporary
#if DEBUG_SENSOR
      SendString((uint8_t*)"\n\nWF_RecordState.sector: \r");
      rec[0] = READSPECIFICFILE_SECTOR; // note: its 32 bits, while rec[0] only holds uint8_t
      ByteToHex(rec[0], testbyte);
      SendString(testbyte);

      SendString((uint8_t*)"\nWF_RecordState.page: \r");
      rec[0] = i;  // Note: In this case its Counter.y1, not Counter.y0.  Watch out if you copy & paste it elsewhere!
      ByteToHex(rec[0], testbyte);
      SendString(testbyte);

      SendString((uint8_t*)"\n");
#endif

      // Print out the page
      PrintOutRecord(READSPECIFICFILE_SECTOR, i);

#if DEBUG_SENSOR
      SendString((uint8_t*)"\n\nCC_RecordState.sector: \r");
      rec[0] = RecordState.sector;
      Uint32ToHexString(rec[0], (uint8_t*)testbyte);
      SendString((uint8_t*)testbyte);
      SendString((uint8_t*)"\r\n");

      SendString((uint8_t*)"\n\nCC_RecordState.page: \r");
      rec[0] = RecordState.page; // note: its 32 bits, while rec[0] only holds uint8_t
      ByteToHex(rec[0], testbyte);
      SendString(testbyte);
#endif

      return 0;
    }
    else // some unexpected value
    {
      // Something is wrong - abort (no recording)
#if DEBUG_SENSOR
      SendString((uint8_t*)"\nUnrecognized file slot occupancy indicator detected!  Abort.\n");
#endif
      return 2;
    }
  }
  return 1;  // all file slots occupied, recording not possible
}

// This is for resetting the RecordState data type
// which is related to the file structure.
void RecordingStart(void)
{
  uint8_t recordingabort  = 0;

  //SendString((uint8_t*)"\n\nIn In RecordingStart()...\r");

  if((ReedSwitch.state == ACTIVATED) && (RecordState.started == RECORDING_NOTSTARTED))
  {
	//RecordState.started = RECORDING_ONGOING;
#if DEBUG_SENSOR
	SendString((uint8_t*)"\n\nIn Write_FileSettings, Outcome is RECORDING_ONGOING...\r");
#endif

    for(uint16_t i = 0; i < PAGEDATA_W_ARRAY; i++)
    {
      PageData.pagedata_w[i] = FILE_SLOT_EMPTY;
    }
    for(uint16_t j = 0; j < PAGEDATA_R_ARRAY; j++)
    {
	  PageData.pagedata_r[j] = 0;
    }

    // RecordState.started = RECORDING_STARTED will be allocated externally, on command from Viewer or Ring Switch
    RecordState.saverecord = RECORDSTATE_DONOTHING; // tells memory FILLPAGE routine what to do (WRITEPAGE, UPDATEPAGE) while recording.
    RecordState.pageaddress = 0;
    if (Sampling.rate <= 4)
    {
      RecordState.savetime = LESSTHAN30S;
    }
    else
    {
      RecordState.savetime = GREATERTHAN30S;
    }
    RecordState.page = 0;
    RecordState.fileslot = 0;
    RecordState.totalrecordcount = 0;
    // This allows the system to "reset" so it can try to find a slot
    // the next time the switch is flipped.
    RecordState.started = RECORDING_NOTSTARTED;
    RecordState.sector = 0; // Clear the sector so it's not pointing at Sector 3

    RecordHeader.filenumber = 0;  // 0 to 255 // must increment file number
    RecordHeader.seqnumber = 0;   // Increment with every page written
    RecordHeader.recordcount = 0; // Number of 32 byte records currently in this page
    RecordHeader.status = RECORDSTATUS_ACTIVE;
    RecordHeader.crc = 0;         // Header Checksum

    // Statics
    for(uint8_t x = 0; x < PERSSURESENSOR_ROM_ARRAY; x++)
    {
      C[x] = 0; // Calibration coefficients C0..C6 (7 total)
    }
    D1 = 0; // raw Digital Pressure Value from pressure sensor
    D2 = 0; // raw Digital Temperature Value from pressure sensor

    // Analyze EEPROM and set accordingly
    //AnalyzeEEPROM();  // currently empty - must read sector 2, page 0 to 4 to know how next file slot

    //ClearShowFilesAndMetadata(); // temporarily added - testing.

    // Looking for empty file slot and choosing it if available for recording
    recordingabort = Write_FileSettings();

    if(recordingabort == 0) // found an empty file slot to record
    {
      RecordState.started = RECORDING_ONGOING;
      ApplyRecordingPlan.started = PLAN_STARTED; // so we can indicate to Settings page that the recording session has started (needed for the button)
      DeterminePageAddress();
#if DEBUG_SENSOR
      SendString((uint8_t*)"\n\nIn Write_FileSettings, Outcome is RECORDING_ONGOING...\r");
#endif
    }
    else if(recordingabort == 1) // all file slots full, recording aborted
    {
      RecordState.started = RECORDING_NOSPACE;
#if DEBUG_SENSOR
      SendString((uint8_t*)"\n\nIn Write_FileSettings, Outcome is RECORDING_NOSPACE...\r");
#endif
    }
    else // unexpected file slot occupancy indicator encountered, recording aborted
    {
      RecordState.started = RECORDING_UNEXPECTED;  // perhaps we should use a separate define for that, but how do we clear it?
#if DEBUG_SENSOR
   	  SendString((uint8_t*)"\n\nIn Write_FileSettings, Outcome is RECORDING_UNEXPECTED...\r");
#endif
    }
  }
}

void RecordingStop(void)
{
  // if(RecordState.started == RECORDING_ONGOING)
  if(ReedSwitch.state == DEACTIVATED)
  {
    // Save RecordHeader current state.
    // Note: Reset of ReacordHeader is done in RecordingStart
    RecordHeaderPrev.filenumber = RecordHeader.filenumber;  // 0 to 255
    RecordHeaderPrev.seqnumber = RecordHeader.seqnumber;   // Increment with every page written
    RecordHeaderPrev.recordcount = RecordHeader.recordcount; // Number of 32 byte records currently in this page
    RecordHeaderPrev.status = RecordHeader.status;      // 0x01 (Active), 0x00 (Deleted)
    RecordHeaderPrev.crc = RecordHeader.crc;         // Header Checksum

    // Save RecordState current state.
    // Note: Reset of ReacordState is done in RecordingStart
    RecordStatePrev.started = RecordState.started;
    RecordStatePrev.saverecord = RecordState.saverecord;
    RecordStatePrev.pageaddress = RecordState.pageaddress;
    RecordStatePrev.savetime = RecordState.savetime;
    RecordStatePrev.sector = RecordState.sector;
    RecordStatePrev.page = RecordState.page;

    // Return to inavtive state
    RecordHeader.status = RECORDSTATUS_UNKNOWN;
  }
}

uint8_t Get24Hour(uint8_t hour12, uint8_t ampm)
{
  uint8_t hour24 = hour12;
  if (ampm == 1) { // PM
      if (hour24 != 12) {
          hour24 += 12;
      }
  } else { // AM
      if (hour24 == 12) {
          hour24 = 0;
      }
  }
  return hour24;
}

uint8_t IsTargetTimeReached(void)
{
  RTC_ReadTime();
  uint8_t current_hour24 = Get24Hour(Time.read_hour, Time.read_ampm);
  uint8_t target_hour24 = Get24Hour(Activation.start_hour, Activation.start_ampm);

  if (Time.read_year > Activation.start_year) return 1;
  if (Time.read_year < Activation.start_year) return 0;

  if (Time.read_month > Activation.start_month) return 1;
  if (Time.read_month < Activation.start_month) return 0;

  if (Time.read_day > Activation.start_day) return 1;
  if (Time.read_day < Activation.start_day) return 0;

  if (current_hour24 > target_hour24) return 1;
  if (current_hour24 < target_hour24) return 0;

  if (Time.read_minute > Activation.start_minute) return 1;
  if (Time.read_minute < Activation.start_minute) return 0;

  if (Time.read_second >= Activation.start_second) return 1;

  return 0;
}

uint8_t IsEndTimeReached(void)
{
  RTC_ReadTime();
  uint8_t current_hour24 = Get24Hour(Time.read_hour, Time.read_ampm);
  uint8_t end_hour24 = Get24Hour(Activation.end_hour, Activation.end_ampm);

  if (Time.read_year > Activation.end_year) return 1;
  if (Time.read_year < Activation.end_year) return 0;

  if (Time.read_month > Activation.end_month) return 1;
  if (Time.read_month < Activation.end_month) return 0;

  if (Time.read_day > Activation.end_day) return 1;
  if (Time.read_day < Activation.end_day) return 0;

  if (current_hour24 > end_hour24) return 1;
  if (current_hour24 < end_hour24) return 0;

  if (Time.read_minute > Activation.end_minute) return 1;
  if (Time.read_minute < Activation.end_minute) return 0;

  if (Time.read_second >= Activation.end_second) return 1;

  return 0;
}

volatile uint32_t last_power_on_time = 0;
extern volatile uint32_t last_activity_time;
extern volatile bool is_sleeping;
#ifdef IR_USART2_SEL
extern IRDA_HandleTypeDef hirda2;
#else
extern UART_HandleTypeDef hlpuart1;
#endif
extern uint8_t rx_buffer[];
extern void SystemClock_Config(void);
extern RTC_HandleTypeDef hrtc;
extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim2;

void Set_REC_START_Pin_As_Input(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = REC_START_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(REC_START_GPIO_Port, &GPIO_InitStruct);
}

void Set_REC_START_Pin_As_Interrupt(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = REC_START_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(REC_START_GPIO_Port, &GPIO_InitStruct);
}

void Enter_Deep_Sleep(void)
{
  is_sleeping = true;

  // 1. Configure REC_START pin as EXTI interrupt to wake the CPU
  Set_REC_START_Pin_As_Interrupt();
  __HAL_GPIO_EXTI_CLEAR_IT(REC_START_Pin);
  HAL_NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  // 2. Shut off the load switch to power down sensors and EEPROM
  HAL_GPIO_WritePin(PWRDIST_GEN_PWR_EN_GPIO_Port, PWRDIST_GEN_PWR_EN_Pin, GPIO_PIN_RESET);

  // 3. Put SPI and I2C pins into Analog No-Pull mode to prevent back-feeding current
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Clear pending EXTI line flags for Reed Switch / Photodiode wakeup lines
  __HAL_GPIO_EXTI_CLEAR_IT(REC_START_Pin);
  HAL_NVIC_ClearPendingIRQ(EXTI4_15_IRQn);

  // Clear SysTick pending interrupt flag
  SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;

  // 4. Enter Stop Mode
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  // 5. On Wakeup: immediately restore clocks and resume
  Exit_Deep_Sleep();
}

void Exit_Deep_Sleep(void)
{
  // 1. Restore System Clock (HSE/PLL)
  SystemClock_Config();

  // 2. Disable EXTI vector and restore pin to standard digital input
  HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
  Set_REC_START_Pin_As_Input();

  // 3. Power up the general load switch immediately on wakeup (powers IrDA transceiver and sensor EEPROM)
  HAL_GPIO_WritePin(PWRDIST_GEN_PWR_EN_GPIO_Port, PWRDIST_GEN_PWR_EN_Pin, GPIO_PIN_SET);
  last_power_on_time = HAL_GetTick();

  // 4. Re-initialize SPI1 and I2C1 pin mappings
  HAL_SPI_MspInit(&hspi1);
  HAL_I2C_MspInit(&hi2c1);

  // 5. Clear pending UART errors & flush RX data register, then restart RX
#ifdef IR_USART2_SEL
  __HAL_IRDA_CLEAR_FLAG(&hirda2, IRDA_CLEAR_OREF | IRDA_CLEAR_NEF | IRDA_CLEAR_PEF | IRDA_CLEAR_FEF);
  volatile uint32_t temp = USART2->RDR;
  (void)temp;
  HAL_IRDA_Receive_IT(&hirda2, &rx_buffer[0], 1);
#else
  __HAL_UART_CLEAR_FLAG(&hlpuart1, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
  volatile uint32_t temp = LPUART1->RDR;
  (void)temp;
  HAL_UART_Receive_IT(&hlpuart1, &rx_buffer[0], 1);
#endif

  is_sleeping = false;
}

uint32_t Get_Sampling_Interval_Seconds(void)
{
    switch (Sampling.rate) {
        case 4:  return 10;
        case 5:  return 30;
        case 6:  return 60;
        case 7:  return 300;
        case 8:  return 600;
        case 9:  return 1800;
        case 10: return 3600;
        default: return 0; // rates < 10 sec are not eligible for low-power sleep
    }
}

extern volatile uint8_t recording_timer_expired;

void Enter_Recording_Sleep(uint32_t interval_seconds)
{
  is_sleeping = true;

#if DEBUG_SENSOR
  // Verify if the RTC clock is actually ticking
  RTC_ReadTime();
  char t1[64];
  sprintf(t1, "[DEBUG] Clock before delay: %02d:%02d:%02d\r\n", Time.read_hour, Time.read_minute, Time.read_second);
  SendString((uint8_t*)t1);

  HAL_Delay(1000);

  RTC_ReadTime();
  char t2[64];
  sprintf(t2, "[DEBUG] Clock after delay:  %02d:%02d:%02d\r\n", Time.read_hour, Time.read_minute, Time.read_second);
  SendString((uint8_t*)t2);
#endif

  // 1. Configure RTC Wakeup Timer to trigger (interval_seconds - 1) seconds from now
  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

  // Clear Wakeup Flag
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

  // The timer count is 0-indexed, so we subtract 1.
  // We wake up 1 second early, so we configure it for (interval_seconds - 1) - 1.
  HAL_StatusTypeDef status = HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, interval_seconds - 2, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
#if DEBUG_SENSOR
  if (status != HAL_OK)
  {
      char err_msg[64];
      sprintf(err_msg, "[DEBUG] SetWakeUpTimer failed: %d\r\n", status);
      SendString((uint8_t*)err_msg);
  }
  else
  {
      SendString((uint8_t*)"[DEBUG] SetWakeUpTimer succeeded (HAL_OK)\r\n");
  }

  // Allow the debug messages to finish transmitting
  HAL_Delay(50);
#endif

  // Configure REC_START (phototransceiver / photodiode) pin as EXTI interrupt to wake CPU
  Set_REC_START_Pin_As_Interrupt();
  __HAL_GPIO_EXTI_CLEAR_IT(REC_START_Pin);
  HAL_NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  // Turn off LED_A during sleep to save power and provide status indication
  HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_RESET);

  // Suspend the 1 ms SysTick timer to prevent pending tick interrupts from waking CPU
  HAL_SuspendTick();

  // Disable IrDA, LPUART, and TIM2 interrupts in the NVIC to prevent premature wakeups
  HAL_NVIC_DisableIRQ(USART2_IRQn);
  HAL_NVIC_DisableIRQ(LPUART1_IRQn);
  HAL_NVIC_DisableIRQ(TIM2_IRQn);

  // 2. Shut off the load switch to power down sensors and EEPROM
  HAL_GPIO_WritePin(PWRDIST_GEN_PWR_EN_GPIO_Port, PWRDIST_GEN_PWR_EN_Pin, GPIO_PIN_RESET);

  // 3. Put SPI and I2C pins into Analog No-Pull mode to prevent back-feeding current
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Clear any pending TIM2 interrupts at both peripheral and NVIC levels
  __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
  HAL_NVIC_ClearPendingIRQ(TIM2_IRQn);

  // Clear pending EXTI line flags for Reed Switch / Photodiode wakeup lines
  __HAL_GPIO_EXTI_CLEAR_IT(REC_START_Pin);
  HAL_NVIC_ClearPendingIRQ(EXTI4_15_IRQn);

  // Clear any pending UART / LPUART interrupts at the NVIC level
  HAL_NVIC_ClearPendingIRQ(USART2_IRQn);
  HAL_NVIC_ClearPendingIRQ(LPUART1_IRQn);

  // Clear any pending RTC/EXTI interrupts at both EXTI and NVIC levels
  __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
  HAL_NVIC_ClearPendingIRQ(RTC_IRQn);

  // Clear SysTick pending interrupt flag
  SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;

  // 4. Enter Stop Mode
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  // 5. On Wakeup: immediately restore clocks and resume
  Exit_Recording_Sleep();
}

void Exit_Recording_Sleep(void)
{
  // 1. Restore System Clock (HSE/PLL)
  SystemClock_Config();

  // Clear pending UART flags and restart receiver interrupts
#ifdef IR_USART2_SEL
  __HAL_IRDA_CLEAR_FLAG(&hirda2, IRDA_CLEAR_OREF | IRDA_CLEAR_NEF | IRDA_CLEAR_PEF | IRDA_CLEAR_FEF);
  volatile uint32_t temp = USART2->RDR;
  (void)temp;
  HAL_IRDA_Receive_IT(&hirda2, &rx_buffer[0], 1);
#else
  __HAL_UART_CLEAR_FLAG(&hlpuart1, UART_FLAG_ORE | UART_FLAG_NE | UART_FLAG_FE | UART_FLAG_PE);
  volatile uint32_t temp = LPUART1->RDR;
  (void)temp;
  HAL_UART_Receive_IT(&hlpuart1, &rx_buffer[0], 1);
#endif

  // Disable EXTI interrupt for reed switch/photodiode and restore pin to normal input
  HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
  Set_REC_START_Pin_As_Input();

  // Re-enable IrDA and LPUART interrupts in the NVIC
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  HAL_NVIC_EnableIRQ(LPUART1_IRQn);

  // Resume SysTick timer
  HAL_ResumeTick();

  // Power up the general load switch immediately on wakeup (powers IrDA transceiver and sensor EEPROM)
  HAL_GPIO_WritePin(PWRDIST_GEN_PWR_EN_GPIO_Port, PWRDIST_GEN_PWR_EN_Pin, GPIO_PIN_SET);
  last_power_on_time = HAL_GetTick();

  // Re-initialize SPI1 and I2C1 pin mappings
  HAL_SPI_MspInit(&hspi1);
  HAL_I2C_MspInit(&hi2c1);

  // If we woke up due to the RTC timer expiring (the wakeup callback ran and set recording_timer_expired to 1)
  if (recording_timer_expired)
  {
      // 2. Disable RTC Wakeup Timer to prevent spurious wakeups
      HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

      // Wait 25 ms for the sensor voltages to stabilize before we perform reading/sampling
      HAL_Delay(25);
  }

  is_sleeping = false;
}
