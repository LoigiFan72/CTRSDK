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
namespace{
    bool s_IsEnableSelect;
}

PadReader::PadReader(Pad& pad): 
    m_Pad(pad),
#if NN_VERSION_MAJOR > 2
    m_IndexOfRead(-1), 
    m_IsReadLatestFirst(true), 
    m_TickOfRead(-1)
#else
    m_IndexOfRead(-1), 
    m_MinOfStickClampCircle(MIN_OF_STICK_CLAMP_MODE_CIRCLE),
    m_MinOfStickClampCross(MIN_OF_STICK_CLAMP_MODE_CROSS),
    m_MinOfStickClampMinimum(MIN_OF_STICK_CLAMP_MODE_CIRCLE),
    m_MaxOfStickClampCircle(LIMIT_OF_STICK_CLAMP_MAX),
    m_MaxOfStickClampCross(LIMIT_OF_STICK_CLAMP_MAX),
    m_MaxOfStickClampMinimum(LIMIT_OF_STICK_CLAMP_MAX),
    m_StickClampMode(AnalogStickClamper::STICK_CLAMP_MODE_CIRCLE),
    m_IsReadLatestFirst(true),
    m_TickOfRead(-1)
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
    
    this->m_StickClamper.ClampValueOfClamp();
    reinterpret_cast<nn::hidlow::CTR::PadLifoRing*>(mPad.GetResource())->ReadData(pBuf, 1, &readLen, &tick, &index);
    if(0 < readLen)
    {
        this->m_StickClamper.ClampCore(&pBuf->stick.x,&pBuf->stick.y,pBuf->stick.x,pBuf->stick.y);

        if(m_IsReadLatestFirst != false){
            m_LatestHold = pBuf->hold;
            m_IsReadLatestFirst = false;
        }

		pBuf->hold &= ~BUTTON_RESERVED;
        pBuf->trigger = (pBuf->hold ^ m_LatestHold) & ~m_LatestHold;
        pBuf->release = (pBuf->hold ^ m_LatestHold) & m_LatestHold;

        if((applet::CTR::IsInitialized()) && (!applet::CTR::detail::IsActive())){
            this->HideKeyInfo(pBuf);
        }

        m_LatestHold = pBuf->hold;

        if(s_IsEnableSelect == false){
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
    reinterpret_cast<nn::hidlow::CTR::PadLifoRing*>(m_Pad.GetResource())->ReadData(pBuf, 1, &readLen, &tick, &index);
    if(0 < readLen)    
    {
        this->ClampCore(&pBuf->stick.x,&pBuf->stick.y,pBuf->stick.x,pBuf->stick.y);

        if(m_IsReadLatestFirst != false){
            m_LatestHold = pBuf->hold;
            m_IsReadLatestFirst = false;
        }

		pBuf->hold &= ~BUTTON_RESERVED;
        pBuf->trigger = (pBuf->hold ^ m_LatestHold) & ~m_LatestHold;
        pBuf->release = (pBuf->hold ^ m_LatestHold) & m_LatestHold;

        if((applet::CTR::IsInitialized()) && (!applet::CTR::detail::IsActive())){
            this->HideKeyInfo(pBuf);
        }

        m_LatestHold = pBuf->hold;

        if(s_IsEnableSelect == false){
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

    this->m_StickClamper.ClampValueOfClamp();

    reinterpret_cast<nn::hidlow::CTR::PadLifoRing*>(this->m_Pad.GetResource())->ReadData(pBufs, bufLen, pReadLen, &this->m_TickOfRead, &this->m_IndexOfRead);

    if(ExtraPad::IsSampling())
    {
        for(int i = 0; i < *pReadLen; i++){
            this->HideKeyInfo(&pBufs[i]);
        }

        *pReadLen = 0;
        return;
    }

    for(int i = 0; i < *pReadLen; i++){
		pBufs[i].hold    &= ~BUTTON_RESERVED;
		pBufs[i].trigger &= ~BUTTON_RESERVED;
		pBufs[i].release &= ~BUTTON_RESERVED;

        if((applet::CTR::IsInitialized()) && (!applet::CTR::detail::IsActive()))
        {
            this->HideKeyInfo(&pBufs[i]);
        }
        
        if(!sIsEnableSelect)
        {
            hidlow::GatherStartAndSelect(&pBufs[i]);
        }

        this->mStickClamper.ClampCore(&pBufs[i].stick.x, &pBufs[i].stick.y, pBufs[i].stick.x, pBufs[i].stick.y);
    }
#else
    NN_TASSERT_(NULL != pBufs);

    this->ClampValueOfClamp();

    reinterpret_cast<nn::hidlow::CTR::PadLifoRing*>(this->m_Pad.GetResource())->ReadData(pBufs, bufLen, pReadLen, &this->m_TickOfRead, &this->m_IndexOfRead);

    if(ExtraPad::IsSampling())
    {
        for(int i = 0; i < *pReadLen; i++){
            this->HideKeyInfo(&pBufs[i]);
        }

        *pReadLen = 0;
        return;
    }

    for(int i = 0; i < *pReadLen; i++){
		pBufs[i].hold    &= ~BUTTON_RESERVED;
		pBufs[i].trigger &= ~BUTTON_RESERVED;
		pBufs[i].release &= ~BUTTON_RESERVED;

        if((applet::CTR::IsInitialized()) && (!applet::CTR::detail::IsActive()))
        {
            this->HideKeyInfo(&pBufs[i]);
        }
        
        if(!s_IsEnableSelect)
        {
            hidlow::GatherStartAndSelect(&pBufs[i]);
        }

        this->ClampCore(&pBufs[i].stick.x, &pBufs[i].stick.x, pBufs[i].stick.x, pBufs[i].stick.y);
    }
#endif
}

void PadReader::SetStickClamp(short min, short max){
#if NN_VERSION_MAJOR > 2
    return this->m_StickClamper.SetStickClamp(min, max);
#else
    NN_TASSERT_(0 <= min);
    NN_TASSERT_(min < max);
    
    if (LIMIT_OF_STICK_CLAMP_MAX < max){
        max = LIMIT_OF_STICK_CLAMP_MAX;
    }

    if (m_StickClampMode == STICK_CLAMP_MODE_CIRCLE){
        if (min < MIN_OF_STICK_CLAMP_MODE_CIRCLE){
            min = MIN_OF_STICK_CLAMP_MODE_CIRCLE;
        }
        m_MinOfStickClampCircle = min;
        m_MaxOfStickClampCircle = max;
    
    }
    else if (m_StickClampMode == STICK_CLAMP_MODE_CROSS){
        if (min < MIN_OF_STICK_CLAMP_MODE_CROSS){
            min = MIN_OF_STICK_CLAMP_MODE_CROSS;
        }
        m_MinOfStickClampCross = min;
        m_MaxOfStickClampCross = max;
    }
    else{
        m_MaxOfStickClampMinimum = max;
    }
#endif
}

f32 PadReader::NormalizeStick(short x){
#if NN_VERSION_MAJOR > 2
    return this->m_StickClamper.NormalizeStick(pos);
#else
    f32 fx = (f32)x;
    s16 threshold;

    switch (m_StickClampMode){
    case STICK_CLAMP_MODE_CIRCLE:
        threshold = m_MaxOfStickClampCircle - m_MinOfStickClampCircle;
        break;

    case STICK_CLAMP_MODE_CROSS:
        threshold = m_MaxOfStickClampCross - m_MinOfStickClampCross;
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
    //return this->m_StickClamper.NormalizeStickWithScale(normalized_x, normalized_y, x, y);
}

void PadReader::SetNormalizeStickScaleSettings(f32 scale, s16 threshold){
#ifdef NN_VERSION_MAJOR > 2
    return this->m_StickClamper.SetNormalizeStickScaleSettings(scale,threshold);
#else
    if(LIMIT_OF_STICK_CLAMP_MAX < threshold) 
        threshold = LIMIT_OF_STICK_CLAMP_MAX;
    m_Scale = scale;
    m_Threshold = threshold;
#endif
}

}
}
}