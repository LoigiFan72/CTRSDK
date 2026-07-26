// Filename: err_FatalErrClient.cpp
//
// Project: Horizon

#include <nn/err/CTR/err_Api.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace err{
namespace CTR{

Result FatalErr::Throw(FatalErrInfo& info){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(1, 32, 0, 0);
    ipcMsg.SetRaw(1, info);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}
}
}
}