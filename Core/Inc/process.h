/*
 * process.h
 *
 *  Created on: Sep 4, 2025
 *      Author: MT
 */

#ifndef INC_PROCESS_H_
#define INC_PROCESS_H_

void ProcessMsg(void);

void Instrument_Resp(void);
void CTD_Reading_Processed_Resp(void);
void Submersible_Info_Resp(void);
void Instrument_StoreData(void);

void Time_Resp(void);
void Time_StoreData(void);

void Sampling_Resp(void);
void Sampling_Set(void);

void Activation_Resp(void);
void Activation_Set(void);

void Notes_Resp(void);
void Notes_Set(void);

void ApplyRecordingPlan_Resp(void);
void ApplyRecordingPlan_Set(void);

void Log_ShowFiles_Resp(void);

void Log_ReadSpecificFile_Set(void);
void Log_ReadSpecificFile_Resp(void);

void Log_TransmitData_Set(void);
void Log_TransmitData_Resp(void);

// Bootloader
void Bootloader_FWUpdate_Resp(void);
void Bootloader_JumpToBootloader(void);

void SendByte(uint8_t data);
void DebugSensor(void);

void SendHeader(uint8_t msg_length, uint8_t msg_id);

#endif /* INC_PROCESS_H_ */
