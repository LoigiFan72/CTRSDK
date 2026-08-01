// Filename: dsp_Ipc.cpp
//
// Project: Horizon

#include <nn/dsp/CTR/MPCore/dsp_Ipc.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace dsp{
namespace CTR{

Result DSP::RecvData(u16 regNo, u16* pValue){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x1, 1, 0, 0);
    ipcMsg.SetRaw(1, regNo);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pValue = ipcMsg.GetRaw<u16>(2);

    return ipcMsg.GetRaw<Result>(1);
}
Result DSP::RecvDataIsReady(u16 regNo, bool* pReady){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x2, 1, 0, 0);
    ipcMsg.SetRaw(1, regNo);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pReady = ipcMsg.GetRaw<bool>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result DSP::SetSemaphore(u16 mask){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x7, 1, 0, 0);
    ipcMsg.SetRaw(1, mask);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result DSP::ConvertProcessAddressFromDspDram(uptr _address, uptr* address){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xC, 1, 0, 0);
    ipcMsg.SetRaw(1, _address);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *address = ipcMsg.GetRaw<uptr>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result DSP::WriteProcessPipe(s32 port, const u8 pBuffer[], size_t length){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xD, 2, 2, 0);
    ipcMsg.SetRaw(1, port);
    ipcMsg.SetRaw(2, length);
    ipcMsg.SetPointerHeader(3, 1, sizeof(*pBuffer) * length);
    ipcMsg.SetPointer(4, pBuffer);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result DSP::ReadPipeIfPossible(s32 port, s32 peer, u8 pBuffer[], u16 length, u16* lengthRead){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x10, 3, 0, 0);
    ipcMsg.SetRaw(1, port);
    ipcMsg.SetRaw(2, peer);
    ipcMsg.SetRaw(3, length);

    bit32* ipcRcvBuf = GetReceiveBuffer();
    bit32 rcvBufRefuge[2];

    std::memcpy(rcvBufRefuge, ipcRcvBuf, sizeof(rcvBufRefuge));

    MessageBuffer ipcRcv(ipcRcvBuf);
    ipcRcv.SetPointerHeaderForReceive(0, sizeof(*pBuffer) * length);
    ipcRcv.SetPointer(1, pBuffer);

    Result ipcResult = SendSyncRequest(this->mSession);

    std::memcpy(ipcRcvBuf, rcvBufRefuge, sizeof(rcvBufRefuge));

    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *lengthRead = ipcMsg.GetRaw<u16>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result DSP::LoadComponent(const u8 pComponent[], size_t size, bit16 maskPram, bit16 maskDram, bool* pStatus){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x11, 3, 2, 0);
    ipcMsg.SetRaw(1, size);
    ipcMsg.SetRaw(2, maskPram);
    ipcMsg.SetRaw(3, maskDram);
    ipcMsg.SetSend(4, pComponent, sizeof(*pComponent) * size);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pStatus = ipcMsg.GetRaw<bool>(2);

    return ipcMsg.GetRaw<Result>(1);
}

Result DSP::UnloadComponent(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x12, 0, 0, 0);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

Result DSP::FlushDataCache(nn::Handle clientProcess, uptr addr, size_t size){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x13, 2, 2, 0);
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

nn::Result DSP::RegisterInterruptEvents(nn::Handle handle, s32 type, s32 port){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x15, 2, 2, 0);
    ipcMsg.SetRaw(1, type);
    ipcMsg.SetRaw(2, port);
    ipcMsg.SetCopyHandleHeader(3, 1);
    ipcMsg.SetHandle(4, handle);


    nn::Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result DSP::GetSemaphoreEventHandle(nn::Handle* handle){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x16, 0, 0, 0);


    nn::Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *handle = ipcMsg.GetHandle(3);

    return ipcMsg.GetRaw<Result>(1);
}

Result DSP::SetSemaphoreMask(bit16 mask){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x17, 1, 0, 0);
    ipcMsg.SetRaw(1, mask);


    Result ipcResult = SendSyncRequest(this->mSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<Result>(1);
}

}
}
}