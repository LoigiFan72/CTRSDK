#pragma once

#include <nn/types.h>

#ifdef __cplusplus

namespace nn {
namespace nstd {

s32 TVSNPrintf (char* dst, size_t len, const char* fmt, va_list vlist);
s32 TSNPrintf (char* dst, size_t len, const char* fmt, ...);
s32 TSPrintf (char* dst, const char* fmt, ...);

s32 TVSNWPrintf (wchar_t* dst, size_t len, const wchar_t* fmt, va_list vlist);
s32 TSNWPrintf(wchar_t *dst, size_t len, const wchar_t *fmt, ...);
s32 TSWPrintf (wchar_t* dst, const wchar_t* fmt, ...);

} // namespace nstd
} // namespace nn

#endif

extern "C" {

s32 nnnstdTVSNPrintf (char* dst, size_t len, const char* fmt, va_list vlist);
s32 nnnstdTSNPrintf (char* dst, size_t len, const char* fmt, ...);
s32 nnnstdTSPrintf (char* dst, const char* fmt, ...);

s32 nnnstdTVSNWPrintf (wchar_t* dst, size_t len, const wchar_t* fmt, va_list vlist);
s32 nnnstdTSNWPrintf (wchar_t* dst, size_t len, const wchar_t* fmt, ...);
s32 nnnstdTSWPrintf (wchar_t* dst, const wchar_t* fmt, ...);
}