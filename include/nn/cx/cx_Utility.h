#pragma once

#include <nn/types.h>

namespace nn{
namespace cx{
namespace internal{

inline u32 Read32Le(const u8* p){
    return (p[0] <<  0) | (p[1] <<  8) | (p[2] << 16) | (p[3] << 24);
}

}
}
}