#include "Accelerometer.h"
#include "Registers.h"
#include "TransientConfig.h"
#include "Config.h"
#include "I2C/I2C.h"

using namespace MMA845x;

Accelerometer::Accelerometer(uint8_t uI2CAddress /*= 0x1c*/) 
  : m_I2CAddr(uI2CAddress)
{
  m_State = STATE_Off;
}


bool Accelerometer::Start()
{
  TransientConfig Config;
  return Start(TransientConfig::m_RegisterOrder, (uint8_t*)&Config, Config.m_ControlRegister1, TransientConfig::NUM_REGISTERS);
}

bool Accelerometer::Start(const uint8_t *pRegisterAddresses, const uint8_t *pRegisterValues, uint8_t uRegister1BaseValue, uint8_t uRegisters)
{
  I2c.begin();

  const uint8_t *pRegister, *pRegisterValue;
  int8_t iRegister, nAttempts;
  uint8_t uTest;

  I2c.setSpeed(1);  // High speed (400 kHz).
  I2c.timeOut(10);  // Set timeout to recover from I2c bus lockup. [ms]

  // Write the configuration data out to the device. We assume that the
  // registers are in the correct order and that the first one places the
  // device in standby mode ready to be reconfigured. On some devices, 
  // configuration has been unreliable so we try several times. 
  while(uRegisters--)
  {
    if (!ReliableWrite(*pRegisterAddresses++, *pRegisterValues++))
    {
      m_State = STATE_Fault;
      return false; // Configuration failed. 
    }
  }

  // Switch the device to active mode. 
  if (ReliableWrite(REG_CONTROL1, uRegister1BaseValue | CR1_ACTIVE))
    m_State = STATE_Active;
  else
    m_State = STATE_Fault;

  return m_State == STATE_Active;
}

void Accelerometer::Shutdown()
{
  // Place device in sleep mode. 
  if (m_State != STATE_Off && ReliableWrite(REG_CONTROL1, 0))
    m_State = STATE_Off;
}

void Accelerometer::ReadAcceleration(AccelerationData &rData)
{
  I2c.read(m_I2CAddr, REG_OUT_X_MSB, sizeof(rData), (uint8_t*)&rData);

  // Data 3 x 2 bytes. Each pair of bytes represents a 10-bit number. 
  // The first 8 bits store the most significant bits (in two's complement
  // form). Bits 7 &6 of the second byte contains the least significant
  // bits of the 10-bit number. 
  int8_t *pData = (int8_t*)&rData;
  int8_t nTemp;

  for(int i=0; i < 3; ++i)
  {
    nTemp = *pData;
    *pData = (nTemp << 2) | ((pData[1] >> 6) & 0x03);
    ++pData;
    *pData = nTemp >> 6;
    ++pData;
  }
}

bool Accelerometer::ReliableWrite(uint8_t uRegister, uint8_t uValue)
{
  int nAttempts;
  uint8_t uTest;

  nAttempts = 0;
  do
  {
    I2c.write(m_I2CAddr, uRegister, uValue);
    I2c.read(m_I2CAddr, uRegister, 1, &uTest);
    ++nAttempts;
  } while (uTest != uValue && nAttempts < 20);

  return uTest == uValue; 
}

bool Accelerometer::CheckIdentity()
{
  uint8_t uId;

  I2c.read(m_I2CAddr, REG_WHO_AM_I, sizeof(uId), (uint8_t*)&uId);

  return uId == 0x3A; // 3A is the device id expected for the MMA845x
}
