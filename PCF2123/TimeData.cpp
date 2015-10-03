#include "TimeData.h"


void TimeData::InitalizeFrom( RTCTimeStamp uTimestamp )
{
  // Probably smaller. 
  years = uTimestamp / (13UL * 32UL * 24UL * 60UL * 60UL);
  uTimestamp -= years * (13UL * 32UL * 24UL * 60UL * 60UL);
  months = uTimestamp / (32UL * 24UL * 60UL * 60UL);
  uTimestamp -= months * (32UL * 24UL * 60UL * 60UL);
  days = uTimestamp / (24UL * 60UL * 60UL);
  uTimestamp -= days * (24UL * 60UL * 60UL);
  hours = uTimestamp / (60UL * 60UL);
  uTimestamp -= hours * (60UL * 60UL);
  minutes = uTimestamp / (60UL);
  uTimestamp -= minutes * (60UL);
  seconds = uTimestamp;

  // Everything needs to be converted to BCD. 
  // A hack to save save program memory. 
  uint8_t *pConvert;
  uint8_t iConvert;

  pConvert = (uint8_t*)this;
  for (iConvert = 0; iConvert < 7; ++iConvert)
  {
    *pConvert = Decimal2BCD(*pConvert);
    ++pConvert;
  }
  weekdays = 0; // assumed. the RTC data sheet says the meaning can be assigned by user.
}

RTCTimeStamp TimeData::ToTimeStamp() const
{
  uint8_t *pData;
  uint8_t uElement;
  uint32_t uDecimalValue;
  uint32_t aScale[] = {1, 60, 60*60, 24UL*60UL*60UL, 0, 32UL*24UL*60UL*60UL, 13UL*32UL*24UL*60UL*60UL};
  uint32_t uResult;
  uint32_t *pScale;

  pData = (uint8_t*)this;
  pScale = aScale;
  uElement = 7;
  uResult = 0;
  while(uElement--)
  {
    uDecimalValue = BCD2Decimal(*pData++);
    uDecimalValue *= *pScale++;
    uResult += uDecimalValue;
  }

  return uResult;
}

uint8_t Decimal2BCD( uint8_t uDecimalValue )
{
  return ((uDecimalValue / 10) << 4) | (uDecimalValue % 10);
}

uint8_t BCD2Decimal( uint8_t uBDCValue )
{
  return (uBDCValue >> 4) * 10 + (uBDCValue & 0xF);
}

void PrintBCD(Print &rOutput, uint8_t uValue)
{
  unsigned uNibble;
  char chOut;

  // High nibble. 
  uNibble = (uValue>>4)&0x0f;
  chOut = uNibble + ((uNibble < 10) ? '0' : ('A' - 10));
  rOutput.write(chOut);

  // Low nibble. 
  uNibble = uValue&0x0f;
  chOut = uNibble + ((uNibble < 10) ? '0' : ('A' - 10));
  rOutput.write(chOut);
}

void PrintTime( const TimeData &rTime, Print &rDestination /*= Serial*/ )
{

  rDestination.print("20");
  PrintBCD(rDestination, rTime.years);
  rDestination.print('-');
  PrintBCD(rDestination, rTime.months);
  rDestination.print('-');
  PrintBCD(rDestination, rTime.days);      
  rDestination.print(' ');
  PrintBCD(rDestination, rTime.hours);
  rDestination.print(':');
  PrintBCD(rDestination, rTime.minutes);
  rDestination.print(':');
  PrintBCD(rDestination, rTime.seconds);
}

