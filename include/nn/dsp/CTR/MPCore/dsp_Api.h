#pragma once

#include <nn/dsp/CTR/MPCore/dsp_Ipc.h>
#include <nn/svc.h>

namespace nn {
namespace dsp {
namespace CTR {
namespace detail {
    inline Result InitializeBase(nn::Handle* pSession, const char* name){ }
    inline Result FinalizeBase(nn::Handle* pSession){ Result res = svc::CloseHandle(pSession->m_Handle); }
}
namespace {
    const char PORT_NAME_DSP[] = "dsp::DSP";
}

    inline Result InitializeIpc(nn::Handle* pSession) { return detail::InitializeBase(pSession, PORT_NAME_DSP); }
    inline Result FinalizeIpc(nn::Handle* pSession)   { return detail::FinalizeBase(pSession); }

    Result Initialize();
    void Finalize();

    Result LoadComponent(const u8* pComponent, size_t size, bit16 maskPram = 0xff, bit16 maskDram = 0xff);
    Result LoadComponentCore(const u8* pComponent, size_t size, bit16 maskPram = 0xff, bit16 maskDram = 0xff);
    Result LoadDefaultComponent();
    Result UnloadComponent();
    Result UnloadComponentCore();

    Result RegisterInterruptEvents(nn::Handle handle, s32 type, s32 port = 0);
    Result RecvData(u16 regNo, u16* pValue);
    Result SendData(u16 regNo, u16 value);
    Result RecvDataIsReady(u16 regNo, bool* pStatus);
    Result SendDataIsEmpty(u16 regNo, bool* pStatus);

    Result ConvertProcessAddressFromDspDram(uptr addressOnDsp, uptr* pAddressOnHost);

    Result ReadPipeIfPossible(int port, void* buffer, u16 length, u16* pLengthRead);
    Result WriteProcessPipe(int port, const void* buffer, u32 length);

    Result FlushDataCache(uptr addr, size_t size);
    Result InvalidateDataCache(uptr addr, size_t size);

    Result GetSemaphoreEventHandle(Handle* pHandle);
    Result SetSemaphoreEventMask(bit16 mask);
    Result SetSemaphore(ushort);

    bool IsComponentLoaded();
    bool Sleep();
    void WakeUp();
    void Awake();
    void OrderToWaitForFinalize();

    bool RegisterSleepWakeUpCallback(void (*sleepCallback)(),void (*wakeUpCallback)(),void (*orderToWaitForFinalizeCallback)() = nullptr);
    bool ClearSleepWakeUpCallback(void (*sleepCallback)(),void (*wakeUpCallback)(),void (*orderToWaitForFinalizeCallback)() = nullptr);

} // namespace CTR
} // namespace dsp
} // namespace nn