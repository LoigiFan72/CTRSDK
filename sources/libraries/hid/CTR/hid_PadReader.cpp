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

PadReader::PadReader(Pad& pad): 
    mPad(pad),
#if NN_VERSION_MAJOR > 2
    mIndexOfRead(-1), 
    mIsReadLatestFirst(true), 
    mTickOfRead(-1)
#else
    mIndexOfRead(-1), 
    mMinOfStickClampCircle(MIN_OF_STICK_CLAMP_MODE_CIRCLE),
    mMinOfStickClampCross(MIN_OF_STICK_CLAMP_MODE_CROSS),
    mMinOfStickClampMinimum(MIN_OF_STICK_CLAMP_MODE_CIRCLE),
    mMaxOfStickClampCircle(LIMIT_OF_STICK_CLAMP_MAX),
    mMaxOfStickClampCross(LIMIT_OF_STICK_CLAMP_MAX),
    mMaxOfStickClampMinimum(LIMIT_OF_STICK_CLAMP_MAX),
    mStickClampMode(AnalogStickClamper::STICK_CLAMP_MODE_CIRCLE),
    mIsReadLatestFirst(true),
    mTickOfRead(-1)
#endif
{

}

bool PadReader::ReadLatest(PadStatus* pBuf)
{
#if NN_VERSION_MAJOR > 2
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
#else
    s64 tick = -1LL;
    s32 index = -1;
    s32 readLen;
    uint newHold;

    if(ExtraPad::IsSampling())
        return false;
    
    this->ClampValueOfClamp();
    reinterpret_cast<nn::hidlow::CTR::PadLifoRing*>(mPad.GetResource())->ReadData(pBuf, 1, &readLen, &tick, &index);
    if(0 < readLen){
        this->ClampCore(&pBuf->stick.x, &pBuf->stick.y, pBuf->stick.x, pBuf->stick.y);
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
#endif
}

void PadReader::Read(PadStatus* pBufs, s32* pReadLen, s32 bufLen){
#if NN_VERSION_MAJOR > 2
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
#else
    NN_TASSERT_(NULL != pBufs);

    this->ClampValueOfClamp();

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
        this->ClampCore(&pBufs[i].stick.x, &pBufs[i].stick.x, pBufs[i].stick.x, pBufs[i].stick.y);
    }
#endif
}

void PadReader::SetStickClamp(short min, short max){
#if NN_VERSION_MAJOR > 2
    return this->mStickClamper.SetStickClamp(min, max);
#else
    NN_TASSERT_(0 <= min);
    NN_TASSERT_(min < max);
    
    if (LIMIT_OF_STICK_CLAMP_MAX < max){
        max = LIMIT_OF_STICK_CLAMP_MAX;
    }

    if (mStickClampMode == STICK_CLAMP_MODE_CIRCLE){
        if (min < MIN_OF_STICK_CLAMP_MODE_CIRCLE){
            min = MIN_OF_STICK_CLAMP_MODE_CIRCLE;
        }
        mMinOfStickClampCircle = min;
        mMaxOfStickClampCircle = max;
    
    }
    else if (mStickClampMode == STICK_CLAMP_MODE_CROSS){
        if (min < MIN_OF_STICK_CLAMP_MODE_CROSS){
            min = MIN_OF_STICK_CLAMP_MODE_CROSS;
        }
        mMinOfStickClampCross = min;
        mMaxOfStickClampCross = max;
    }
    else{
        mMaxOfStickClampMinimum = max;
    }
#endif
}

f32 PadReader::NormalizeStick(short x){
#if NN_VERSION_MAJOR > 2
    return this->mStickClamper.NormalizeStick(pos);
#else
    f32 fx = (f32)x;
    s16 threshold;

    switch (mStickClampMode){
    case STICK_CLAMP_MODE_CIRCLE:
        threshold = mMaxOfStickClampCircle - mMinOfStickClampCircle;
        break;

    case STICK_CLAMP_MODE_CROSS:
        threshold = mMaxOfStickClampCross - mMinOfStickClampCross;
        break;

    case STICK_CLAMP_MODE_MINIMUM:
        threshold = LIMIT_OF_STICK_CLAMP_MAX - MIN_OF_STICK_CLAMP_MODE_CIRCLE;
        break;
    }

    if (0 == x)
        return 0.0f;
    else if(threshold <= x)
        return 1.0f;

    return fx / threshold;
#endif
}

void PadReader::NormalizeStickWithScale(f32* normalized_x, f32* normalized_y, s16 x, s16 y){
    //return this->mStickClamper.NormalizeStickWithScale(normalized_x, normalized_y, x, y);
}

void PadReader::SetNormalizeStickScaleSettings(f32 scale, s16 threshold){
#ifdef NN_VERSION_MAJOR > 2
    return this->mStickClamper.SetNormalizeStickScaleSettings(scale,threshold);
#else
    if(LIMIT_OF_STICK_CLAMP_MAX < threshold) 
        threshold = LIMIT_OF_STICK_CLAMP_MAX;
    mScale = scale;
    mThreshold = threshold;
#endif
}

}
}
}