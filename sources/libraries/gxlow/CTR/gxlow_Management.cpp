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
    bool s_Initialized = false;
    bool s_IsAppletMode = false;
    bool s_IsFatalErrMode = false;
    nn::os::CriticalSection s_GeneralLock = nn::WithInitialize();
    nn::os::CriticalSection s_LcdOpLock = nn::WithInitialize();
    static const char* GPU_IPC_PORT_NAME = "gsp::Gpu";
    static const char* LCD_IPC_PORT_NAME = "gsp::Lcd";
    Handle s_GpuSession = INVALID_HANDLE_VALUE;
    Gpu s_GpuIpc(INVALID_HANDLE_VALUE);
    Handle s_LcdSession = INVALID_HANDLE_VALUE;
    
    bit8 s_BufferForInterruptReceiver[sizeof(InterruptReceiver)];
    InterruptReceiver* s_pInterruptReceiver;
    s32 s_NumSpecultiveReqs = 3;
    bool s_IsSrvReady   = false;

    inline void InitializeSrv(){
        if (!s_IsSrvReady){
            nn::Result result = nn::srv::Initialize();
            if (result.GetDescription() != nn::Result::DESCRIPTION_ALREADY_INITIALIZED){
                NN_GXLOW_RESULT_ASSERT(result, "nn::srv::Initialize");
            }
            s_IsSrvReady = true;
        }
    }

    inline void InitializeGpuSession(){
        nn::Result result = nn::srv::GetServiceHandle(&s_GpuSession, GPU_IPC_PORT_NAME);
        NN_GXLOW_RESULT_ASSERT(result, "Can't connect to the GraphicsServer");
        
        s_GpuIpc = Gpu(s_GpuSession);
    }
    
    inline void FinalizeGpuSession(){
        nn::Result result;
        result = nn::svc::CloseHandle(s_GpuSession);
        NN_GXLOW_RESULT_ASSERT(result, "CloseHandle");

        s_GpuIpc = Gpu(INVALID_HANDLE_VALUE);
    }
}

void Initialize(void){
    Lock();

    if (s_Initialized){
        Unlock();
        return;
    }
    s_Initialized = true;

    InitializeSrv();

    InitializeGpuSession();

    s_GpuIpc.AcquireRight(nn::PSEUDO_HANDLE_CURRENT_PROCESS, s_IsFatalErrMode);

    s_pInterruptReceiver = new (s_BufferForInterruptReceiver) InterruptReceiver();
    s_pInterruptReceiver->Initialize();

    Unlock();

    return;
}

void Finalize(void){
    Lock();

    if (!s_Initialized){
        Unlock();
        return;
    }

    s_pInterruptReceiver->Finalize();

    FinalizeGpuSession();

    s_Initialized = false;
    Unlock();

    return;
}

bool IsFirstInitialization(void){
    return s_pInterruptReceiver->IsFirstConnection();
}

void Lock(){
    s_GeneralLock.Enter();
}

void Unlock(){
    s_GeneralLock.Leave();
}

void YieldThread(){
    s_pInterruptReceiver->WaitAnyHandlerDone();
}

nngxlowFuncPtr RegisterInterruptHandler(nngxlowFuncPtr interruptHandler,nngxlowInterrupt interruptType){
    return s_pInterruptReceiver->RegisterInterruptHandler(interruptHandler,interruptType);
}

void SetAppletMode(){
    s_IsAppletMode = true;
}

void SetFatalErrorMode(){
    s_IsFatalErrMode = true;
    s_IsAppletMode = true;
}

s32 GetNumSpeculativeRequests(){
    return s_NumSpecultiveReqs;
}

void SetNumSpeculativeRequests(s32 num){
    s_NumSpecultiveReqs = num;
}

void StartLcdDisplay(){
    if (s_Initialized){
       s_GpuIpc.SetLcdForceBlack(false);
    }
}

void StopLcdDisplay(){
    if (s_Initialized){
       s_GpuIpc.SetLcdForceBlack(true);
    }
    else{
        Lock();
        InitializeGpuSession();
        s_GpuIpc.SetLcdForceBlack(true);
        FinalizeGpuSession();
        Unlock();
    }
}

bool IsInitialized(){
    return s_Initialized;
}

namespace detail{

Gpu* GetGpuIpc(){
    NN_TASSERT_(sGpuSession != INVALID_HANDLE_VALUE);
    return &s_GpuIpc;
}

InterruptReceiver* GetInterruptReceiver(){
    return s_pInterruptReceiver;
}

bool IsAppletMode(){
    return s_IsAppletMode;
}

bool IsFatalErrMode(){
    return s_IsFatalErrMode;
}

}
}
}
}