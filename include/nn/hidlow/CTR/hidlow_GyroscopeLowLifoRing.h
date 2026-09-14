#pragma once

#include <nn/hid/CTR/hid_GyroscopeLowStatus.h>
#include <nn/hid/CTR/hid_DeviceStatus.h>
#include <nn/hidlow/hidlow_LifoRing.h>

namespace nn{
namespace hidlow{
namespace CTR{

const s32 GYROSCOPELOW_LIFORING_BUFFER_NUM = 32;
    
struct IGyroscopeLowStatus
{
    fnd::InterlockedVariable<s16> x;
    fnd::InterlockedVariable<s16> y;
    fnd::InterlockedVariable<s16> z;
};

class GyroscopeLowLifoRing : public LifoRing
{
public:
    GyroscopeLowLifoRing()
    {
        m_Raw.x = 0;
        m_Raw.y = 0;
        m_Raw.z = 0;
    };
    ~GyroscopeLowLifoRing() 
    {
    };

    void ReadRaw(nn::hid::CTR::GyroscopeLowStatus* pBuf)
    {
        pBuf->x = m_Raw.x;
        pBuf->y = m_Raw.y;
        pBuf->z = m_Raw.z;
    }

    void ReadData(hid::CTR::GyroscopeLowStatus* pBuffers, s32 bufferNum, s32* pReadCount, s64* pTick, s32* pIndex);\

    IGyroscopeLowStatus m_Raw;
    s16 rev;
    IGyroscopeLowStatus m_Buffers[GYROSCOPELOW_LIFORING_BUFFER_NUM];
};

}
}
}