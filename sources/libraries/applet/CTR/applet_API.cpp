// Filename: applet_API.cpp
//
// Project: Horizon

#include <nn/applet.h>
#include <nn/fs.h>
#include <nn/applet/CTR/applet_Connect.h>
#include <nn/applet/CTR/applet_Info.h>
#include <nn/applet/CTR/applet_TimeoutChecker.h>
#include <nn/applet/CTR/applet_ClientThread.h>
#include <nn/applet/CTR/applet_InitialParamaters.h>
#include <nn/applet/CTR/applet_Ipc.h>
#include <nn/camera/CTR/camera_API.h>
#include <nn/srv.h>
#include <nn/os/os_Thread.h>
#include <nn/err/CTR/err_Api.h>
#include <nn/gxlow/CTR/gxlow_SystemUse.h>
#include <nn/gxlow/CTR/gxlow_Management.h>
#include <nn/dsp/CTR/MPCore/dsp_Api.h>

#include <nn/dbg/dbg_DebugString.h>

#include <nn/gx/CTR/gx_CTR.h>

namespace{
    bool                        isInitialized = false;
    bool                        isGpuRightGiven = false;
    bool                        isDspSleeping   = false;

    class ExitHandler : public NotificationHandler{
    public:
        virtual void HandleNotification(bit32 message){
            NN_TLOG_("**** Exit handle id=%x \n", GetId());
        }
    };

    ExitHandler exitHandler;
}

