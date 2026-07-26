#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/math/math_Matrix34.h>
#include <nn/hid/CTR/hid_Api.h>
#include <nn/hid/CTR/hid_Gyroscope.h>
#include <nn/hid/CTR/hid_AccelerometerReader.h>
#include <nn/hid/CTR/hid_DeviceStatus.h>
#include <nn/util/util_SizedEnum.h>
#include <nn/util/util_NonCopyable.h>

namespace nn{
namespace hid{
namespace CTR{

enum ZeroDriftMode{
    GYROSCOPE_ZERODRIFT_LOOSE = 0,
    GYROSCOPE_ZERODRIFT_STANDARD = 1,
    GYROSCOPE_ZERODRIFT_TIGHT = 2,
    GYROSCOPE_ZERODRIFT_NUM = 3,
};

struct GyroscopeLowCalibrateAxisParam{
    short rpm0;
    short rpmPositive;
    short rpmNegative;
};

struct GyroscopeLowCalibrateParam{
    GyroscopeLowCalibrateAxisParam x;
    GyroscopeLowCalibrateAxisParam y;
    GyroscopeLowCalibrateAxisParam z;
};

class GyroscopeReader : private nn::util::ADLFireWall::NonCopyable<GyroscopeReader>{
protected:
    static const s32 GYROSCOPE_LOCAL_BUFFER_SIZE = 32;

    s32 mGyroscopeLocalBufferSize;
    GyroscopeStatus mGyroscopeStatusLocalBuffer[GYROSCOPE_LOCAL_BUFFER_SIZE];
    bool mIsFirstRead;
    s8 mZeroDriftMode;
    short rev;
    AccelerometerReader mDefaultAccelerometerReader;
    GyroscopeStatus mCurrentStatus;
    AccelerometerReader* mpAccelerometerReader;
    nn::math::VEC3 mSpeedOld;
    nn::math::VEC3 mSpeedVector;
    nn::math::VEC3 mSpeedScale;
    f32 mDirectionMagnification;
    f32 mPeriod;
    f32 mFreqDegree;
    f32 mFreqRadian;
    bool mEnableZeroPlay;
    bool mEnableZeroDrift;
    bool mEnableAccRevise;
    bool mEnableRotate;
    f32 mZeroPlayRadius;
    f32 mZeroDriftRadius;
    s32 mZeroDriftCount;
    f32 mZeroDriftPower;
    f32 mAccRevisePower;
    f32 mAccReviseRange;
    f32 mZeroPlayEffect;
    f32 mZeroDriftEffect;
    f32 mAccRevEffect;
    nn::math::VEC3 mCalibrationZero;
    f64 mCalibrationScale[3];
    nn::math::VEC3 mCountZero;
    s32 mCountIdx;
    s32 mCountT[3];
    f32 mDpsPitchMagnification;
    f32 mDpsYawMagnification;
    f32 mDpsRollMagnification;
    s32 rev2;
    Gyroscope& mGyroscope;
    s32 mIndexOfRead;
    s64 mTickOfRead;
    nn::math::MTX34 mRotateMtx;
public:
    GyroscopeReader(AccelerometerReader* pAccelerometerReader = NULL, Gyroscope& gyroscope = CTR::GetGyroscope());
    ~GyroscopeReader();
    void Read(GyroscopeStatus* pBufs, s32* pReadLen, s32 bufLen);
    bool ReadLatest(GyroscopeStatus* pBuf);

    void Reset();

    void EnableZeroDrift(); 
    void EnableAccRevise();
    void EnableZeroPlay();

    void DisableAccRevise();
    void DisableZeroDrift();
    
    void DisableZeroPlay();

    void CalculateDirection();
    void InitializeCalibrationData();

    void ResetZeroDriftMode();
    void ResetAxisRotationMatrix();
    f32 ReviseDirection_Acceleration(Direction& rev_dir, const nn::math::VEC3& acc);

    void SetAxisRotationMatrix(const math::MTX34& axis);

    // Zero Modes
    void SetZeroPlayParam(f32& radius);
    void SetZeroDriftMode(const ZeroDriftMode& mode);
};

}
}
}