#pragma once

#include "nn/Result.h"
#include "nn/err/CTR/err_Api.h"
#include "nn/applet/CTR/applet_Paramaters.h"
#include "nn/os/os_CriticalSection.h"
#include "nn/fnd/fnd_TimeSpan.h"

namespace nn { 
namespace applet {
namespace CTR {

    typedef bool (*AppletCommandCallback)(uptr parameter, AppletId id, AppletCommand command, u8 pParam[], size_t paramSize, s32 readLen, nn::Handle handle);

    void InitializeWrapper();

    void EnableSleep(bool isSleepCheck);
    void SetSleepQueryCallback(AppletSleepQueryCallback callback,uptr arg);
    void DisableSleep(bool isReplyReject);
    void SetAwakeCallback(AppletAwakeCallback callback,uptr arg);
    //void SetSleepCanceledCallback(AppletSleepCanceledCallback callback, uptr arg);

    bool ReceiveCallbackForCommands(uptr callback);

    bool ProcessPowerButton();
    bool ProcessPowerButtonAndWait();

    bool ProcessHomeButton();
    bool ProcessHomeButtonAndWait();
    
    void ClearHomeButtonState();
    bool IsExpectedToProcessHomeButton();
    void ReplySleepQuery(QueryReply reply);
    SleepNotificationState IsExpectedToReplySleepQuery();

    void CloseAppletHook();
    
    bool IsEnableSleep();

    void SetCommandCallback(s32 callback, uptr arg);


class SysSleepAcceptedCallbackInfo{
public:
    SysSleepAcceptedCallbackInfo(AppletSysSleepAcceptedCallback callback, uptr parameter, int priority=DEFAULT_PRIORITY):
    m_pPrev(NULL), m_pNext(NULL),m_Callback(callback),m_Parameter(parameter),m_Priority(priority)
    {
    }
    ~SysSleepAcceptedCallbackInfo()
    {
    }
    enum {
        MIN_PRIORITY = 0,
        MAX_PRIORITY = 0xFFFF,
        DEFAULT_PRIORITY = 0x8000
    };

    SysSleepAcceptedCallbackInfo* m_pPrev;
    SysSleepAcceptedCallbackInfo* m_pNext;
    AppletSysSleepAcceptedCallback m_Callback;
    uptr m_Parameter;
    int m_Priority;

    void Register();
    void Unregister();
    static void CallCallbacks();
    void Call(){
        if(m_Callback)
            m_Callback(m_Parameter);
    }

    static SysSleepAcceptedCallbackInfo* s_pHead;
    static SysSleepAcceptedCallbackInfo* s_pTail;

    int GetPriority() const{ return m_Priority; }

    void SetNext(SysSleepAcceptedCallbackInfo* p){ m_pNext = p;  return; }
    void SetPrev(SysSleepAcceptedCallbackInfo* p){ m_pPrev = p;  return; }
        
    SysSleepAcceptedCallbackInfo* GetNext(){ return m_pNext; }
    SysSleepAcceptedCallbackInfo* GetPrev(){ return m_pPrev; }

    static SysSleepAcceptedCallbackInfo* GetHead() { return s_pHead; }
    static SysSleepAcceptedCallbackInfo* GetHead() { return s_pTail; }
};

namespace detail{
    // TODO
    Result CaptureScreenForSystemApplet(AppletId id);
    Result WaitToCaptureScreen(AppletId id, Handle* pHandle);
    bool ReceiveCallbackForCommands(uptr ptr);
    void WaitForAppletPreloaded(AppletId id);
    AppletWakeupState WaitForStarting(AppletId* pSenderId,  u8* pParam, size_t paramSize, s32* pReadLen, Handle* pHandle, fnd::TimeSpan timeout);

}
}
}
}