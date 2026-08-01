// Filename: gxlow_Management.cpp
//
// Project: Horizon

#include <nn/gxlow/CTR/gxlow_CTR.h>
#include <nn/gx/CTR/gx_CTRPrivate.h>
#include <nn/gxlow/CTR/gxlow_SystemUse.h>
#include <nn/gxlow/CTR/gxlow_Management.h>
#include <nn/gxlow/CTR/gxlow_InterruptReceiver.h>
#include <nn/gxlow/CTR/gxlow_Result.h>
#include <nn/srv.h>
#include <nn/assert.h>

namespace nn {
namespace gxlow {
namespace CTR {
namespace{
    bool sInitialized = false;
    bool sIsAppletMode = false;
    bool sIsFatalErrMode = false;
    CriticalSection sGeneralLock = nn::WithInitialize();
    CriticalSection sLcdOpLock = nn::WithInitialize();
    static const char* GPU_IPC_PORT_NAME = "gsp::Gpu";
    static const char* LCD_IPC_PORT_NAME = "gsp::Lcd";
    Handle sGpuSession = INVALID_HANDLE_VALUE;
    Gpu sGpuIpc(INVALID_HANDLE_VALUE);
    Handle sLcdSession = INVALID_HANDLE_VALUE;
    
    bit8 sBufferForInterruptReceiver[sizeof(InterruptReceiver)];
    InterruptReceiver* spInterruptReceiver;
    s32 sNumSpecultiveReqs = 3;
    bool sIsSrvReady   = false;

    inline void InitializeSrv(){
        if (!sIsSrvReady){
            nn::Result result = nn::srv::Initialize();
            if (result.GetDescription() != nn::Result::DESCRIPTION_ALREADY_INITIALIZED){
                NN_GXLOW_RESULT_ASSERT(result, "nn::srv::Initialize");
            }
            sIsSrvReady = true;
        }
    }

    inline void InitializeGpuSession(){
        nn::Result result = nn::srv::GetServiceHandle(&sGpuSession, GPU_IPC_PORT_NAME);
        NN_GXLOW_RESULT_ASSERT(result, "Can't connect to the GraphicsServer");
        
        sGpuIpc = Gpu(sGpuSession);
    }
    
    inline void FinalizeGpuSession(){
        nn::Result result;
        result = nn::svc::CloseHandle(sGpuSession);
        NN_GXLOW_RESULT_ASSERT(result, "CloseHandle");

        sGpuIpc = Gpu(INVALID_HANDLE_VALUE);
    }
}

void Initialize(void){
    Lock();

    if (sInitialized){
        Unlock();
        return;
    }
    sInitialized = true;

    InitializeSrv();

    InitializeGpuSession();

    sGpuIpc.AcquireRight(nn::PSEUDO_HANDLE_CURRENT_PROCESS, sIsFatalErrMode);

    spInterruptReceiver = new (sBufferForInterruptReceiver) InterruptReceiver();
    spInterruptReceiver->Initialize();

    Unlock();

    return;
}

void Finalize(void){
    Lock();

    if (!sInitialized){
        Unlock();
        return;
    }

    spInterruptReceiver->Finalize();

    FinalizeGpuSession();

    sInitialized = false;
    Unlock();

    return;
}

bool IsFirstInitialization(void){
    return spInterruptReceiver->IsFirstConnection();
}

void Lock(){
    sGeneralLock.Enter();
}

void Unlock(){
    sGeneralLock.Leave();
}

void YieldThread(){
    spInterruptReceiver->WaitAnyHandlerDone();
}

nngxlowFuncPtr RegisterInterruptHandler(nngxlowFuncPtr interruptHandler,nngxlowInterrupt interruptType){
    return spInterruptReceiver->RegisterInterruptHandler(interruptHandler,interruptType);
}

void SetAppletMode(){
    sIsAppletMode = true;
}

void SetFatalErrorMode(){
    sIsFatalErrMode = true;
    sIsAppletMode = true;
}

s32 GetNumSpeculativeRequests(){
    return sNumSpecultiveReqs;
}

void SetNumSpeculativeRequests(s32 num){
    sNumSpecultiveReqs = num;
}

void StartLcdDisplay(){
    if (sInitialized){
       sGpuIpc.SetLcdForceBlack(false);
    }
}

void StopLcdDisplay(){
    if (sInitialized){
       sGpuIpc.SetLcdForceBlack(true);
    }
    else{
        Lock();
        InitializeGpuSession();
        sGpuIpc.SetLcdForceBlack(true);
        FinalizeGpuSession();
        Unlock();
    }
}

bool IsInitialized(){
    return sInitialized;
}

namespace detail{

Gpu* GetGpuIpc(){
    NN_TASSERT_(sGpuSession != INVALID_HANDLE_VALUE);
    return &sGpuIpc;
}

InterruptReceiver* GetInterruptReceiver(){
    return spInterruptReceiver;
}

bool IsAppletMode(){
    return sIsAppletMode;
}

bool IsFatalErrMode(){
    return sIsFatalErrMode;
}

}
}
}
}