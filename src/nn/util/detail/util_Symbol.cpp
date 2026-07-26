// Filename: util_Symbol.cpp
//
// Project: Horizon

#include <nn/util/detail/util_Symbol.h>


#ifdef __cplusplus
extern "C" {
#endif

asm void nnutilReferSymbol_(const void* sym __attribute__((unused)), ...){
    bx lr
}

#ifdef __cplusplus
} // extern "C"
#endif