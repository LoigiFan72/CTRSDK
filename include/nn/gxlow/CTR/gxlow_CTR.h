#pragma once

#include <nn/types.h>

typedef enum nngxlowInterrupt{
    NN_GXLOW_INTERRUPT_GPU_PSC_0 = 0,
    NN_GXLOW_INTERRUPT_GPU_PSC_1 = 1,
    NN_GXLOW_INTERRUPT_GPU_PDC_0 = 2,
    NN_GXLOW_INTERRUPT_GPU_PDC_1 = 3,
    NN_GXLOW_INTERRUPT_GPU_PPF   = 4,
    NN_GXLOW_INTERRUPT_GPU_P3D   = 5,
    NN_GXLOW_INTERRUPT_DMAC_0    = 6,
    
    NN_GXLOW_NUM_INTERRUPTS      = 7
} nngxlowInterrupt;

typedef void (*nngxlowFuncPtr)(void);

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
void nngxlowRequestDMA(void* pDst, const void* pSrc, size_t size);
void nngxlowRequestDMAEx(void* pDst, const void* pSrc, size_t size, bool flushCache);
void nngxlowSetBufferSwap(s32 channel,s32 nextBank,void *pBuf,void *pBufB,u32 size,bit32 mode,bit32 swap);
void nngxlowSetCommandlist(void* pCmdBuf, size_t size);
void nngxlowSetCommandlistEx(void* pCmdBuf,size_t size,bool flushCache,bool autoGasAcc);
void nngxlowSetDisplayTransfer(void* pSrc, u16 srcWidth, u16 srcHeight, void* pDst, u16 dstWidth, u16 dstHeight, bit32 mode);
void nngxlowSetMemoryFill(void *startAddr0,void *endAddr0,bit32 data0,bit32 ctrl0,void *startAddr1,void *endAddr1,bit32 data1,bit32 ctrl1);
void nngxlowSetTextureCopy(void *pSrc,void *pDst,u32 dmaSize,u16 srcIntv,u16 srcIntiv,u16 dstIntv,u16 dstIntiv,bit32 mode);
void nngxlowWriteHWRegs(uint regOffset, const void* pSrc, size_t size);
s64 nngxlowGetSystemTick();
void nngxlowWriteHWRegsWithMask(uint regOffset, const void* pSrc, const void* pMask, size_t size);



#ifdef __cplusplus
}
#endif