namespace nn{
namespace applet{
namespace CTR{

nn::Handle HANDLE_NONE = 0;

bool IsInitialized(){
    return isInitialized;
}

namespace detail{
namespace{

inline bool DisableSleepForTransition(){
    bool isSleep = CTR::IsEnableSleep();
    if(isSleep) DisableSleep(true);
    return isSleep;
}

inline bool EnableSleepForTransition(){
    bool isSleep = CTR::IsEnableSleep();
    if(!isSleep) EnableSleep(true);
    return isSleep;
}

inline void RestoreSleepForTransition(bool e){
    if(e)
        EnableSleepForTransition();
    else
        DisableSleepForTransition();
}

bool s_IsVramSaved = false;

inline Result SaveVramSysArea(){
    s_IsVramSaved = true;
    return gxlow::CTR::SaveVramSysArea();
}

}

/* Rights */

void AssignGpuRight(bool flag){
    Result res;
    if(flag){
        isGpuRightGiven = true;
        res = gxlow::CTR::AcquireGpuRight();
        NN_ERR_THROW_FATAL(res);
    }
    else{
        if (!isGpuRightGiven){
            return;
        }
        isGpuRightGiven = false;
        res = gxlow::CTR::ReleaseGpuRight();
        if(res == Result(0xd8a02a05))
            return;
        else if(res == Result(0xd9001bf7)){
            NN_TLOG_("applet_API: Warning: Release GPU right despite no gx init.\n");
        }
        else{
            NN_ERR_THROW_FATAL(res);
        }
    }
}

void AssignDspRight(bool flag){
    if(flag){
        if(isDspSleeping){
            dsp::CTR::WakeUp();
            sIsDspSleeping = false;
        }
    }
    else{
        if(dsp::CTR::IsComponentLoaded()){
            dsp::CTR::Sleep();
            isDspSleeping = true;
        }
    }
}

void AssignCameraRight(bool flag){
    if(flag){
        camera::CTR::detail::LeaveApplication();
    }
    else{
        camera::CTR::detail::ArriveApplication();
    }
}

/* Initialization */

Result $Sub$$Initialize(AppletAttr appletAttr) {
    if (!detail::IsAppletMode()) {
        isGpuRightGiven = true;
        appletAttr = AppletAttr(appletAttr & ~7);
        nn::srv::RegisterNotificationHandler(&exitHandler, 0x100);
        Result result = detail::InitializeConnect(0x300, appletAttr, 0xF);
        if (result.IsFailure())
            return result;
    }
    return ResultSuccess();
}

Result InitializeConnect(AppletId appletId, AppletAttr appletAttr, s32 threadPriority){
    if (isInitialized)
        return Result(0xE0A0CFF9);

    isInitialized = true;

    Connect();
    {
        Handle handle;
        AppletAttr attrDecided;
        bit32 miscState;
        Result res = APPLET::GetLockHandle(&handle,appletAttr,&attrDecided,&miscState);
    
        if (!res.IsSuccess()){
            Disconnect();
            return res;
        }
        InitializeMutex(handle);
        
        appletAttr = attrDecided;
        
        SetPowerButtonState((miscState & MISC_STATE_POWER_BUTTON) ? POWER_BUTTON_STATE_CLICK: POWER_BUTTON_STATE_NONE);
        SetOrderToCloseState((miscState & MISC_STATE_SHUTDOWN_PROCESSING) ? ORDER_TO_CLOSE_STATE_RECEIVED: ORDER_TO_CLOSE_STATE_NONE);
    }
    Disconnect();
    
    SetId(appletId);
    SetAttribute(appletAttr);

    if (IsInfoAccess())
        return ResultSuccess();
    LockAndConnect();
    {
        SetActive();
        nn::Handle handleForCont;
        nn::Handle handleForMesg;
        
        Result res = APPLET::Initialize(GetId(),GetAttribute(), &handleForMesg, &handleForCont);
        NN_ERR_THROW_FATAL(res);
        
        InitializeWrapper();
        InitializeClientThread(threadPriority, handleForCont, handleForMesg);
    }
    DisconnectAndUnlock();

    if (!IsApplication())
        gxlow::CTR::SetAppletMode();
    return ResultSuccess();
}

/* Reply Sleep */

void ReplySleepQueryToManager(QueryReply reply){
    Result res;
    LockAndConnect();
    res = APPLET::ReplySleepQuery(GetId(),reply);
    NN_ERR_THROW_FATAL(res);
    DisconnectAndUnlock();
}

void ReplySleepNotificationCompleteToManager(){
    Result res;
    LockAndConnect();
    res = APPLET::ReplySleepNotificationComplete(GetId());
    NN_ERR_THROW_FATAL(res);
    DisconnectAndUnlock();
}

void Enable(bool isSleepEnable){
    NN_TASSERTMSG_(!nn::gxlow::CTR::IsInitialized(), "%s must be called before initializing graphics library\n", NN_FUNCTION );
    NN_TASSERTMSG_(!nn::camera::CTR::detail::IsInitialized(), "%s must be called before initializing camera\n", NN_FUNCTION );
    NN_TASSERTMSG_(!nn::dsp::CTR::IsComponentLoaded(), "%s must be called before loading dspcomponent\n", NN_FUNCTION );
    if(isSleepEnable){
        EnableSleep(false);
    }

    LockAndConnect();
    Result result = APPLET::Enable(GetAttribute());
    NN_ERR_THROW_FATAL(result);
    DisconnectAndUnlock();
    if(nn::applet::CTR::IsApplication() && !(GetAttribute(), & *(AppletAttr*)0x20)){
        AppletId id;
        s32 size;
        SetTransitionType(TRANSITION_ENABLE_APPLET);
        WakeupState state = WaitForStarting(&id,GetInitialParamBuffer(),0x1000,&size);
        SetInitialParamSenderId(id);
        SetInitialParamSenderSize(size);
        SetInitialParamValid();
        SetInitialWakeupState(state);
    }
}

/* Get Applet Things */

AppletId GetHomeMenuAppletId(){
    AppletPos pos;
    AppletId id1; 
    AppletId id2; 
    AppletId id3; 
    LockAndConnect();
    Result res = APPLET::GetAppletManInfo(POS_NONE, &pos, &id1, &id2, &id3);
    NN_ERR_THROW_FATAL(res);
    DisconnectAndUnlock();
    return id2;
}

void GetAppletManInfo(AppletPos requestPos,AppletPos *pCurrentPos,AppletId *pRequestedId,AppletId *pHomeMenuId,AppletId *pCurrentId){
    AppletPos currentPos; AppletId requestedId; AppletId homeMenuId; AppletId currentId; Result result;
    LockAndConnect();
    result = APPLET::GetAppletManInfo(requestPos, &currentPos, &requestedId, &homeMenuId, &currentId);
    NN_ERR_THROW_FATAL(result);
    DisconnectAndUnlock();
    if (pCurrentPos)  *pCurrentPos  = currentPos;
    if (pRequestedId) *pRequestedId = requestedId;
    if (pHomeMenuId)  *pHomeMenuId  = homeMenuId;
    if (pCurrentId)   *pCurrentId   = currentId;
}

bool GetAppletInfo(AppletId appletId, ProgramId* pProgramId, nn::fs::MediaType* pMediaType, bool* pIsUsed, bool* pIsPreloaded, AppletAttr* pAttr){
    Result res;
    ProgramId programId;
    nn::fs::MediaType mediaType;
    bool isUsed;
    bool isPreloaded;
    AppletAttr appletAttr;

    detail::LockAndConnect();
    res = detail::APPLET::GetAppletInfo(appletId, &programId, &mediaType, &isUsed, &isPreloaded, &appletAttr);
    detail::DisconnectAndUnlock();

    if (res.IsSuccess()){
        if (pProgramId)
            *pProgramId = programId;
        if (pMediaType)
            *pMediaType = mediaType;
        if (pIsUsed)
            *pIsUsed = isUsed;
        if (pIsPreloaded)
                *pIsPreloaded = isPreloaded;
        if (pAttr)
            *pAttr = appletAttr;
        return true;
    }
    return false;
}

/* APT Registers */

bool IsRegistered(AppletId id){
    bool isRegistered;

    LockAndConnect();
    Result res = APPLET::IsRegistered(id, &isRegistered);
    NN_ERR_THROW_FATAL(res);
    DisconnectAndUnlock();
    return isRegistered;
}

bool WaitForRegister(AppletId appletId, nn::fnd::TimeSpan span){
    TimeoutChecker checker(span);

    while(!detail::IsRegistered(appletId)){
        if (checker.Check()){
            return false;
        }
        WaitBySleep(10);
    }
    return true;
}

/* Sending Applet Parameters */

Result TrySend(AppletId receiverId, u32 command, const u8* pParam, size_t paramSize, Handle handle){
    bool isFinalize = (command & 0x10000) ? true: false;
    if(paramSize > 0x1000)
        NN_TPANIC_("Too long parameter buffer size");
    if((pParam == NULL) || (paramSize == 0)){
        pParam = 0;
        paramSize = 0;
    }
    Result res;
    LockAndConnect();
    res = APPLET::SendParameter(GetId(), receiverId, command, pParam, paramSize, handle);
    if (res.IsSuccess()){
        if (isFinalize){
            FinalizeClientThread();
        }
    }
    DisconnectAndUnlock();
    return res;
}

Result Send(AppletId receiverId, u32 command, const u8* pParam, size_t paramSize, nn::Handle handle, nn::fnd::TimeSpan timeout){
    TimeoutChecker checker(timeout);
    Result res;
    while(1){
        res = TrySend( receiverId, command, pParam, paramSize, handle );
        if (res.IsSuccess()){
            break;
        }
        else if (res == ResultNotEmpty()){
            if (checker.Check()){
                break;
            }
        }
        else{
             break;
        }

        WaitBySleep(10);
    }

    return res;
}

/* Receiving APT Parameters */

Result TryReceive(AppletId *pSenderId,u32 *pCommand,u8 *pParam,size_t paramSize,s32 *pReadLen, Handle *pHandle,bool isTry){
    Result res;
    if(!isTry)
        WaitForControlEvent();
    else{
        if(!TryWaitForControlEvent()){
            return Result(0xc8a0cfef); // something
        }
    }
    if(!GetMessageCommand()){
        AppletId dummyId;
        pSenderId = (!pSenderId)? &dummyId: pSenderId;

        u32 dummyCommand;
        pCommand = (!pCommand)? &dummyCommand: pCommand;

        u8 dummyParam;
        paramSize = (!pParam)? 0: paramSize;
        pParam = (paramSize==0)? &dummyParam: pParam;

        s32 dummyLen;
        pReadLen = (!pReadLen)? &dummyLen: pReadLen;

        nn::Handle dummyHandle = nn::Handle();
        pHandle = (!pHandle)? &dummyHandle: pHandle;
        LockAndConnect();
        res = APPLET::ReceiveParameter(pSenderId, GetId(), pCommand, pParam, paramSize, pReadLen, pHandle);
        DisconnectAndUnlock();
        if(dummyHandle.IsValid()) svc::CloseHandle(dummyHandle);
    }
    else{
        *pCommand = GetMessageCommand();
        SetMessageCommand(0);
        ClearControlEvent();
        res = ResultSuccess();
        if(pSenderId) *pSenderId = 0;
        if(pReadLen) *pReadLen = 0;
        if(pHandle) *pHandle = HANDLE_NONE;
    }
    ClearControlEvent();
    return res;
}

Result Receive( AppletId* pSenderId, u32* pCommand, u8* pParam, size_t paramSize, s32* pReadLen, nn::Handle *pHandle, nn::fnd::TimeSpan timeout ){
    Result res;
    if (timeout == WAIT_INFINITE){
        res = TryReceive(pSenderId, pCommand, pParam, paramSize, pReadLen, pHandle, false);
        return res;
    }

    TimeoutChecker checker(timeout);
    while(true){
        res = TryReceive(pSenderId, pCommand, pParam, paramSize, pReadLen, pHandle, true);
        if (res.IsSuccess()){
            break;
        }
        else if (res == ResultNoData()){
            if (checker.Check()){
                    break;
            }
        }
        WaitBySleep(10);
    }

    return res;
}

Result Glance(AppletId* pSenderId, u32* pCommand, u8* pParam, size_t paramSize, s32* pReadLen, Handle* pHandle){
    Result res;

    LockAndConnect();
    {
        AppletId tmpSenderId;
        AppletId* pSenderId0 = (pSenderId)? pSenderId: &tmpSenderId;

        u32 tmpCommand;
        u32* pCommand0 = (pCommand)? pCommand: &tmpCommand;

        u8 tmpBuf[1];
        u8* pParam0 = pParam;
        if (pParam == NULL || paramSize == 0 ){
            pParam0 = &tmpBuf[0];
            paramSize = 0;
        }
        s32 tmpReadLen;
        s32* pReadLen0 = (pReadLen)? pReadLen: &tmpReadLen;

        nn::Handle tmpHandle = nn::Handle();
        nn::Handle* pHandle0 = (pHandle)? pHandle: &tmpHandle;

        res = detail::APPLET::GlanceParameter( pSenderId0, GetId(), pCommand0, pParam0, paramSize, pReadLen0, pHandle0 );
        if (tmpHandle.IsValid()){
            svc::CloseHandle(tmpHandle);
        }
    }
    DisconnectAndUnlock();
    return res;
}

/* Cancelling APT Parameters */

bool CancelParameter(bool isSenderCheck, nn::applet::CTR::AppletId senderId, bool isReceiverCheck, nn::applet::CTR::AppletId receiverId){
    bool isCanceled;
    LockAndConnect();
    Result res = APPLET::CancelParameter(isSenderCheck, senderId, isReceiverCheck, receiverId, &isCanceled);
    NN_ERR_THROW_FATAL(res);
    DisconnectAndUnlock();
    return isCanceled;
}

Result SendMessage(AppletId receiverId, const u8* pParam, size_t paramSize, nn::Handle handle, nn::fnd::TimeSpan timeout){
    return detail::Send( receiverId, COMMAND_MESSAGE, pParam, paramSize, handle, timeout );
}

/* APT Utilitys */

inline Result CallUtility(u32 utilityId){
    return CallUtility(utilityId,0,0,0,0,0);
}

inline Result CallUtility(u32 utilityId, u8* pInParam, size_t inParamSize){
    return CallUtility(utilityId, pInParam, inParamSize, 0,0,0);
}

void UnlockTransition(u32 action){
    Result res = CallUtility(7,reinterpret_cast<u8*>(&action), sizeof(action) );
    NN_UNUSED_VAR(res);
}

void LockTransition(u32 action,bool isForced){
    LockTransitionParam param = {action, isForced};
    Result res = CallUtility(5,reinterpret_cast<u8*>(&param), sizeof(LockTransitionParam));
    NN_UNUSED_VAR(res);
}

void SleepIfShellClosed() {
    Result result = CallUtility(4);
    NN_UNUSED_VAR( result );
}

bool IsRetryRequired(Result result){
    bool ret = true;
    if(result == nn::applet::CTR::ResultBusy())
        return ret;
    if(result == nn::applet::CTR::ResultTransitionBusy())
        return ret;
    if(result == nn::applet::CTR::ResultNotEmpty())
        return ret;
    return false;
}

/* Library Applet */

Result CancelLibraryApplet(bool isApplicationEnd){
    Result res;
    SetTransitionType(TRANSITION_CANCEL_APPLIB);
    while(true){
        LockAndConnect();
        res = APPLET::CancelLibraryApplet(isApplicationEnd);
        DisconnectAndUnlock();
        if(!IsRetryRequired(res)) break;
        WaitBySleep(10);
    }
    return res;
}

Result CancelLibraryAppletIfRegistered(bool isApplicationEnd, AppletWakeupState* pWakeupState){
    Result res = ResultSuccess();
    if(pWakeupState){
        *pWakeupState = WAKEUP_SKIP;
    }
    if((!IsApplication() || IsRegistered(0x400)) &&
       (!IsSystemApplet() || IsRegistered(0x200))){
        res = CancelLibraryApplet(isApplicationEnd);
        if(res == ResultSuccess()){
            AppletWakeupState wakeup;
            wakeup = WaitForStarting(NULL,NULL,0,NULL,NULL,CTR::WAIT_INFINITE);
            if(pWakeupState)
                *pWakeupState = wakeup;
            
        }
    }
    return res;
}

/* SystemApplet */

Result PrepareToStartSystemApplet(AppletId id){
    Result res;
    nngxGetIsRunning();
    CancelLibraryAppletIfRegistered(false);
    SetTransitionType(TRANSITION_START_SYS);
    bool sleep = DisableSleepForTransition();
    while(true){
        LockAndConnect();
        res = APPLET::PrepareToStartSystemApplet(id);
        DisconnectAndUnlock();
        if(!IsRetryRequired(res)) break;
        WaitBySleep(10);
    }
    RestoreSleepForTransition(sleep);
    if(IsApplication()){
        if(res == Result(0xc8a0cffc)){
            res = ResultSuccess();
        }
    }
    return res;
}

/* StartSystemApplet */

Result StartSystemApplet(AppletId id,u8* pParam,size_t paramSize,Handle h){
    Result res;

    if (!IsApplication() && !IsSystemApplet())
        return Result(0xC8A0CC04);

    if (IsSystemApplet()){
        AssignGpuRight(false);
    }
    else if (IsApplication()){
        SaveVramSysArea();

        AssignDspRight(false);
        AssignGpuRight(false);
        AssignCameraRight(false);
    }

    bool sleepEnabled = IsEnableSleep();

    if (sleepEnabled)
        DisableSleep(true);

    while (true){
        LockAndConnect();
        res = APPLET::StartSystemApplet(id,pParam,paramSize,h);
        DisconnectAndUnlock();
        if (!IsRetryRequired(res)) break;
        WaitBySleep(10);
    }

    if (sleepEnabled){
        if (!IsEnableSleep())
            EnableSleep(true);
    }
    else{
        if (IsEnableSleep())
            DisableSleep(true);
    }

    NN_ERR_THROW_FATAL_ALL(res);
    SetInactive();
    if (IsApplication())
        res = CaptureScreenForSystemApplet(id);

    if (IsSystemApplet())
        svc::ExitProcess();

    return res;
}

/* Close Applet */

Result PrepareToCloseApplication(bool isCancelPreload){
    CancelLibraryAppletIfRegistered(false);
    SetTransitionType(TRANSITION_CLOSE_APP);
    Result res;
    while(true){
        LockAndConnect();
        res = APPLET::PrepareToCloseApplication(!isCancelPreload);
        DisconnectAndUnlock();
        if(!IsRetryRequired(res)) break;
        WaitBySleep(10);
    }
    NN_ERR_THROW_FATAL(res);
    return res;
}

Result CloseApplication(u8* pParam, size_t paramSize, Handle handle){
    if(GetTransitionType() != TRANSITION_CLOSE_APP){
        PrepareToCloseApplication(false);
    }
    CloseAppletHook();
    AssignGpuRight(false);
    Result res;
    while(true){
        LockAndConnect();
        res = APPLET::CloseApplication(pParam, paramSize, handle);
        DisconnectAndUnlock();
        if(!IsRetryRequired(res)) break;
        WaitBySleep(10);
    }
    NN_ERR_THROW_FATAL(res);
    SetInactive();
    svc::ExitProcess();
    return res;
}

/* Starting a Library APT, WIP */

Result StartLibraryApplet(AppletId id, const u8* pParam,size_t paramSize, Handle handle){
    /*Result res;

    if((!IsApplication()) && (!IsSystemApplet()))
        return Result(0xc8a0cc04);
    
    if(IsApplication())
        SaveVramSysArea();
    
    CaptureScreen(id);
    AssignGpuRight(false);
    bool sleep = DisableSleepForTransition();
    while(true){
        LockAndConnect();
        res = APPLET::StartLibrary(id,pParam,paramSize, handle);
        DisconnectAndUnlock();
        if(!IsRetryRequired(res)) break;
        WaitBySleep(10);
    }
    RestoreSleepForTransition(sleep);
    NN_ERR_THROW_FATAL(res);
    SetInactive();

    return res;*/
}

/* Jumping to HomeMenu */

Result PrepareToJumpToHomeMenu(){
    SetTransitionType(TRANSITION_JUMP_HOME);
        
    Result res;
    while(true){
        LockAndConnect();
        res = APPLET::PrepareToJumpToHomeMenu();
        DisconnectAndUnlock();
        if (!IsRetryRequired(res)) break;
            WaitBySleep(10);
    }
    return res;
}

Result JumpToHomeMenu(u8* pParam, size_t paramSize, Handle handle){
    AppletId appletId; AppletId homemenuId; 
    Result res; 
    Handle hand_local;
    appletId = CTR::detail::GetHomeMenuAppletId();
    if(IsApplication()){
        while(!IsRegistered(appletId)){
            WaitBySleep(10);
        }
        SaveVramSysArea();
        res = CaptureScreenForSystemApplet(appletId);
    }
    AssignDspRight(false);
    AssignGpuRight(false);
    AssignCameraRight(false);
    LockAndConnect();
    res = APPLET::JumpToHomeMenu(pParam, paramSize, handle);
    NN_ERR_THROW_FATAL(res);
    DisconnectAndUnlock();
    SetInactive();
    return res;
}

void NotifyToWait(){
    Result res;
    LockAndConnect();
    res = APPLET::NotifyToWait(GetId());
    NN_ERR_THROW_FATAL(res);
    DisconnectAndUnlock();
}

Result SendCaptureBufferInfo(u8* pParam, size_t paramSize){
    Result res;
    LockAndConnect();
    res = APPLET::SendCaptureBufferInfo(pParam,paramSize);
    DisconnectAndUnlock();
    return res;
}

Result CallUtility(u32 utilityId, u8* pInParam, size_t inParamSize, u8* pOutParam, size_t outParamSize, s32* pReadSize){
    Result res;
    u8 dummyInParam[1];
    u8 dummyOutParam[1];
    s32 dummyReadSize;
    size_t paramInSize;
    size_t paramOutSize;
    u8* pParamOut;
    LockAndConnect();
    {
        res = APPLET::AppletUtility(utilityId,(pInParam && inParamSize>0)? pInParam: dummyInParam, (pInParam && inParamSize>0)? inParamSize: 1,(pOutParam && outParamSize>0)? pOutParam: dummyOutParam, (pOutParam && outParamSize>0)? outParamSize: 1, &dummyReadSize);
    }
    NN_ERR_THROW_FATAL(res);

    if (!pOutParam || outParamSize==0) dummyReadSize = 0;
    if(pReadSize) *pReadSize = dummyReadSize;

    DisconnectAndUnlock();

    return res;
}

} // detail
} // CTR
} // applet
} // nn