#include "globals.h"

struct Lpuartl1 Lpuart1;
struct Lpuartshadow11 Lpuart1shadow;
struct Counter1 Counter;
struct Version1 Version;
struct Status1 Status;
struct Instrument1 Instrument;
struct Communication1 Communication;
struct Power1 Power;
struct Time1 Time;
struct Sampling1 Sampling;
struct Activation1 Activation;
struct Notes1 Notes;
struct Cloud1 Cloud;
struct Misc1 Misc;
struct ApplyRecordingPlan1 ApplyRecordingPlan;
volatile uint8_t is_timer_triggered = 0;
volatile uint8_t recording_timer_expired = 0;
uint8_t time_is_set = 0;
uint8_t sampling_is_set = 0;
struct Memory1 Memory;

struct Surface1 Surface;
struct Battery1 Battery;
struct Usage1 Usage;
struct Messages1 Messages;
struct TabletTime1 TabletTime;
struct DeviceTime1 DeviceTime;
struct UpcomingRecordingTime1 UpcomingRecordingTime;
struct CTD1 CTD;
struct ReedSwitch1 ReedSwitch;
struct Temporary1 Temporary;
struct EEPROMRecord1 EEPROMRecord;
struct ShowFiles1 ShowFiles;
struct RecordState1 RecordState;
struct RecordStatePrev1 RecordStatePrev;
struct RecordHeader1 RecordHeader;
struct RecordHeaderPrev1 RecordHeaderPrev;
struct PageData1 PageData;
struct Sensor1 Sensor;
struct LogReadSpecificFile1 LogReadSpecificFile;
struct LogTransmitData1 LogTransmitData;
struct CTDReadingsTareSensor1 CTDReadingsTareSensor;

// MS5837 Variables
uint16_t C[PERSSURESENSOR_ROM_ARRAY];
uint32_t D1;
uint32_t D2;
