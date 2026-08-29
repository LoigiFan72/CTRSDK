#pragma once

#include <nn/applet/CTR/applet_Paramaters.h>

namespace nn{
namespace applet{ 
namespace CTR { 
namespace detail { 

class APPLET{
public:
    static Result AppletUtility(u32 id,u8 *pInParam,size_t inParamSize,u8 *pOutParam,size_t outParamSize,s32 *pReadLen);
    static Result CancelLibraryApplet(bool isCallerEnd);    
    static Result CancelParameter(bool isSenderCheck, AppletId senderId, bool isReceiverCheck, AppletId receiverId, bool* pIdCanceled);
    static Result CloseApplication(u8 *pParam,size_t paramSize,Handle handle);
    static Result Enable(AppletAttr appletAttr);
    static Result GetAppletManInfo(AppletPos appletPos,AppletPos *currentPos,AppletId *requestedAppletId,AppletId *homeMenuAppletId,AppletId *currentAppletId);
    static Result GetLockHandle(Handle *pMutexHandle,AppletAttr appletAttr,AppletAttr *pAttrDecided,bit32 *pMiscState);
    static Result GlanceParameter(AppletId *pSenderId,AppletId receiverId,u32 *pCommand,u8 *pParam,size_t paramSize,s32 *pReadLen,Handle *pHandle);
    static Result Initialize(AppletId appletId,AppletAttr appletAttr,Handle *pEventHandle_mesg,Handle *pEventHandle_cont);
    static Result InquireNotification(AppletId appletId,AppletNotification *notification);
    static Result IsRegistered(AppletId appletId,bool *pRegistered);
    static Result JumpToHomeMenu(u8 *pParam,size_t paramSize,Handle handle);
    static Result NotifyToWait(AppletId id);
    static Result PrepareToCloseApplication(bool isJumpToHome);
    static Result PrepareToJumpToHomeMenu();
    static Result PrepareToStartSystemApplet(AppletId id);
    static Result ReceiveParameter(AppletId *pSenderId,AppletId receiverId,u32 *pCommand,u8 *pParam,size_t paramSize,s32 *pReadLen,Handle *pHandle);
    static Result ReplySleepNotificationComplete(AppletId id);
    static Result ReplySleepQuery(AppletId id,AppletQueryReply reply);
    static Result SendCaptureBufferInfo(u8 *pParam,size_t paramSize);
    static Result SendParameter(AppletId senderId,AppletId receiverId,u32 command,const u8 *pParam,size_t paramSize,Handle pHandle);
    static Result SleepSystem(bit64 awakeReason);
    static Result StartSystemApplet(AppletId id,u8 *pParam,size_t paramSize,Handle handle);

    static nn::Handle s_Session;
};

}
}
}
}