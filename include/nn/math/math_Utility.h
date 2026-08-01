#pragma once

#include <nn/types.h>

namespace nn { 
namespace math {

template <typename T, typename U, typename S>
inline S Max(T a, U b){
    return (a >= b) ? a: b;
}

template <typename T>
inline T Max(T a, T b){
    return (a >= b) ? a: b;
}

template <typename T, typename U, typename S>
inline S Min(T a, U b){
    return (a <= b) ? a: b;
}

template <typename T>
inline T Min(T a, T b){
    return (a <= b) ? a: b;
}

inline int Min(int a, int b){
    return (a <= b) ? a: b;
}

template <typename T>
inline T RoundUp(T x, u32 base){
    return static_cast<T>( (x + (base - 1)) & ~(base - 1) );
}

template <typename T>
inline T RoundDown(T x, u32 base){
    return static_cast<T>(x & ~(base - 1));
}

template <>
inline void* RoundDown(void* x, u32 base){
    return reinterpret_cast<void*>( RoundDown(reinterpret_cast<uptr>(x), base) );
}

template <>
inline const void* RoundDown(const void* x, u32 base){
    return reinterpret_cast<const void*>( RoundDown(reinterpret_cast<uptr>(x), base) );
}

template <typename T>
inline T GetBits(bit32 v, int pos, int len){
    return static_cast<T>( (v >> pos) & ((1u << len) - 1) );
}

template <typename T>
inline T GetBits(bit64 v, int pos, int len){
    return static_cast<T>( (v >> pos) & ((1ull << len) - 1) );
}

template <typename T>
inline T Clamp(T x, T low, T high){
    return (x >= high) ? high : ((x <= low) ? low: x);
}

}
}