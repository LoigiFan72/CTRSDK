#pragma once

#include <nn/types.h>


#ifdef __cplusplus

namespace nn{
namespace gxlow{
namespace CTR{

void FlushDataCache(const void* pData, size_t size);
uptr GetPhysicalAddr(uptr virtualAddr);


}
}
}

#endif