#pragma once

#include <nn/hid/CTR/hid_Accelerometer.h>
#include <nn/hidlow/hidlow_LifoRing.h>

namespace nn{
namespace hidlow{
namespace CTR{

const s32 ACCELEROMETER_LIFORING_BUFFER_NUM = 8;

struct IAccelerometerStatus
{
    fnd::InterlockedVariable<s16> x;
    fnd::InterlockedVariable<s16> y;
    fnd::InterlockedVariable<s16> z;
};

class AccelerometerLifoRing : public hidlow::LifoRing
{
public:
    IAccelerometerStatus mRaw;
    short rev;
    IAccelerometerStatus mBuffers[8];

    void ReadData(hid::CTR::AccelerometerStatus *status,s32 bufferNum,s32 *pReadCount,s64 *pTick,s32 *pIndex);
};

}
}
}