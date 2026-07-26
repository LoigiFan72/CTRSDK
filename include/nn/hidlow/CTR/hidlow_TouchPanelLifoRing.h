#pragma once

#include <nn/hid/CTR/hid_TouchPanel.h>
#include <nn/hidlow/hidlow_LifoRing.h>

namespace nn{
namespace hidlow{
namespace CTR{
struct ITouchPanelStatus{
    fnd::InterlockedVariable<u16> x;
    fnd::InterlockedVariable<u16> y;
    fnd::InterlockedVariable<u8> touch;
    u8 rev[3];
};

class TouchPanelLifoRing : public hidlow::LifoRing{
public:
    const int TOUCHPANEL_LIFORING_BUFFER_NUM = 8;

    ITouchPanelStatus mRaw;
    ITouchPanelStatus mBuffers[TOUCHPANEL_LIFORING_BUFFER_NUM];

    void ReadData(hid::CTR::TouchPanelStatus *status,s32 bufferNum,s32 *pReadCount,s64 *pTick,s32 *pIndex);
};
}
}
}