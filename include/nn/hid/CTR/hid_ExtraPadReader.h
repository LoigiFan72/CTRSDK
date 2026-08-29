#pragma once

#include <nn/hid/CTR/hid_ExtraPad.h>
#include <nn/hid/CTR/hid_ExtraPadStatus.h>
#include <nn/hid/CTR/hid_AnalogStickClamper.h>
#include <nn/hid/CTR/hid_PadReader.h>

namespace nn{
namespace hid{
namespace CTR{

class ExtraPadReader{
public:

    typedef enum{
        STICK_CLAMP_MODE_CIRCLE = AnalogStickClamper::STICK_CLAMP_MODE_CIRCLE,
        STICK_CLAMP_MODE_CROSS = AnalogStickClamper::STICK_CLAMP_MODE_CROSS,
        STICK_CLAMP_MODE_MINIMUM = AnalogStickClamper::STICK_CLAMP_MODE_MINIMUM
    }StickClampMode;

    ExtraPadReader();
    virtual ~ExtraPadReader();

    bool ReadLatest(ExtraPadStatus* pBuf);
    void Read(ExtraPadStatus* pBufs, s32* pReadLen, s32 bufLen);

    f32 NormalizeStick(s16 x);
    void NormalizeStickWithScale(f32* normalized_x, f32* normalized_y, s16 x, s16);
    void SetNormalizeStickScaleSettings(f32 scale, s16 threshold);
    void GetNormalizeStickScaleSettings(f32* scale, s16* threshold) const;

    f32 NormalizeExtraStick(s16 x){
        return this->m_ExtraStickClamper.NormalizeStick(x);
    }
    void NormalizeExtraStickWithScale(f32* normalized_x, f32* normalized_y, s16 x, s16 y){
        return this->m_ExtraStickClamper.NormalizeStickWithScale(normalized_x,normalized_y,x,y);
    }

    void SetExtraStickClamp(s16 min, s16 max){
        return this->m_ExtraStickClamper.SetStickClamp(min,max);
    }
    void GetExtraStickClamp(s16* pMin, s16* pMax) const{
        return this->m_ExtraStickClamper.GetStickClamp(pMin,pMax);
    }
    void SetExtraStickClampMode(StickClampMode mode){
        return this->m_ExtraStickClamper.SetStickClampMode(PadReader::ClamperClampMode((PadReader::StickClampMode)mode));
    }
    void SetNormalizeExtraStickScaleSettings(f32 scale, s16 threshold){
        return this->m_ExtraStickClamper.SetNormalizeStickScaleSettings(scale,threshold);
    }
    void GetNormalizeExtraStickScaleSettings(f32* scale, s16* threshold) const{
        return this->m_ExtraStickClamper.GetNormalizeStickScaleSettings(scale,threshold);
    }

protected:
    s32 rev;
    AnalogStickClamper m_ExtraStickClamper;
    AnalogStickClamper m_StickClamper;
    s32 m_IndexOfRead;
    bit32 m_LatestHold;
    PadReader m_PadReader;
    bool m_IsReadLatestFirst;
    s8 rev2[3];
    s32 rev3;
    s64 m_TickOfRead;
};

}
}
}