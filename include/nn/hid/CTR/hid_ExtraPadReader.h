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
        return this->mExtraStickClamper.NormalizeStick(x);
    }
    void NormalizeExtraStickWithScale(f32* normalized_x, f32* normalized_y, s16 x, s16 y){
        return this->mExtraStickClamper.NormalizeStickWithScale(normalized_x,normalized_y,x,y);
    }

    void SetExtraStickClamp(s16 min, s16 max){
        return this->mExtraStickClamper.SetStickClamp(min,max);
    }
    void GetExtraStickClamp(s16* pMin, s16* pMax) const{
        return this->mExtraStickClamper.GetStickClamp(pMin,pMax);
    }
    void SetExtraStickClampMode(StickClampMode mode){
        return this->mExtraStickClamper.SetStickClampMode(PadReader::ClamperClampMode((PadReader::StickClampMode)mode));
    }
    void SetNormalizeExtraStickScaleSettings(f32 scale, s16 threshold){
        return this->mExtraStickClamper.SetNormalizeStickScaleSettings(scale,threshold);
    }
    void GetNormalizeExtraStickScaleSettings(f32* scale, s16* threshold) const{
        return this->mExtraStickClamper.GetNormalizeStickScaleSettings(scale,threshold);
    }

protected:
    s32 rev;
    AnalogStickClamper mExtraStickClamper;
    AnalogStickClamper mStickClamper;
    s32 mIndexOfRead;
    bit32 mLatestHold;
    PadReader mPadReader;
    bool mIsReadLatestFirst;
    s8 rev2[3];
    s32 rev3;
    s64 mTickOfRead;
};

}
}
}