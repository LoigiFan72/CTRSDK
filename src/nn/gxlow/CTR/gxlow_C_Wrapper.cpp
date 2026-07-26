// Filename: gxlow_C_Wrapper.cpp
//
// Project: Horizon

#include <nn/os.h>
#include <nn/gxlow/CTR/gxlow_CTR.h>
#include <nn/gxlow/CTR/gxlow_RegAccess.h>
#include <nn/gxlow/CTR/gxlow_Management.h>
#include <nn/gxlow/CTR/gxlow_Misc.h>
#include <nn/gx/CTR/gx_Lcd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void nngxlowInitialize(void){
    nn::gxlow::CTR::Initialize();
}

void nngxlowFinalize(void){
    nn::gxlow::CTR::Finalize();
}

bool nngxlowIsFirstInitialization(void){
    return nn::gxlow::CTR::IsFirstInitialization();
}

void nngxlowLock(void){
    nn::gxlow::CTR::Lock();
}

void nngxlowUnlock(void){
    nn::gxlow::CTR::Unlock();
}

void nngxlowWriteHWRegs(u32 regOffset, const void* pSrc, size_t size){
    nn::gxlow::CTR::WriteHWRegs(regOffset, pSrc, size);
}

void nngxlowWriteHWRegsWithMask(u32 regOffset, const void* pSrc,const void* pMask, size_t size){
    nn::gxlow::CTR::WriteHWRegsWithMask(regOffset, pSrc, pMask, size);
}

void nngxlowReadHWRegs(u32 regOffset, void* pDst, size_t size){
    nn::gxlow::CTR::ReadHWRegs(regOffset, pDst, size);
}

void nngxlowSetBufferSwap(s32 channel, s32 nextBank,void* addr, void* addrB,u32 size, bit32 mode, bit32 swap){
    nn::gxlow::CTR::SetBufferSwap(channel,nextBank,addr,addrB,size,mode,swap);
}

void nngxlowSetDisplayTransfer(void* srcAddr, u16 srcWidth, u16 srcHeight,void* dstAddr, u16 dstWidth, u16 dstHeight,bit32 mode){
    nn::gxlow::CTR::SetDisplayTransfer(srcAddr,srcWidth,srcHeight,dstAddr,dstWidth,dstHeight,mode);
}

void nngxlowSetTextureCopy(void* srcAddr, void* dstAddr, u32 dmaSize,u16 srcIntv, u16 srcIntiv, u16 dstIntv,u16 dstIntiv, bit32 mode){
    nn::gxlow::CTR::SetTextureCopy(srcAddr,dstAddr,dmaSize,srcIntv,srcIntiv,dstIntv,dstIntiv,mode);
}

void nngxlowSetMemoryFill(void* startAddr0, void* endAddr0,bit32 data0, bit32 ctrl0,void* startAddr1, void* endAddr1,bit32 data1, bit32 ctrl1){
    nn::gxlow::CTR::SetMemoryFill(startAddr0,endAddr0,data0,ctrl0,startAddr1,endAddr1,data1,ctrl1);
}

void nngxlowSetCommandlistEx(void* pBuffer,size_t size,bool flushCache,bool autoGasAcc){
    nn::gxlow::CTR::SetCommandlist(pBuffer, size, flushCache, autoGasAcc);
}

void nngxlowRequestDMA(void* pDst, const void* pSrc, size_t size){
    nn::gxlow::CTR::RequestDma(pDst, pSrc, size, true, true);
}

void nngxlowRequestDMAEx(void* pDst, const void* pSrc, size_t size, bool flushCache){
    nn::gxlow::CTR::RequestDma(pDst, pSrc, size, flushCache, true);
}

nngxlowFuncPtr nngxlowRegisterInterruptHandler(nngxlowFuncPtr interruptHandler,nngxlowInterrupt interruptType){
    return nn::gxlow::CTR::RegisterInterruptHandler(interruptHandler,interruptType);
}

uptr nngxlowGetPhysicalAddr(uptr virtualAddr){
    return nn::gxlow::CTR::GetPhysicalAddr(virtualAddr);
}

bool nngxlowIsDeviceMemory(uptr virtualAddr){
    return nn::gxlow::CTR::detail::IsDeviceMemory(virtualAddr);
}

bool nngxlowIsVram(uptr virtualAddr){
    return nn::gxlow::CTR::detail::IsVram(virtualAddr);
}

void nngxlowStartLcdDisplay(void){
    return nn::gxlow::CTR::StartLcdDisplay();
}

void nngxlowStopLcdDisplay(void){
    return nn::gxlow::CTR::StopLcdDisplay();
}

void nngxlowFlushDataCache(const void* pData, size_t size){
    return nn::gxlow::CTR::FlushDataCache(pData, size);
}

s64 nngxlowGetSystemTick(void){
    return static_cast<s64>(nn::os::Tick::GetSystemCurrent());
}

void nngxlowYieldThread(void){
    return nn::gxlow::CTR::YieldThread();
}

s32 nngxlowGetNumSpeculativeRequests(void){
    return nn::gxlow::CTR::GetNumSpeculativeRequests();
}

void nngxlowSetSyncMode(bool mode){
    return nn::gxlow::CTR::SetSyncMode(mode);
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus