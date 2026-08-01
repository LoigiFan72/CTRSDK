// Filename: fs_IpcFile.cpp
//
// Project: Horizon

#include <nn/fs/fs_IpcFile.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace fs{
namespace ipc{

Result File::OpenSubFile(Handle* pOut, s64 offset, s64 length){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x801, 4, 0, 0);
    ipcMsg.SetRaw(1, offset);
    ipcMsg.SetRaw(3, length);

    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetHandle(3);

    return ipcMsg.GetRaw<Result>(1);
}

Result File::Read(s32* pRead, s64 offset, void* pBuffer, size_t size){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x802, 3, 2, 0);
    ipcMsg.SetRaw(1, offset);
    ipcMsg.SetRaw(3, size);
    ipcMsg.SetReceive(4, pBuffer, size);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pRead = ipcMsg.GetRaw<s32>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result File::Write(s32* pWritten, s64 offset, const void* pBuffer, size_t size, fs::WriteOption option){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x803, 4, 2, 0);
    ipcMsg.SetRaw(1, offset);
    ipcMsg.SetRaw(3, size);
    ipcMsg.SetRaw(4, option);
    ipcMsg.SetSend(5, pBuffer, size);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pWritten = ipcMsg.GetRaw<s32>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result File::GetSize( s64* pOut ){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x804, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetRaw<s64>(2);

    return ipcMsg.GetRaw<nn::Result>(1);
}
Result File::SetSize(s64 size){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x805, 2, 0, 0);
    ipcMsg.SetRaw(1, size);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result File::Close(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x808, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result File::SetPriority(s32 priority){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x80A, 1, 0, 0);
    ipcMsg.SetRaw(1, priority);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result File::GetPriority( s32* pOut ){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x80B, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetRaw<s32>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result File::OpenLinkFile(Handle* pOut){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x80C, 0, 0, 0);

    
    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pOut = ipcMsg.GetHandle(3);

    return ipcMsg.GetRaw<Result>(1);
}


}
}
}