#pragma once

#include <nn/types.h>

typedef s32 NNSiIntPtr;
typedef u32 NNSiUIntPtr;

namespace nn{ 
namespace fnd { 
namespace detail {

#define  NNSi_FndGetBitValue(data, st, bits) \
    (((data) >>(st)) & ((1 <<(bits)) -1))

inline NNSiUIntPtr NNSiGetUIntPtr(const void* ptr)
{
    return (NNSiUIntPtr)ptr;
}

inline void* AddU32ToPtr(void* ptr, u32 val)
{
    return (void*)( NNSiGetUIntPtr(ptr) + val);
}

inline void* SubU32ToPtr(void* ptr, u32 val)
{
    return (void*)( NNSiGetUIntPtr(ptr) - val);
}

inline u32 GetOffsetFromPtr(const void* start, const void* end)
{
    return NNSiGetUIntPtr(end) - NNSiGetUIntPtr(start);
}

inline int ComparePtr(const void* a, const void* b)
{
    const u8* wa = reinterpret_cast<const u8*>(a);
    const u8* wb = reinterpret_cast<const u8*>(b);

    return wa - wb;
}

#define NNSI_CREATE_HEAP_SIGNATURE(a,b,c,d)  ( ((a & 0xFF) << 24) | ((b & 0xFF) << 16) | ((c & 0xFF) << 8) | ((d & 0xFF) << 0) )

#define NNSI_EXPHEAP_SIGNATURE    NNSI_CREATE_HEAP_SIGNATURE('E','X','P','H')

}
}
}