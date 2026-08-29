// Filename: hid_GyroscopeReader.cpp
//
// Project: Horizon

#include <nn/hid/CTR/hid_GyroscopeReader.h>
#include <nn/hid/CTR/hid_IpcClient.h>
#include <nn/hidlow.h>
#include <nn/math/inline/math_Vector3.ipp>
#include <nn/module.h>

#include <cstring>

namespace nn{
namespace hid{
namespace CTR{
namespace{
    const s32 SAMPLING_FREQUENCY = 100;


    const f32 INIT_ZERO_PLAY_RADIUS = 0.005f;
    const f32 INIT_ACC_REVISE_PW = 0.030f;
    const f32 INIT_ACC_REVISE_RANGE = 0.400f;

    const f32 INIT_ZERO_DRIFT_RADIUS_TIGHT = 0.005f;
    const s32 INIT_ZERO_DRIFT_CT_TIGHT = 100;
    const f32 INIT_ZERO_DRIFT_PW_TIGHT = 0.010f;

    const f32 INIT_ZERO_DRIFT_RADIUS_STANDARD = 0.010f;
    const s32 INIT_ZERO_DRIFT_CT_STANDARD = 100;
    const f32 INIT_ZERO_DRIFT_PW_STANDARD = 0.020f;

    const f32 INIT_ZERO_DRIFT_RADIUS_LOOSE = 0.020f;
    const s32 INIT_ZERO_DRIFT_CT_LOOSE = 100;
    const f32 INIT_ZERO_DRIFT_PW_LOOSE = 0.040f;

    const nn::math::VEC3 ZERO_VEC = nn::math::VEC3::Zero();
    const nn::hid::CTR::Direction E_DIR = nn::math::MTX33::Identity();

    nn::os::Tick s_TickOfStart = nn::os::Tick(0);
    s32 s_NumOfInstance = 0;
    const s32 MARGIN_OF_STARTING_SAMPLING = 150;

