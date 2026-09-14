#pragma once

#include <nn/hidlow.h>

namespace nn{
namespace hidlow{
namespace CTR{

class LifoRingCollector{
private:
    PadLifoRing m_PadLifoRing;
    TouchPanelLifoRing m_TouchPanelLifoRing;
    AccelerometerLifoRing m_AccelerometerLifoRing;
    GyroscopeLowLifoRing m_GyroscopeLowLifoRing;
    DebugPadLifoRing m_DebugPadLifoRing;
public:
    PadLifoRing* GetPadLifoRingAddress(){ return &this->m_PadLifoRing; }
    TouchPanelLifoRing* GetTouchPanelLifoRingAddress(){ return &this->m_TouchPanelLifoRing; }
    AccelerometerLifoRing* GetAccelerometerLifoRingAddress(){ return &this->m_AccelerometerLifoRing; }
    GyroscopeLowLifoRing* GetGyroscopeLowLifoRingAddress(){ return &this->m_GyroscopeLowLifoRing; }
    DebugPadLifoRing* GetDebugPadLifoRingAddress(){ return &this->m_DebugPadLifoRing; }
};
}
}
}