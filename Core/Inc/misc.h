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

#endif /* INC_MISC_H_ */

