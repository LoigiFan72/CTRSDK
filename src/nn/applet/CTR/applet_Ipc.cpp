// Filename: applet_Ipc.cpp
//
// Project: Horizon

#include <nn/applet/CTR/applet_Ipc.h>
#include <nn/os/ipc/os_Message.h>
#include <nn/svc.h>

namespace nn{
namespace applet{
namespace CTR{
namespace detail{

nn::Handle APPLET::sSession;

Result APPLET::AppletUtility(u32 id,u8 *pInParam,size_t inParamSize,u8 *pOutParam,size_t outParamSize,s32 *pReadLen){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x4B, 3, 2, 0);
    ipcMsg.SetRaw(1, id);
    ipcMsg.SetRaw(2, inParamSize);
    ipcMsg.SetRaw(3, outParamSize);
    ipcMsg.SetPointerHeader(4, 1, sizeof(*pInParam) * inParamSize);
    ipcMsg.SetPointer(5, pInParam);

    bit32* ipcRcvBuf = GetReceiveBuffer();
    bit32 rcvBufRefuge[2];

    std::memcpy(rcvBufRefuge, ipcRcvBuf, sizeof(rcvBufRefuge));

    MessageBuffer ipcRcv(ipcRcvBuf);
    ipcRcv.SetPointerHeaderForReceive(0, sizeof(*pOutParam) * outParamSize);
    ipcRcv.SetPointer(1, pOutParam);

    nn::Result ipcResult = SendSyncRequest(sSession);

