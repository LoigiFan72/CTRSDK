// Filename: srv_Service.cpp
//
// Project: Horizon

#include <nn/srv/srv_Service.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace srv{
namespace detail{

Handle Service::sSession = nn::WithoutInitialize();

Result Service::EnableNotication(Handle* pSemaphore){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(2, 0, 0, 0);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pSemaphore = ipcMsg.GetHandle(3);

    return ipcMsg.GetRaw<Result>(1);
}

Result Service::GetServiceHandle(Handle* pOut, const char* name, s32 nameLen, u32 flags){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(5, 4, 0, 0);
    ipcMsg.SetRawArray(1, name, sizeof(*name) * 8);
    ipcMsg.SetRaw(3, nameLen);
    ipcMsg.SetRaw(4, flags);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetHandle(3);

    return ipcMsg.GetRaw<Result>(1);
}

Result Service::ReceiveNotification(bit32 *pOut){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xB, 0, 0, 0);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetRaw<bit32>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result Service::RegisterClient(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(1, 0, 2, 0);
    ipcMsg.SetProcessIdHeader(1);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Service::Subscribe( bit32 message ){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(9, 1, 0, 0);
    ipcMsg.SetRaw(1, message);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

}
}
}