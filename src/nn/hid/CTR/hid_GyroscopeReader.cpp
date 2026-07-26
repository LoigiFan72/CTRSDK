// Filename: hid_GyroscopeReader.cpp
//
// Project: Horizon

#include <nn/hid/CTR/hid_GyroscopeReader.h>
#include <nn/hid/CTR/hid_IpcClient.h>
#include <nn/math.h>
#include <nn/math/math_Matrix33.h>
#include <nn/module.h>

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

    nn::os::Tick sTickOfStart = nn::os::Tick(0);
    s32 sNumOfInstance = 0;
    const s32 MARGIN_OF_STARTING_SAMPLING = 150;

    NN_MAKE_MODULE(sDetectableString, "NINTENDO", "Gyroscope");
}

GyroscopeReader::GyroscopeReader(AccelerometerReader* pAccelerometerReader,Gyroscope& gyroscope) :   
    mIsFirstRead(true),
    mDefaultAccelerometerReader(),
    mGyroscope(gyroscope),
    mIndexOfRead(-1),
    mTickOfRead(-1){
    NN_REFER_MODULE(sDetectableString);
    detail::Ipc::EnableGyroscopeLow();
    if (pAccelerometerReader){
        mpAccelerometerReader = pAccelerometerReader;
    }
    else{
        mpAccelerometerReader = &mDefaultAccelerometerReader;
    }

    this->Reset();
    s32 len = 0;

    if(sNumOfInstance == 0){
        //sTickOfStart = nn::os::Tick::GetSystemCurrent() + nn::os::Tick(nn::fnd::TimeSpan::FromMilliSeconds(MARGIN_OF_STARTING_SAMPLING));
    }
    else{
        this->Read(&this->mCurrentStatus,&len,1);
    }
    sNumOfInstance++;
}

GyroscopeReader::~GyroscopeReader(){
    detail::Ipc::DisableGyroscopeLow();
    sNumOfInstance--;
}

void GyroscopeReader::EnableZeroDrift(){
    this->mEnableZeroDrift = true;
}

void GyroscopeReader::EnableAccRevise(){
    this->mEnableAccRevise = true;
}

void GyroscopeReader::EnableZeroPlay(){
    this->mEnableZeroPlay = true;
}

void GyroscopeReader::DisableAccRevise(){
    this->mEnableAccRevise = false;
}

void GyroscopeReader::DisableZeroDrift(){
    this->mEnableZeroDrift = false;
}

void GyroscopeReader::DisableZeroPlay(){
    this->mEnableZeroPlay = false;
}

void GyroscopeReader::CalculateDirection(){

}

void GyroscopeReader::InitializeCalibrationData(){
    // TODO
}

void GyroscopeReader::ResetZeroDriftMode(){
    this->mZeroDriftMode = GYROSCOPE_ZERODRIFT_STANDARD;
    this->mZeroDriftRadius = 0.005;
    this->mZeroDriftCount = 100;
    this->mZeroDriftPower = 0.0199;
}

void GyroscopeReader::ResetAxisRotationMatrix(){
    math::MTX34::Identity();
    //this->SetAxis
    this->mpAccelerometerReader->ResetAxisRotationMatrix();
}

f32 GyroscopeReader::ReviseDirection_Acceleration(Direction& rev_dir, const nn::math::VEC3& acc){
    
}

void GyroscopeReader::SetZeroPlayParam(f32& radius){
    this->mZeroPlayRadius = 0.005;
}

void GyroscopeReader::SetZeroDriftMode(const ZeroDriftMode& mode){
    this->mZeroDriftMode = mode;
}


}
}
}