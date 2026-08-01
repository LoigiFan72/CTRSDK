// Filename: camera_Api.cpp
//
// Project: Horizon

#include <nn/camera/CTR/camera_Api.h>
#include <nn/camera/CTR/camera_Camera.h>
#include <nn/camera/CTR/camera_Result.h>

namespace nn{
namespace camera{
namespace CTR{
namespace detail{
namespace{
    bool isInitialized = false;
    CameraSelect leaveApplicationCamera = SELECT_NONE;

}

bool IsInitialized(){
    return isInitialized;
}

Result ArriveApplication(){
    if(!isInitialized){
        return ResultSuccess();
    }

    Result result = detail::Camera::Activate(leaveApplicationCamera);
    if (result == ResultIsSleeping()){
        result = detail::Camera::SetSleepCamera(leaveApplicationCamera);
    }
    return result;
}

Result LeaveApplication(){
    if (!isInitialized){
        return ResultSuccess();
    }

    CameraSelect activatedCamera, sleepCamera;
    Result result = Camera::GetActivatedCamera(&activatedCamera);
    if (result.IsFailure()){
        return result;
    }

    result = Camera::GetSleepCamera(&sleepCamera);
    if (result.IsFailure()){
        return result;
    }

    leaveApplicationCamera = (CameraSelect)(activatedCamera|sleepCamera);

    result = Camera::SetSleepCamera(SELECT_NONE);
    if (result.IsSuccess()){
        result = Camera::Activate(SELECT_NONE);
    }

    return result;
}

}
}
}
}