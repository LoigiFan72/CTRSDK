#pragma once

#include <nn/Result.h>
#include <nn/dbg/dbg_Enum.h>

/* Panic() and Break() */

/* Use these! */
#ifdef __cplusplus
extern "C"{
#endif

Result nndbgBreak(nn::dbg::BreakReason reason);

void nndbgPanic();


void nndbgBreakWithMessage_ (nndbgBreakReason reason, const char* filename, int lineno, const char* fmt, ...);
void nndbgBreakWithTMessage_(nndbgBreakReason reason, const char* filename, int lineno, const char* fmt, ...);

void nndbgBreakWithResultMessage_ (nndbgBreakReason reason, nnResult result, const char* filename, int lineno, const char* fmt, ...);
void nndbgBreakWithResultTMessage_(nndbgBreakReason reason, nnResult result, const char* filename, int lineno, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

namespace nn{
namespace dbg{

    typedef void (*BreakHandler)(BreakReason reason, Result* pResult, const char* filename, int lineno, const char* fmt, std::va_list args);

    Result Break(BreakReason reason);
    void Panic();

namespace detail{
namespace CTR{

struct DllInfo{
    uptr pathAddress;
    s32  pathLength;
    uptr erAddress;
    uptr rwAddress;
};

} // namespace CTR

Result NotifyDllLoadedToDebugger(const void* pDllInfo, size_t size);
Result NotifyDllUnloadingToDebugger(const void* pDllInfo, size_t size);

} // namespace detail
} // namespace dbg
} // namespace nn