/* *****************************************************************************
*  Interface to M95M02-DR EEPRom chip attached to the SPI bus Seems to be generic 
*  for other SPI devices too. Typically things that change between devices is 
*  the number of bytes needed for the address & the device page size. 
*  Currently only supporting 3 byte addressing of the eeprom. Though its not
*  hard to change to 2 or 4 byte addresses (see SendAddress). 
*  ***************************************************************************** */
#pragma once
#include <Arduino.h>


class SPI_EEPROM
{
private: 
  const uint8_t c_uChipSelectPin;
  const uint8_t c_uWriteProtectPin;
  const uint8_t c_uHoldPin;
public:
  enum EConstants 
  { 
    // Number of addressable bytes in the chip. 
    EndAddress = 262144,
  };


  SPI_EEPROM(uint8_t uChipSelect, uint8_t uWriteProtectPin, uint8_t uHoldPin);
  ~SPI_EEPROM();

  void Write(uint32_t uAddress, const uint8_t *pData, uint8_t uDataSize);
  void Read(uint32_t uAddress, uint8_t *pData, uint8_t uDataSize);
  uint16_t CalculateChecksum(uint32_t uStartAddress, uint32_t uLength);
private:
  void Initialize();
  void Select(bool bSelect);
  void WaitForWriteCompletion();

  void SendAddress(uint32_t uAddress);
};

