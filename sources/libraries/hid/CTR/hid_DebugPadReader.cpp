// Filename: hid_DebugPadReader.cpp
//
// Project: Horizon

#include <nn/hid/CTR/hid_DebugPadReader.h>
#include <nn/hidlow/hidlow_Utils.h>
#include <nn/hidlow/CTR/hidlow_DebugPadRawStatus.h>
#include <nn/hidlow/CTR/hidlow_DebugPadLifoRing.h>

namespace nn{
namespace hid{
namespace CTR{

hidlow::CTR::DebugPadRawStatus s_DebugPadRawStatus[hidlow::CTR::DEBUGPAD_LIFORING_BUFFER_NUM];

bool DebugPadReader::ReadLatest(DebugPadStatus* pBuf){
    s64 tick = -1LL;
    s32 index = -1;
    s32 readLen;
    s32 min = 0;
    reinterpret_cast<nn::hidlow::CTR::DebugPadLifoRing*>(this->m_DebugPad.GetResource())->ReadData(s_DebugPadRawStatus, 1, &readLen, &tick, &index);
    if(STICK_CLAMP_MODE_CIRCLE_WITH_PLAY == this->m_StickClampMode) 
        min = 15;
    if(0 < readLen){
        if(m_IsReadLatestFirst){
            m_LatestHold = s_DebugPadRawStatus[0].hold;
            m_IsReadLatestFirst = false;
        }
        pBuf->trigger = (s_DebugPadRawStatus[0].hold ^ m_LatestHold) & ~m_LatestHold;
        pBuf->release = (s_DebugPadRawStatus[0].hold ^ m_LatestHold) & m_LatestHold;
        m_LatestHold  = pBuf->hold = s_DebugPadRawStatus[0].hold;
        hidlow::ClampStickCrossFloat(&pBuf->leftStickX, &pBuf->leftStickY,s_DebugPadRawStatus[0].leftStickX << 2, s_DebugPadRawStatus[0].leftStickY << 2, min, 0x4d);
        hidlow::ClampStickCrossFloat(&pBuf->rightStickX, &pBuf->rightStickY,s_DebugPadRawStatus[0].rightStickX << 3, s_DebugPadRawStatus[0].rightStickY << 3, min, 0x4d);
        return true;
    }
    return false;
}

void DebugPadReader::Read(DebugPadStatus* pBufs, s32* pReadLen, s32 bufLen){
    NN_TASSERT_(NULL != pBufs);
    s32 min = 0;

    if(bufLen > hidlow::CTR::DEBUGPAD_LIFORING_BUFFER_NUM){
        bufLen = hidlow::CTR::DEBUGPAD_LIFORING_BUFFER_NUM;
    }

    reinterpret_cast<nn::hidlow::CTR::DebugPadLifoRing*>(this->m_DebugPad.GetResource())->ReadData(s_DebugPadRawStatus, bufLen, pReadLen, &this->m_TickOfRead, &this->m_IndexOfRead);
    if(this->m_StickClampMode == STICK_CLAMP_MODE_CIRCLE_WITH_PLAY) 
        min = 0xf;

    for(int i = 0; i < *pReadLen; i++){
        pBufs[i].hold = s_DebugPadRawStatus[i].hold;
        pBufs[i].release = s_DebugPadRawStatus[i].release;
        pBufs[i].trigger = s_DebugPadRawStatus[i].trigger;
        hidlow::ClampStickCrossFloat(&pBufs[i].leftStickX, &pBufs[i].leftStickY, s_DebugPadRawStatus[i].leftStickX << 2, s_DebugPadRawStatus[i].leftStickY << 2, min, 0x4d);
        hidlow::ClampStickCrossFloat(&pBufs[i].rightStickX, &pBufs[i].rightStickY, s_DebugPadRawStatus[i].rightStickX << 3, s_DebugPadRawStatus[i].rightStickY << 2, min, 0x4d);
    }
}
}
}
}