#pragma once

#include <nn/types.h>
#include <nn/assert.h>

namespace nn {
namespace math {

template <u32 MUL, u32 DIV>
struct MakeRate
{
    static const u64 VALUE = (static_cast<u64>(MUL) << 32) / DIV;
};
template <u32 MUL, u32 DIV>
struct MakeRate32
{
    static const u32 VALUE = static_cast<u32>( (static_cast<u64>(MUL) << 32) / DIV );
};

}
}

extern "C"{

inline s64 nnmathMultiplyAndDivide(s64 x, s32 mul, s32 div)
{
    const s64 rate = ((s64)(mul) << 32) / div;
    const s32 r_hi = (s32)(rate >> 32);
    const u32 r_lo = (u32)(rate >>  0);
    const s32 x_hi = (s32)(x    >> 32);
    const u32 x_lo = (u32)(x    >>  0);

    return (((s64)(x_hi) * r_hi) << 32)
         + ( (s64)(x_hi) * r_lo
           + (s64)(r_hi) * x_lo )
         + (((u64)(x_lo) * r_lo) >> 32);
}

inline u64 nnmathMakeRate(u32 mul, u32 div)
{
    return ((u64)(mul) << 32) / div;
}

inline u32 nnmathMakeRate32(u32 mul, u32 div)
{
    return (u32)( ((u64)(mul) << 32) / div );
}

inline u64 nnmathMultiplyRate(u64 x, u64 rate)
{
    const u32 r_hi = (u32)(rate >> 32);
    const u32 r_lo = (u32)(rate >>  0);
    const s32 x_hi = (u32)(x    >> 32);
    const u32 x_lo = (u32)(x    >>  0);

    return (((u64)(x_hi) * r_hi) << 32)
         + ( (u64)(x_hi) * r_lo
           + (u64)(r_hi) * x_lo )
         + (((u64)(x_lo) * r_lo) >> 32);
}

inline u64 nnmathMultiplyRate32(u64 x, u32 rate)
{
    const s32 x_hi = (u32)(x >> 32);
    const u32 x_lo = (u32)(x >>  0);

    return   (u64)(x_hi) * rate
         + (((u64)(x_lo) * rate) >> 32);
}

}