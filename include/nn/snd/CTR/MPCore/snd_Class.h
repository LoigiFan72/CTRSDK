#pragma once

#include "nn/Assert.h"

namespace nn{
namespace snd{
namespace CTR{

inline void memcpy16(void* pDst, void* pSrc, size_t n){
    NN_TASSERT_(n % sizeof(bit16) == 0);
    for(uint i = 0; i < n >> 1; i++){
        *(ushort*)((ushort*)pDst + i * 2) = *(ushort*)((ushort*)pSrc + i * 2);
    }
}

}
}
}