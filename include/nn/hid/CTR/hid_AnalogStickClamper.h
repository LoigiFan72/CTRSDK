#pragma once

#include <nn/types.h>
#include <nn/Result.h>
#include <nn/util/util_SizedEnum.h>

using namespace nn::util;

namespace nn{
namespace hid{
namespace CTR{

    const f32 DEFAULT_SCALE_OF_NORMALIZE_STICK = 1.5f;
    const s16 DEFAULT_THRESHOLD_OF_NORMALIZE_STICK = 141;
    const s16 MIN_OF_STICK_CLAMP_MODE_CIRCLE = 40;
    const s16 MIN_OF_STICK_CLAMP_MODE_CROSS = 36;
    const s16 LIMIT_OF_STICK_CLAMP_MAX = 145;

class AnalogStickClamper{
public:
enum ClampMode{
    STICK_CLAMP_MODE_CIRCLE = 0,
    STICK_CLAMP_MODE_CROSS,
    STICK_CLAMP_MODE_MINIMUM
};
protected:
    short m_MinOfStickClampCircle;
    short m_MinOfStickClampCross;
    short m_MinOfStickClampMinimum;
    short m_MaxOfStickClampCircle;
    short m_MaxOfStickClampCross;
    short m_MaxOfStickClampMinimum;
    SizedEnum1<ClampMode> m_StickClampMode;
    s8 rev;
    short m_Threshold;
    f32 m_Scale;
    f32 m_Stroke;
    f32 m_StrokeVelocity;
    f32 m_LastLength;
    f32 m_LastDiff;
public:
    AnalogStickClamper();
    ~AnalogStickClamper(){ }
    void ClampCore(short* pOutX, short* pOutY,  s32 x, s32 y);
    void ClampValueOfClamp();

    f32 NormalizeStick(short stick);
    void NormalizeStickWithScale(f32* normalized_x, f32* normalized_y, s16 x, s16 y);

    void GetStickClamp(s16* pMin, s16* pMax) const;
    ClampMode GetStickClampMode() const;
    void GetNormalizeStickScaleSettings(f32* scale, s16* threshold ) const{}

    void SetStickClamp(short min, short max);
    void SetStickClampMode(ClampMode mode);
    void SetStickClampFree(s16 min, s16 max);
    void SetNormalizeStickScaleSettings(f32 scale, s16 threshold);
};

inline void AnalogStickClamper::GetStickClamp(s16* pMin, s16* pMax) const{
    if (this->m_StickClampMode == STICK_CLAMP_MODE_CIRCLE){
       *pMin = m_MinOfStickClampCircle;
       *pMax = m_MaxOfStickClampCircle;
    }
    else if (this->m_StickClampMode == STICK_CLAMP_MODE_CROSS){
       *pMin = m_MinOfStickClampCross;
       *pMax = m_MaxOfStickClampCross;
    }
    else{
        *pMin = m_MinOfStickClampMinimum;
        *pMax = m_MaxOfStickClampMinimum;
    }
}

inline AnalogStickClamper::ClampMode AnalogStickClamper::GetStickClampMode() const{
    return m_StickClampMode;
}

inline void AnalogStickClamper::SetStickClampMode(ClampMode mode){
    m_StickClampMode = mode;
}

}
}
}