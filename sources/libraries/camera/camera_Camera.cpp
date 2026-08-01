// Filename: camera_Camera.cpp
//
// Project: Horizon

#include <nn/camera/CTR/camera_Camera.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace camera{
namespace CTR{
namespace detail{

Handle Camera::sSession;

Result Camera::Activate(CameraSelect camera){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x13, 1, 0, 0);
    ipcMsg.SetRaw(1, camera);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Camera::GetActivatedCamera(CameraSelect* pSelect){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x3B, 0, 0, 0);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pSelect = ipcMsg.GetRaw<CameraSelect>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result Camera::GetSleepCamera(CameraSelect* pSelect){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x3C, 0, 0, 0);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pSelect = ipcMsg.GetRaw<CameraSelect>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result Camera::SetSleepCamera(CameraSelect select){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x3D, 1, 0, 0);
    ipcMsg.SetRaw(1, select);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

}
}
}
}