#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/hid/CTR/hid_Accelerometer.h>
#include <nn/hid/CTR/hid_DeviceStatus.h>
#include <nn/util/util_SizedEnum.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/math/math_Matrix34.h>
#include <nn/hid/CTR/hid_Api.h>

const s16  MAX_OF_ACCELEROMETER_VALUE = 930;
const s16  ACCELEROMETER_VALUE_PER_1G = 512;
const f32  ACCELERATION_SCALE_PER_1G = 1.f / ACCELEROMETER_VALUE_PER_1G;
const s16  MAX_OF_ACCELEROMETER_PLAY = MAX_OF_ACCELEROMETER_VALUE * 2;
const s16  ACCELEROMETER_SENSITIVITY_SCALE_SHIFT = 7;
const s16  MAX_OF_ACCELEROMETER_SENSITIVITY = 128;

namespace nn{
namespace hid{
namespace CTR{

class AccelerometerReader : private nn::util::ADLFireWall::NonCopyable<AccelerometerReader>{
protected:
    Accelerometer& m_Accelerometer;
    s16 m_Play;
    s16 m_Sensitivity;
    AccelerometerStatus m_LatestCalculatedStatus;
    AccelerometerStatus m_OffsetAccStatus;
    s32 rev;
    MTX34 m_RotateMtx;
    bool m_EnableOffset;
    bool m_EnableRotate;
    short rev2;
    s32 m_IndexOfRead;
    s64 m_TickOfRead;
public:

    AccelerometerReader(Accelerometer& accelerometer = CTR::GetAccelerometer());
    ~AccelerometerReader();
    void ConvertToAcceleration(AccelerationFloat* pAcceleration, s32 bufLen, AccelerometerStatus* pSamplingData, s32 samplingLen = -1);
    void Read(AccelerometerStatus* status, s32* pReadLen, s32 bufLen);
    bool ReadLatest(AccelerometerStatus* status);
    void ResetAxisRotationMatrix();
    void DisableOffset();
    void DisableAxisRotation();
    void SetAxisRotationMatrix(const nn::math::MTX34& mtx);

    void GetSensitivity(s16* pPlay, s16* pSensitivity) const;
    void SetSensitivity(s16 play, s16 sensitivity);

    void ResetOffset();
    void SetOffset(s16 x, s16 y, s16 z);
private:
    void Transform(AccelerometerStatus* status);

};

/* Inlines */

inline void AccelerometerReader::ResetOffset(){ 
    SetOffset(0, 0, 0); 
}

inline void AccelerometerReader::SetOffset(s16 x, s16 y, s16 z){
    m_OffsetAccStatus.x = x; m_OffsetAccStatus.y = y; m_OffsetAccStatus.z = z;
}

inline void AccelerometerReader::SetSensitivity(s16 play, s16 sensitivity){
    NN_TASSERT_(0 <= play && MAX_OF_ACCELEROMETER_PLAY >= play && 0 <= sensitivity && MAX_OF_ACCELEROMETER_SENSITIVITY >= sensitivity);
    m_Play = play;
    m_Sensitivity = sensitivity;
}

inline void AccelerometerReader::GetSensitivity(s16* pPlay, s16* pSensitivity) const{
    *pPlay = m_Play;
    *pSensitivity = m_Sensitivity;
}

}
}
}