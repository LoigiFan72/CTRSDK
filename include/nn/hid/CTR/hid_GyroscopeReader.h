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

    s32 m_GyroscopeLocalBufferSize;
    GyroscopeStatus m_GyroscopeStatusLocalBuffer[GYROSCOPE_LOCAL_BUFFER_SIZE];
    bool m_IsFirstRead;
    util::SizedEnum1<ZeroDriftMode> m_ZeroDriftMode;
    short rev;
    AccelerometerReader m_DefaultAccelerometerReader;
    GyroscopeStatus m_CurrentStatus;
    AccelerometerReader* m_pAccelerometerReader;
    VEC3 m_SpeedOld;
    VEC3 m_SpeedVector;
    VEC3 m_SpeedScale;
    f32 m_DirectionMagnification;
    f32 m_Period;
    f32 m_FreqDegree;
    f32 m_FreqRadian;
    bool m_EnableZeroPlay;
    bool m_EnableZeroDrift;
    bool m_EnableAccRevise;
    bool m_EnableRotate;
    f32 m_ZeroPlayRadius;
    f32 m_ZeroDriftRadius;
    s32 m_ZeroDriftCount;
    f32 m_ZeroDriftPower;
    f32 m_AccRevisePower;
    f32 m_AccReviseRange;
    f32 m_ZeroPlayEffect;
    f32 m_ZeroDriftEffect;
    f32 m_AccRevEffect;
    VEC3 m_CalibrationZero;
    f64 m_CalibrationScale[3];
    VEC3 m_CountZero;
    s32 m_CountIdx;
    s32 m_CountT[3][GYROSCOPE_DRIFT_COUNT_MAX];
    f32 m_DpsPitchMagnification;
    f32 m_DpsYawMagnification;
    f32 m_DpsRollMagnification;
    s32 rev2;
    Gyroscope& m_Gyroscope;
    s32 m_IndexOfRead;
    s64 m_TickOfRead;
    MTX34 m_RotateMtx;
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