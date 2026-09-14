#pragma once

#include <nn/types.h>

#ifdef __cplusplus

namespace nn {
namespace nstd {

s32 TVSNPrintf (char* dst, size_t len, const char* fmt, va_list vlist);
s32 TSNPrintf (char* dst, size_t len, const char* fmt, ...);
s32 TSPrintf (char* dst, const char* fmt, ...);

s32 TVSNPrintf (wchar_t* dst, size_t len, const wchar_t* fmt, va_list vlist);
s32 TSNPrintf(wchar_t *dst, size_t len, const wchar_t *fmt, ...);
s32 TSPrintf (wchar_t* dst, const wchar_t* fmt, ...);

} // namespace nstd
} // namespace nn

#endif

extern "C" {

s32 nnnstdTVSNWPrintf (char* dst, size_t len, const char* fmt, va_list vlist);
s32 nnnstdTSNWPrintf (char* dst, size_t len, const char* fmt, ...);
s32 nnnstdTSPWrintf (char* dst, const char* fmt, ...);

s32 nnnstdTVSNPrintf (wchar_t* dst, size_t len, const wchar_t* fmt, va_list vlist);
s32 nnnstdTSNPrintf (wchar_t* dst, size_t len, const wchar_t* fmt, ...);
s32 nnnstdTSPrintf (wchar_t* dst, const wchar_t* fmt, ...);
}