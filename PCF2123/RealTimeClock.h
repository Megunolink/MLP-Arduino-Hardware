/* *****************************************************************************
*  Class to interface with PCF2123 Real Time Clock. 
*  ***************************************************************************** */

#pragma once
#include "Arduino.h"
#include "TimeData.h"
#include "../SharedSPI/SharedSPI.h"

class RealTimeClock
{
private:
  SharedSPI::CSharedSPI m_SPI;

  const uint8_t c_uInterruptPin;

  // A millis count based off the real time clock interrupts. The normal Arduino
  // RTC loses time when the arduino sleeps.
  uint32_t volatile m_uMillisCounter; // Current millis count. Based off RTC timer. Granularity is m_uInterruptPeriod. 

  uint32_t m_uInterruptPeriod; // Number of milliseconds between each tick of the RTC timer.

  bool m_bFault; // True if a fault is detected. 
public:
  enum EConstants { NO_INTERRUPT = 0xff };

  enum EWakePeriod64Hz
  {
    WakeOff = 0,
    WakeEvery62500us = 4,  // Exact
    WakeEvery78125us = 5,  // Exact
    WakeEvery93750us = 6,  // Exact
    WakeEvery125ms = 8,    // Exact
    WakeEvery250ms = 16,   // Exact
    WakeEvery375ms = 24,   // Exact
    WakeEvery500ms = 32,   // Exact
    WakeEvery625ms = 40,   // Exact
    WakeEvery875ms = 56,   // Exact
    WakeEvery1000ms = 64,  // Exact
    WakeEvery1500ms = 96,  // Exact
    WakeEvery2000ms = 128, // Exact
    WakeEvery2500ms = 160, // Exact
    WakeEvery3000ms = 192, // Exact
    WakeEvery3875ms = 248, // Exact
  } PACKED;

  RealTimeClock(uint8_t uChipSelectPin, uint8_t uInterruptPin = NO_INTERRUPT);
  bool begin();

  void SetTime(const TimeData& Time);
  void GetTime(TimeData& Time) const;
  RTCTimeStamp GetTimeStamp() const;

  bool SetTimerPeriod(EWakePeriod64Hz Period);
  void StopTimer();
  uint32_t GetMilliSeconds() volatile const;
  uint32_t GetTimerPeriodMilliseconds() const { return m_uInterruptPeriod; }

private:
  static void InterruptHandler();


};