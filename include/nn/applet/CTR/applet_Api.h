#pragma once

#include <nn/applet/CTR/applet_Paramaters.h>
#include <nn/applet/CTR/applet_Result.h>
#include <nn/fnd/fnd_TimeSpan.h>
#include <nn/fs.h>
#include <nn/os/os_TransferMemoryBlock.h>
#include <nn/os/os_Thread.h>

// Mostly the detail ones, so yeah.

namespace nn { 
namespace applet {
namespace CTR {
bool IsInitialized();

    extern nn::Handle HANDLE_NONE;

namespace detail{
    //--- Initialize
    Result Initialize(AppletAttr applerAttr);
    void Enable(bool isSleepEnabled = true);
    bool IsRegistered(AppletId appletId);
    bool WaitForRegister(AppletId appletId, nn::fnd::TimeSpan span = WAIT_INFINITE);
    Result InitializeConnect(AppletId appletId, AppletAttr attr, s32 threadPriority);
    void SetActive();

    //--- Message
    Result SendMessage(AppletId receiverId, const u8* pParam, size_t paramSize, Handle handle=NN_APPLET_HANDLE_NONE, nn::fnd::TimeSpan timeout=WAIT_INFINITE);
    Result TrySendMessage(AppletId receiverId, const u8* pParam, size_t paramSize, Handle handle=NN_APPLET_HANDLE_NONE);

    //--- Library Applet Loading
    Result PreloadLibraryApplet(AppletId id);
    Result CancelLibraryApplet(bool isApplicationEnd = false);
    Result CancelLibraryAppletIfRegistered(bool isApplicationEnd = false, AppletWakeupState *pWakeupState=NULL);
    Result PrepareToStartLibraryApplet(AppletId id);
    Result StartLibraryApplet(AppletId id, const u8* pParam=NULL, size_t paramSize=0, Handle handle=NN_APPLET_HANDLE_NONE);

    //--- Close Application
    Result PrepareToCloseApplication(bool isCancelPreload = false);
    Result CloseApplication(u8 *pParam,size_t paramSize,nn::Handle handle = HANDLE_NONE);

    //--- Utility / Transition
    Result CallUtility(u32 utilityId, u8* pInParam, size_t inParamSize, u8* outParam, size_t outParamSize, s32* readSize);
    void UnlockTransition(u32 action);
    void LockTransition(u32 action,bool isForced);

    //--- GPU / CAMERA / DSP Rights
    void AssignGpuRight(bool flag);
    void AssignCameraRight(bool flag);
    void AssignDspRight(bool flag);

    //--- Home Menu Jumping
    Result JumpToHomeMenu(u8 *pParam,size_t paramSize,Handle handle);
    Result PrepareToJumpToHomeMenu();

    //--- Paremater
    bool CancelParemater(bool isSenderCheck, AppletId senderId, bool isReceiverCheck, AppletId receiverId);

    //--- Application Info
    void GetAppletManInfo(AppletPos requestPos,AppletPos *pCurrentPos,AppletId *pRequestedId,AppletId *pHomeMenuId,AppletId *pCurrentId);
    bool GetAppletInfo(AppletId appletId, ProgramId* pProgramId, nn::fs::MediaType* pMediaType, bool* pIsUsed, bool* pIsPreloaded, AppletAttr* pAttr);

    //--- Sleep
    void SleepIfShellClosed();

    //--- Capture Info
    Result SendCaptureBufferInfo(u8* pParam, size_t paramSize);

    //--- Sending Parameter IDS
    Result TrySend(AppletId receiverId, u32 command, const u8* pParam, size_t paramSize, Handle handle);
    Result Send(AppletId receiverId,u32 command,const u8 *pParam,size_t paramSize,Handle handle,fnd::TimeSpan timeout);

    //--- Sleep Query Manager
    void ReplySleepQueryToManager(QueryReply);
    void ReplySleepNotificationCompleteToManager();

    //--- System Application Starting
    Result PrepareToStartSystemApplet(AppletId id);
    Result StartSystemApplet(AppletId id, u8* pParam, size_t size, nn::Handle handle);

    //--- Glance to Application
    Result Glance(AppletId *pSenderId,u32 *pCommand,u8 *pParam,size_t paramSize,s32 *pReadLen,Handle *pHandle);

    //--- Notify Application to Wait
    void NotifyToWait();

    //--- Receiving Parameter IDS
    Result TryReceive(AppletId *pSenderId,u32 *pCommand,u8 *pParam,size_t paramSize,s32 *pReadLen, Handle *pHandle,bool isTry);
    Result Receive(AppletId *pSenderId,u32 *pCommand,u8 *pParam,size_t paramSize,s32 *pReadLen,Handle *pHandle,fnd::TimeSpan timeout);

    struct LockTransitionParam{
        u32 action;
        bool isForced;
        s8 rev[3];
    };

    inline void WaitBySleep(int msecs){
        fnd::TimeSpan span = fnd::TimeSpan::FromMilliSeconds(msecs);
        os::Thread::Sleep(span);
    }
}
}
}
}