// Filename: ndm_Interface.cpp
//
// Project: Horizon

#include <nn/ndm/ndm_Interface.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace ndm{
namespace CTR{
namespace detail{

Handle Interface::sSession;

Result Interface::OverrideDefaultDaemons(bit32 mask){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x14, 1, 0, 0);
    ipcMsg.SetRaw(1, mask);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Interface::SuspendDaemons(bit32 mask){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(6, 1, 0, 0);
    ipcMsg.SetRaw(1, mask);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

nn::Result Interface::ResumeDaemons( bit32 mask ){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(7, 1, 0, 0);
    ipcMsg.SetRaw(1, mask);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Interface::SuspendScheduler(bool bAsync){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(8, 1, 0, 0);
    ipcMsg.SetRaw(1, bAsync);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Interface::ResumeScheduler(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(9, 0, 0, 0);


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