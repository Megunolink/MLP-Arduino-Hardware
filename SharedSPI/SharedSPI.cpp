#include "SharedSPI.h"
#include "SPI/SPI.h"

// This is the bit in the SPI address that marks it as a write
#define SPI_WRITE_MASK 0x80

namespace SharedSPI
{
  // Last user of spi port. 
  CSharedSPI const * g_pLast = NULL;

  CSharedSPI::CSharedSPI(uint8_t uChipSelectPin, uint8_t uClockDivider)
    : m_uChipSelectPin(uChipSelectPin)
    , m_uClockDivider(uClockDivider)
  {
    pinMode(uChipSelectPin, OUTPUT);
    digitalWrite(uChipSelectPin, HIGH);
  }

  void CSharedSPI::Initialize() const
  {
    uint8_t uSREG = SREG;
    cli();

    pinMode(m_uChipSelectPin, OUTPUT);
    digitalWrite(m_uChipSelectPin, HIGH);
    
    SPI.begin();
    SPI.setDataMode(SPI_MODE0); 
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(m_uClockDivider);
    
    g_pLast = this; 

    SREG = uSREG;

  }

  void CSharedSPI::Write( uint8_t uRegister, uint8_t uValue ) const
  {
    uint8_t uSREG = SREG;
    cli();
    if (g_pLast != this)
      Initialize();

    digitalWrite(m_uChipSelectPin, LOW);
    SPI.transfer(uRegister | SPI_WRITE_MASK); // Send the address with the write mask on
    SPI.transfer(uValue); // New value follows
    digitalWrite(m_uChipSelectPin, HIGH);


    SREG = uSREG;
  }

  uint8_t CSharedSPI::Read( uint8_t uRegister ) const
  {
    uint8_t uSREG = SREG;
    cli();
    if (g_pLast != this)
      Initialize();

    uint8_t uValue;

    digitalWrite(m_uChipSelectPin, LOW);
    SPI.transfer(uRegister & ~SPI_WRITE_MASK); // Send the address with the write mask off
    uValue = SPI.transfer(0); // The written value is ignored, reg value is read
    digitalWrite(m_uChipSelectPin, HIGH);


    SREG = uSREG;
    return uValue;
  }

  void CSharedSPI::BurstWrite( uint8_t uFirstRegister, const void *pRawData, uint8_t uLength )
  {
    uint8_t const *puData = (uint8_t const*)pRawData;
    uint8_t uSREG = SREG;
    cli();
    if (g_pLast != this)
      Initialize();

    digitalWrite(m_uChipSelectPin, LOW);
    SPI.transfer(uFirstRegister | SPI_WRITE_MASK); // Send the start address with the write mask on
    while (uLength--)
      SPI.transfer(*puData++);
    digitalWrite(m_uChipSelectPin, HIGH);

    SREG = uSREG;
  }

  void CSharedSPI::BurstRead( uint8_t uFirstRegister, void *pRawData, uint8_t uLength ) const
  {
    uint8_t *puData = (uint8_t *)pRawData;
    uint8_t uSREG = SREG;
    cli();
    if (g_pLast != this)
      Initialize();

    digitalWrite(m_uChipSelectPin, LOW);
    SPI.transfer(uFirstRegister & ~SPI_WRITE_MASK); // Send the start address with the write mask off
    while (uLength--)
      *puData++ = SPI.transfer(0);
    digitalWrite(m_uChipSelectPin, HIGH);

    SREG = uSREG;
  }
}