    NN_MAKE_MODULE(s_DetectableString, "NINTENDO", "Gyroscope");
}

static inline void OrthonormalizeDirection(Direction& dir, f32 threshold){
    f32 f1, e1;
    Direction vec;

    do{
        dir.x = dir.x.Normalize();
        dir.y = dir.y.Normalize();
        dir.z = dir.z.Normalize();

        vec.x.x = dir.y.y * dir.z.z - dir.y.z * dir.z.y;
        vec.x.y = dir.y.z * dir.z.x - dir.y.x * dir.z.z;
        vec.x.z = dir.y.x * dir.z.y - dir.y.y * dir.z.x;
        vec.y.x = dir.z.y * dir.x.z - dir.z.z * dir.x.y;
        vec.y.y = dir.z.z * dir.x.x - dir.z.x * dir.x.z;
        vec.y.z = dir.z.x * dir.x.y - dir.z.y * dir.x.x;
        vec.z.x = dir.x.y * dir.y.z - dir.x.z * dir.y.y;
        vec.z.y = dir.x.z * dir.y.x - dir.x.x * dir.y.z;
        vec.z.z = dir.x.x * dir.y.y - dir.x.y * dir.y.x;

        f1 = 1.0f / (e1 = vec.x.Length());
        dir.x.x = (f1 * vec.x.x + dir.x.x) * 0.5f;
        dir.x.y = (f1 * vec.x.y + dir.x.y) * 0.5f;
        dir.x.z = (f1 * vec.x.z + dir.x.z) * 0.5f;
        e1 += (f1 = vec.y.Length());
        f1 = 1.0f / f1;
        dir.y.x = (f1 * vec.y.x + dir.y.x) * 0.5f;
        dir.y.y = (f1 * vec.y.y + dir.y.y) * 0.5f;
        dir.y.z = (f1 * vec.y.z + dir.y.z) * 0.5f;
        e1 += (f1 = vec.z.Length());
        f1 = 1.0f / f1;
        dir.z.x = (f1 * vec.z.x + dir.z.x) * 0.5f;
        dir.z.y = (f1 * vec.z.y + dir.z.y) * 0.5f;
        dir.z.z = (f1 * vec.z.z + dir.z.z) * 0.5f;
    } while (e1 < threshold);
}

static inline void MakeVectorDirection(Direction& mtx, const nn::math::VEC3& vec1, const nn::math::VEC3& vec2){
    nn::math::VEC3 dX, dY1, dY2;
    f32 f1, f2;
    f32 z2y1, x2z1, y2x1, y2z1, z2x1, x2y1;

    dX.x = (z2y1 = vec1.y * vec2.z) - (y2z1 = vec1.z * vec2.y);
    dX.y = (x2z1 = vec1.z * vec2.x) - (z2x1 = vec1.x * vec2.z);
    dX.z = (y2x1 = vec1.x * vec2.y) - (x2y1 = vec1.y * vec2.x);
    if (dX.Length() == 0.0f){
        mtx = E_DIR;
        return;
    }
    dX = dX.Normalize();

    dY1.x = vec1.y * dX.z - vec1.z * dX.y;
    dY1.y = vec1.z * dX.x - vec1.x * dX.z;
    dY1.z = vec1.x * dX.y - vec1.y * dX.x;
    dY2.x = vec2.y * dX.z - vec2.z * dX.y;
    dY2.y = vec2.z * dX.x - vec2.x * dX.z;
    dY2.z = vec2.x * dX.y - vec2.y * dX.x;

    mtx.x.x = dX.x * dX.x + dY2.x * dY1.x + vec2.x * vec1.x;
    mtx.x.y = (f1 = dX.y * dX.x) + dY2.y * dY1.x + y2x1;
    mtx.x.z = (f2 = dX.z * dX.x) + dY2.z * dY1.x + z2x1;

    mtx.y.x = f1 + dY2.x * dY1.y + x2y1;
    mtx.y.y = dX.y * dX.y + dY2.y * dY1.y + vec2.y * vec1.y;
    mtx.y.z = (f1 = dX.z * dX.y) + dY2.z * dY1.y + z2y1;

    mtx.z.x = f2 + dY2.x * dY1.z + x2z1;
    mtx.z.y = f1 + dY2.y * dY1.z + y2z1;
    mtx.z.z = dX.z * dX.z + dY2.z * dY1.z + vec2.z * vec1.z;
}

static inline void MultDirection(const Direction& m1, const Direction& m2, Direction& m3){

    m3.x.x = m2.x.x * m1.x.x + m2.x.y * m1.y.x + m2.x.z * m1.z.x;
    m3.x.y = m2.x.x * m1.x.y + m2.x.y * m1.y.y + m2.x.z * m1.z.y;
    m3.x.z = m2.x.x * m1.x.z + m2.x.y * m1.y.z + m2.x.z * m1.z.z;

    m3.y.x = m2.y.x * m1.x.x + m2.y.y * m1.y.x + m2.y.z * m1.z.x;
    m3.y.y = m2.y.x * m1.x.y + m2.y.y * m1.y.y + m2.y.z * m1.z.y;
    m3.y.z = m2.y.x * m1.x.z + m2.y.y * m1.y.z + m2.y.z * m1.z.z;

    m3.z.x = m2.z.x * m1.x.x + m2.z.y * m1.y.x + m2.z.z * m1.z.x;
    m3.z.y = m2.z.x * m1.x.y + m2.z.y * m1.y.y + m2.z.z * m1.z.y;
    m3.z.z = m2.z.x * m1.x.z + m2.z.y * m1.y.z + m2.z.z * m1.z.z;
}

static inline s32 F32toi(f32 f){
    if (f < 0.0f){
        return ((s32) (f - 0.5f));
    }
    else{
        return ((s32) (f + 0.5f));
    }
}

static inline s32 F64toi(f64 f){
    if (f < 0.0){
        return ((s32) (f - 0.5));
    }
    else{
        return ((s32) (f + 0.5));
    }
}

static inline s32 CalibrateLowData(s32 lowValue, f32 zero, f64 scale, f32 mag){
    return F64toi((static_cast<f64> (lowValue) - zero) * scale * mag);
}

static void ReadLow(GyroscopeLowStatus* pBufs, s32* pReadLen, s32 bufLen, Gyroscope& gyroscope, s32* pIndexOfRead, s64* pTickOfRead){
    reinterpret_cast<nn::hidlow::CTR::GyroscopeLowLifoRing*> (gyroscope.GetResource())->ReadData(pBufs, bufLen, pReadLen, pTickOfRead, pIndexOfRead);
}

static f32 GetRawToDpsCoefficient(){
    f32 coefficient;
    NN_UTIL_PANIC_IF_FAILED(detail::Ipc::GetGyroscopeLowRawToDpsCoefficient(&coefficient));
    return coefficient;
}

static void GetCalibrateParam(GyroscopeLowCalibrateScaleParam* pParam){
    GyroscopeLowCalibrateParam lowParam;

    NN_UTIL_PANIC_IF_FAILED(detail::Ipc::GetGyroscopeLowCalibrateParam(&lowParam));

    pParam->x.rpm0 = lowParam.x.rpm0;
    pParam->x.scale = static_cast<f64> (lowParam.x.rpmPositive - lowParam.x.rpmNegative) / (6 * 156);
    pParam->y.rpm0 = lowParam.y.rpm0;
    pParam->y.scale = static_cast<f64> (lowParam.y.rpmPositive - lowParam.y.rpmNegative) / (6 * 156);
    pParam->z.rpm0 = lowParam.z.rpm0;
    pParam->z.scale = static_cast<f64> (lowParam.z.rpmPositive - lowParam.z.rpmNegative) / (6 * 156);
}

GyroscopeReader::GyroscopeReader(AccelerometerReader* pAccelerometerReader,Gyroscope& gyroscope) :   
    m_IsFirstRead(true),
    m_DefaultAccelerometerReader(),
    m_Gyroscope(gyroscope),
    m_IndexOfRead(-1),
    m_TickOfRead(-1){
    NN_REFER_MODULE(s_DetectableString);
    detail::Ipc::EnableGyroscopeLow();
    if (pAccelerometerReader){
        m_pAccelerometerReader = pAccelerometerReader;
    }
    else{
        m_pAccelerometerReader = &m_DefaultAccelerometerReader;
    }

    this->Reset();
    s32 len = 0;

    if(s_NumOfInstance == 0){
        s_TickOfStart = nn::os::Tick::GetSystemCurrent() + nn::os::Tick(nn::fnd::TimeSpan::FromMilliSeconds(MARGIN_OF_STARTING_SAMPLING));
    }
    else{
        this->Read(&this->m_CurrentStatus,&len,1);
    }
    s_NumOfInstance++;
}

GyroscopeReader::~GyroscopeReader(){
    detail::Ipc::DisableGyroscopeLow();
    s_NumOfInstance--;
}

bool GyroscopeReader::ReadLatest(GyroscopeStatus* pBuf){
    if(m_GyroscopeLocalBufferSize == 0){
        this->ReadLocal(m_GyroscopeStatusLocalBuffer,&m_GyroscopeLocalBufferSize,GYROSCOPE_LOCAL_BUFFER_SIZE);
    }
    else{
        s32 currentReadLen = 0;
        GyroscopeStatus currentLocalBuff[GYROSCOPE_LOCAL_BUFFER_SIZE];
        this->ReadLocal(currentLocalBuff,&currentReadLen,GYROSCOPE_LOCAL_BUFFER_SIZE);

        m_GyroscopeLocalBufferSize = m_GyroscopeLocalBufferSize < GYROSCOPE_LOCAL_BUFFER_SIZE - currentReadLen ? m_GyroscopeLocalBufferSize: GYROSCOPE_LOCAL_BUFFER_SIZE - currentReadLen;

        std::memcpy(this->m_GyroscopeStatusLocalBuffer + currentReadLen,this->m_GyroscopeStatusLocalBuffer,sizeof(GyroscopeStatus) * this->m_GyroscopeLocalBufferSize);

        std::memcpy(this->m_GyroscopeStatusLocalBuffer,currentLocalBuff,sizeof(GyroscopeStatus) * currentReadLen);

        m_GyroscopeLocalBufferSize += currentReadLen;
    }

    if(m_TickOfRead == -1){
        return false;
    }
    else{
        *pBuf = m_CurrentStatus;
        return true;
    }

}

void GyroscopeReader::ReadLocal(GyroscopeStatus* pBufs, s32* pReadLen, s32 bufLen){
    GyroscopeLowStatus lowStatus[GYROSCOPE_LOCAL_BUFFER_SIZE];

    s32 lowReadableSize = GYROSCOPE_LOCAL_BUFFER_SIZE;

    s32 lowReadLen = 0;

    if(m_IsFirstRead){
        s64 pastMilliSecond = (nn::os::Tick::GetSystemCurrent() - s_TickOfStart).ToTimeSpan().GetMilliSeconds();
        s32 pastReadableSize = (pastMilliSecond / SAMPLING_FREQUENCY) + 1;
        if(pastMilliSecond < 0 || pastReadableSize <= 0){
            *pReadLen = 0;
            return;
        }
        else{
            if(lowReadableSize > pastReadableSize){
                lowReadableSize = pastReadableSize;
            }
            m_IsFirstRead = false;
        }
    }

    ReadLow(lowStatus, &lowReadLen, lowReadableSize, m_Gyroscope, &this->m_IndexOfRead, &this->m_TickOfRead);

    {
        s32 nearSamplingNumX, nearSamplingNumY, nearSamplingNumZ;
        s32 gyroscopeLowData;
        f32 f1;

        if (lowReadLen <= 0){
            *pReadLen = 0;
            if(bufLen > 0){
                pBufs[0] = m_CurrentStatus;
            }
            return;
        }

        m_ZeroPlayEffect = 1.0f;
        m_ZeroDriftEffect = 1.0f;

        s32 idx = lowReadLen - 1;
        do{
            const nn::hid::CTR::GyroscopeLowStatus& gyro_status = lowStatus[idx];

            m_SpeedOld = m_CurrentStatus.speed;
            m_CountIdx = (m_CountIdx + 1) & (GYROSCOPE_DRIFT_COUNT_MAX - 1);

            gyroscopeLowData = CalibrateLowData(gyro_status.x, m_CalibrationZero.x, m_CalibrationScale[0],m_DpsPitchMagnification);
            CalculateGyroscopeAxisStatus(&m_CurrentStatus.speed.x, &nearSamplingNumX, &m_CountZero.x, gyroscopeLowData, m_SpeedScale.x, m_CountT[0]);

            gyroscopeLowData = CalibrateLowData(gyro_status.y, m_CalibrationZero.y, m_CalibrationScale[1],m_DpsYawMagnification);
            CalculateGyroscopeAxisStatus(&m_CurrentStatus.speed.y, &nearSamplingNumY, &m_CountZero.y, gyroscopeLowData, m_SpeedScale.y, m_CountT[1]);

            gyroscopeLowData = CalibrateLowData(gyro_status.z, m_CalibrationZero.z, m_CalibrationScale[2],m_DpsRollMagnification);
            CalculateGyroscopeAxisStatus(&m_CurrentStatus.speed.z, &nearSamplingNumZ, &m_CountZero.z, gyroscopeLowData, m_SpeedScale.z, m_CountT[2]);

            if(m_EnableRotate && !m_RotateMtx.IsIdentity()){
                VEC3Transform(&m_CurrentStatus.speed,&m_RotateMtx,&m_CurrentStatus.speed);
            }

            m_SpeedVector.x = m_CurrentStatus.speed.x - m_SpeedOld.x;
            m_SpeedVector.y = m_CurrentStatus.speed.y - m_SpeedOld.y;
            m_SpeedVector.z = m_CurrentStatus.speed.z - m_SpeedOld.z;

            if (nearSamplingNumX < nearSamplingNumY){
                if (nearSamplingNumX < nearSamplingNumZ){
                    nearSamplingNumZ = nearSamplingNumX;
                }
            }
            else{
                if (nearSamplingNumY < nearSamplingNumZ){
                    nearSamplingNumZ = nearSamplingNumY;
                }
            }
            f1 = static_cast<f32> (nearSamplingNumZ - 1) / static_cast<f32> (m_ZeroDriftCount - 1);
            if (f1 < m_ZeroDriftEffect){
                m_ZeroDriftEffect = f1;
            }

            m_CurrentStatus.angle.x += m_Period * m_CurrentStatus.speed.x;
            m_CurrentStatus.angle.y += m_Period * m_CurrentStatus.speed.y;
            m_CurrentStatus.angle.z += m_Period * m_CurrentStatus.speed.z;

            this->CalculateDirection();

            if (idx < bufLen){
                pBufs[idx] = m_CurrentStatus;
            }

        } while (idx--);
    }

    if(lowReadLen > bufLen){
        *pReadLen = bufLen;
    }
    else{
        *pReadLen = lowReadLen;
    }

    AccelerometerStatus accStatus;
    if (m_EnableAccRevise && this->m_pAccelerometerReader->ReadLatest(&accStatus)){
        AccelerationFloat acceleration;

        this->m_pAccelerometerReader->ConvertToAcceleration(&acceleration, 1, &accStatus);
        nn::math::VEC3 acc(acceleration.x, acceleration.y, acceleration.z);

        m_AccRevEffect = this->ReviseDirection_Acceleration(m_CurrentStatus.direction, acc);

        if(bufLen > 0){
            pBufs[0] = m_CurrentStatus;
        }
    }
    else{
        m_AccRevEffect = 0.0f;
    }
}

void GyroscopeReader::Reset(){
    m_EnableZeroPlay = false;
    m_EnableZeroDrift = true;
    m_EnableAccRevise = true;

    this->ResetZeroPlayParam();
    this->ResetZeroDriftMode();
    this->ResetAccReviseParam();

    this->ResetAxisRotationMatrix();
    this->DisableAxisRotation();

    this->SetAngle(0.0f, 0.0f, 0.0f);
    this->SetDirection(E_DIR);

    m_GyroscopeLocalBufferSize = 0;

    m_CurrentStatus.speed = m_SpeedOld = ZERO_VEC;
    m_CountZero = ZERO_VEC;

    m_ZeroPlayEffect = m_ZeroDriftEffect = m_AccRevEffect = 0.0f;
    m_DirectionMagnification = 1.0f;
    m_DpsPitchMagnification = m_DpsYawMagnification = m_DpsRollMagnification = 1.0f;

    s32 i = GYROSCOPE_DRIFT_COUNT_MAX - 1;
    do{
        m_CountT[0][i] = m_CountT[1][i] = m_CountT[2][i] = 0;
    } while (--i >= 0);
    m_CountIdx = 0;

    const f64 period = 1.0 / SAMPLING_FREQUENCY;
    m_Period = (f32) period;
    m_FreqDegree = (f32) (period * 360.0);
    m_FreqRadian = (f32) (period * 6.283185307179586);

    this->InitializeCalibrationData();
}

void GyroscopeReader::EnableZeroDrift(){
    m_EnableZeroDrift = true;
}

void GyroscopeReader::EnableAccRevise(){
    m_EnableAccRevise = true;
}

void GyroscopeReader::EnableZeroPlay(){
    m_EnableZeroPlay = true;
}

void GyroscopeReader::DisableAccRevise(){
    m_EnableAccRevise = false;
}

void GyroscopeReader::DisableZeroDrift(){
    m_EnableZeroDrift = false;
}

void GyroscopeReader::DisableZeroPlay(){
    m_EnableZeroPlay = false;
}

void GyroscopeReader::SetAngle(f32 ax, f32 ay, f32 az){
    m_CurrentStatus.angle.x = ax;
    m_CurrentStatus.angle.y = ay;
    m_CurrentStatus.angle.z = az;
}

void GyroscopeReader::SetDirection(const Direction& dir){
    m_CurrentStatus.direction = dir;
}

void GyroscopeReader::SetAxisRotationMatrix(const nn::math::MTX34& mtx){
    this->m_RotateMtx = mtx;
    this->m_DefaultAccelerometerReader.SetAxisRotationMatrix(mtx);
}

void GyroscopeReader::SetZeroPlayParam(f32& radius){
    m_ZeroPlayRadius = 0.005;
}

void GyroscopeReader::SetZeroDriftMode(const ZeroDriftMode& mode){
    NN_TASSERT_(mode == GYROSCOPE_ZERODRIFT_LOOSE || mode == GYROSCOPE_ZERODRIFT_STANDARD || mode == GYROSCOPE_ZERODRIFT_TIGHT);
    m_ZeroDriftMode = mode;
    switch (mode){
    case GYROSCOPE_ZERODRIFT_LOOSE:
        SetZeroDriftParam(INIT_ZERO_DRIFT_RADIUS_LOOSE, INIT_ZERO_DRIFT_CT_LOOSE, INIT_ZERO_DRIFT_PW_LOOSE);
        break;
    case GYROSCOPE_ZERODRIFT_STANDARD:
        SetZeroDriftParam(INIT_ZERO_DRIFT_RADIUS_STANDARD, INIT_ZERO_DRIFT_CT_STANDARD, INIT_ZERO_DRIFT_PW_STANDARD);
        break;
    case GYROSCOPE_ZERODRIFT_TIGHT:
        SetZeroDriftParam(INIT_ZERO_DRIFT_RADIUS_TIGHT, INIT_ZERO_DRIFT_CT_TIGHT, INIT_ZERO_DRIFT_PW_TIGHT);
        break;
    }
}

void GyroscopeReader::SetZeroDriftParam(f32 radius, s32 count, f32 power){
    m_ZeroDriftRadius = radius;
    m_ZeroDriftCount = count;
    m_ZeroDriftPower = power;

    if (m_ZeroDriftCount < 2){
        m_ZeroDriftCount = 2;
    }
    else if (m_ZeroDriftCount > GYROSCOPE_DRIFT_COUNT_MAX){
        m_ZeroDriftCount = GYROSCOPE_DRIFT_COUNT_MAX;
    }
}

void GyroscopeReader::ResetZeroPlayParam(){
    m_ZeroPlayRadius = INIT_ZERO_PLAY_RADIUS;
}

void GyroscopeReader::ResetZeroDriftMode(){
    this->SetZeroDriftMode(GYROSCOPE_ZERODRIFT_STANDARD);
}

void GyroscopeReader::ResetAccReviseParam(){
    m_AccRevisePower = INIT_ACC_REVISE_PW;
    m_AccReviseRange = INIT_ACC_REVISE_RANGE;
}

void GyroscopeReader::ResetAxisRotationMatrix(){
    this->SetAxisRotationMatrix(nn::math::MTX34::Identity());
    this->m_DefaultAccelerometerReader.ResetAxisRotationMatrix();
}

f32 GyroscopeReader::ReviseDirection_Acceleration(Direction& reviseDirection, const nn::math::VEC3& acc){
    nn::math::VEC3 v1, v2, vec;
    Direction d1, d2;
    f32 f1, level;

    f1 = acc.Length();
    if (f1 == 0.0f){
        return 0.0f;

    }
    else if (f1 < 1.0f){
        if (f1 <= (1.0f - m_AccReviseRange)){
            return 0.0f;
        }
        else{
            level = (f1 - (1.0f - m_AccReviseRange)) * (1.0f / m_AccReviseRange);
        }
    }
    else{
        if (f1 >= (1.0f + m_AccReviseRange)){
            return 0.0f;
        }
        else{
            level = (f1 - (1.0f + m_AccReviseRange)) * (-1.0f / m_AccReviseRange);
        }
    }
    level *= level;
    level *= m_AccRevisePower;

    f1 = 1.0f / f1;
    vec.x = f1 * acc.x;
    vec.y = f1 * acc.y;
    vec.z = f1 * acc.z;

    v1.x = vec.x * reviseDirection.x.x + vec.y * reviseDirection.y.x + vec.z * reviseDirection.z.x;
    v1.y = vec.x * reviseDirection.x.y + vec.y * reviseDirection.y.y + vec.z * reviseDirection.z.y;
    v1.z = vec.x * reviseDirection.x.z + vec.y * reviseDirection.y.z + vec.z * reviseDirection.z.z;

    v2.x = (0.0f - v1.x) * level + v1.x;
    v2.y = (-1.0f - v1.y) * level + v1.y;
    v2.z = (0.0f - v1.z) * level + v1.z;

    nn::math::VEC3SafeNormalize(&v2, v2, ZERO_VEC);
    if (v2 == ZERO_VEC){
        return 0.0f;
    }

    MakeVectorDirection(d1, v1, v2);
    MultDirection(d1, reviseDirection, d2);
    reviseDirection = d2;

    OrthonormalizeDirection(reviseDirection, 2.999f);

    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    f1 = v1.Length();

    return f1;
}

void GyroscopeReader::CalculateDirection(){
    f32 f1, v1;

    Direction d1 = m_CurrentStatus.direction;
    Direction& dest = m_CurrentStatus.direction;

    v1 = m_DirectionMagnification * m_CurrentStatus.speed.x;
    f1 = m_FreqDegree * v1;
    f1 = f1 * f1 * 0.0001f + 1.0f;
    f1 *= m_FreqRadian * v1;

    dest.y.x += f1 * d1.z.x;
    dest.y.y += f1 * d1.z.y;
    dest.y.z += f1 * d1.z.z;

    dest.z.x -= f1 * d1.y.x;
    dest.z.y -= f1 * d1.y.y;
    dest.z.z -= f1 * d1.y.z;

    v1 = m_DirectionMagnification * m_CurrentStatus.speed.y;
    f1 = m_FreqDegree * v1;
    f1 = f1 * f1 * 0.0001f + 1.0f;
    f1 *= m_FreqRadian * v1;

    dest.z.x += f1 * d1.x.x;
    dest.z.y += f1 * d1.x.y;
    dest.z.z += f1 * d1.x.z;

    dest.x.x -= f1 * d1.z.x;
    dest.x.y -= f1 * d1.z.y;
    dest.x.z -= f1 * d1.z.z;

    v1 = m_DirectionMagnification * m_CurrentStatus.speed.z;
    f1 = m_FreqDegree * v1;
    f1 = f1 * f1 * 0.0001f + 1.0f;
    f1 *= m_FreqRadian * v1;

    dest.x.x += f1 * d1.y.x;
    dest.x.y += f1 * d1.y.y;
    dest.x.z += f1 * d1.y.z;

    dest.y.x -= f1 * d1.x.x;
    dest.y.y -= f1 * d1.x.y;
    dest.y.z -= f1 * d1.x.z;

    OrthonormalizeDirection(dest, 2.999f);
}
void GyroscopeReader::CalculateGyroscopeAxisStatus(f32 *destSpeed, s32 *nearSamplingNum, f32 *zeroOffset, s32 srcSpeed, f32 speedScale, s32* oldValueArray){
    oldValueArray[m_CountIdx] = srcSpeed;
    *nearSamplingNum = 1;

    s32 zeroDriftRange = F32toi(m_ZeroDriftRadius / speedScale);
    if (zeroDriftRange == 0){
        zeroDriftRange++;
    }

    {
        *destSpeed = static_cast<f32> (srcSpeed);

        s32 sum = srcSpeed;
        s32 n1 = srcSpeed - zeroDriftRange;
        s32 n2 = srcSpeed + zeroDriftRange;
        s32 i = (m_CountIdx - 1) & (GYROSCOPE_DRIFT_COUNT_MAX - 1);
        s32 i2 = (m_CountIdx - m_ZeroDriftCount) & (GYROSCOPE_DRIFT_COUNT_MAX - 1);

        do{
            if (oldValueArray[i] < n1 || oldValueArray[i] > n2)
                break;
            sum += oldValueArray[i];
            ++(*nearSamplingNum);
            i = (i - 1) & (GYROSCOPE_DRIFT_COUNT_MAX - 1);
        } while (i != i2);

        f32 f1 = static_cast<f32> (*nearSamplingNum - 1) / static_cast<f32> (m_ZeroDriftCount - 1);

        f1 *= f1;
        f1 *= f1;
        f1 *= f1;
        f1 *= f1;
        f1 *= f1;

        *destSpeed += (static_cast<f32> (sum) / static_cast<f32> (*nearSamplingNum) - *destSpeed) * f1;

        if (m_EnableZeroDrift){
            f1 *= m_ZeroDriftPower;
            *zeroOffset += (*destSpeed - *zeroOffset) * f1;
        }

        *destSpeed = (*destSpeed - *zeroOffset) * speedScale;
        if (m_EnableZeroPlay){
            if (*destSpeed >= -m_ZeroPlayRadius && *destSpeed <= m_ZeroPlayRadius){
                if (*destSpeed < 0.0f)
                    f1 = -(*destSpeed);
                else
                    f1 = *destSpeed;
                f1 = 1.0f - f1 / m_ZeroPlayRadius;
                if (f1 < m_ZeroPlayEffect)
                    m_ZeroPlayEffect = f1;
                *destSpeed = 0.0f;
            }
            else{
                m_ZeroPlayEffect = 0.0f;
            }
        }
    }
}

void GyroscopeReader::InitializeCalibrationData(){
    GyroscopeLowCalibrateScaleParam param;
    GetCalibrateParam(&param);
    f64 coef = static_cast<f64> (GetRawToDpsCoefficient());

    m_SpeedScale.x = m_SpeedScale.y = m_SpeedScale.z = (f32) (1.0 / (coef * 360.0));

    m_CalibrationScale[0] = coef / param.x.scale;
    m_CalibrationScale[1] = coef / param.y.scale;
    m_CalibrationScale[2] = coef / param.z.scale;

    m_CalibrationZero.x = param.x.rpm0;
    m_CalibrationZero.y = param.y.rpm0;
    m_CalibrationZero.z = param.z.rpm0;
}

}
}
}