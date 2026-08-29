#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/hid/CTR/hid_AnalogStickClamper.h>
#include <nn/hid/CTR/hid_Api.h>
#include <nn/hid/CTR/hid_Pad.h>
#include <nn/hid/CTR/hid_DeviceStatus.h>
#include <nn/util/util_NonCopyable.h>

namespace nn{
namespace hid{
namespace CTR{

class PadReader : private nn::util::ADLFireWall::NonCopyable<PadReader>{
public:
    typedef enum{
        STICK_CLAMP_MODE_CIRCLE = AnalogStickClamper::STICK_CLAMP_MODE_CIRCLE,
        STICK_CLAMP_MODE_CROSS = AnalogStickClamper::STICK_CLAMP_MODE_CROSS,
        STICK_CLAMP_MODE_MINIMUM = AnalogStickClamper::STICK_CLAMP_MODE_MINIMUM
    } StickClampMode;

    PadReader(Pad& pad=GetPad( ));
    ~PadReader() {};
    void Read(PadStatus* pBufs, s32* pReadLen, s32 bufLen);
    bool ReadLatest(PadStatus* pBuf);

    void SetStickClamp(short min, short max);

    void GetStickClamp(s16* pMin, s16* pMax) const
#ifdef NN_VERSION > 2
    {
        this->m_StickClamper.GetStickClamp(pMin,pMax);
    }
#else
    ;
#endif
    void SetStickClampMode(StickClampMode mode)
#if NN_MAJOR_VERSION > 2
    {
        this->m_StickClamper.SetStickClampMode(ClamperClampMode(mode));
    }
#else
    ;
#endif
    
    f32 NormalizeStick(short x);
    void NormalizeStickWithScale(f32* normalized_x, f32* normalized_y, s16 x, s16 y);
    void SetNormalizeStickScaleSettings(f32 scale, s16 threshold);
#if NN_VERSION_MAJOR <= 2
    void ClampCore(short* pOutX, short* pOutY,  s32 x, s32 y);
    void ClampValueOfClamp();
#endif

    static const s8 MAX_READ_NUM = 7;
    
    static void HideKeyInfo(PadStatus* padStatus){
        padStatus->hold = 0;
        padStatus->release = 0;
        padStatus->trigger = 0;
        padStatus->stick.x = 0;
        padStatus->stick.y = 0;
    }
protected:
    Pad& m_Pad;
    s32 m_IndexOfRead;
    bit32 m_LatestHold;
    #if NN_VERSION_MAJOR > 2
        AnalogStickClamper m_StickClamper;
    #else
        short m_MinOfStickClampCircle;
        short m_MinOfStickClampCross;
        short m_MinOfStickClampMinimum;
        short m_MaxOfStickClampCircle;
        short m_MaxOfStickClampCross;
        short m_MaxOfStickClampMinimum;
        SizedEnum1<AnalogStickClamper::ClampMode> m_StickClampMode;
        s8 rev;
        short m_Threshold;
        f32 m_Scale;
        f32 m_Stroke;
        f32 m_StrokeVelocity;
        f32 m_LastLength;
        f32 m_LastDiff;
    #endif 
    bool m_IsReadLatestFirst;
    s8 rev[3];
    s32 rev2;
    s64 m_TickOfRead;

public:
    static AnalogStickClamper::ClampMode  ClamperClampMode(const StickClampMode mode){ return (AnalogStickClamper::ClampMode)mode; }
};

#if NN_VERSION_MAJOR <= 2

inline void PadReader::ClampCore(short* pOutX, short* pOutY,  s32 x, s32 y){
    switch (this->m_StickClampMode) {
    case STICK_CLAMP_MODE_CIRCLE:
        hidlow::ClampStickCircle(pOutX, pOutY, x, y, this->m_MinOfStickClampCircle, this->m_MaxOfStickClampCircle);
        break;
    case STICK_CLAMP_MODE_CROSS:
        hidlow::ClampStickCross(pOutX, pOutY, x, y, this->m_MinOfStickClampCross, this->m_MaxOfStickClampCross);
        break;
    case STICK_CLAMP_MODE_MINIMUM:
        hidlow::ClampStickMinimum(pOutX, pOutY, x, y, this->m_MinOfStickClampMinimum, this->m_MaxOfStickClampMinimum);
        break;
    }
}

inline void PadReader::ClampValueOfClamp(){
  if (m_MinOfStickClampCircle < MIN_OF_STICK_CLAMP_MODE_CIRCLE)
    m_MinOfStickClampCircle = MIN_OF_STICK_CLAMP_MODE_CIRCLE;
    
  if (m_MinOfStickClampCross < MIN_OF_STICK_CLAMP_MODE_CROSS)
    m_MinOfStickClampCross = MIN_OF_STICK_CLAMP_MODE_CROSS;
    
  if (m_MaxOfStickClampCircle > LIMIT_OF_STICK_CLAMP_MAX)
    m_MaxOfStickClampCircle = LIMIT_OF_STICK_CLAMP_MAX;
    
  if (m_MaxOfStickClampCross > LIMIT_OF_STICK_CLAMP_MAX)
    m_MaxOfStickClampCross = LIMIT_OF_STICK_CLAMP_MAX;
    
  if (m_MaxOfStickClampMinimum > LIMIT_OF_STICK_CLAMP_MAX)
    m_MaxOfStickClampMinimum = LIMIT_OF_STICK_CLAMP_MAX;
}

#endif

}
}
}