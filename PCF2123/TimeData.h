/* *****************************************************************************
*  Real time record. Structure layout is chosen for compatibility with the 
*  real time clock hardware. 
*  ***************************************************************************** */
#pragma once
#include "Arduino.h"

typedef uint32_t RTCTimeStamp;

struct TimeData
{
  // Time is stored in BCD format. 
  uint8_t seconds; // [0,59]
  uint8_t minutes; // [0,59]
  uint8_t hours;   // [0,23]
  uint8_t days;    // [1,31]
  uint8_t weekdays;// [0,6]
  uint8_t months;  // [1,12]
  uint8_t years;   // [0,99]

  void InitalizeFrom(RTCTimeStamp uTimestamp);
  RTCTimeStamp ToTimeStamp() const;

} __attribute__((__packed__));


uint8_t Decimal2BCD(uint8_t uDecimalValue);
uint8_t BCD2Decimal(uint8_t uBDCValue);

void PrintDateTime(const TimeData &Time, Print& rDestination = Serial);
void PrintDate(const TimeData &Time, Print& rDestination = Serial);
void PrintTime(const TimeData &Time, Print& rDestination = Serial);
