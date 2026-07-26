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
    u8 isInitialized;
    u8 leaveApplicationCamera;

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
    if (!isInitialized)
        return ResultSuccess();

    u8 activatedCamera;
    Result result = Camera::GetActivatedCamera(&activatedCamera);
    if (result.IsFailure())
        return result;

    u8 sleepCamera;
    result = Camera::GetSleepCamera(&sleepCamera);
    if (result.IsFailure())
        return result;

    leaveApplicationCamera = activatedCamera | sleepCamera;

    result = Camera::SetSleepCamera(0);
    if (result.IsSuccess())
        result = Camera::Activate(0);

    return result;
}

}
}
}
}