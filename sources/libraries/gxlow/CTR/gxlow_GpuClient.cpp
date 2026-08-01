// Filename: gxlow_GpuClient.cpp
//
// Project: Horizon

#include <nn/gxlow/CTR/gxlow_GpuIpc.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn {
namespace gxlow {
namespace CTR {

Result Gpu::WriteHWRegs(u32 regOffset, const u8 pSrc[], size_t size){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x1, 2, 2, 0);
    ipcMsg.SetRaw(1, regOffset);
    ipcMsg.SetRaw(2, size);
    ipcMsg.SetPointerHeader(3, 0, sizeof(*pSrc) * size);
    ipcMsg.SetPointer(4, pSrc);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::WriteHWRegsWithMask(u32 regOffset, const u8 pSrc[], const u8 pMask[], size_t size){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x2, 2, 4, 0);
    ipcMsg.SetRaw(1, regOffset);
    ipcMsg.SetRaw(2, size);
    ipcMsg.SetPointerHeader(3, 0, sizeof(*pSrc) * size);
    ipcMsg.SetPointer(4, pSrc);
    ipcMsg.SetPointerHeader(5, 1, sizeof(*pMask) * size);
    ipcMsg.SetPointer(6, pMask);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::ReadHWRegs(u32 regOffset, u8 pDst[], size_t size){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x4, 2, 0, 0);
    ipcMsg.SetRaw(1, regOffset);
    ipcMsg.SetRaw(2, size);

    bit32* ipcRcvBuf = GetReceiveBuffer();
    bit32 rcvBufRefuge[2];

    std::memcpy(rcvBufRefuge, ipcRcvBuf, sizeof(rcvBufRefuge));

    MessageBuffer ipcRcv(ipcRcvBuf);
    ipcRcv.SetPointerHeaderForReceive(0, sizeof(*pDst) * size);
    ipcRcv.SetPointer(1, pDst);

    Result ipcResult = SendSyncRequest(this->mSession);

    std::memcpy(ipcRcvBuf, rcvBufRefuge, sizeof(rcvBufRefuge));

    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::FlushDataCache(Handle clientProcess, uptr addr, size_t size){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x8, 2, 2, 0);
    ipcMsg.SetRaw(1, addr);
    ipcMsg.SetRaw(2, size);
    ipcMsg.SetCopyHandleHeader(3, 1);
    ipcMsg.SetHandle(4, clientProcess);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::SetLcdForceBlack(bool enable){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xB, 1, 0, 0);
    ipcMsg.SetRaw(1, enable);


    nn::Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::TriggerCmdReqQueue(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xC, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::RegisterInterruptRelayQueue(Handle eventRx, bit32 attribute, Handle* pWorkMem, s32* pIndex){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x13, 1, 2, 0);
    ipcMsg.SetRaw(1, attribute);
    ipcMsg.SetCopyHandleHeader(2, 1);
    ipcMsg.SetHandle(3, eventRx);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pIndex = ipcMsg.GetRaw<s32>(2);
    *pWorkMem = ipcMsg.GetHandle(4);

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::UnregisterInterruptRelayQueue(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x14, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::AcquireRight(Handle clientProcess, bool forced){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x16, 1, 2, 0);
    ipcMsg.SetRaw(1, forced);
    ipcMsg.SetCopyHandleHeader(2, 1);
    ipcMsg.SetHandle(3, clientProcess);


    nn::Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::ReleaseRight(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x17, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::ImportDisplayCaptureInfo(DisplayCaptureInfo* info){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x18, 0, 0, 0);


    nn::Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *info = ipcMsg.GetRaw<DisplayCaptureInfo>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::SaveVramSysArea(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x19, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result Gpu::RestoreVramSysArea(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x1A, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

}
}
}