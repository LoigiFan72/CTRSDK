// Filename: applet_Wrapper.cpp
//
// Project: Horizon

#include <nn/applet.h>
#include <nn/err.h>
#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/os.h>
#include <nn/gx.h>
#include <nn/dsp.h>
#include <nn/types.h>
#include <nn/gx.h>

#include <nn/applet/CTR/applet_Connect.h>
#include <nn/applet/CTR/applet_Info.h>
#include <nn/applet/CTR/applet_Gfx.h>
#include <nn/applet/CTR/applet_ClientThread.h>
#include <nn/applet/CTR/applet_Paramaters.h>
#include <nn/gxlow/CTR/gxlow_SystemUse.h>

namespace nn{
namespace applet{
namespace CTR{
namespace{
    bool sleepEnable;
    int homeButtonCallback;
    int requestMemoryCallback;
    int receiveMessageCallback;
    int dspSleepCallback;
    int dspWakeUpCallback;
    AppletSleepQueryCallback sleepQueryCallback;
    int sleepCanceledCallback;
    int sleepAcceptedCallback;
    AppletAwakeCallback awakeCallback;
    int shutdownCallback;
    int powerButtonCallback;
    int transitionCallback;
    int closeCallback;
    int releaseMemoryCallback;
    int closeAppletCallback;
    int commandCallback;
    int homeButtonCallbackArg;
    int requestMemoryCallbackArg;
    int receiveMessageCallbackArg;
    int dspSleepCallbackArg;
    int dspWakeUpCallbackArg;
    int sleepQueryCallbackArg;
    int sleepCanceledCallbackArg;
    int sleepAcceptedCallbackArg;
    int awakeCallbackArg;
    int shutdownCallbackArg;
    int powerButtonCallbackArg;
    int transitionCallbackArg;
    int closeCallbackArg;
    int releaseMemoryCallbackArg;
    int closeAppletCallbackArg;
    int commandCallbackArg;

}
namespace{
    static os::CriticalSection sSleepAcceptedCriticalSection = nn::WithInitialize();
}

namespace{
    u8 paramBuffer[4096];
}

void SetCommandCallback(int callback, uptr arg){
    commandCallback = callback;
    commandCallbackArg = arg;
}

void SetHomeButtonCallback(int callback, uptr arg){
    homeButtonCallback = callback;
    homeButtonCallbackArg = arg;
}

void SetReceiveMessageCallback(int callback, uptr arg){
    receiveMessageCallback = callback;
    receiveMessageCallbackArg = arg;
}

void SetRequestMemoryCallback(int callback, uptr arg){
    requestMemoryCallback = callback;
    requestMemoryCallbackArg = arg;
}

void SetDspSleepCallback(int callback, uptr arg){
    dspSleepCallback = callback;
    dspSleepCallbackArg = arg;
}

void SetDspWakeUpCallback(int callback, uptr arg){
    dspWakeUpCallback = callback;
    dspWakeUpCallbackArg = arg;
}

void SetSleepQueryCallback(AppletSleepQueryCallback callback, uptr arg){
    sleepQueryCallback = callback;
    sleepQueryCallbackArg = arg;
}

void SetSleepCanceledCallback(int callback, uptr arg){
    sleepCanceledCallback = callback;
    sleepCanceledCallbackArg = arg;
}

void SetSleepAcceptedCallback(int callback, uptr arg){
    sleepAcceptedCallback = callback;
    sleepAcceptedCallbackArg = arg;
}

void SetAwakeCallback(AppletAwakeCallback callback, uptr arg){
    awakeCallback = callback;
    awakeCallbackArg = arg;
}

void SetShutdownCallback(int callback, uptr arg){
    shutdownCallback = callback;
    shutdownCallbackArg = arg;
}

void SetPowerButtonCallback(int callback, uptr arg){
    powerButtonCallback = callback;
    powerButtonCallbackArg = arg;
}

void SetTransitionCallback(int callback, uptr arg){
    transitionCallback = callback;
    transitionCallbackArg = arg;
}

void SetCloseCallback(int callback, uptr arg){
    closeCallback = callback;
    closeCallbackArg = arg;
}

void ReplySleepQuery(QueryReply reply) {
    SleepNotificationState state;
    if (reply == REPLY_REJECT) {
        state = NOTIFY_SLEEP_REJECT;
    } 
    else {
        if (reply != REPLY_ACCEPT) {
            if (reply != REPLY_LATER)
                return;
            SetSleepNotificationState(NOTIFY_SLEEP_QUERY);
            return;
        }
        state = NOTIFY_SLEEP_ACCEPT;
    }
    SetSleepNotificationState(state);
    detail::ReplySleepQueryToManager(reply);
}

SleepNotificationState IsExpectedToReplySleepQuery(){
    SleepNotificationState state = GetSleepNoticationState();
    if(state != NOTIFY_SLEEP_QUERY){
        state = NOTIFY_NONE;
    } return state;
}

bool IsExpectedToCloseApplication();

/* Inlines */

bool ProcessHomeButtonCommand(){
    AppletHomeButtonState state;
    int callback;
    bool active = detail::IsActive();
    state = detail::GetAbsoluteHomeButtonState();
    detail::ClearAbsoluteHomeButtonState();
    callback = 1;
    if(homeButtonCallback != 0){
        callback = reinterpret_cast<int(*)(int,int,int)>(homeButtonCallback)(homeButtonCallbackArg,active,state);
    }
    if(((callback != 0) && (active)) && (GetHomeButtonState() == HOME_BUTTON_NONE)){
        SetHomeButtonState(state);
        if(GetHomeButtonState() != HOME_BUTTON_SINGLE_PRESSED){
            GetHomeButtonState();
        }
    }
    return false;
}

inline void CallTransitionCallback(){
    if(transitionCallback != 0){
        reinterpret_cast<void(*)(int)>(transitionCallback)(transitionCallbackArg);
    }    
}

inline void ProcessShutdownCommand(){
    if(shutdownCallback != 0){
        reinterpret_cast<void(*)(int)>(shutdownCallback)(shutdownCallbackArg);
    }
}

inline void ProcessSleepQueryCommand(){
    AppletQueryReply Callback;
    if(!IsExpectedToCloseApplication()){
            if(sleepQueryCallback != 0)
                Callback = reinterpret_cast<AppletQueryReply(*)(int)>(sleepQueryCallback)(sleepQueryCallbackArg);
            if(!IsEnableSleep() && detail::IsActive())
                Callback = REPLY_REJECT;
    }
    ReplySleepQuery(Callback);
}

inline void ProcessSleepCanceledCommand(){
    if(sleepCanceledCallback){
        if(!IsExpectedToCloseApplication())
            reinterpret_cast<void(*)(int)>(sleepCanceledCallback)(sleepCanceledCallbackArg);
    }
}

inline void ProcessSleepAcceptedCommand(){
    SetSleepNotificationState(NOTIFY_SLEEP_ACCEPTED);
    if(sleepAcceptedCallback != 0){
        reinterpret_cast<void(*)(int)>(sleepAcceptedCallback)(sleepAcceptedCallbackArg);
    }
    SysSleepAcceptedCallbackInfo::CallCallbacks();
    detail::ReplySleepNotificationCompleteToManager();
}

inline void ProcessAwakeCommand(){
    //dsp::CTR::Awake();
    if(awakeCallback != 0){
        reinterpret_cast<void(*)(int)>(awakeCallback)(awakeCallbackArg);
    }
    SetSleepNotificationState(NOTIFY_AWAKE);
}

inline void ProcessCloseCommand(){
    if (closeCallback != 0){
        reinterpret_cast<void(*)(int)>(closeCallback)(closeCallbackArg);
    }
}

/* Applet Wrapper Functions */

void InitializeWrapper(){
    detail::SetReceiveCallback(ReceiveCallbackForCommands,0);
    SetHomeButtonCallback(NULL, 0);
    SetRequestMemoryCallback(NULL, 0);
    SetReceiveMessageCallback(NULL, 0);
    SetDspSleepCallback(NULL, 0);
    SetDspWakeUpCallback(NULL, 0);
    SetSleepQueryCallback(NULL, 0);
    SetSleepCanceledCallback(NULL, 0);
    SetSleepAcceptedCallback(NULL, 0);
    SetAwakeCallback(NULL, 0);
    SetShutdownCallback(NULL, 0);
    SetPowerButtonCallback(NULL, 0);
    SetTransitionCallback(NULL, 0);
    SetCloseCallback(NULL, 0);
    SetCommandCallback(NULL, 0);
}

void ClearHomeButtonState(void) {
    nn::applet::CTR::SetHomeButtonState(HOME_BUTTON_NONE);
    nn::applet::CTR::detail::UnlockTransition(1);
    nn::applet::CTR::detail::SleepIfShellClosed();
}

bool ProcessPowerButton(){
    AppletId reqId;
    AppletId homeId;
    detail::CancelLibraryAppletIfRegistered(false);
    CallTransitionCallback();
    detail::GetAppletManInfo(POS_SYS,0,&reqId,&homeId,0);
    if((!reqId) || (reqId == homeId)){
        detail::PrepareToJumpToHomeMenu();
        detail::JumpToHomeMenu(0,0,HANDLE_NONE);
    }
    else{
        detail::PrepareToStartSystemApplet(reqId);
        detail::StartSystemApplet(reqId, 0,0,HANDLE_NONE);
    }
    return true;
}

bool ProcessPowerButtonAndWait(){
    bool sleep = IsEnableSleep();
    if(sleep){
        sleepEnable = false;
        DisableSleep(true);
    }
    ProcessPowerButton();
    detail::WaitForStarting(0,0,0,0,0,WAIT_INFINITE);
    if(sleep){
        EnableSleep(true);
    }
}

NN_NOINLINE s8 IsToCallShutdownCallback(){
    return sIsToCallPowerButtonCallback;
}

bool ReceiveCallbackForCommands(uptr callback){
    AppletHomeButtonState hbState;
    AppletSleepSysState sleepState;

    hbState = detail::GetAbsoluteHomeButtonState();
    if(IsToCallShutdownCallback()){
        ProcessShutdownCommand();
        ClearShutdownCallbackFlag();
    }

    if(detail::GetOrderToCloseState()){
        ProcessCloseCommand();
        return true;
    }
    else if(hbState == HOME_BUTTON_SINGLE_PRESSED || hbState == HOME_BUTTON_DOUBLE_PRESSED){
        return ProcessHomeButtonCommand();
    }
    else if(detail::GetSleepSysState()){
        sleepState = detail::GetSleepSysState();
        switch(sleepState){
        case SLEEP_SYS_STATE_QUERY:
            ProcessSleepQueryCommand();
            break;
        case SLEEP_SYS_STATE_ACCEPTED:
            ProcessSleepAcceptedCommand();
            break;
        case SLEEP_SYS_STATE_AWAKE:
            ProcessAwakeCommand();
            break;
        case SLEEP_SYS_STATE_CANCELED:
            ProcessSleepCanceledCommand();
            break;
        }
        detail::ClearSleepSysState();
        return false;
    }
    else if(IsToCallPowerButtonCallback()){
        ProcessPowerButton();
        ClearPowerButtonCallbackFlag();
    }
    else{
        s32 *pReadLena;
        s32 *pReadLen;
        Handle h;
        s32 readLen;
        u32 command;
        nn::applet::CTR::AppletId senderId;

        Result res = detail::Glance(&senderId,&command, paramBuffer, 0x1000u, &readLen, &h);
        NN_ERR_THROW_FATAL(res);
        switch(command){
            case 2:
                
        }
    }
}

bool ProcessHomeButton(){
    if(IsExpectedToProcessHomeButton()){
        SetExpectationToJumpToHome(false);
        AppletAttr attr = GetAppletType();
        if((attr == HOME_BUTTON) || (attr = GetAppletType(), attr == MII_SELECT)){
            detail::CancelLibraryAppletIfRegistered(false);
            CallTransitionCallback();
        }
        detail::PrepareToJumpToHomeMenu();
        detail::JumpToHomeMenu(0,0,HANDLE_NONE);
        return true;
    }
}

bool ProcessHomeButtonAndWait(){
    bool sleep = IsEnableSleep();
    if(sleep){
        sleepEnable = false;
        DisableSleep(true);
    }
    ProcessHomeButton();
    detail::WaitForStarting(0,0,0,0,0,WAIT_INFINITE);
    if(sleep){
        EnableSleep(true);
    }
}

void DisableSleep(bool isReplyReject){
    if(!isReplyReject){
        return;
    }
    SetSleepNotificationState(NOTIFY_SLEEP_REJECT);
    detail::ReplySleepQueryToManager(REPLY_REJECT);
}

void EnableSleep(bool isSleepCheck){
    sleepEnable = 1;
    if(isSleepCheck){
        detail::SleepIfShellClosed();
        return;
    }
    return;
}

bool IsEnableSleep(){
    return sleepEnable;
}

bool IsExpectedToCloseApplication(){
    return nn::applet::CTR::detail::GetOrderToCloseState() || nn::applet::CTR::IsReceivedWakeupByCancel();
}

bool IsExpectedToProcessHomeButton(){
    bool Homemenu = IsExpectedToJumpToHomeMenu();
    HomeButtonState hbState;
    if((Homemenu != 0) || (hbState = GetHomeButtonState(), hbState != HOME_BUTTON_NONE)){
        hbState = HOME_BUTTON_SINGLE_PRESSED;
    }
    return hbState;
}

void CloseAppletHook(){
    if (releaseMemoryCallback != 0){
        reinterpret_cast<void(*)(int)>(releaseMemoryCallback)(releaseMemoryCallbackArg);
    }

    if (closeAppletCallback == 0)
        return;

    reinterpret_cast<void(*)(int)>(closeAppletCallback)(closeAppletCallbackArg);
}

//
// SysSleepAcceptedCallbackInfo 
//

SysSleepAcceptedCallbackInfo* SysSleepAcceptedCallbackInfo::spHead = NULL;
SysSleepAcceptedCallbackInfo* SysSleepAcceptedCallbackInfo::spTail = NULL;

void SysSleepAcceptedCallbackInfo::Unregister(){
    nn::os::CriticalSection::ScopedLock lock(sSleepAcceptedCriticalSection);
    SysSleepAcceptedCallbackInfo* p = spHead;
    while (p){
        if (p == this){
            SysSleepAcceptedCallbackInfo* prev = p->mPrev;
            SysSleepAcceptedCallbackInfo* next = p->mNext;

            if (!prev)
                spHead = next;
            else
                prev->mNext = next;
            if (!next)
                spTail = prev;
            else
                next->mPrev = prev;

            p->mPrev = NULL;
            p->mNext = NULL;
            return;
        }
        p = p->mNext;
    }
}

void SysSleepAcceptedCallbackInfo::Register(){
    os::CriticalSection::ScopedLock lock(sSleepAcceptedCriticalSection);
    if (GetPrev() || GetNext() || spHead == this ){
        return;
    }

    SysSleepAcceptedCallbackInfo* p = spHead;

    NN_ASSERT_(MIN_PRIORITY <= GetPriority() && GetPriority() <= MAX_PRIORITY);
    if (!p){
        spHead = this;
        spTail = this;
        this->SetPrev(NULL);
        this->SetNext(NULL);
        return;
    }

    for (; p; p=p->GetNext()){
         NN_TASSERT_(p != this);

        if (p->mPriority > mPriority){
            this->SetPrev(p->GetPrev());
            this->SetNext(p);

            if (p->GetPrev() == NULL){
                spHead = this;
            }
            else{
                p->GetPrev()->SetNext(this);
            }
            p->SetPrev(this);
            return;
        }
    }

    spTail->SetNext(this);
    SetPrev(spTail);
    SetNext(NULL);
    spTail = this;
}

inline void SysSleepAcceptedCallbackInfo::CallCallbacks(){
    os::CriticalSection::ScopedLock locker(sSleepAcceptedCriticalSection);
    for(SysSleepAcceptedCallbackInfo* p = spHead; p; p = p->GetNext()){
        p->Call();
    }
}

namespace detail{

namespace{
    CaptureBufferInfo cInfo;
}

bool IsAppletPreloaded(AppletId id){
    bool isUsed;
    bool isPreloaded;
    bool r = GetAppletInfo( id, NULL, NULL, &isUsed, &isPreloaded, NULL );
    return ( r && isUsed && isPreloaded )? true: false;
}

void WaitForAppletPreloaded( AppletId id ){
    bool e = IsEnableSleep();
    if (e){
        DisableSleep(REPLY_REJECT_IF_LATER);
    }
    while(!IsAppletPreloaded(id)){
        nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(10));
    }
    if (e){
        EnableSleep(SLEEP_IF_SHELL_CLOSED);
    }
}

Result ReceiveCommand(bool* pIsToDo, AppletId* pSenderId, AppletCommand* pCommand, u8 pParam[], size_t paramSize, s32* pReadLen,nn::Handle* pHandle, nn::fnd::TimeSpan span=WAIT_INFINITE){
    bool callbackResult = true;
    AppletId senderId;
    AppletCommand command;
    s32 readLen;
    nn::Handle handle;

    Result res = nn::applet::CTR::detail::Receive( &senderId, &command, pParam, paramSize, &readLen, &handle, span );

    if (res.GetDescription() == Result::DESCRIPTION_NO_DATA){
        return res;
    }

    if (res.IsSuccess()){
        if (commandCallback){
            reinterpret_cast<void (*)(uptr, u32, u32, void*, u32, u32, Handle)>(commandCallback)(commandCallbackArg,senderId,command,pParam,paramSize,readLen,handle);
        }
    }

    if (pIsToDo)
        *pIsToDo = callbackResult;
    if (pCommand)
        *pCommand = command;
    if (pSenderId)
        *pSenderId = senderId;
    if (pReadLen)
        *pReadLen = readLen;
    if (pHandle)
        *pHandle = handle;

        return res;
    }

AppletWakeupState WaitForStarting(AppletId* pSenderId,  u8* pParam, size_t paramSize, s32* pReadLen, Handle* pHandle, fnd::TimeSpan timeout){
    Result res;
    bool isToDo;
    AppletCommand command = 0;
    AppletId sender = 0;

    TransitionType type = GetTransitionType();
    SetTransitionType(TRANSITION_NONE);
    if(type == TRANSITION_SKIP){
        if(pSenderId)
            *pSenderId = 0;
        if(pReadLen)
            *pReadLen = 0;
        if(pHandle)
          *pHandle = HANDLE_NONE;
        return WAKEUP_SKIP;
    }
    NotifyToWait();
    SetInactive();
    if(type == TRANSITION_ENABLE_APPLET)
        SetTransitionType(TRANSITION_NONE);
    else
        SleepIfShellClosed();
    while(true){
        isToDo = true;
        res = ReceiveCommand(&isToDo, &sender, &command, pParam, paramSize, pReadLen, pHandle, timeout);
        if (!isToDo){
            continue;
        }
        if (pSenderId){
            *pSenderId = sender;
        }

        if (!res.IsSuccess()){
            break;
        }
        if (command == COMMAND_WAKEUP || command == COMMAND_WAKEUP_BY_PAUSE || command == COMMAND_WAKEUP_BY_EXIT ||command == COMMAND_WAKEUP_BY_CANCEL ||
                 command == COMMAND_WAKEUP_BY_CANCELALL || command == COMMAND_WAKEUP_TO_JUMP_HOME || command == COMMAND_WAKEUP_BY_POWER_BUTTON_CLICK ||
                 command == COMMAND_WAKEUP_TO_LAUNCH_APPLICATION || command == COMMAND_WAKEUP_BY_TERMINATION_APPLICATION || command == COMMAND_WAKEUP_BY_TERMINATION_SYSTEM_APPLET ){
            SetActive();
            break;
        }
    }

    if (res.GetDescription() == Result::DESCRIPTION_NO_DATA){
        return WAKEUP_BY_TIMEOUT;
    }

    if (GetAppletType() == TYPE_APP &&  type != TRANSITION_CANCEL_APPLIB ){
        if (command != COMMAND_WAKEUP_BY_CANCEL && command != COMMAND_WAKEUP ){
            AssignGpuRight(true);

            res = gxlow::CTR::RestoreVramSysArea();
            NN_ERR_THROW_FATAL(res);
        }
    }
    AppletWakeupState wstate;
    switch(command){
    case COMMAND_WAKEUP:
        wstate = WAKEUP_TO_START;
        break;

    case COMMAND_WAKEUP_BY_EXIT:
        wstate = WAKEUP_BY_EXIT;
        break;

    case COMMAND_WAKEUP_BY_PAUSE:
        wstate = WAKEUP_BY_PAUSE;
        break;

    case COMMAND_WAKEUP_BY_CANCEL:
        wstate = WAKEUP_BY_CANCEL;
        dsp::CTR::OrderToWaitForFinalize();
        SetReceivedWakeupByCancelFlag();
        break;

    case COMMAND_WAKEUP_BY_CANCELALL:
        wstate = WAKEUP_BY_CANCELALL;
        dsp::CTR::OrderToWaitForFinalize();
        break;

    case COMMAND_WAKEUP_TO_JUMP_HOME:
        wstate = WAKEUP_TO_JUMP_HOME;
        SetHomeButtonState(HOME_BUTTON_SINGLE_PRESSED);
        SetExpectationToJumpToHome(true);
        break;

    case COMMAND_WAKEUP_BY_POWER_BUTTON_CLICK:
        wstate = WAKEUP_BY_POWER_BUTTON_CLICK;
        break;

    case COMMAND_WAKEUP_TO_LAUNCH_APPLICATION:
        wstate = WAKEUP_TO_LAUNCH_APPLICATION;
        break;

    case COMMAND_WAKEUP_BY_TERMINATION_APPLICATION:
        wstate = static_cast<WakeupState>(WAKEUP_BY_TERMINATION_APPLICATION);
        break;

    case COMMAND_WAKEUP_BY_TERMINATION_SYSTEM_APPLET:
        wstate = static_cast<WakeupState>(WAKEUP_BY_TERMINATION_SYSTEM_APPLET);
        break;

    default:
        wstate = WAKEUP_TO_START;
        break;
    }
    if (!(wstate == WAKEUP_BY_CANCEL || wstate == WAKEUP_BY_CANCELALL || wstate == WAKEUP_TO_LAUNCH_APPLICATION)){
        AssignDspRight(true);

        if (type == TRANSITION_JUMP_HOME || type == TRANSITION_START_SYS ){
            AssignCameraRight(true);
        }
    }

    if (wstate == WAKEUP_TO_START || wstate == WAKEUP_BY_EXIT || wstate == WAKEUP_BY_PAUSE || wstate == WAKEUP_BY_CANCEL ||
        wstate == WAKEUP_BY_CANCELALL || wstate == WAKEUP_BY_POWER_BUTTON_CLICK || wstate == WAKEUP_TO_LAUNCH_APPLICATION ||
        wstate == WAKEUP_BY_TERMINATION_APPLICATION || wstate == WAKEUP_BY_TERMINATION_SYSTEM_APPLET){
    u32 lock = 0;

        switch (GetAppletType()){
        case TYPE_APP:
            lock |= LOCK_TRANSITION_TO_APP;
            break;
        case TYPE_SYS:
            lock |= LOCK_TRANSITION_TO_SYS;
            break;
        case TYPE_APPLIB:
            lock |= LOCK_TRANSITION_TO_APPLIB;
            break;
        case TYPE_SYSLIB:
            lock |= LOCK_TRANSITION_TO_SYSLIB;
            break;
        }
        UnlockTransition( lock );

        SleepIfShellClosed();
    }

    if (wstate == WAKEUP_TO_JUMP_HOME ){
        LockTransition(LOCK_TRANSITION_HOME_BUTTON, true );
    }

    if (type == TRANSITION_JUMP_HOME || type == TRANSITION_START_APPLIB || type == TRANSITION_START_SYS || type == TRANSITION_JUMP_APP ){
        if (!(wstate == WAKEUP_BY_CANCEL || wstate == WAKEUP_BY_CANCELALL ) ){
            AssignGpuRight(true);
        }

        if (GetAppletType() == TYPE_APP){
            if (wstate != WAKEUP_TO_JUMP_HOME){
                ClearHomeButtonState();
            }
        }
    }

    return wstate;
}

Result WaitToCaptureScreen(AppletId id, nn::Handle* pHandle){
    bool isToDo;
    AppletCommand command = COMMAND_NONE;
    Result res;

    while(1){
        AppletId sender = 0;
        res = ReceiveCommand(&isToDo, &sender, &command, NULL, 0, NULL, pHandle);
        if (!isToDo){
            continue;
        }

        if (!res.IsSuccess()){
            break;
        }

        if (command == COMMAND_RESPONSE){
            if (id == sender){
                break;
            }
        }
    }
    return res;
}

Result RequestCaptureBuffer(AppletId id, CaptureBufferInfo* info, AppletCommand command){
    Send(id,command,reinterpret_cast<u8*>(info), 0x20, HANDLE_NONE, CTR::WAIT_INFINITE);
}

Result CaptureScreenForSystemApplet(AppletId id){
    AppletDisplayInfo info;
    GetDisplayInfo(&info);

    WaitForRegister(id);
    cInfo.d[0].mode   = info.d[0].mode;
    cInfo.d[1].mode   = info.d[1].mode;

    if (info.d[0].addr != info.d[0].addrB)
        cInfo.is3DCapture = true;
    else
        cInfo.is3DCapture = false;
    CalcCaptureBufferInfo(&cInfo);
    RequestCaptureBuffer(id,&cInfo,0x10);
    WaitToCaptureScreen(id, 0);
    SendCaptureBufferInfo(reinterpret_cast<u8*>(&cInfo),0x20);

    return ResultSuccess();
}
}
}
}
}