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
    Accelerometer& mAccelerometer;
    short mPlay;
    short mSensitivity;
    AccelerometerStatus mLatestCalculatedStatus;
    AccelerometerStatus mOffsetAccStatus;
    s32 rev;
    nn::math::MTX34 mRotateMtx;
    bool mEnableOffset;
    bool mEnableRotate;
    short rev2;
    s32 mIndexOfRead;
    s64 mTickOfRead;
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
    void SetOffset(short x, short y, short z);
private:
    void Transform(AccelerometerStatus* status);

};

/* Inlines */

inline void AccelerometerReader::ResetOffset(){ SetOffset(0,0,0); }

inline void AccelerometerReader::SetOffset(short x, short y, short z){
    mOffsetAccStatus.x = x; mOffsetAccStatus.y = y; mOffsetAccStatus.z = z;
}

inline void AccelerometerReader::SetSensitivity(s16 play, s16 sensitivity){
    NN_TASSERT_(0 <= play && MAX_OF_ACCELEROMETER_PLAY >= play && 0 <= sensitivity && MAX_OF_ACCELEROMETER_SENSITIVITY >= sensitivity);
    mPlay = play;
    mSensitivity = sensitivity;
}

inline void AccelerometerReader::GetSensitivity(s16* pPlay, s16* pSensitivity) const{
    *pPlay = mPlay;
    *pSensitivity = mSensitivity;
}

}
}
}