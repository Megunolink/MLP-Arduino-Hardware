#pragma once
#include "stdint.h"

namespace MMA845x
{
  /* **
  * Constants for configuration of control register 1. 
  * Control register 1 sets the output data rate, sleep mode data rate, low noise
  * mode and actives sleep or active mode. 
  ** */
#define REG_CONTROL1 0x2a

  // Options for output data rate. Enumeration values correspond to values required
  // for CTRL_REG1 (0x2a). Mask matches only bits used to configure the output data rate.
  // Bits 3..5 are used for the output data rate. 
#define CR1_ODR_MASK 0x38 
#define CR1_ODR_800_Hz 0x00 
#define CR1_ODR_400_Hz 0x08 
#define CR1_ODR_200_Hz 0x10 
#define CR1_ODR_100_Hz 0x18 
#define CR1_ODR_50_Hz 0x20
#define CR1_ODR_12o5_Hz 0x28 
#define CR1_ODR_6o25_Hz 0x30 
#define CR1_ODR_1o563_Hz 0x38 

  // Options for sleep mode rate. CTRL_REG1 (0x2a). Mask matches only bits used to configure
  // the sleep mode rate. Bits 6 & 7 are used. 
#define CR1_ASLP_MASK    0xC0
#define CR1_ASLP_50_Hz   0x00
#define CR1_ASLP_12o5_Hz 0x40
#define CR1_ASLP_6o25_Hz 0x80
#define CR1_ASLP_1o56_Hz 0xC0

  // Low noise mode when set in CTRL_REG1 (0x2a).
#define CR1_LOW_NOISE 0x04

  // Switches devices to active when set in CTRL_REG1 (0x2a). Configuration can only be updated when inactive. 
#define CR1_ACTIVE 0x01

  /* **
  * Constants for configuration of control register 2. 
  * Control register 2 sets the sleep and active mode power modes & enables auto sleep. It
  * can also be used to reset the device. 
  ** */
#define REG_CONTROL2 0x2b

#define CR2_SOFT_RESET 0x40

  // Sleep mode power scheme selection
#define CR2_SMOD_NORMAL             0x00
#define CR2_SMOD_LOWNOISE_LOWPOWER  0x08
#define CR2_SMOD_HIGH_RESOLUTION    0x10
#define CR2_SMOD_LOW_POWER          0x18

  // Enable sleep mode when set. 
#define CRS_SMOD_ENABLE 0x04

  // Active mode power scheme selection
#define CR2_MOD_NORMAL             0x00
#define CR2_MOD_LOWNOISE_LOWPOWER  0x01
#define CR2_MOD_HIGH_RESOLUTION    0x02
#define CR2_MOD_LOW_POWER          0x03

  // -----------------------------------------------------------------------------------------
  // Motion configuration. 

  /* **
  * Constants for configuration of the motion configuration register (0x15)
  * Sets the axes that are enabled for motion detection. 
  ** */
  struct MotionConfig
  {
    // Motion is latched until read when set.
    static const uint8_t MC_LATCH_EVENT = 0x80;

    // Flags to enable motion in a given axis
    static const uint8_t MC_ENABLE_X = 0x08;
    static const uint8_t MC_ENABLE_Y = 0x10;
    static const uint8_t MC_ENABLE_Z = 0x20;

    // Flag to set motion or free fall detection option. 
    static const uint8_t MC_DETECT_FREEFALL = 0;
    static const uint8_t MC_DETECT_MOTION = 0x40;

    /* **
    * Constants to find out information about a motion event. Includes polarity and 
    * axis information. 
    ** */

    // Motion signalled in one or more axes. 
    static const uint8_t MotionSignalled = 0x80;

    // Signals a transient event on the given axis.
    static const uint8_t MotionSignalled_X = 0x02;
    static const uint8_t MotionSignalled_Y = 0x08;
    static const uint8_t MotionSignalled_Z = 0x20;

    // Signals polarity of transient event on given axis. 1 => negative g, 0 => positive g. 
    static const uint8_t NegativeMotion_X = 0x01;
    static const uint8_t NegativeMotion_Y = 0x04;
    static const uint8_t NegativeMotion_Z = 0x10;

    /* **
    * Constants to se the motion threshold. 
    * Motion threshold is [0, 127]. Most significant bit selects debounce counter mode. 
    ** */

    static const uint8_t MotionThresholdMask = 0x7f;
    static const uint8_t MotionDebounceFlag = 0x80;
  };


  // -----------------------------------------------------------------------------------------
  // Transient configuration. 


  /* **
  * Constants for configuration of the transient configuration registery (0x1D)
  * Set which axes are enabled for transient detection, enables high pass filter and 
  * determines if events are latched into the transient source register.
  ** */
#define REG_TRANSIENT_CFG 0x1D

  // Transient flags are latched into the transient source register when set. 
#define TC_LATCH_EVENT 0x10

  // Flags to enable transient detection for a given axis. 
#define TC_ENABLE_X 0x02
#define TC_ENABLE_Y 0x04
#define TC_ENABLE_Z 0x08

  // Flag to bypass high-pass filter
#define TC_BYPASS_HPF 0x01

  /* **
  * Constants to find out information about a transient source from the transient source 
  * register. Latched values are cleared when read from this register (read only). 
  ** */
#define REG_TRANSIENT_SRC 0x1E

  // Signals one or more event has been asserted
#define TS_EVENT_ACTIVE 0x40

  // Signals a transient event on the given axis.
#define TS_X_TRANSIENT 0x02
#define TS_Y_TRANSIENT 0x08
#define TS_Z_TRANSIENT 0x20

  // Signals polarity of transient event on given axis. 1 => negative g, 0 => positive g. 
#define TS_X_POL_NEGATIVE 0x01
#define TS_Y_POL_NEGATIVE 0x04
#define TS_Z_POL_NEGATIVE 0x10

  /* **
  * Constants to configure the transient threshold and debounce mode. 
  ** */
#define REG_TRANSIENT_THRESHOLD 0x1F

  // Mask for debounce bit. 0=> increments/ decrements debounce; 1=> increments or clears counter
#define TT_DEBOUNCE 0x80

  // Mask for transient threshold
#define TT_THRESHOLD_MASK 0x7F

  /* **
  * Constants to configure the debounce counts required to continuously match the condition where the
  * unsigned value of high-pass filtered data is greater than the user specified threshold. 
  ** */
#define REG_TRANSIENT_DEBOUNCE 0x20

  /* **
  * Constants to configure maximum full scale range of the accelerometer sensor. XYZ_DATA_CFG 
  * (register 0x0e)
  ** */
#define REG_XYZ_DATA_CFG 0x0e

  // Full scale for measurements. 
#define FS_2g 0
#define FS_4g 1
#define FS_8g 2

  /* **
  * Constants to configure high pass cutoff register
  ** */
#define REG_HIGH_PASS_CUTOFF 0x0f

#define HPF_FREQ_MASK 0x03


}
