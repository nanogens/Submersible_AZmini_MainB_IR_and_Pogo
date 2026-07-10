#ifndef INC_MISC_H_
#define INC_MISC_H_

#include <stdint.h>

// Functions defined in misc.c
void ReedSwitch_Detect(void);
void FileHeader_TestFill(void);
void SpecificFileHeader_TestFill(void);
void Clear_PagedataWrite_Array(void);
void Clear_PagedataRead_Array(void);

void TestingRuns(void);
void FileMeta_ClearSector(void);

void AnalyzeEEPROM(void);
void RecordingStart(void);
void RecordingStop(void);
uint8_t Write_FileSettings(void);

void DebugMemory4(void);

uint8_t Get24Hour(uint8_t hour12, uint8_t ampm);
uint8_t IsTargetTimeReached(void);
uint8_t IsEndTimeReached(void);
uint32_t GetSecondsUntilStart(void);

void Enter_Deep_Sleep(void);
void Exit_Deep_Sleep(void);
void Set_REC_START_Pin_As_Input(void);
void Set_REC_START_Pin_As_Interrupt(void);

void Enter_Recording_Sleep(uint32_t interval_seconds);
void Exit_Recording_Sleep(void);
uint32_t Get_Sampling_Interval_Seconds(void);

#endif /* INC_MISC_H_ */
