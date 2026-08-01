// Filename: hidlow_TouchPanelLifoRing.cpp
//
// Project: Horizon

#include <nn/hidlow/CTR/hidlow_TouchPanelLifoRing.h>
#include <nn/math/math_Utility.h>

namespace nn{
namespace hidlow{
namespace CTR{

void TouchPanelLifoRing::ReadData(nn::hid::CTR::TouchPanelStatus* pBuffers, s32 bufferNum, s32* pReadCount, s64* pTick, s32* pIndex){
    NN_TASSERT_(NULL != pBuffers && NULL != pReadCount && NULL != pTick &&  NULL != pIndex);
    NN_TASSERT_(-1 <= *pIndex && TOUCHPANEL_LIFORING_BUFFER_NUM > *pIndex);
    NN_TASSERT_((*pIndex >= 0) == (*pTick >= 0));

    *pReadCount = 0;

    if(0 >= bufferNum){
        return;
    }

    const s32 maxRead = nn::math::Min(bufferNum, TOUCHPANEL_LIFORING_BUFFER_NUM - 1);

    for(;;){
        const s32 orgWritePointer = mWritePointer;
        s32 writePointer = orgWritePointer;
        const s64 tickWriteZero = mTickWriteZero;
        const s64 oldTickWriteZero = mOldTickWriteZero;

        NN_TASSERT_(-1 <= writePointer && TOUCHPANEL_LIFORING_BUFFER_NUM > writePointer);

        if (writePointer > 0){}

        else if (writePointer == 0){
            if (tickWriteZero == oldTickWriteZero){

                if (tickWriteZero < 0){
                    return;
                }

                writePointer = TOUCHPANEL_LIFORING_BUFFER_NUM - 1;
            }
        }

        else{
            return;
        }

        NN_TASSERT_(*pTick <= tickWriteZero);

        s32 readCount = 0;

        if (tickWriteZero <= *pTick){
            readCount = writePointer - *pIndex;
            NN_TASSERT_(0 <= readCount);
        }

        else if (oldTickWriteZero < 0){
            readCount = writePointer + 1;
        }

        else if (oldTickWriteZero <= *pTick){
            readCount = writePointer + TOUCHPANEL_LIFORING_BUFFER_NUM - *pIndex;
        }

        else{
            readCount = maxRead;
        }

        if (maxRead < readCount){
            readCount = maxRead;
        }

        for (s32 i = 0; i < readCount; i++){
            s32 srcIdx = (TOUCHPANEL_LIFORING_BUFFER_NUM + writePointer - i)%TOUCHPANEL_LIFORING_BUFFER_NUM;
            pBuffers[i].x = mBuffers[srcIdx].x;
            pBuffers[i].y = mBuffers[srcIdx].y;
            pBuffers[i].touch = mBuffers[srcIdx].touch;
        }

        {
            const s32 checkWritePointer = mWritePointer;
            const s64 checkTickWriteZero = mTickWriteZero;
            const s64 checkOldTickWriteZero = mOldTickWriteZero;

            if (false || (checkWritePointer != orgWritePointer) || (checkTickWriteZero != tickWriteZero)|| (checkOldTickWriteZero != oldTickWriteZero)|| false){
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