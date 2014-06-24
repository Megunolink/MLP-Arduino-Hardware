#pragma once

#include <inttypes.h>
#include "Stream.h"
#include "CircularBuffer.h"

class SPISerial : public Stream
{
public:
  SPISerial(uint8_t uChipSelectPin, uint8_t uInterruptPin);
  ~SPISerial();

  void begin(unsigned long uBaudRate);

  void EnableEnhancedFeatures();

  enum TransmitError
  {
    TxErr_None, 
    TxErr_Timeout
  };

  // Implementation of Stream
  virtual int available();
  virtual int read();
  virtual int peek();
  virtual void flush();

  // Implementation required of Print
  virtual size_t write(uint8_t uData);

private:
  void Select(bool bSelect);
  uint8_t ReadRegister(uint8_t uRegister);
  void WriteRegister(uint8_t uRegister, uint8_t uData);
  static void InterruptHandler();
  static SPISerial *s_pThis;

private:
  const uint8_t m_uChipSelectPin;
  const uint8_t m_uInterruptPin;
  const uint8_t m_uInterruptChannel;

  // Timeout for sending data. If 0, no timeout occurs. 
  uint16_t m_uTransmitTimeout;

  TransmitError m_LastTxError; 

  enum Constants 
  {
    RX_FIFO_SIZE = 64,
    TX_FIFO_SIZE = 64
  };

  NSPISerial::CircularBuffer<uint8_t, RX_FIFO_SIZE> m_ReceiveBuffer;

};

