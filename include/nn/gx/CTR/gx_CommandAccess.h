#pragma once

#include <nn/gx/CTR/gx_PicaReg.h>
#include <nn/gx/CTR/gx_PicaCommon.h>
#include <nn/gx/CTR/gx_PicaFragment.h>
#include <nn/gx/CTR/gx_PicaOld.h>
#include <nn/gx/CTR/gx_PicaTexture.h>
#include <nn/gx/CTR/gx_PicaShader.h>
#include <nn/gx/CTR/gx_PicaMisc.h>

#ifdef __cplusplus
extern "C" {
#endif
    uptr nngxGetPhysicalAddr(uptr virtualAddr);
    void nngxUpdateBuffer(const void* pBuffer, size_t size);
#ifdef __cplusplus
}
#endif

namespace nn {
namespace gx {
namespace CTR {
    inline uptr GetPhysicalAddr(uptr virtualAddr) 
    {
        return nngxGetPhysicalAddr(virtualAddr);
    }

    inline void UpdateBuffer(const void* pBuffer, size_t size) 
    {
        nngxUpdateBuffer(pBuffer, size);
    }
}
}
}