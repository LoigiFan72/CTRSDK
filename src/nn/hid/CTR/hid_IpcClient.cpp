// Filename: hid_IpcClient.cpp
//
// Project: Horizon

#include <nn/hid/CTR/hid_IpcClient.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace hid{
namespace CTR{
namespace detail{

nn::Handle Ipc::sSession;

Result Ipc::GetIPCHandles(Handle* pSharedMemoryHandle, Handle* pPadEvent, Handle* pTouchPanelEvent, Handle* pAccelerometerEvent, Handle* pGyroscopeLowEvent, Handle* pDebugPadEvent){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xA, 0, 0, 0);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pSharedMemoryHandle = ipcMsg.GetHandle(3);
    *pPadEvent = ipcMsg.GetHandle(4);
    *pTouchPanelEvent = ipcMsg.GetHandle(5);
    *pAccelerometerEvent = ipcMsg.GetHandle(6);
    *pGyroscopeLowEvent = ipcMsg.GetHandle(7);
    *pDebugPadEvent = ipcMsg.GetHandle(8);

    return ipcMsg.GetRaw<Result>(1);
}

Result Ipc::EnableAccelerometer(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x11, 0, 0, 0);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Ipc::DisableAccelerometer(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x12, 0, 0, 0);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Ipc::EnableGyroscopeLow(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x13, 0, 0, 0);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Ipc::DisableGyroscopeLow(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x14, 0, 0, 0);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Ipc::GetGyroscopeLowRawToDpsCoefficient(f32* pCoefficient){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x15, 0, 0, 0);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pCoefficient = ipcMsg.GetRaw<f32>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result Ipc::GetGyroscopeLowCalibrateParam( nn::hid::CTR::GyroscopeLowCalibrateParam* param ){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x16, 0, 0, 0);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *param = ipcMsg.GetRaw<nn::hid::CTR::GyroscopeLowCalibrateParam>(2);

    return ipcMsg.GetRaw<Result>(1);
}

}
}
}
}