#pragma once

#include <nn/gxlow/CTR/gxlow_Management.h>

#ifdef __cplusplus
extern "C" {
#endif

void nngxlowInitialize();
bool nngxlowIsFirstInitialization();

void nngxlowFlushDataCache(const void* pData, size_t size);
uptr nngxlowGetPhysicalAddr(uptr virtualAddr);

void nngxlowFinalize();

s32 nngxlowGetNumSpeculativeRequests();

nngxlowFuncPtr nngxlowRegisterInterruptHandler(nngxlowFuncPtr interruptHandler,nngxlowInterrupt type);

void nngxlowLock();
void nngxlowUnlock();

void nngxlowYieldThread();

void nngxlowReadHWRegs(uint regOffset, void* pDst, size_t size);
void nngxlowRequestDma(const void* pDst, void* pSrc, size_t size, bool flushCache, bool check);
void nngxlowSetBufferSwap(s32 channel,s32 nextBank,void *pBuf,void *pBufB,u32 size,bit32 mode,bit32 swap);
void nngxlowSetCommandList(void* pCmdBuf, size_t size, bool flush, bool autoGasAcc);
void nngxlowSetDisplayTransfer(void* pSrc, u16 srcWidth, u16 srcHeight, void* pDst, u16 dstWidth, u16 dstHeight, bit32 mode);
void nngxlowSetMemoryFill(void *startAddr0,void *endAddr0,bit32 data0,bit32 ctrl0,void *startAddr1,void *endAddr1,bit32 data1,bit32 ctrl1);
void nngxlowSetTextureCopy(void *pSrc,void *pDst,u32 dmaSize,u16 srcIntv,u16 srcIntiv,u16 dstIntv,u16 dstIntiv,bit32 mode);
void nngxlowWriteHWRegs(uint regOffset, const void* pSrc, size_t size);
void nngxlowWriteHWRegsWithMask(uint regOffset, const void* pSrc, void* pMask, size_t size);



#ifdef __cplusplus
}
#endif