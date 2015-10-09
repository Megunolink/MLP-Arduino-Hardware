/* *****************************************************************************
*  Register definitions for the MMA845x accelerometer
*  ***************************************************************************** */
#pragma once
#include "stdint.h"

namespace MMA845x
{
  const uint8_t REG_STATUS = 0x00; //(R) Real time status
  const uint8_t REG_OUT_X_MSB = 0x01; //(R) [7:0] are 8 MSBs of 10-bit sample
  const uint8_t REG_OUT_X_LSB = 0x02; //(R) [7:6] are 2 LSBs of 10-bit sample
  const uint8_t REG_OUT_Y_MSB = 0x03; //(R) [7:0] are 8 MSBs of 10-bit sample
  const uint8_t REG_OUT_Y_LSB = 0x04; //(R) [7:6] are 2 LSBs of 10-bit sample
  const uint8_t REG_OUT_Z_MSB = 0x05; //(R) [7:0] are 8 MSBs of 10-bit sample
  const uint8_t REG_OUT_Z_LSB = 0x06; //(R) [7:6] are 2 LSBs of 10-bit sample
  const uint8_t REG_SYSMOD = 0x0b; //(R) Current system mode
  const uint8_t REG_INT_SOURCE = 0x0c; //(R) Interrupt status
  const uint8_t REG_WHO_AM_I = 0x0d; //(R) Device ID (0x3A)
  //const uint8_t REG_XYZ_DATA_CFG = 0xe; //(R/W) Dynamic range settings
  const uint8_t REG_HP_FILTER_CUTOFF = 0x0f; //(R/W) cut-off frequency is set to 16Hz @ 800Hz
  const uint8_t REG_PL_STATUS = 0x10; //(R) Landscape/Portrait orientation status
  const uint8_t REG_PL_CFG = 0x11; //(R/W) Landscape/Portrait configuration
  const uint8_t REG_PL_COUNT = 0x12; //(R) Landscape/Portrait debounce counter
  const uint8_t REG_PL_BF_ZCOMP = 0x13; //(R) Back-Front, Z-Lock trip threshold
  const uint8_t REG_P_L_THS_REG = 0x14; //(R/W) Portrait to Landscape trip angle is 29 degree
  const uint8_t REG_FF_MT_CFG = 0x15; //(R/W) Freefall/motion functional block configuration
  const uint8_t REG_FF_MT_SRC = 0x16; //(R) Freefall/motion event source register
  const uint8_t REG_FF_MT_THS = 0x17; //(R/W) Freefall/motion threshold register
  const uint8_t REG_FF_MT_COUNT = 0x18; //(R/W) Freefall/motion debounce counter
  //const uint8_t REG_TRANSIENT_CFG = 0x1d; //(R/W) Transient functional block configuration
  //const uint8_t REG_TRANSIENT_SRC = 0x1e; //(R) Transient event status register
  const uint8_t REG_TRANSIENT_THS = 0x1f; //(R/W) Transient event threshold
  const uint8_t REG_TRANSIENT_COUNT = 0x20; //(R/W) Transient debounce counter
  const uint8_t REG_PULSE_CFG = 0x21; //(R/W) ELE, Double_XYZ or Single_XYZ
  const uint8_t REG_PULSE_SRC = 0x22; //(R) EA, Double_XYZ or Single_XYZ
  const uint8_t REG_PULSE_THSX = 0x23; //(R/W) X pulse threshold
  const uint8_t REG_PULSE_THSY = 0x24; //(R/W) Y pulse threshold
  const uint8_t REG_PULSE_THSZ = 0x25; //(R/W) Z pulse threshold
  const uint8_t REG_PULSE_TMLT = 0x26; //(R/W) Time limit for pulse
  const uint8_t REG_PULSE_LTCY = 0x27; //(R/W) Latency time for 2nd pulse
  const uint8_t REG_PULSE_WIND = 0x28; //(R/W) Window time for 2nd pulse
  const uint8_t REG_ASLP_COUNT = 0x29; //(R/W) Counter setting for auto-sleep
  const uint8_t REG_CTRL_REG1 = 0x2a; //(R/W) ODR = 800 Hz, STANDBY mode
  const uint8_t REG_CTRL_REG2 = 0x2b; //(R/W) Sleep enable, OS Modes, RST, ST
  const uint8_t REG_CTRL_REG3 = 0x2c; //(R/W) Wake from sleep, IPOL, PP_OD
  const uint8_t REG_CTRL_REG4 = 0x2d; //(R/W) Interrupt enable register
  const uint8_t REG_CTRL_REG5 = 0x2e; //(R/W) Interrupt pin (INT1/INT2) map
  const uint8_t REG_OFF_X = 0x2f; //(R/W) X-axis offset adjust
  const uint8_t REG_OFF_Y = 0x30; //(R/W) Y-axis offset adjust
  const uint8_t REG_OFF_Z = 0x31; //(R/W) Z-axis offset adjust

  const uint8_t FULL_SCALE_RANGE_2g = 0x0;
  const uint8_t FULL_SCALE_RANGE_4g = 0x1;
  const uint8_t FULL_SCALE_RANGE_8g = 0x2;

}
