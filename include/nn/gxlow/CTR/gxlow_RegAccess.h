#pragma once

#include <nn/gxlow/CTR/gxlow_InterruptReceiver.h>

#ifdef __cplusplus

namespace nn{
namespace gxlow{
namespace CTR{

    void ReadHWRegs(uint regOffset, void* pDst, size_t size);
    void RequestDma(void* pDst, const void* pSrc, size_t size, bool flushCache, bool check);
    void SetBufferSwap(s32 channel,s32 nextBank,void *pBuf,void *pBufB,u32 size,bit32 mode,bit32 swap);
    void SetCommandlist(void* pCmdBuf, size_t size, bool flush, bool autoGasAcc);
    void SetDisplayTransfer(void* pSrc, u16 srcWidth, u16 srcHeight, void* pDst, u16 dstWidth, u16 dstHeight, bit32 mode);
    void SetMemoryFill(void *startAddr0,void *endAddr0,bit32 data0,bit32 ctrl0,void *startAddr1,void *endAddr1,bit32 data1,bit32 ctrl1);
    void SetSyncMode(bool mode);
    void SetTextureCopy(void *pSrc,void *pDst,u32 dmaSize,u16 srcIntv,u16 srcIntiv,u16 dstIntv,u16 dstIntiv,bit32 mode);
    void WriteHWRegs(uint regOffset, const void* pSrc, size_t size);
    void WriteHWRegsWithMask(uint regOffset, const void* pSrc, const void* pMask, size_t size);

}
}
}

#endif