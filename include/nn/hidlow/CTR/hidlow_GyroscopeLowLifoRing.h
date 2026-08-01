#pragma once

#include <nn/hid/CTR/hid_GyroscopeLowStatus.h>
#include <nn/hid/CTR/hid_DeviceStatus.h>
#include <nn/hidlow/hidlow_LifoRing.h>

namespace nn{
namespace hidlow{
namespace CTR{

const s32 GYROSCOPELOW_LIFORING_BUFFER_NUM = 32;
    
struct IGyroscopeLowStatus{
    fnd::InterlockedVariable<s16> x;
    fnd::InterlockedVariable<s16> y;
    fnd::InterlockedVariable<s16> z;
};

class GyroscopeLowLifoRing : public LifoRing{
public:
    GyroscopeLowLifoRing(){
        mRaw.x = 0;
        mRaw.y = 0;
        mRaw.z = 0;
    };
    ~GyroscopeLowLifoRing() {};

    void ReadRaw(nn::hid::CTR::GyroscopeLowStatus* pBuf){
        pBuf->x = mRaw.x;
        pBuf->y = mRaw.y;
        pBuf->z = mRaw.z;
    }

    void ReadData(hid::CTR::GyroscopeLowStatus* pBuffers, s32 bufferNum, s32* pReadCount, s64* pTick, s32* pIndex);\

    IGyroscopeLowStatus mRaw;
    s16 rev;
    IGyroscopeLowStatus mBuffers[GYROSCOPELOW_LIFORING_BUFFER_NUM];
};

}
}
}