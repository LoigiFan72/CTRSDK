// Filename: gxlow_RegAccess.cpp
//
// Project: Horizon

#pragma once

#include <nn/gxlow/CTR/gxlow_RegAccess.h>
#include <nn/gxlow/CTR/gxlow_Management.h>
#include <nn/gxlow/CTR/detail/gxlow_DisplaySwapInfoPad.h>
#include <nn/gx/CTR/gx_CTRPrivate.h>
#include <nn/gx.h>

namespace nn {
namespace gxlow {
namespace CTR {
namespace detail{
    inline Result EnqueueCmdReq(const CmdReq* pReq){ 
        return GetInterruptReceiver()->GetCmdReqQueue()->TryEnqueue(pReq); 
    }
}
namespace{
    static const bit32 DefaultCmdReqPacket[] ={
        0x01000100, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000
    };
    const detail::CmdReq* DefaultCmdReq = reinterpret_cast<const detail::CmdReq*>(DefaultCmdReqPacket);
    
    bool sSyncMode = true;

}

void SetSyncMode(bool mode){
    sSyncMode = mode;
}


void WriteHWRegs(u32 regOffset, const void* pSrc, size_t size){
    NN_TASSERT_(pSrc != NULL);
    NN_TASSERT_(regOffset % 4 == 0);
    NN_TASSERT_(size % 4 == 0);

    nn::Result result;

    Gpu* gpu = detail::GetGpuIpc();
    result = gpu->WriteHWRegs(regOffset,reinterpret_cast<const u8*>(pSrc),size);

    NN_GXLOW_RESULT_ASSERT(result, "[gxlow::WriteHWRegs]");

    return;
}

void WriteHWRegsWithMask(u32 regOffset, const void* pSrc,const void* pMask, size_t size){
    NN_TASSERT_(pSrc != NULL);
    NN_TASSERT_(pMask != NULL);
    NN_TASSERT_(regOffset % 4 == 0);
    NN_TASSERT_(size % 4 == 0);

    nn::Result result;

    Gpu* gpu = detail::GetGpuIpc();
    result = gpu->WriteHWRegsWithMask(regOffset,reinterpret_cast<const u8*>(pSrc),reinterpret_cast<const u8*>(pMask),size);
    NN_GXLOW_RESULT_ASSERT(result, "[gxlow::WriteHWRegsWithMask]");

    return;
}

void ReadHWRegs(u32 regOffset, void* pDst, size_t size){
    NN_TASSERT_(pDst != NULL);
    NN_TASSERT_(regOffset % 4 == 0);
    NN_TASSERT_(size % 4 == 0);

    nn::Result result;

    Gpu* gpu = detail::GetGpuIpc();
    result = gpu->ReadHWRegs(regOffset,reinterpret_cast<u8*>(pDst),size);
    NN_GXLOW_RESULT_ASSERT(result, "[gxlow::ReadHWRegs]");

    return;
}

void SetBufferSwap(s32 channel,s32 nextBank,void* pBuf,void* pBufB,u32 size,bit32 mode,bit32 swap){
    NN_TASSERT_(channel == 0 || channel == 1);
    NN_TASSERT_(nextBank == 0 || nextBank == 1);

    uptr address  = reinterpret_cast<uptr>(pBuf);
    uptr addressB = reinterpret_cast<uptr>(pBufB);
// didnt do debug here because it sucks!!!!

    Result result;

    {
        detail::DisplaySwapInfo info;
        
        info.nextBank = nextBank;
        info.address = address;
        info.addressB = addressB;
        info.size = size;
        info.mode = mode;
        info.swap = swap;
        info.attribute = 0;
        
        result = detail::GetInterruptReceiver()->GetSwapInfoPad()->Push(&info, channel);
    }

    NN_GXLOW_RESULT_ASSERT(result, "[gxlow::SetSwapSettings]");
    return;
}

void SetTextureCopy(void* pSrc,void* pDst,u32 dmaSize,u16 srcIntv,u16 srcIntiv,u16 dstIntv,u16 dstIntiv,bit32 mode){
    nn::Result result;

    NN_GX_ASSERT_DEVICE_MEMORY_OR_VRAM(reinterpret_cast<uptr>(pDst));
    NN_GX_ASSERT_DEVICE_MEMORY_OR_VRAM(reinterpret_cast<uptr>(pSrc));

    {
        detail::CmdReq  req = *DefaultCmdReq;
        
        req.sync                  = sSyncMode;
        req.param.ctx.srcAddr     = reinterpret_cast<uptr>(pSrc);
        req.param.ctx.dstAddr     = reinterpret_cast<uptr>(pDst);
        req.param.ctx.dmaSize     = dmaSize;
        req.param.ctx.srcInterval = (srcIntiv << 16) | srcIntv;
        req.param.ctx.dstInterval = (dstIntiv << 16) | dstIntv;
        req.param.ctx.mode        = mode;
        
        result = EnqueueCmdReq(&req);
    }

    NN_GXLOW_RESULT_ASSERT(result, "[gxlow::SetTextureCopy]");

    return;
}

void SetCommandlist(void* pCmdBuf,size_t size,bool flushCache,bool autoGasAcc){
    NN_TASSERT_(reinterpret_cast<uptr>(pCmdBuf) % 8 == 0);
    NN_TASSERT_(size % 8 == 0);

    nn::Result result = ResultSuccess();

    {
        detail::CmdReq  req;
        
        if (flushCache){
            req = *DefaultCmdReq;
            req.id                   = detail::REQ_ID_CACHE_FLUSH;
            req.sync                 = false;
            req.param.cf.addr0       = reinterpret_cast<uptr>(pCmdBuf);
            req.param.cf.size0       = size;
            
            result = EnqueueCmdReq(&req);
        }
        
        if (result.IsSuccess()){
            req = *DefaultCmdReq;
            req.id                   = detail::REQ_ID_3D_CMD;
            req.sync                 = sSyncMode;
            req.param.ren.addr       = reinterpret_cast<uptr>(pCmdBuf);
            req.param.ren.size       = size;
            req.param.ren.control    = autoGasAcc;
            
            result = EnqueueCmdReq(&req);
        }
    }

    return;
}

void SetDisplayTransfer(void* pSrc,u16 srcWidth,u16 srcHeight,void* pDst,u16 dstWidth,u16 dstHeight,bit32 mode){
    nn::Result result;

    NN_GX_ASSERT_DEVICE_MEMORY_OR_VRAM(reinterpret_cast<uptr>(pSrc));
    NN_GX_ASSERT_DEVICE_MEMORY_OR_VRAM(reinterpret_cast<uptr>(pDst));

    {
        detail::CmdReq  req = *DefaultCmdReq;
        
        req.id                   = detail::REQ_ID_DISP_COPY;
        req.sync                 = sSyncMode;
        req.param.pf.srcAddr     = reinterpret_cast<uptr>(pSrc);
        req.param.pf.dstAddr     = reinterpret_cast<uptr>(pDst);
        req.param.pf.srcSize     = (srcHeight << 16) | srcWidth;
        req.param.pf.dstSize     = (dstHeight << 16) | dstWidth;
        req.param.pf.mode        = mode;
        
        result = EnqueueCmdReq(&req);
    }

    NN_GXLOW_RESULT_ASSERT(result, "[gxlow::SetDisplayTransfer]");

    return;
}

void SetMemoryFill(void* startAddr0,void* endAddr0,bit32 data0,bit32 ctrl0,void* startAddr1,void* endAddr1,bit32 data1,bit32 ctrl1){
    nn::Result result;

    if (startAddr0){
        NN_GX_ASSERT_VRAM(reinterpret_cast<uptr>(startAddr0));
    }
    if (startAddr1){
        NN_GX_ASSERT_VRAM(reinterpret_cast<uptr>(startAddr1));
    }

    {
        detail::CmdReq  req = *DefaultCmdReq;
        
        req.id               = detail::REQ_ID_MEM_FILL;
        req.sync             = sSyncMode;
        req.param.mf.start0  = reinterpret_cast<uptr>(startAddr0);
        req.param.mf.data0   = data0;
        req.param.mf.end0    = reinterpret_cast<uptr>(endAddr0);
        req.param.mf.start1  = reinterpret_cast<uptr>(startAddr1);
        req.param.mf.data1   = data1;
        req.param.mf.end1    = reinterpret_cast<uptr>(endAddr1);
        req.param.mf.ctrl0   = static_cast<bit16>(ctrl0);
        req.param.mf.ctrl1   = static_cast<bit16>(ctrl1);
        
        result = EnqueueCmdReq(&req);
    }

    NN_GXLOW_RESULT_ASSERT(result, "[gxlow::SetMemoryFill]");

    return;
}

void RequestDma(void* pDst, const void* pSrc, size_t size, bool flushCache, bool check){
    if (check){
        if (!detail::IsVram(reinterpret_cast<uptr>(pDst)))
            return;
    }

    Result result;

    {
        detail::CmdReq  req = *DefaultCmdReq;
        
        req.id                   = detail::REQ_ID_DMA;
        req.sync                 = sSyncMode;
        req.param.dma.srcAddr    = reinterpret_cast<uptr>(pSrc);
        req.param.dma.dstAddr    = reinterpret_cast<uptr>(pDst);
        req.param.dma.size       = size;
        req.param.dma.cacheFlush = flushCache;

        result = EnqueueCmdReq(&req);
    }

    NN_GXLOW_RESULT_ASSERT(result, "[gxlow::RequestDma]");

    return;
}

}
}
}