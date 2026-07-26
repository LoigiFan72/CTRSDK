#pragma once

#include "nn/Result.h"
#include "nn/err/CTR/err_Api.h"
#include "nn/applet/CTR/applet_Paramaters.h"
#include "nn/os/os_CriticalSection.h"
#include "nn/fnd/fnd_TimeSpan.h"

namespace nn { 
namespace applet {
namespace CTR {
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
        enum {
            MIN_PRIORITY = 0,
            MAX_PRIORITY = 0xFFFF,
            DEFAULT_PRIORITY = 0x8000
        };
        SysSleepAcceptedCallbackInfo* mPrev;
        SysSleepAcceptedCallbackInfo* mNext;
        AppletSysSleepAcceptedCallback mCallback;
        uptr mParamater;
        int mPriority;

        void Register();
        void Unregister();
        static void CallCallbacks();
        void Call(){
            if(this->mCallback)
                this->mCallback(this->mParamater);
        }

        int GetPriority() const{ return mPriority; }

        void SetNext(SysSleepAcceptedCallbackInfo* p){ this->mNext = p;  return; }
        void SetPrev(SysSleepAcceptedCallbackInfo* p){ this->mPrev = p;  return; }
        SysSleepAcceptedCallbackInfo* GetNext(){ return this->mNext; }
        SysSleepAcceptedCallbackInfo* GetPrev(){ return this->mPrev; }

        static SysSleepAcceptedCallbackInfo* spHead;
        static SysSleepAcceptedCallbackInfo* spTail;
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