#include "RealTimeClock.h"
#include "SPI/SPI.h"

#define RTC_00_Control1 0x00
#define RTC_01_Control2 0x01
#define RTC_02_Sec 0x02
#define RTC_03_Min 0x03
#define RTC_04_Hour 0x04
#define RTC_05_Day 0x05
#define RTC_06_Weekday 0x06
#define RTC_07_Month 0x07
#define RTC_08_Year 0x08
#define RTC_09_AlarmMin 0x09
#define RTC_0A_AlarmHour 0x0A
#define RTC_0B_AlarmDay 0x0B
#define RTC_0C_AlarmWeekday 0x0C
#define RTC_0D_Offset 0x0D
#define RTC_0E_TimerClkOut 0x0E
#define RTC_0F_TimerCountDown 0x0F

#define RTC_TIMER_CLKOUT_NONE 0x70
#define RTC_TIMER_TE_ENABLE 0x08
#define RTC_TIMER_TE_DISABLE 0x00
#define RTC_TIMER_SOURCE_4096Hz 0x00
#define RTC_TIMER_SOURCE_64Hz 0x01
#define RTC_TIMER_SOURCE_1Hz 0x10
#define RTC_TIMER_SOURCE_60thHz 0x11


static RealTimeClock *s_pInterruptHandler;

RealTimeClock::RealTimeClock( uint8_t uChipSelectPin, uint8_t uInterruptPin /*= NO_INTERRUPT*/ )
#if F_CPU < 16000000L 
  : m_SPI(uChipSelectPin, SPI_CLOCK_DIV8)
#else
  : m_SPI(uChipSelectPin, SPI_CLOCK_DIV16)
#endif 
  , c_uInterruptPin(uInterruptPin)
{

}

bool RealTimeClock::begin()
{
  m_SPI.Initialize();

  // The seconds register contains a flag which indicates whether the 
  // clock is valid. If it is not valid, reset the device & restore time to zero. 
  uint8_t uSeconds;

  uSeconds = m_SPI.Read(RTC_02_Sec);
  if (uSeconds & 0x80)
  {
    // Clock integrity is not guaranteed. Oscillator has stopped or has been 
    // interrupted. Start with a software reset to restore to a known state. 
    m_SPI.Write(RTC_00_Control1, 0x58); // 0x58=>software reset.
    delay(40); // Give time for reset. 
  }

  // Hook up interrupts. 
  if (c_uInterruptPin != NO_INTERRUPT)
  {
    pinMode(c_uInterruptPin, INPUT_PULLUP); // RTC interrupt is open drain, active low. 
    uint8_t CurrentInterruptState = SREG; // Save interrupt state. 
    cli();  // disable interrupts. 
    s_pInterruptHandler = this;
#ifdef digitalPinToInterrupt
    attachInterrupt(digitalPinToInterrupt(m_SPI.m_uChipSelectPin), InterruptHandler, LOW);
#else
    attachInterrupt(2, InterruptHandler, LOW);
#endif
    m_SPI.Write(RTC_01_Control2, 0x01); // Interrupt from countdown timer. 
    SREG = CurrentInterruptState;
  }

  return true;
}

void RealTimeClock::SetTime( const TimeData& Time )
{
  m_SPI.BurstWrite(RTC_02_Sec,(const uint8_t*)&Time,sizeof(Time));
}

void RealTimeClock::GetTime( TimeData& Time ) const
{
  m_SPI.BurstRead(RTC_02_Sec,(uint8_t*)&Time,sizeof(Time));
}

RTCTimeStamp RealTimeClock::GetTimeStamp() const
{
  TimeData currentTime;
  memset(&currentTime,0,sizeof(currentTime));
  GetTime(currentTime);

  return currentTime.ToTimeStamp();
}

bool RealTimeClock::SetTimerPeriod( EWakePeriod64Hz Period )
{
  uint8_t uValue;
  uint8_t CurIntReg = SREG;

  if (m_bFault)
    return false;

  if (Period == WakeOff)
  {
    StopTimer();
    return true; 
  }
  else
  {
    cli();

    // First disable the timer so we can update without the risk of it generating an interrupt. 
    uValue = RTC_TIMER_CLKOUT_NONE | RTC_TIMER_SOURCE_64Hz | RTC_TIMER_TE_DISABLE;
    m_SPI.Write(RTC_0E_TimerClkOut, uValue);

    // Update the timer tick counter
    m_SPI.Write(RTC_0F_TimerCountDown, Period);
    m_uInterruptPeriod = ((uint32_t)Period * 1000) / 64;

    // Enable the timer again. 
    uValue = RTC_TIMER_CLKOUT_NONE | RTC_TIMER_SOURCE_64Hz | RTC_TIMER_TE_ENABLE;
    m_SPI.Write(RTC_0E_TimerClkOut, uValue);

    SREG = CurIntReg;
  }

  return true;
}

void RealTimeClock::StopTimer()
{
  uint8_t CurIntReg = SREG;
  cli();
  m_SPI.Write(RTC_0E_TimerClkOut, RTC_TIMER_CLKOUT_NONE);
  m_SPI.Write(RTC_01_Control2, 0x01); // Clear timer signal flag. 
  m_uInterruptPeriod = 0;
  SREG = CurIntReg;
}

uint32_t RealTimeClock::GetMilliSeconds() volatile const
{
  uint8_t CurrentInterruptState = SREG;  // Save interrupts. 

  cli(); // Disable interrupts

  uint32_t uResult = m_uMillisCounter;

  SREG = CurrentInterruptState; // Restore interrupts. 

  return uResult;
}

void RealTimeClock::InterruptHandler()
{
  // Update current time. 
  s_pInterruptHandler->m_uMillisCounter += s_pInterruptHandler->m_uInterruptPeriod;

  // Clear timer interrupt flag. 
  s_pInterruptHandler->m_SPI.Write(RTC_01_Control2, 0x01);
}
