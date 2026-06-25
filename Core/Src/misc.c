#include "main.h"
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


  	SendString((uint8_t*)"\n\nIn File Slot Finder For Loop...\r");

    uint32_t test_address = (BYTES_PER_SECTOR * READSPECIFICFILE_SECTOR) + (PAGEDATA_R_ARRAY * i); // careful not to use Counter.y0 because its used below
    // Clear the array that is to hold the pagedata values to be read first
  	SendString((uint8_t*)"\n\nClearing pagedata_r array to 0...\r");
    for(uint16_t j = 0; j < PAGEDATA_R_ARRAY; j++)
    {
	  PageData.pagedata_r[j] = 0;
    }

  	SendString((uint8_t*)"\n\nReading Sector 2, Page X...\r");
    Mem_ReadData(test_address, PageData.pagedata_r, PAGEDATA_R_ARRAY);

    // Debugging
    SendString((uint8_t*)"Value at [0]: ");
    ByteToHex(PageData.pagedata_r[0], testbyte);
    SendString(testbyte);

    if(PageData.pagedata_r[0] == FILE_SLOT_OCCUPIED)
    {
      // Page has a record!
      SendString((uint8_t*)"\nPage is Occupied DLE...\n");
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

      SendString((uint8_t*)"\n\nPage is Empty 0xFF...\r");
      // 1. check structure to see all settings are there (should be done before we even enter this function!)
    	  // abort if not
      // 2. write occupied flag and settings to sector 2, (page number = file number)
    	 // Clear the array that is to hold the pagedata values to be read first
      Clear_PagedataWrite_Array(); // sets pagedata_w to 0xFF (not 0x00)
      PageData.pagedata_w[0] = FILE_SLOT_OCCUPIED;  // shows its now occupied
      PageData.pagedata_w[1] = 0x01; // the bytes in this page are settings which are as yet undecided.  we put dummy bytes here for now.
      PageData.pagedata_w[2] = 0x02;
      PageData.pagedata_w[3] = 0x03;
      PageData.pagedata_w[4] = 0x04;
      PageData.pagedata_w[5] = 0x05;

      // Write the meta data to the sector & page specified
      test_address = (BYTES_PER_SECTOR * READSPECIFICFILE_SECTOR) + (PAGEDATA_W_ARRAY * i);
      Mem_WriteData(test_address, PageData.pagedata_w, PAGEDATA_W_ARRAY);

      // Test printout -- temporary
      SendString((uint8_t*)"\n\nWF_RecordState.sector: \r");
      rec[0] = READSPECIFICFILE_SECTOR; // note: its 32 bits, while rec[0] only holds uint8_t
      ByteToHex(rec[0], testbyte);
      SendString(testbyte);

      SendString((uint8_t*)"\nWF_RecordState.page: \r");
      rec[0] = i;  // Note: In this case its Counter.y1, not Counter.y0.  Watch out if you copy & paste it elsewhere!
      ByteToHex(rec[0], testbyte);
      SendString(testbyte);

      SendString((uint8_t*)"\n");

      // Print out the page
      PrintOutRecord(READSPECIFICFILE_SECTOR, i);

      SendString((uint8_t*)"\n\nCC_RecordState.sector: \r");
      rec[0] = RecordState.sector;
      Uint32ToHexString(rec[0], (uint8_t*)testbyte);
      SendString((uint8_t*)testbyte);
      SendString((uint8_t*)"\r\n");

      SendString((uint8_t*)"\n\nCC_RecordState.page: \r");
      rec[0] = RecordState.page; // note: its 32 bits, while rec[0] only holds uint8_t
      ByteToHex(rec[0], testbyte);
      SendString(testbyte);

      return 0;
    }
    else // some unexpected value
    {
      // Something is wrong - abort (no recording)
      SendString((uint8_t*)"\nUnrecognized file slot occupancy indicator detected!  Abort.\n");
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
	SendString((uint8_t*)"\n\nIn Write_FileSettings, Outcome is RECORDING_ONGOING...\r");

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
    RecordState.savetime = LESSTHAN30S; // CHANGE TO MATCH SETTINGS FROM VIEWER LATER   LESSTHAN30S; // GREATERTHAN30S;
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
      SendString((uint8_t*)"\n\nIn Write_FileSettings, Outcome is RECORDING_ONGOING...\r");
    }
    else if(recordingabort == 1) // all file slots full, recording aborted
    {
      RecordState.started = RECORDING_NOSPACE;
      SendString((uint8_t*)"\n\nIn Write_FileSettings, Outcome is RECORDING_NOSPACE...\r");
    }
    else // unexpected file slot occupancy indicator encountered, recording aborted
    {
      RecordState.started = RECORDING_UNEXPECTED;  // perhaps we should use a separate define for that, but how do we clear it?
  	  SendString((uint8_t*)"\n\nIn Write_FileSettings, Outcome is RECORDING_UNEXPECTED...\r");
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

