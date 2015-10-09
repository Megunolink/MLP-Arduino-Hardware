#pragma once

#include "Arduino.h"

struct TransientConfig
{
  // Code that configures device assumes registers are stored in this structure in the
  // following order: control register 1, control register 2, full scale, transient
  // configuration, threshold configuration, debounce configuration, high pass cutoff
  uint8_t m_ControlRegister1; // 0x2a
  uint8_t m_ControlRegister2; // 0x2b
  uint8_t m_FullScale;        // 0x0e

  uint8_t m_TransientConfiguration;           // 0x1d
  uint8_t m_TransientThresholdConfiguration;  // 0x1f
  uint8_t m_TransientDebounceConfiguration;   // 0x20
  uint8_t m_HighPassCutoff;                   // 0x0f

  // The number of registers to configure. 
  enum EConstants { NUM_REGISTERS = 7}__attribute__((__packed__));

  // Maps declaration order to register number. 
  static const uint8_t m_RegisterOrder[NUM_REGISTERS];

  TransientConfig();

} __attribute__((__packed__));
