#pragma once

#include <nn/gxlow/CTR/gxlow_GpuIpc.h>
#include <nn/gxlow/CTR/gxlow_CTR.h>

#ifdef __cplusplus
namespace nn{
namespace gxlow{
namespace CTR{
    class InterruptReceiver;

    void Initialize();
    bool IsInitialized();
    bool IsFirstInitialization();

    void Finalize();

    s32 GetNumSpeculativeRequests();

    nngxlowFuncPtr RegisterInterruptHandler(nngxlowFuncPtr interruptHandler,nngxlowInterrupt type);

    void Lock();
    void Unlock();

    void YieldThread();

    void SetAppletMode();

    void StartLcdDisplay();
    void StopLcdDisplay();

namespace detail{
    InterruptReceiver* GetInterruptReceiver();

    bool IsAppletMode();
    bool IsFatalErrMode();

    Gpu* GetGpuIpc();

    inline bool IsDeviceMemory(uptr addr){ }
    inline bool IsContinuousMemory(uptr addr){ return addr >= 0x14000000 && addr < 0x1C000000 ;}
    inline bool IsVram(uptr addr){ return addr >= 0x1F000000 && addr <= 0x1F5FFFFF; }
}
}
}
}

#endif