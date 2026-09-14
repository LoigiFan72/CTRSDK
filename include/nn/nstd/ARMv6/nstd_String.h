#pragma once

#include <nn/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void nnnstdMemMove (void* pOut, const void* pIn, u32 size);
void nnnstdMemCpy (void* pOut, const void* pIn, u32 size);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

namespace nn { 
namespace nstd {
namespace ARMv6{
    inline void MemCpy(void *dstp, const void *srcp, size_t size)
    {
        nnnstdMemCpy(dstp, srcp, size);
    }

    inline void MemMove(void *dstp, const void *srcp, size_t size)
    {
        nnnstdMemMove(dstp, srcp, size);
    }
}
}
}

#endif