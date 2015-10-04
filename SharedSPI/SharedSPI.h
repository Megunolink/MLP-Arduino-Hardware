/* *****************************************************************************
*  Implements sharing for the SPI bus. 
*  ***************************************************************************** */
#pragma once
#include <Arduino.h>

namespace SharedSPI
{
  class CSharedSPI
  {
  public:
    const uint8_t m_uChipSelectPin;
    const uint8_t m_uClockDivider;
    const bool m_bActiveLow;

    CSharedSPI(uint8_t uChipSelectPin, uint8_t uClockDivider, bool bActiveLow = true);
    void Initialize() const;

    void Write(uint8_t uRegister, uint8_t uValue) const;
    uint8_t Read(uint8_t uRegister) const;
    void BurstWrite(uint8_t uFirstRegister, const void *pData, uint8_t uLength);
    void BurstRead(uint8_t uFirstRegister, void *pData, uint8_t uLength) const;

  private:
    void SelectDevice() const;
    void DeselectDevice() const;
  };
}