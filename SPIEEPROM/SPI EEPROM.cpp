#include "SPI EEPROM.h"
#include "SPI/SPI.h"
#include "avr/crc16.h"
#include "avr/wdt.h"

// Op-codes for eeprom. 
#define OP_WRITE 2 // Writing data to memory
#define OP_READ 3  // Reading data from device memory. 
#define OP_WRITE_ENABLE 6 // Enable writing data to the device. 
#define OP_WRITE_DISABLE 4 // Disable writing data to the device
#define OP_READ_STATUS_REG 5
#define OP_WRITE_STATUS_REG 1


SPI_EEPROM::SPI_EEPROM(uint8_t uChipSelect /*= SS*/, uint8_t uWriteProtectPin, uint8_t uHoldPin)
  : c_uChipSelectPin(uChipSelect)
  , c_uHoldPin(uHoldPin)
  , c_uWriteProtectPin(uWriteProtectPin)
{
}


SPI_EEPROM::~SPI_EEPROM(void)
{
}

void SPI_EEPROM::Write( uint32_t uAddress, const uint8_t *pData, uint8_t uDataSize )
{

  Initialize();

  // We only need to send the address once for each page. Within a page, 
  // it will automatically increment to the next byte. This device
  // has a 256 byte page. Paging only applies to writes. 
  const uint32_t uPageMask = 0xffff00;
  const uint32_t uAddressMask = 0xffffff; // Forces valid address. 

  uint32_t uByteAddress = uAddress & uAddressMask;  // device is 24 bit address. 
  while(uDataSize)
  {
    uint16_t uPage = uByteAddress & uPageMask;
    uint8_t uSREGEntry = SREG;
    cli();

    Select(true);
    SPI.transfer(OP_WRITE_ENABLE);  // Enable writing. 
    Select(false);

    Select(true);
    SPI.transfer(OP_WRITE);         // Initiate write to memory operation.
    SendAddress(uAddress);
    while (uPage == (uByteAddress & uPageMask) && uDataSize)
    {
      SPI.transfer(*pData++);
      ++uByteAddress;
      --uDataSize;
    }
    Select(false); // Also initiates write operation. 
    SREG = uSREGEntry;

    WaitForWriteCompletion();
  }
}

void SPI_EEPROM::Read( uint32_t uAddress, uint8_t *pData, uint8_t uDataSize )
{
  Initialize();

  uint8_t uSREGEntry = SREG;
  cli();
  Select(true);
  SPI.transfer(OP_READ);
  SendAddress(uAddress);
  while (uDataSize--)
  {
    *pData++ = SPI.transfer(0); // Read data. 
  }
  Select(false);
  SREG = uSREGEntry;
}


uint16_t SPI_EEPROM::CalculateChecksum( uint32_t uStartAddress, uint32_t uLength )
{
  Initialize();
  Serial.println(uStartAddress);
  Serial.println(uLength);
  wdt_reset();
  uint16_t uChecksum = 0; 
  uint8_t uSREGEntry = SREG;
  cli();
  Select(true);
  SPI.transfer(OP_READ);
  SendAddress(uStartAddress);
  while (uLength--)
  {
    uint8_t uValue = SPI.transfer(0); // Read data. 
    uChecksum = _crc16_update(uChecksum, uValue);
  }
  Select(false);
  SREG = uSREGEntry;
  return uChecksum;
}

void SPI_EEPROM::Initialize()
{
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);
  pinMode(c_uChipSelectPin, OUTPUT);
  Select(false);

  // hold and write protect function not currently used,
  // but we have to disable write protect and turn off hold. 
  // Both are active low. 
  pinMode(c_uHoldPin, OUTPUT);
  pinMode(c_uWriteProtectPin, OUTPUT);
  digitalWrite(c_uHoldPin, HIGH);
  digitalWrite(c_uWriteProtectPin, HIGH);
}

void SPI_EEPROM::Select( bool bSelect )
{
  digitalWrite(c_uChipSelectPin, bSelect ? LOW : HIGH);
}

void SPI_EEPROM::WaitForWriteCompletion()
{
  uint8_t uStatus;
  uint32_t uStartTime = millis();

  uint8_t uSREGEntry = SREG;
  cli();
  Select(true);
  SPI.transfer(OP_READ_STATUS_REG);
  do 
  {
    uStatus = SPI.transfer(0);
  } while ((uStatus & 0x01) && ((millis() - uStartTime) < 1000));

  Select(false);
  SREG = uSREGEntry;
#if 0
  do 
  {
    delay(2);

    // Busy status is indicated by a 1 in bit 0 of the status register. 
    uint8_t uSREGEntry = SREG;
    cli();

    Select(true);
    uStatus = SPI.transfer(OP_READ_STATUS_REG);
    Select(false);
    SREG = uSREGEntry;

  } while ((uStatus & 0x01) && !Timer.TimePassed_Milliseconds(1000));
#endif
}

void SPI_EEPROM::SendAddress( uint32_t uAddress )
{
  SPI.transfer((uAddress >> 16) & 0xff);
  SPI.transfer((uAddress >> 8) & 0xff);
  SPI.transfer((uAddress) & 0xff);
}
