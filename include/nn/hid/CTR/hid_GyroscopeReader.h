#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/math.h>
#include <nn/hid/CTR/hid_Api.h>
#include <nn/hid/CTR/hid_Gyroscope.h>
#include <nn/hid/CTR/hid_GyroscopeLowStatus.h>
#include <nn/hid/CTR/hid_AccelerometerReader.h>
#include <nn/hid/CTR/hid_DeviceStatus.h>
#include <nn/util/util_SizedEnum.h>
#include <nn/util/util_NonCopyable.h>

namespace nn{
namespace hid{
namespace CTR{

enum ZeroDriftMode{
    GYROSCOPE_ZERODRIFT_LOOSE,
    GYROSCOPE_ZERODRIFT_STANDARD,
    GYROSCOPE_ZERODRIFT_TIGHT,
    GYROSCOPE_ZERODRIFT_NUM,
};

class GyroscopeReader : private nn::util::ADLFireWall::NonCopyable<GyroscopeReader>{
protected:
    static const s32 GYROSCOPE_LOCAL_BUFFER_SIZE = 32;
    static const u32 GYROSCOPE_DRIFT_COUNT_MAX = 256;

    s32 mGyroscopeLocalBufferSize;
    GyroscopeStatus mGyroscopeStatusLocalBuffer[GYROSCOPE_LOCAL_BUFFER_SIZE];
    bool mIsFirstRead;
    util::SizedEnum1<ZeroDriftMode> mZeroDriftMode;
    short rev;
    AccelerometerReader mDefaultAccelerometerReader;
    GyroscopeStatus mCurrentStatus;
    AccelerometerReader* mpAccelerometerReader;
    VEC3 mSpeedOld;
    VEC3 mSpeedVector;
    VEC3 mSpeedScale;
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
    VEC3 mCalibrationZero;
    f64 mCalibrationScale[3];
    VEC3 mCountZero;
    s32 mCountIdx;
    s32 mCountT[3][GYROSCOPE_DRIFT_COUNT_MAX];
    f32 mDpsPitchMagnification;
    f32 mDpsYawMagnification;
    f32 mDpsRollMagnification;
    s32 rev2;
    Gyroscope& mGyroscope;
    s32 mIndexOfRead;
    s64 mTickOfRead;
    MTX34 mRotateMtx;
public:
    GyroscopeReader(AccelerometerReader* pAccelerometerReader = NULL, Gyroscope& gyroscope = CTR::GetGyroscope());
    ~GyroscopeReader();
    void Read(GyroscopeStatus* pBufs, s32* pReadLen, s32 bufLen);
    bool ReadLatest(GyroscopeStatus* pBuf);
    void ReadLocal(GyroscopeStatus* pBufs, s32* pReadLen, s32 bufLen);

    void EnableZeroDrift(); 
    void EnableAccRevise();
    void EnableZeroPlay();

    void DisableAccRevise();
    void DisableAxisRotation();
    void DisableZeroDrift();
    void DisableZeroPlay();

    void CalculateDirection();
    void CalculateGyroscopeAxisStatus(f32 *destSpeed,s32 *nearSamplingNum,f32 *zeroOffset,s32 srcSpeed,f32 speedScale,s32* oldValueArray);
    void InitializeCalibrationData();

    void Reset();
    void ResetAccReviseParam();
    void ResetZeroPlayParam();
    void ResetZeroDriftMode();
    void ResetAxisRotationMatrix();
    f32 ReviseDirection_Acceleration(Direction& rev_dir, const nn::math::VEC3& acc);

    void SetAngle(f32 ax, f32 ay, f32 az);
    void SetDirection(const Direction& direction);
    void SetAxisRotationMatrix(const math::MTX34& axis);
    void SetZeroDriftParam(f32 radius, s32 count, f32 power);
    void SetZeroPlayParam(f32& radius);
    void SetZeroDriftMode(const ZeroDriftMode& mode);
};

}
}
}