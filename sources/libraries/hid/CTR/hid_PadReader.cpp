// Filename: hid_PadReader.cpp
//
// Project: Horizon

#include <nn/hid/CTR/hid_PadReader.h>
#include <nn/hid/CTR/hid_ExtraPad.h>
#include <nn/hidlow/CTR/hidlow_PadLifoRing.h>
#include <nn/hidlow/hidlow_Utils.h>
#include <nn/applet/CTR/applet_Api.h>
#include <nn/applet/CTR/applet_Info.h>

#include <nn/dbg/dbg_Break.h>

namespace nn{
namespace hid{
namespace CTR{

PadReader::PadReader(Pad& pad) : 
    mPad(pad), 
    mIndexOfRead(-1), 
    mIsReadLatestFirst(true), 
    mTickOfRead(-1){
}

bool PadReader::ReadLatest(PadStatus* pBuf){
    s64 tick = -1LL;
    s32 index = -1;
    s32 readLen;
    uint newHold;

    if(ExtraPad::IsSampling())
        return false;
    
    this->mStickClamper.ClampValueOfClamp();
    reinterpret_cast<nn::hidlow::CTR::PadLifoRing*>(mPad.GetResource())->ReadData(pBuf, 1, &readLen, &tick, &index);
    if(0 < readLen){
        this->mStickClamper.ClampCore(&pBuf->stick.x,&pBuf->stick.y,pBuf->stick.x,pBuf->stick.y);
        if(this->mIsReadLatestFirst != false){
            this->mLatestHold = pBuf->hold;
            this->mIsReadLatestFirst = false;
        }
        newHold = pBuf->hold & 0xffffdfff;
        pBuf->hold = newHold;
        uint mLatestHold = this->mLatestHold;
        pBuf->trigger = (newHold ^ mLatestHold) & ~mLatestHold;
        pBuf->release = this->mLatestHold & ~newHold;
        if((applet::CTR::IsInitialized()) && (!applet::CTR::detail::IsActive())){
            this->HideKeyInfo(pBuf);
        }
        this->mLatestHold = pBuf->hold;
        if(sIsEnableSelect == false){
            hidlow::GatherStartAndSelect(pBuf);
        }
        return true;
    }
    return false;
}

void PadReader::Read(PadStatus* pBufs, s32* pReadLen, s32 bufLen){
    NN_TASSERT_(NULL != pBufs);
    this->mStickClamper.ClampValueOfClamp();
    reinterpret_cast<nn::hidlow::CTR::PadLifoRing*>(this->mPad.GetResource())->ReadData(pBufs, bufLen, pReadLen, &this->mTickOfRead, &this->mIndexOfRead);

    if(ExtraPad::IsSampling()){
        for(int i = 0; i < *pReadLen; i++){
            this->HideKeyInfo(&pBufs[i]);
        }
        *pReadLen = 0;
        return;
    }
    for(int i = 0; i < *pReadLen; i++){
        pBufs[i].hold &= 0xFFFFDFFF;
        pBufs[i].trigger &= 0xFFFFDFFF;
        pBufs[i].release &= 0xFFFFDFFF;
        if((applet::CTR::IsInitialized()) && (!applet::CTR::detail::IsActive())){
            this->HideKeyInfo(&pBufs[i]);
        }
        
        if(!sIsEnableSelect){
               hidlow::GatherStartAndSelect(&pBufs[i]);
        }
        this->mStickClamper.ClampCore(&pBufs[i].stick.x, &pBufs[i].stick.y, pBufs[i].stick.x, pBufs[i].stick.y);
    }
}

void PadReader::SetStickClamp(short min, short max){
    return this->mStickClamper.SetStickClamp(min,max);
}

f32 PadReader::NormalizeStick(short pos){
    return this->mStickClamper.NormalizeStick(pos);
}

void PadReader::NormalizeStickWithScale(f32* normalized_x, f32* normalized_y, s16 x, s16 y){
    //return this->mStickClamper.NormalizeStickWithScale(normalized_x, normalized_y, x, y);
}

void PadReader::SetNormalizeStickScaleSettings(f32 scale, s16 threshold){
    return this->mStickClamper.SetNormalizeStickScaleSettings(scale,threshold);
}

}
}
}