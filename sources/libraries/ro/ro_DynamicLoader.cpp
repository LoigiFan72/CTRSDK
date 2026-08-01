// Filename: ro_DynamicLoader.cpp
//
// Project: Horizon

#include <nn/ro/ro_DynamicLoader.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace ro{
namespace detail{

Handle DynamicLoader::sSession;

Result DynamicLoader::Startup(Handle process, uptr staticInfo, size_t staticInfoSize, uptr locateAddr){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(1, 3, 2, 0);
    ipcMsg.SetRaw(1, staticInfo);
    ipcMsg.SetRaw(2, staticInfoSize);
    ipcMsg.SetRaw(3, locateAddr);
    ipcMsg.SetCopyHandleHeader(4, 1);
    ipcMsg.SetHandle(5, process);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result DynamicLoader::RegisterList(Handle process, uptr rr, size_t rrSize){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(2, 2, 2, 0);
    ipcMsg.SetRaw(1, rr);
    ipcMsg.SetRaw(2, rrSize);
    ipcMsg.SetCopyHandleHeader(3, 1);
    ipcMsg.SetHandle(4, process);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result DynamicLoader::UnregisterList(Handle process, uptr rr){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(3, 1, 2, 0);
    ipcMsg.SetRaw(1, rr);
    ipcMsg.SetCopyHandleHeader(2, 1);
    ipcMsg.SetHandle(3, process);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result DynamicLoader::Load(size_t* pFixedSize, Handle process, uptr ro, uptr roRelocate, size_t roSize, uptr dataAddr, uptr dataRelocate, size_t dataSize, uptr bssAddr, uptr bssSize, bool doRegister, nn::ro::FixLevel fixLevel, uptr rr){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(4, 11, 2, 0);
    ipcMsg.SetRaw(1, ro);
    ipcMsg.SetRaw(2, roRelocate);
    ipcMsg.SetRaw(3, roSize);
    ipcMsg.SetRaw(4, dataAddr);
    ipcMsg.SetRaw(5, dataRelocate);
    ipcMsg.SetRaw(6, dataSize);
    ipcMsg.SetRaw(7, bssAddr);
    ipcMsg.SetRaw(8, bssSize);
    ipcMsg.SetRaw(9, doRegister);
    ipcMsg.SetRaw(10, fixLevel);
    ipcMsg.SetRaw(11, rr);
    ipcMsg.SetCopyHandleHeader(12, 1);
    ipcMsg.SetHandle(13, process);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pFixedSize = ipcMsg.GetRaw<size_t>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result DynamicLoader::Unload(Handle process, uptr roModule, size_t roSize, uptr originalAddr){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(5, 3, 2, 0);
    ipcMsg.SetRaw(1, roModule);
    ipcMsg.SetRaw(2, roSize);
    ipcMsg.SetRaw(3, originalAddr);
    ipcMsg.SetCopyHandleHeader(4, 1);
    ipcMsg.SetHandle(5, process);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result DynamicLoader::Link(Handle process, uptr roModule){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(6, 1, 2, 0);
    ipcMsg.SetRaw(1, roModule);
    ipcMsg.SetCopyHandleHeader(2, 1);
    ipcMsg.SetHandle(3, process);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result DynamicLoader::Unlink(Handle process, uptr roModule){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(7, 1, 2, 0);
    ipcMsg.SetRaw(1, roModule);
    ipcMsg.SetCopyHandleHeader(2, 1);
    ipcMsg.SetHandle(3, process);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result DynamicLoader::Cleanup(Handle process, uptr originalAddr){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(8, 1, 2, 0);
    ipcMsg.SetRaw(1, originalAddr);
    ipcMsg.SetCopyHandleHeader(2, 1);
    ipcMsg.SetHandle(3, process);


    Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

}
}
}