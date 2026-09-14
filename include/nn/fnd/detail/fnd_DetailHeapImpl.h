#pragma once

#include <nn/types.h>
#include <nn/fnd/detail/fnd_DetailCommon.h>
#include <nn/fnd/detail/fnd_DetailHeapHead.h>
#include <nn/Assert.h>

namespace nn{
namespace fnd{
namespace detail{

#define NNSi_FndSetBitValue(data, st, bits, val)                           \
                do                                                          \
                {                                                           \
                    u32 maskBits = (u32)((1 <<(bits)) -1);                   \
                    u32 newVal = (val) & maskBits;                             \
                    (void)(maskBits <<= st);                                 \
                    (data) &= ~maskBits;                                       \
                    (data) |= newVal <<(st);                                 \
                } while(false);

#define NNSi_FndRoundUp(value, alignment) \
    (((value) + (alignment-1)) & ~(alignment-1))

#define NNSi_FndRoundUpPtr(ptr, alignment) \
    ((void*)NNSi_FndRoundUp(NNSiGetUIntPtr(ptr), alignment))

#define NNSi_FndRoundDown(value, alignment) \
    ((value) & ~(alignment-1))

#define NNSi_FndRoundDownPtr(ptr, alignment) \
    ((void*)NNSi_FndRoundDown(NNSiGetUIntPtr(ptr), alignment))

inline u16 GetOptForHeap(const NNSiFndHeapHead* pHeapHd){
    return (u16)NNSi_FndGetBitValue(pHeapHd->attribute, 0, 8);
}

inline void SetOptForHeap(NNSiFndHeapHead* pHeapHd,u16 optFlag){
    NNSi_FndSetBitValue(pHeapHd->attribute, 0, 8, optFlag);
}

inline void MI_CpuFill32(void* dest, u32 data, u32 size){
    u32* pDest = reinterpret_cast<u32*>(dest);
    for(int i = 0; i < size / 4; ++i){
        pDest[i] = data;
    }
}

inline void FillAllocMemory(NNSiFndHeapHead* pHeapHd,void* address,u32 size)
{
    if (GetOptForHeap(pHeapHd) & 1)
    {
        MI_CpuFill32(address, 0, size);
    }
    else
    {
        #ifdef NN_DEBUG
            if (GetOptForHeap(pHeapHd) & 2){
                MI_CpuFill32(address, GetFillValForHeap(0), size);
            }
        #endif
    }
}

#define FillNoUseMemory(pHeapHd, address, size)  ((void) 0)
#define FillFreeMemory(pHeapHd, address, size)  ((void) 0)

}
}
}