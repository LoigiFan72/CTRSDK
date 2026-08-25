// Filename: hid_ExtraPadReader.cpp
//
// Project: Horizon

#include <nn/hid/CTR/hid_ExtraPadReader.h>
#include <nn/hidlow/CTR/hidlow_ExtraPadLifoRing.h>
#include <nn/hidlow/hidlow_Utils.h>
#include <nn/applet/CTR/applet_Api.h>
#include <nn/applet/CTR/applet_Info.h>

#if NN_VERSION_MAJOR > 2

namespace nn{
namespace hid{
namespace CTR{

ExtraPadReader::ExtraPadReader():
    mIndexOfRead(-1),
    mIsReadLatestFirst(true),
    mTickOfRead(-1)
{}

ExtraPadReader::~ExtraPadReader(){ }

bool ExtraPadReader::ReadLatest(ExtraPadStatus* pBuf){
    s64 tick = -1LL;
    s32 index = -1;
    s32 readLen;
    PadStatus padStatus;

    if(ExtraPad::IsSampling()){
        this->mExtraStickClamper.ClampValueOfClamp();
        this->mStickClamper.ClampValueOfClamp();
        hidlow::CTR::ExtraPadLifoRing* ring = (hidlow::CTR::ExtraPadLifoRing*)ExtraPad::GetResource();
        ring->ReadData(pBuf,1,&readLen,&tick,&index);
        
        if(this->mIsReadLatestFirst){
            this->mLatestHold = pBuf->hold;
            this->mIsReadLatestFirst = false;
        }

        pBuf->hold &= ~0x2000u;
        pBuf->trigger = (pBuf->hold ^ this->mLatestHold) & ~this->mLatestHold;
        pBuf->release = (pBuf->hold ^ this->mLatestHold) &  this->mLatestHold;
        if((applet::CTR::IsInitialized()) && (!applet::CTR::detail::IsActive())){
            pBuf->hold = 0;
            pBuf->trigger = 0;
            pBuf->release = 0;
            pBuf->extraStick.x = 0;
            pBuf->extraStick.y = 0;
        }
        this->mLatestHold = pBuf->hold;
        if(sIsEnableSelect == false)
            hidlow::GatherStartAndSelect(pBuf);
        this->mExtraStickClamper.ClampCore(&pBuf->extraStick.x, &pBuf->extraStick.y, pBuf->extraStick.x, pBuf->extraStick.y);
        this->mStickClamper.ClampCore(&pBuf->stick.x, &pBuf->stick.y,pBuf->stick.x, pBuf->stick.y);

        return readLen > 0;
    }
    else{
        if(this->mPadReader.ReadLatest(&padStatus)){
            pBuf->stick.x = padStatus.stick.x;
            pBuf->stick.y = padStatus.stick.y;
            pBuf->hold = padStatus.hold;
            pBuf->trigger = (pBuf->hold ^ this->mLatestHold) & ~this->mLatestHold;
            pBuf->release = (pBuf->hold ^ this->mLatestHold) & this->mLatestHold;
            pBuf->isConnected = false;
            pBuf->batteryLevel = 0;
            pBuf->extraStick.x = 0;
            pBuf->extraStick.y = 0;

            this->mLatestHold = pBuf->hold;
            return true;
        }
        else
            return false;
    }
}

void ExtraPadReader::Read(ExtraPadStatus* pBufs, s32* pReadLen, s32 bufLen){
    NN_TASSERT_(NULL != pBufs);
    PadStatus padStatus[PadReader::MAX_READ_NUM];
    s32 padLen = 0;

    this->mPadReader.Read(padStatus,&padLen, bufLen);
    hidlow::CTR::ExtraPadLifoRing* ring = (hidlow::CTR::ExtraPadLifoRing*)ExtraPad::GetResource();
    ring->ReadData(pBufs,bufLen,pReadLen,&this->mTickOfRead,&this->mIndexOfRead);
    if(ExtraPad::IsSampling()){
        this->mExtraStickClamper.ClampValueOfClamp();
        this->mStickClamper.ClampValueOfClamp();
        for (s32 i = 0; i < *pReadLen; i++){
            pBufs[i].hold &= ~0x2000u;
            pBufs[i].trigger &= ~0x2000u;
            pBufs[i].release &= ~0x2000u;
            if (nn::applet::CTR::IsInitialized() && !applet::CTR::detail::IsActive()){
                pBufs[i].hold = 0;
                pBufs[i].trigger = 0;
                pBufs[i].release = 0;
                pBufs[i].stick.x = 0;
                pBufs[i].stick.y = 0;
                pBufs[i].extraStick.x = 0;
                pBufs[i].extraStick.y = 0;
            }
            if (!sIsEnableSelect) nn::hidlow::GatherStartAndSelect(&pBufs[i]);

            this->mExtraStickClamper.ClampCore(&pBufs[i].extraStick.x, &pBufs[i].extraStick.y,pBufs[i].extraStick.x, pBufs[i].extraStick.y);
            this->mStickClamper.ClampCore(&pBufs[i].stick.x, &pBufs[i].stick.y, pBufs[i].stick.x, pBufs[i].stick.y);
        }
    }
    else{
        for (s32 i = 0; i < padLen; i++){
            pBufs[i].stick.x = padStatus[i].stick.x;
            pBufs[i].stick.y = padStatus[i].stick.y;
            pBufs[i].hold = padStatus[i].hold;
            pBufs[i].trigger = padStatus[i].trigger;
            pBufs[i].release = padStatus[i].release;
            pBufs[i].isConnected = false;
            pBufs[i].batteryLevel = 0;
            pBufs[i].extraStick.x = 0;
            pBufs[i].extraStick.y = 0;
        }
        *pReadLen = padLen;
    }
}

f32 ExtraPadReader::NormalizeStick(s16 x){
    return this->mStickClamper.NormalizeStick(x);
}

void ExtraPadReader::NormalizeStickWithScale(f32* normalized_x, f32* normalized_y, s16 x, s16 y){
    //return this->mStickClamper.NormalizeStickWithScale(normalized_x,normalized_y,x,y);
}

void ExtraPadReader::SetNormalizeStickScaleSettings(f32 scale, s16 threshold){
    this->mStickClamper.SetNormalizeStickScaleSettings(scale,threshold);
    this->mPadReader.SetNormalizeStickScaleSettings(scale,threshold);
}

void ExtraPadReader::GetNormalizeStickScaleSettings(f32* scale, s16* threshold) const{
    return this->mStickClamper.GetNormalizeStickScaleSettings(scale,threshold);
}

}
}
}

#endif