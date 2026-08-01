// Filename: fs_IpcDirectory.cpp
//
// Project: Horizon

#include <nn/fs/fs_IpcDirectory.h>

#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn {
namespace fs {
namespace ipc {

Result Directory::Read(s32* pNumEntriesOut, fs::DirectoryEntry pEntries[], s32 numEntries){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x801, 1, 2, 0);
    ipcMsg.SetRaw(1, numEntries);
    ipcMsg.SetReceive(2, pEntries, sizeof(*pEntries) * numEntries);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pNumEntriesOut = ipcMsg.GetRaw<s32>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result Directory::Close(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x802, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Directory::SetPriority(s32 priority){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x803, 1, 0, 0);
    ipcMsg.SetRaw(1, priority);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Directory::GetPriority(s32* pOut){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x804, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetRaw<s32>(2);

    return ipcMsg.GetRaw<Result>(1);
}

}
}
}