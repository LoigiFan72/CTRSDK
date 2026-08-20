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
    void GetStickClamp(s16* pMin, s16* pMax) const{
        this->mStickClamper.GetStickClamp(pMin,pMax);
    }
    void SetStickClampMode(StickClampMode mode){
        this->mStickClamper.SetStickClampMode(ClamperClampMode(mode));
    }
    
    f32 NormalizeStick(short x);
    void NormalizeStickWithScale(f32* normalized_x, f32* normalized_y, s16 x, s16 y);
    void SetNormalizeStickScaleSettings(f32 scale, s16 threshold);

    static const s8 MAX_READ_NUM = 7;
    
    static void HideKeyInfo(PadStatus* padStatus){
        padStatus->hold = 0;
        padStatus->release = 0;
        padStatus->trigger = 0;
        padStatus->stick.x = 0;
        padStatus->stick.y = 0;
    }
protected:
    Pad& mPad;
    s32 mIndexOfRead;
    bit32 mLatestHold;
    #if NN_VERSION_MAJOR > 2
        AnalogStickClamper mStickClamper;
    #else
        short mMinOfStickClampCircle;
        short mMinOfStickClampCross;
        short mMinOfStickClampMinimum;
        short mMaxOfStickClampCircle;
        short mMaxOfStickClampCross;
        short mMaxOfStickClampMinimum;
        SizedEnum1<AnalogStickClamper::ClampMode> mStickClampMode;
        s8 rev;
        short mThreshold;
        f32 mScale;
        f32 mStroke;
        f32 mStrokeVelocity;
        f32 mLastLength;
        f32 mLastDiff;
    #endif 
    bool mIsReadLatestFirst;
    s8 rev[3];
    s32 rev2;
    s64 mTickOfRead;

public:
    static AnalogStickClamper::ClampMode  ClamperClampMode(const StickClampMode mode){ return (AnalogStickClamper::ClampMode)mode; }
};


namespace{
    bool sIsEnableSelect;
}

}
}
}