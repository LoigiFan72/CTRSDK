// Filename: gx_Misc.cpp
//
// Project: Horizon

#include <nn/gxlow/CTR/gxlow_CTR.h>
#include <nn/gxlow/CTR/gxlow_Misc.h>
#include <nn/gx.h>
#include <nn/gx/CTR/gx_CTRPrivate.h>
#include <nn/gx/CTR/gx_CommandAccess.h>

#ifdef __cplusplus
extern "C" {
#endif

uptr nngxGetVramStartAddr(s32 area){
    switch(area){
    case NN_GX_MEM_VRAMA:
        return (nn::gxlow::CTR::detail::IsAppletMode()) ? NN_GX_VRAMA_SYS_START : NN_GX_VRAMA_USER_START;
    case NN_GX_MEM_VRAMB:
        return (nn::gxlow::CTR::detail::IsAppletMode()) ? NN_GX_VRAMB_SYS_START : NN_GX_VRAMB_USER_START;
    default:
        return NULL;
    }
}

uptr nngxGetVramEndAddr(s32 area){
    switch(area){
    case NN_GX_MEM_VRAMA:
        return (nn::gxlow::CTR::detail::IsAppletMode()) ? NN_GX_VRAMA_SYS_END : NN_GX_VRAMA_USER_END;
    case NN_GX_MEM_VRAMB:
        return (nn::gxlow::CTR::detail::IsAppletMode()) ? NN_GX_VRAMB_SYS_END : NN_GX_VRAMB_USER_END;
    default:
        return NULL;
    }
}

void nngxStartLcdDisplay(){
    nn::gxlow::CTR::StartLcdDisplay();
}

uptr nngxGetPhysicalAddr(uptr virtualAddr){
    return nn::gxlow::CTR::GetPhysicalAddr(virtualAddr);
}

void nngxUpdateBuffer(const void* pBuffer, size_t size){
    nn::gxlow::CTR::FlushDataCache(pBuffer, size);
}

#ifdef __cplusplus
} // extern "C"
#endif