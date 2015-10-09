/* *****************************************************************************
*  Library to read data from MMA845x accelerometer (the one used on Melvyn)
*  ***************************************************************************** */
#pragma once

#include "Arduino.h"

struct AccelerationData
{
  int16_t m_nX;
  int16_t m_nY;
  int16_t m_nZ;
};

class Accelerometer
{
public:
  Accelerometer(uint8_t uI2CAddress = 0x1c);
  bool Start();
  void Shutdown();
  void ReadAcceleration(AccelerationData &rData);

  enum EState
  {
    STATE_Fault, // couldn't start device. 
    STATE_Off,   // device turned off
    STATE_Active // Device on
  } __attribute__((__packed__));

  EState GetState() const { return m_State; }

protected:
  bool Start(const uint8_t *pRegisterAddresses, const uint8_t *pRegisterValues, uint8_t uRegister1BaseValue, uint8_t uRegisters);
  bool CheckIdentity();

  bool ReliableWrite(uint8_t uRegister, uint8_t uValue);

  // The address of the accelerometer on the i2c bus. Typically 0x1c or 0x1d.
  const uint8_t m_I2CAddr; 

private:
  // Current state of sensor. 
  EState m_State; 

};
