// Filename: cx_Uncompression.cpp
//
// Project: Horizon

#include <nn/cx/cx_Uncompression.h>
#include <nn/cx/cx_Utility.h>
#include <nn/Assert.h>

namespace nn{
namespace cx{

u32 GetUncompressedSize(const void *pData){
    NN_TASSERT_(pData);

    const u8* p = static_cast<const u8*>(pData);

    u32 size = internal::Read32Le(p) >> 8;
    if (size == 0){
        size = internal::Read32Le(p + 4);
    }
    return size;
}

}
}