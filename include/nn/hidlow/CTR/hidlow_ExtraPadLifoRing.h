#pragma once

#include <nn/hidlow/hidlow_LifoRing.h>
#include <nn/hidlow/CTR/hidlow_AnalogStick.h>
#include <nn/hid/CTR/hid_ExtraPadStatus.h>

namespace nn{
namespace hidlow{
namespace CTR{

struct IExtraPadStatus
{
    fnd::InterlockedVariable<s32> hold;
    fnd::InterlockedVariable<s32> trigger;
    fnd::InterlockedVariable<s32> release;
    fnd::InterlockedVariable<s8> voltage;
    fnd::InterlockedVariable<s8> isConnected;
    short rev;
    IAnalogStickStatus stick;
    IAnalogStickStatus extraStick;
};

const s32 EXTRA_PAD_LIFORING_BUFFER_NUM = 8;

class ExtraPadLifoRing : public LifoRing
{
public:
    ExtraPadLifoRing(){}
    ~ExtraPadLifoRing(){}

    void ReadData(hid::CTR::ExtraPadStatus* pBuffers, s32 bufferNum,s32 *pReadCount,s64 *pTick,s32 *pIndex)
    {
        NN_TASSERT_(NULL != pBuffers && NULL != pReadCount && NULL != pTick &&  NULL != pIndex);
        
        if(EXTRA_PAD_LIFORING_BUFFER_NUM <= *pIndex)
        {
            NN_TASSERT_(EXTRA_PAD_LIFORING_BUFFER_NUM > *pIndex);
            *pIndex = *pIndex % EXTRA_PAD_LIFORING_BUFFER_NUM;
        }

        if (0 > m_writePointer)
        {
            *pReadCount = 0;
            return;
        }

        if (0 >= bufferNum)
        {
            *pReadCount = 0;
            return;
        }

        if (0 > *pTick)
        {
            *pReadCount = 0 > m_oldTickWriteZero ? m_writePointer + 1 : EXTRA_PAD_LIFORING_BUFFER_NUM;
        }
        else if (m_oldTickWriteZero > *pTick)
        {
            *pReadCount = EXTRA_PAD_LIFORING_BUFFER_NUM;
        }

        else if (m_tickWriteZero > *pTick)
        {
            *pReadCount = m_writePointer + 1 + (0 > m_oldTickWriteZero ? 0 : EXTRA_PAD_LIFORING_BUFFER_NUM - *pIndex -1);
            if (*pReadCount > EXTRA_PAD_LIFORING_BUFFER_NUM)
            {
                *pReadCount = EXTRA_PAD_LIFORING_BUFFER_NUM;
            }
        }
        else
        {
            *pReadCount = m_writePointer - *pIndex;
        }

        NN_ASSERT_(*pReadCount <= EXTRA_PAD_LIFORING_BUFFER_NUM);

        if (bufferNum < *pReadCount)
        {
            *pReadCount = bufferNum;
        }

        if(*pReadCount >= EXTRA_PAD_LIFORING_BUFFER_NUM)
        {
            *pReadCount = EXTRA_PAD_LIFORING_BUFFER_NUM - 1;
        }

        s32 tmpWritePointer = m_writePointer;

        for (s32 i = 0; i < *pReadCount; i ++)
        {
            nn::hidlow::CTR::IExtraPadStatus* buffer = &m_Buffers[(EXTRA_PAD_LIFORING_BUFFER_NUM + tmpWritePointer - i) % EXTRA_PAD_LIFORING_BUFFER_NUM];

            pBuffers[i].hold = buffer->hold;
            pBuffers[i].trigger = buffer->trigger;
            pBuffers[i].release = buffer->release;
            pBuffers[i].batteryLevel= buffer->voltage;
            pBuffers[i].isConnected= buffer->isConnected;
            pBuffers[i].stick.x = buffer->stick.x;
            pBuffers[i].stick.y = buffer->stick.y;
            pBuffers[i].extraStick.x = buffer->extraStick.x;
            pBuffers[i].extraStick.y = buffer->extraStick.y;
        }

        *pTick = m_tickWriteZero;
        *pIndex = tmpWritePointer;

        return;
    }

    IExtraPadStatus m_RawData;
    IExtraPadStatus m_Buffers[EXTRA_PAD_LIFORING_BUFFER_NUM];
};

}
}
}