    std::memcpy(ipcRcvBuf, rcvBufRefuge, sizeof(rcvBufRefuge));

    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pReadLen = ipcMsg.GetRaw<s32>(2);

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::CancelLibraryApplet(bool isCallerEnd){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x3B, 1, 0, 0);
    ipcMsg.SetRaw(1, isCallerEnd);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::CancelParameter(bool isSenderCheck, AppletId senderId, bool isReceiverCheck, AppletId receiverId, bool* pIdCanceled){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xF, 4, 0, 0);
    ipcMsg.SetRaw(1, isSenderCheck);
    ipcMsg.SetRaw(2, senderId);
    ipcMsg.SetRaw(3, isReceiverCheck);
    ipcMsg.SetRaw(4, receiverId);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pIdCanceled = ipcMsg.GetRaw<bool>(2);

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::CloseApplication(u8 *pParam,size_t paramSize,Handle handle){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x27, 1, 4, 0);
    ipcMsg.SetRaw(1, paramSize);
    ipcMsg.SetCopyHandleHeader(2, 1);
    ipcMsg.SetHandle(3, handle);
    ipcMsg.SetPointerHeader(4, 0, sizeof(*pParam) * paramSize);
    ipcMsg.SetPointer(5, pParam);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::Enable(AppletAttr appletAttr){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(3, 1, 0, 0);
    ipcMsg.SetRaw(1, appletAttr);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::GetAppletManInfo(AppletPos appletPos,AppletPos *currentPos,AppletId *requestedAppletId,AppletId *homeMenuAppletId,AppletId *currentAppletId){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(5, 1, 0, 0);
    ipcMsg.SetRaw(1, appletPos);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *currentPos = ipcMsg.GetRaw<nn::applet::CTR::AppletPos>(2);
    *requestedAppletId = ipcMsg.GetRaw<nn::applet::CTR::AppletId>(3);
    *homeMenuAppletId = ipcMsg.GetRaw<nn::applet::CTR::AppletId>(4);
    *currentAppletId = ipcMsg.GetRaw<nn::applet::CTR::AppletId>(5);

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::GetLockHandle(Handle *pMutexHandle,AppletAttr appletAttr,AppletAttr *pAttrDecided,bit32 *pMiscState){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(1, 1, 0, 0);
    ipcMsg.SetRaw(1, appletAttr);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pAttrDecided = ipcMsg.GetRaw<nn::applet::CTR::AppletAttr>(2);
    *pMiscState = ipcMsg.GetRaw<bit32>(3);
    *pMutexHandle = ipcMsg.GetHandle(5);

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::GlanceParameter(AppletId *pSenderId,AppletId receiverId,u32 *pCommand,u8 *pParam,size_t paramSize,s32 *pReadLen,Handle *pHandle){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xE, 2, 0, 0);
    ipcMsg.SetRaw(1, receiverId);
    ipcMsg.SetRaw(2, paramSize);

    bit32* ipcRcvBuf = GetReceiveBuffer();
    bit32 rcvBufRefuge[2];

    std::memcpy(rcvBufRefuge, ipcRcvBuf, sizeof(rcvBufRefuge));

    MessageBuffer ipcRcv(ipcRcvBuf);
    ipcRcv.SetPointerHeaderForReceive(0, sizeof(*pParam) * paramSize);
    ipcRcv.SetPointer(1, pParam);

    nn::Result ipcResult = SendSyncRequest(sSession);

    std::memcpy(ipcRcvBuf, rcvBufRefuge, sizeof(rcvBufRefuge));

    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pSenderId = ipcMsg.GetRaw<nn::applet::CTR::AppletId>(2);
    *pCommand = ipcMsg.GetRaw<u32>(3);
    *pReadLen = ipcMsg.GetRaw<s32>(4);
    *pHandle = ipcMsg.GetHandle(6);

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::Initialize(AppletId appletId,AppletAttr appletAttr,Handle *pEventHandle_mesg,Handle *pEventHandle_cont){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(2, 2, 0, 0);
    ipcMsg.SetRaw(1, appletId);
    ipcMsg.SetRaw(2, appletAttr);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pEventHandle_mesg = ipcMsg.GetHandle(3);
    *pEventHandle_cont = ipcMsg.GetHandle(4);

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::InquireNotification(AppletId appletId,AppletNotification *notification){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xB, 1, 0, 0);
    ipcMsg.SetRaw(1, appletId);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *notification = ipcMsg.GetRaw<nn::applet::CTR::AppletNotification>(2);

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::IsRegistered(AppletId appletId,bool *pRegistered){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(9, 1, 0, 0);
    ipcMsg.SetRaw(1, appletId);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pRegistered = ipcMsg.GetRaw<bool>(2);

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::JumpToHomeMenu(u8 *pParam,size_t paramSize,Handle handle){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x2C, 1, 4, 0);
    ipcMsg.SetRaw(1, paramSize);
    ipcMsg.SetCopyHandleHeader(2, 1);
    ipcMsg.SetHandle(3, handle);
    ipcMsg.SetPointerHeader(4, 0, sizeof(*pParam) * paramSize);
    ipcMsg.SetPointer(5, pParam);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::NotifyToWait(AppletId id){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x43, 1, 0, 0);
    ipcMsg.SetRaw(1, id);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::PrepareToStartSystemApplet(AppletId id){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x19, 1, 0, 0);
    ipcMsg.SetRaw(1, id);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::PrepareToJumpToHomeMenu(){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x2B, 0, 0, 0);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::ReceiveParameter(AppletId *pSenderId,AppletId receiverId,u32 *pCommand,u8 *pParam,size_t paramSize,s32 *pReadLen,Handle *pHandle){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xD, 2, 0, 0);
    ipcMsg.SetRaw(1, receiverId);
    ipcMsg.SetRaw(2, paramSize);

    bit32* ipcRcvBuf = GetReceiveBuffer();
    bit32 rcvBufRefuge[2];

    std::memcpy(rcvBufRefuge, ipcRcvBuf, sizeof(rcvBufRefuge));

    MessageBuffer ipcRcv(ipcRcvBuf);
    ipcRcv.SetPointerHeaderForReceive(0, sizeof(*pParam) * paramSize);
    ipcRcv.SetPointer(1, pParam);

    nn::Result ipcResult = SendSyncRequest(sSession);

    std::memcpy(ipcRcvBuf, rcvBufRefuge, sizeof(rcvBufRefuge));

    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    *pSenderId = ipcMsg.GetRaw<nn::applet::CTR::AppletId>(2);
    *pCommand = ipcMsg.GetRaw<u32>(3);
    *pReadLen = ipcMsg.GetRaw<s32>(4);
    *pHandle = ipcMsg.GetHandle(6);

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::PrepareToCloseApplication(bool isJumpToHome){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x22, 1, 0, 0);
    ipcMsg.SetRaw(1, isJumpToHome);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::ReplySleepNotificationComplete(AppletId id){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x3F, 1, 0, 0);
    ipcMsg.SetRaw(1, id);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::ReplySleepQuery(AppletId id,AppletQueryReply reply){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x3E, 2, 0, 0);
    ipcMsg.SetRaw(1, id);
    ipcMsg.SetRaw(2, reply);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::SendCaptureBufferInfo(u8 *pParam,size_t paramSize){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x50, 1, 2, 0);
    ipcMsg.SetRaw(1, paramSize);
    ipcMsg.SetPointerHeader(2, 0, sizeof(*pParam) * paramSize);
    ipcMsg.SetPointer(3, pParam);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::SendParameter(AppletId senderId,AppletId receiverId,u32 command,const u8 *pParam,size_t paramSize,Handle pHandle){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0xC, 4, 4, 0);
    ipcMsg.SetRaw(1, senderId);
    ipcMsg.SetRaw(2, receiverId);
    ipcMsg.SetRaw(3, command);
    ipcMsg.SetRaw(4, paramSize);
    ipcMsg.SetCopyHandleHeader(5, 1);
    ipcMsg.SetHandle(6, pHandle);
    ipcMsg.SetPointerHeader(7, 0, sizeof(*pParam) * paramSize);
    ipcMsg.SetPointer(8, pParam);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::SleepSystem(bit64 awakeReason){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x42, 2, 0, 0);
    ipcMsg.SetRaw(1, awakeReason);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

Result APPLET::StartSystemApplet(AppletId id,u8 *pParam,size_t paramSize,Handle handle){
    MessageBuffer ipcMsg(GetMessageBuffer());
    ipcMsg.SetHeader(0x1F, 2, 4, 0);
    ipcMsg.SetRaw(1, id);
    ipcMsg.SetRaw(2, paramSize);
    ipcMsg.SetCopyHandleHeader(3, 1);
    ipcMsg.SetHandle(4, handle);
    ipcMsg.SetPointerHeader(5, 0, sizeof(*pParam) * paramSize);
    ipcMsg.SetPointer(6, pParam);


    nn::Result ipcResult = SendSyncRequest(sSession);
    if(ipcResult.IsFailure()){
        return ipcResult;
    }

    return ipcMsg.GetRaw<nn::Result>(1);
}

}
}
}
}