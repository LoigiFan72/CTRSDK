#pragma once

#include <nn/types.h>

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef NN_GX_MEM_VRAMA
#define NN_GX_MEM_VRAMA     0x00020000
#endif

#ifndef NN_GX_MEM_VRAMB
#define NN_GX_MEM_VRAMB     0x00030000
#endif

#ifndef NN_GX_MEM_FCRAM
#define NN_GX_MEM_FCRAM     0x2010000
#endif

uptr nngxGetVramStartAddr(s32 area);

uptr nngxGetVramEndAddr(s32 area);
    
size_t nngxGetVramSize(s32 area);
    
#ifdef  __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef  __cplusplus
namespace nn {
namespace gx {
namespace CTR {

enum VramArea{
    MEM_VRAMA = NN_GX_MEM_VRAMA,
    MEM_VRAMB = NN_GX_MEM_VRAMB
};

inline uptr GetVramStartAddr(VramArea area)
{
    return nngxGetVramStartAddr(area);
}

inline uptr GetVramEndAddr(VramArea area)
{
    return nngxGetVramEndAddr(area);
}

inline size_t GetVramSize(VramArea area)
{
    return nngxGetVramSize(area);
}

}
}
}
#endif