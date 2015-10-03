#include "SPI EEPROM.h"
#include "SPI/SPI.h"
#include "avr/crc16.h"
#include "avr/wdt.h"

// Op-codes for eeprom. 
#define OP_WRITE 2 // Writing data to memory
#define OP_READ 3  // Reading data from device memory. 
#define OP_ID_WRITE 0x82 // Write to id page
#define OP_ID_READ 0x83  // Read from id page. 

#define OP_WRITE_ENABLE 6 // Enable writing data to the device. 
#define OP_WRITE_DISABLE 4 // Disable writing data to the device
#define OP_READ_STATUS_REG 5
#define OP_WRITE_STATUS_REG 1

// We wait until the eeprom has finished writing, but not forever. 
#define MAX_COMPLETION_ITERATIONS 0xffff

SPI_EEPROM::SPI_EEPROM(uint8_t uChipSelect /*= SS*/, uint8_t uWriteProtectPin, uint8_t uHoldPin)
  : c_uChipSelectPin(uChipSelect)
  , c_uHoldPin(uHoldPin)
  , c_uWriteProtectPin(uWriteProtectPin)
{
}


SPI_EEPROM::~SPI_EEPROM(void)
{
}

/* Verifies the eeprom is present and functional by reading magic bytes from the 
identification region of the chip. If the bytes aren't found, writes the bytes, then
tries to read them back (if bWriteMagicBytes is true). */
bool SPI_EEPROM::CheckEeprom(bool bWriteMagicBytes /*= true*/)
{
  const static char MagicBytes[] PROGMEM = { "Dragonfly\0" };
  uint8_t auRamMagicBytes[10]; // Must be length of magic bytes string (including null terminator)

  // Read magic bytes to ram. 
  for(int iByte = 0; iByte < sizeof(auRamMagicBytes); ++iByte)
  {
#if defined(__AVR_ATmega2560__)
    uint8_t uByte = pgm_read_byte_far(MagicBytes + iByte);
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega328P__)
    uint8_t uByte = pgm_read_byte_near(MagicBytes + iByte);
#else
#pragma error('unknown processor')
#endif
    auRamMagicBytes[iByte] = uByte;
  }

  if (IdSectionStartsWith(auRamMagicBytes, sizeof(auRamMagicBytes)))
  {
    return true; 
  }

  if (bWriteMagicBytes)
    if (!WriteId(auRamMagicBytes, sizeof(auRamMagicBytes)))
      return false; 

  return IdSectionStartsWith(auRamMagicBytes, sizeof(auRamMagicBytes));
}

bool SPI_EEPROM::IdSectionStartsWith( uint8_t *puRamMagicBytes, uint8_t uMagicBytesLength )
{
  uint8_t auTestBuffer[uMagicBytesLength];

  Read(0, auTestBuffer, sizeof(auTestBuffer), OP_ID_READ);
  return memcmp(auTestBuffer, puRamMagicBytes, sizeof(auTestBuffer)) == 0;
} 

bool SPI_EEPROM::WriteId( uint8_t *puRamMagicBytes, uint8_t MagicBytesLength )
{
  return Write(0, puRamMagicBytes, MagicBytesLength, OP_ID_WRITE);
}

bool SPI_EEPROM::Write( uint32_t uAddress, const uint8_t *pData, uint32_t uDataSize )
{
  return Write(uAddress, pData, uDataSize, OP_WRITE);
}

bool SPI_EEPROM::Write( uint32_t uAddress, const uint8_t *pData, uint32_t uDataSize, uint8_t uInstruction )
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
    SPI.transfer(uInstruction);         // Initiate write to memory operation.
    SendAddress(uAddress);
    while (uPage == (uByteAddress & uPageMask) && uDataSize)
    {
      SPI.transfer(*pData++);
      ++uByteAddress;
      --uDataSize;
    }
    Select(false); // Also initiates write operation. 
    SREG = uSREGEntry;
    if (!WaitForWriteCompletion())
    { 
      Serial.println(F("EEPROM write error"));
      return false; 
    }
  }

  return true; 
}

void SPI_EEPROM::Read( uint32_t uAddress, uint8_t *pData, uint32_t uDataSize )
{
  Read(uAddress, pData, uDataSize, OP_READ);
}

void SPI_EEPROM::Read( uint32_t uAddress, uint8_t *pData, uint32_t uDataSize, uint8_t uInstruction )
{
  Initialize();

  uint8_t uSREGEntry = SREG;
  cli();
  Select(true);
  SPI.transfer(uInstruction);
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

bool SPI_EEPROM::WaitForWriteCompletion()
{
  uint8_t uStatus;
  uint32_t uStartTime = millis();
  uint32_t uIterations = 0; 
  uint8_t uSREGEntry = SREG;


  cli();
  Select(true);
  SPI.transfer(OP_READ_STATUS_REG);
  do 
  {
    uStatus = SPI.transfer(0);

    // using loop counter to 'time-out' because millis time won't 
    // function properly in here (interrupts are turned off). 
    ++uIterations;

  } while ((uStatus & 0x01) && uIterations < MAX_COMPLETION_ITERATIONS);
  Select(false);
  SREG = uSREGEntry;

  return uIterations < MAX_COMPLETION_ITERATIONS;
}

void SPI_EEPROM::SendAddress( uint32_t uAddress )
{
  SPI.transfer((uAddress >> 16) & 0xff);
  SPI.transfer((uAddress >> 8) & 0xff);
  SPI.transfer((uAddress) & 0xff);
}

