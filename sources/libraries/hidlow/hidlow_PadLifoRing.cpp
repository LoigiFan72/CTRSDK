// Filename: hidlow_PadLifoRing.cpp
//
// Project: Horizon

#include <nn/hidlow/CTR/hidlow_PadLifoRing.h>
#include <nn/math/math_Utility.h>

namespace nn{
namespace hidlow{
namespace CTR{

void PadLifoRing::ReadData(hid::CTR::PadStatus* pBuffers, s32 bufferNum,s32* pReadCount, s64* pTick, s32* pIndex)
{
    NN_TASSERT_(NULL != pBuffers && NULL != pReadCount && NULL != pTick &&  NULL != pIndex);
    NN_TASSERT_(-1 <= *pIndex && PAD_LIFORING_BUFFER_NUM > *pIndex);
    NN_TASSERT_((*pIndex >= 0) == (*pTick >= 0));

    *pReadCount = 0;
    if (0 >= bufferNum)
    {
        return;
    }

    const s32 maxRead = nn::math::Min(bufferNum, PAD_LIFORING_BUFFER_NUM - 1);

    for(;;)
    {
        const s32 orgWritePointer = m_writePointer;
        s32 writePointer = orgWritePointer;
        const s64 tickWriteZero = m_tickWriteZero;
        const s64 oldTickWriteZero = m_oldTickWriteZero;

        NN_TASSERT_(-1 <= writePointer && PAD_LIFORING_BUFFER_NUM > writePointer);

        if (writePointer > 0)
        {

        }
        else if (writePointer == 0)
        {
            if (tickWriteZero == oldTickWriteZero)
            {
                if (tickWriteZero < 0)
                {
                    return;
                }
                writePointer = PAD_LIFORING_BUFFER_NUM - 1;
            }
        }

        else{
            return;
        }

        NN_TASSERT_(*pTick <= tickWriteZero);

        s32 readCount = 0;
        if (tickWriteZero <= *pTick)
        {
            readCount = writePointer - *pIndex;
            NN_TASSERT_(0 <= readCount);
        }

        else if (oldTickWriteZero < 0)
        {
            readCount = writePointer + 1;
        }
        else if (oldTickWriteZero <= *pTick)
        {
            readCount = writePointer + PAD_LIFORING_BUFFER_NUM - *pIndex;
        }
        else{
            readCount = maxRead;
        }
        if (maxRead < readCount)
        {
            readCount = maxRead;
        }

        for (s32 i = 0; i < readCount; i++)
        {
            s32 srcIdx = (PAD_LIFORING_BUFFER_NUM + writePointer - i)%PAD_LIFORING_BUFFER_NUM;
            pBuffers[i].hold    = m_Buffers[srcIdx].hold;
            pBuffers[i].trigger = m_Buffers[srcIdx].trigger;
            pBuffers[i].release = m_Buffers[srcIdx].release;
            pBuffers[i].stick.x = m_Buffers[srcIdx].stick.x;
            pBuffers[i].stick.y = m_Buffers[srcIdx].stick.y;
        }


        {
            const s32 checkWritePointer = m_writePointer;
            const s64 checkTickWriteZero = m_tickWriteZero;
            const s64 checkOldTickWriteZero = m_oldTickWriteZero;
            if (false || (checkWritePointer != orgWritePointer) || (checkTickWriteZero != tickWriteZero) || (checkOldTickWriteZero != oldTickWriteZero)|| false)
            {
                continue;
            }

        }

        *pReadCount = readCount;
        *pTick = tickWriteZero;
        *pIndex = writePointer;

        break;
    }

    return;
}

}
}
}