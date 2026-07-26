#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <new>
#include <limits>

#ifndef NULL
#define NULL (void*)0
#endif

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef unsigned short ushort;
typedef unsigned int uint;

typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;

typedef uintptr_t uptr;
typedef intptr_t ptr;

typedef float              f32;
typedef double             f64;

typedef volatile u8       vu8;
typedef volatile u16      vu16;
typedef volatile u32      vu32;
typedef volatile u64      vu64;

typedef volatile s8       vs8;
typedef volatile s16      vs16;
typedef volatile s32      vs32;
typedef volatile s64      vs64;

typedef volatile f32      vf32;
typedef volatile f64      vf64;

typedef char     char8;
typedef wchar_t  char16;

typedef unsigned char           bit8;
typedef unsigned short          bit16;
typedef unsigned int            bit32;
typedef unsigned long long int  bit64; 

typedef s32 sptr;
typedef u32 uptr;

typedef bit64 ProgramId;

#ifndef nullptr
#define nullptr NULL
#endif

#ifndef NN_UNUSED
#define NN_UNUSED __attribute__ ((unused))
#endif

#define NN_UNUSED_VAR(x) ((void)(x))

#define splits(S) __attribute__((section("i." #S))) S

/* Defines */

#define NN_INLINE inline
#define NN_NOINLINE __attribute__((noinline))

#ifdef NN_BUILD_DEBUG
    #pragma O0
#endif