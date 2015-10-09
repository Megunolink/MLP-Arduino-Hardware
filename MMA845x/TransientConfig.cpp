#include "TransientConfig.h"
#include "Registers.h"
#include "Config.h"

using namespace MMA845x;

// Must reflect order of register values in structure. 
const uint8_t TransientConfig::m_RegisterOrder[TransientConfig::NUM_REGISTERS] = 
{REG_CONTROL1, REG_CONTROL2, REG_XYZ_DATA_CFG, REG_TRANSIENT_CFG, 
REG_TRANSIENT_THRESHOLD, REG_TRANSIENT_DEBOUNCE, REG_HIGH_PASS_CUTOFF};

TransientConfig::TransientConfig()
{
  m_ControlRegister1 = CR1_ODR_100_Hz;
  m_ControlRegister2 = CR2_MOD_LOW_POWER | CR2_SMOD_LOW_POWER;
  m_FullScale = FS_2g;

  m_TransientConfiguration = TC_ENABLE_X | TC_ENABLE_Y | TC_ENABLE_Z | TC_LATCH_EVENT;
  m_TransientThresholdConfiguration = 1;
  m_TransientDebounceConfiguration = 0;
  m_HighPassCutoff = 0;
}

