#pragma once

#include <nn/hid/CTR/hid_DebugPad.h>
#include <nn/hidlow/hidlow_LifoRing.h>
#include <nn/hidlow/CTR/hidlow_DebugPadRawStatus.h>

namespace nn{
namespace hidlow{
namespace CTR{

const s32 DEBUGPAD_LIFORING_BUFFER_NUM = 8;

struct IDebugPadRawStatus{
    fnd::InterlockedVariable<s16> hold;
    fnd::InterlockedVariable<s16> trigger;
    fnd::InterlockedVariable<s16> release;
    fnd::InterlockedVariable<s8> leftStickX;
    fnd::InterlockedVariable<s8> leftStickY;
    fnd::InterlockedVariable<s8> rightStickX;
    fnd::InterlockedVariable<s8> rightStickY;
};

class DebugPadLifoRing : public LifoRing{
public:
    IDebugPadRawStatus mBuffers[DEBUGPAD_LIFORING_BUFFER_NUM];

    void ReadData(DebugPadRawStatus *status,s32 bufferNum,s32 *pReadCount,s64 *pTick,s32 *pIndex);
};

}
}
}