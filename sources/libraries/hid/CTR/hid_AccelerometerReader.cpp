// Filename: hid_AccelerometerReader.cpp
//
// Project: Horizon

#include <nn/hid/CTR/hid_AccelerometerReader.h>
#include <nn/hid/CTR/hid_IpcClient.h>
#include <nn/hidlow/CTR/hidlow_AccelerometerLifoRing.h>
#include <nn/hidlow/hidlow_Utils.h>
#include <nn/Assert.h>

#include <nn/types.h>
#include <string.h>

namespace nn{
namespace hid{
namespace CTR{
namespace detail{

short CalculateAccelerationTightly(short targetValue, short currentValue, short playRadius, short sensitivity){
    s32 diff = targetValue - currentValue;

    if (diff < -playRadius) {
        s32 delta = diff + playRadius;
        return (short)(currentValue + ((delta * sensitivity) >> 7));
    }

    if (diff <= playRadius) {
        return currentValue;
    }

    s32 delta = diff - playRadius;
    return (short)(currentValue + ((delta * sensitivity) >> 7));
}

}

AccelerometerReader::AccelerometerReader(Accelerometer& accelerometer): 
    m_Accelerometer(accelerometer),
    m_Play(0),
    m_Sensitivity(MAX_OF_ACCELEROMETER_SENSITIVITY),
    m_EnableOffset(false),
    m_EnableRotate(false),
    m_IndexOfRead(-1),
    m_TickOfRead(-1LL){
    AccelerometerStatus tempStatus;
    s32 tempLen;

    detail::Ipc::EnableAccelerometer();
    m_LatestCalculatedStatus.x = 0;
    m_LatestCalculatedStatus.y = 0;
    m_LatestCalculatedStatus.z = 0;
    this->ResetOffset();
    this->ResetAxisRotationMatrix();
    this->DisableOffset();
    this->DisableAxisRotation();
    this->Read(&tempStatus, &tempLen, 1);
}

AccelerometerReader::~AccelerometerReader(){
    detail::Ipc::DisableAccelerometer();
}

void AccelerometerReader::ConvertToAcceleration(AccelerationFloat* pAcceleration, s32 bufLen, AccelerometerStatus* pSamplingData, s32 samplingLen){
    NN_TASSERT_(NULL != pAcceleration);
    NN_TASSERT_(NULL != pSamplingData);
    NN_TASSERT_(bufLen<=samplingLen);
    for(int i = 0; i < bufLen; i++){
        pAcceleration[i].x = pSamplingData[i].x * 0.001953125;
        pAcceleration[i].y = pSamplingData[i].y * 0.001953125;
        pAcceleration[i].z = pSamplingData[i].z * 0.001953125;
    }
}


void AccelerometerReader::Read(AccelerometerStatus* pBufs, s32* pReadLen, s32 bufLen){
    Accelerometer& accelerometer = m_Accelerometer;
    reinterpret_cast<nn::hidlow::CTR::AccelerometerLifoRing*>(this->m_Accelerometer.GetResource())->ReadData(pBufs, bufLen, pReadLen, &this->m_TickOfRead, &this->m_IndexOfRead);

    s16 (*calculateAccelerationFunc)(s16 targetValue, s16 currentValue, s16 playRadius, s16 sensitivity);
    calculateAccelerationFunc = detail::CalculateAccelerationTightly;
        
    for(int i =*pReadLen - 1; i >= 0; --i){
        m_LatestCalculatedStatus.x = pBufs[i].x = calculateAccelerationFunc(pBufs[i].x, m_LatestCalculatedStatus.x, m_Play, m_Sensitivity);
        m_LatestCalculatedStatus.y = pBufs[i].y = calculateAccelerationFunc(pBufs[i].y, m_LatestCalculatedStatus.y, m_Play, m_Sensitivity);
        m_LatestCalculatedStatus.z = pBufs[i].z = calculateAccelerationFunc(pBufs[i].z, m_LatestCalculatedStatus.z, m_Play, m_Sensitivity);

        this->Transform(&pBufs[i]);
    }
}

bool AccelerometerReader::ReadLatest(AccelerometerStatus* pBuf){
    s64 tick = -1LL;
    s32 index = -1;
    s32 readLen;

    reinterpret_cast<nn::hidlow::CTR::AccelerometerLifoRing*>(this->m_Accelerometer.GetResource())->ReadData(pBuf, 1, &readLen, &tick, &index);

    if (readLen <= 0) {
        return false;
    }

    s16 (*calculateAccelerationFunc)(s16 targetValue, s16 currentValue, s16 playRadius, s16 sensitivity);
    calculateAccelerationFunc = detail::CalculateAccelerationTightly;

    m_LatestCalculatedStatus.x = pBuf->x = calculateAccelerationFunc(pBuf->x, m_LatestCalculatedStatus.x, m_Play, m_Sensitivity);
    m_LatestCalculatedStatus.y = pBuf->y = calculateAccelerationFunc(pBuf->y, m_LatestCalculatedStatus.y, m_Play, m_Sensitivity);
    m_LatestCalculatedStatus.z = pBuf->z = calculateAccelerationFunc(pBuf->z, m_LatestCalculatedStatus.z, m_Play, m_Sensitivity);

    this->Transform(pBuf);
    return true;
}

NN_NOINLINE void AccelerometerReader::ResetAxisRotationMatrix(){
    this->SetAxisRotationMatrix(math::MTX34::Identity());
}

void AccelerometerReader::Transform(AccelerometerStatus* pAcclStatus){
    if(mEnableOffset != false){
        pAcclStatus->x = pAcclStatus->x - mOffsetAccStatus.x;
        pAcclStatus->y = pAcclStatus->y - mOffsetAccStatus.y;
        pAcclStatus->z = pAcclStatus->z - mOffsetAccStatus.z;
    }
    
    if ((mEnableRotate != false && !this->m_RotateMtx.IsIdentity())) {
        nn::math::VEC3 vec(pAcclStatus->x,pAcclStatus->y,pAcclStatus->z);

        math::VEC3Transform(&vec,&this->m_RotateMtx,&vec);
        pAcclStatus->x = vec.x;
        pAcclStatus->y = vec.y;
        pAcclStatus->z = vec.z;
    }
}

void AccelerometerReader::DisableAxisRotation(){
    m_EnableRotate = false;
}

void AccelerometerReader::DisableOffset(){
    m_EnableOffset = false;
}

void AccelerometerReader::SetAxisRotationMatrix(const nn::math::MTX34& mtx){
    m_RotateMtx = mtx;
}

}
}
}