#pragma once

#include <nn/hidlow/hidlow_LifoRing.h>
#include <nn/hidlow/CTR/hidlow_AnalogStick.h>
#include <nn/hid/CTR/hid_Pad.h>


namespace nn{
namespace hidlow{
namespace CTR{

const s32 PAD_LIFORING_BUFFER_NUM = 8;

struct IPadStatus
{
    fnd::InterlockedVariable<s32> hold;
    fnd::InterlockedVariable<s32> trigger;
    fnd::InterlockedVariable<s32> release;
    IAnalogStickStatus stick;
};

class PadLifoRing : public hidlow::LifoRing
{
public:
    f32 m_Svr2Volume;
    bit32 m_RawButtons;
    IAnalogStickStatus m_RawAnalogStick;
    s32 rev;
    IPadStatus m_Buffers[8];

    void ReadData(hid::CTR::PadStatus *status,s32 bufferNum,s32 *pReadCount,s64 *pTick,s32 *pIndex);
};

}
}
}