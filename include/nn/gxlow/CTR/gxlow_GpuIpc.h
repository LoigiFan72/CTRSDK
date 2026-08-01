#pragma once

#include <nn/gxlow/CTR/gxlow_Parameters.h>
#include <nn/Handle.h>
#include <nn/Result.h>

namespace nn{
namespace gxlow{
namespace CTR{

class Gpu{
public:
    Handle mSession;
public:
    Gpu(Handle session):
        mSession(session)
    {}
    Result AcquireRight(Handle eventHandle, bool forced);
    Result FlushDataCache(Handle clientProcess, uptr addr, size_t size);
    Result ImportDisplayCaptureInfo(DisplayCaptureInfo* info);
    Result ReadHWRegs(u32 regOffset, u8* pDst, size_t size);
    Result RegisterInterruptRelayQueue(Handle eventRx, bit32 attribute, Handle* pWorkMem, s32* pIndex);
    Result ReleaseRight();
    Result RestoreVramSysArea();
    Result SaveVramSysArea();
    Result SetLcdForceBlack(bool enable);
    Result TriggerCmdReqQueue();
    Result UnregisterInterruptRelayQueue();
    Result WriteHWRegs(u32 regOffset, const u8*pSrc, size_t size);
    Result WriteHWRegsWithMask(u32 regOffset, const u8* pSrc, const u8* pMask, size_t size);
};
}
}
}