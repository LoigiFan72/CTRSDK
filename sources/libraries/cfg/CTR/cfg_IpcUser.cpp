// Filename: cfg_IpcUser.cpp
//
// Project: Horizon

#include <nn/cfg/CTR/cfg_IpcUser.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn {
namespace cfg {
namespace CTR {
namespace detail {

Handle IpcUser::s_Session;

Result IpcUser::GetConfig(void* pData, size_t size, bit32 key){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(1, 2, 2, 0);
    ipcMsg.SetRaw(1, size);
    ipcMsg.SetRaw(2, key);
    ipcMsg.SetReceive(3, pData, size);


    Result ipcResult = SendSyncRequest(s_Session);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result IpcUser::GetRegion(CfgRegionCode* regionCode){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(2, 0, 0, 0);


    Result ipcResult = SendSyncRequest(s_Session);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *regionCode = ipcMsg.GetRaw<CfgRegionCode>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result IpcUser::GetTransferableId(bit32 uniqueId, bit64* transferableId){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(3, 1, 0, 0);
    ipcMsg.SetRaw(1, uniqueId);


    Result ipcResult = SendSyncRequest(s_Session);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *transferableId = ipcMsg.GetRaw<bit64>(2);

    return ipcMsg.GetRaw<Result>(1);
}

}
}
}
}