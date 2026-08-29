// Filename: hid_TouchPanelReader.cpp
//
// Project: Horizon

#include <nn/hid/CTR/hid_TouchPanelReader.h>
#include <nn/hidlow/CTR/hidlow_TouchPanelLifoRing.h>
#include <nn/applet/CTR/applet_Api.h>
#include <nn/applet/CTR/applet_Info.h>
#include <nn/Assert.h>

namespace nn{
namespace hid{
namespace CTR{

bool TouchPanelReader::ReadLatest(TouchPanelStatus *pBuf){
    s32 readLen;
    s64 tick = -1LL;
    s32 index = -1;
    Result res;
    
    NN_TASSERT_(NULL != pBuf);
    reinterpret_cast<nn::hidlow::CTR::TouchPanelLifoRing*>(this->m_TouchPanel.GetResource())->ReadData(pBuf, 1, &readLen, &tick, &index);
    if((applet::CTR::IsInitialized()) && (!applet::CTR::detail::IsActive())){
        pBuf->x = 0;
        pBuf->y = 0;
        pBuf->touch = 0;
    }
    return 0 < readLen;
}

void TouchPanelReader::Read(TouchPanelStatus* pBufs, s32* pReadLen, s32 bufLen){
    NN_TASSERT_(NULL != pBufs);
    reinterpret_cast<nn::hidlow::CTR::TouchPanelLifoRing*>(this->m_TouchPanel.GetResource())->ReadData(pBufs, bufLen, pReadLen, &this->m_TickOfRead, &this->m_IndexOfRead);
    for(int i = 0; i < *pReadLen; i++){
        if((applet::CTR::IsInitialized()) && (!applet::CTR::detail::IsActive())){
            pBufs[i].x = 0;
            pBufs[i].y = 0;
            pBufs[i].touch = 0;
        }
    }
}

}
}
}