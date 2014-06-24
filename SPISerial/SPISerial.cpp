#include "SPISerial.h"
#include "Spi/SPI.h"
#include "Registers.h"

// data flags to indicate read/ write operations. 
#define SPISER_WRITE 0x00
#define SPISER_READ 0x80

#define SPISER_XTAL_FREQ 14745600 // Hz

// For interrupt handler. 
SPISerial * SPISerial::s_pThis = NULL;


SPISerial::SPISerial( uint8_t uChipSelectPin, uint8_t uInterruptPin )
  : m_uChipSelectPin(uChipSelectPin)
  , m_uInterruptPin(uInterruptPin)
  , m_uInterruptChannel(uInterruptPin - 2)
{
  m_uTransmitTimeout = 0; 
  m_LastTxError = TxErr_None;
}


SPISerial::~SPISerial(void)
{
}

void SPISerial::begin( unsigned long uBaudRate )
{
  //SPI interface will run at 1MHz if 8MHz chip or 2Mhz if 16Mhz
  //Data is clocked on the rising edge and clock is low when inactive
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);  
  SPI.setClockDivider(SPI_CLOCK_DIV8); 
  SPI.setDataMode(SPI_MODE0);

  // Configure chip select pin. 
  pinMode(m_uChipSelectPin, OUTPUT);  
  Select(false);

  // Configure interrupt pin. 
  pinMode(m_uInterruptPin, INPUT_PULLUP);

  // Software reset
  WriteRegister(REG_IO_CONTROL, IOCTRL_SW_RESET);
  delay(50); // give a little time for the reset. 

  // Configure interrupts.
  WriteRegister(REG_INTERRUPT_ENABLE, IER_RX_FIFO_THRESHOLD);

  // Configure UART baud rate, parity, stop bits, bits in a word... 
  uint32_t uDivider = (SPISER_XTAL_FREQ / 16) / uBaudRate;
  uint8_t uControl = LCR_NO_PARITY | LCR_8_BIT_WORD_1_STOP;
  WriteRegister(REG_LINE_CONTROL, LCR_EN_DIVISOR_LATCH | LCR_NO_PARITY | LCR_8_BIT_WORD_1_STOP); // latches divider. 
  WriteRegister(REGS_DIVIDER_LOW, uDivider & 0xff);
  WriteRegister(REGS_DIVIDER_HIGH, (uDivider >> 8) & 0xff);
  
  // Configure fifos.
  
  // Turn on enhanced mode in line control so we can turn on advanced features. 
  EnableEnhancedFeatures();
  WriteRegister(REG_MODEM_CONTROL, MCREH_DIVIDE_BY_1 | MCREH_EN_FIFO_TRIGGER);
  WriteRegister(REG_FIFO_CONTROL, FCR_RX_TRIGGER_56_CHAR | 
    FCR_RESET_TX_FIFO | FCR_RESET_RX_FIFO | FCR_ENABLE_FIFOS);
  WriteRegister(REG_LINE_CONTROL, LCR_NO_PARITY | LCR_8_BIT_WORD_1_STOP);

  s_pThis = this;
  attachInterrupt(m_uInterruptChannel, InterruptHandler, FALLING);
}

int SPISerial::available()
{
  return m_ReceiveBuffer.IsEmpty() ? 0 : 1;
}

int SPISerial::read()
{
  uint8_t uSREGEntry = SREG;
  cli();
  int nResult = -1;
  if (!m_ReceiveBuffer.IsEmpty())
  { 
    nResult = *m_ReceiveBuffer.Tail();
    m_ReceiveBuffer.PopTail();
  }
  SREG = uSREGEntry;
  return nResult;
}

int SPISerial::peek()
{
  uint8_t uSREGEntry = SREG;
  cli();
  int nResult = -1;
  if (!m_ReceiveBuffer.IsEmpty())
  { 
    nResult = *m_ReceiveBuffer.Tail();
  }
  SREG = uSREGEntry;
  return nResult;
}

void SPISerial::flush()
{

}

size_t SPISerial::write( uint8_t uData)
{
  if (m_LastTxError != TxErr_None)
    return 0; 
  
  uint32_t uStartTime = millis();
  bool bTimeout; 
  uint8_t uLevel;

  do 
  {
    uLevel = ReadRegister(REG_TX_FIFO_LEVEL);
    bTimeout = m_uTransmitTimeout > 0 && (millis() - uStartTime) > m_uTransmitTimeout;

  } while (uLevel == 0 && !bTimeout);

  if (bTimeout)
  {
    m_LastTxError = TxErr_Timeout;
    Serial.println("Timeout");
    return 0; 
  }

  WriteRegister(REG_RXTX_FIFO, uData);

  return 1; 
}

void SPISerial::Select( bool bSelect )
{
  digitalWrite(m_uChipSelectPin, bSelect ? LOW : HIGH);
}

uint8_t SPISerial::ReadRegister(uint8_t uRegister)
{
  uint8_t uResult;

  // SC16IS740 expects a R/W  bit first, followed by the 4 bit
  // register address of the byte.
  uRegister = uRegister << 3;
  uint8_t uSend = uRegister | SPISER_READ;

  uint8_t uSREGEntry = SREG;
  cli();

  Select(true);
  SPI.transfer(uSend); // Send register location
  uResult = SPI.transfer(0);  // Get Value from register
  Select(false);

  SREG = uSREGEntry;

  return uResult;
} 

void SPISerial::WriteRegister( uint8_t uRegister, uint8_t uData )
{
  // SC16IS740 expects a R/W  bit first, followed by the 4 bit
  // register address of the byte.
  uRegister = uRegister << 3;
  uint8_t uSend = uRegister | SPISER_WRITE;

  uint8_t uSREGEntry = SREG;
  cli();

  Select(true);
  SPI.transfer(uSend); 
  SPI.transfer(uData); 
  Select(false);

  SREG = uSREGEntry;
}

void SPISerial::EnableEnhancedFeatures()
{
  WriteRegister(REG_LINE_CONTROL, LCR_ENHANCED_MODE); // needed to set fifo trigger levels & divider
  WriteRegister(REGEH_ENHANCED_FEATURES, 0x10); // Enable enhanced functions
  WriteRegister(REG_LINE_CONTROL, 0);
}

void SPISerial::InterruptHandler()
{
  if (s_pThis != NULL)
  {
    uint8_t uBytesAvailable = s_pThis->ReadRegister(REG_RX_LEVEL);
    while (uBytesAvailable--)
    {
      s_pThis->m_ReceiveBuffer.Add(s_pThis->ReadRegister(REG_RXTX_FIFO));
    }
  }
}

