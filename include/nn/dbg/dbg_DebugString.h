#include "nn/Result.h"
#include "nn/types.h"
#include <cstdarg>

namespace nn{
namespace dbg{
namespace detail{
    void Printf(const char* fmt, ...);
    void TPrintf(const char* fmt, ...);
    void VPrintf(const char* fmt, ::std::va_list arg);
    void TVPrintf(const char* fmt, ::std::va_list arg);
    void PrintResult(Result res);
    void PutString(const char* text, s32 length);
    void PutString(const char* text);
}
}
}

extern "C"{
    void nndbgPrintWarning_ (const char* filename, int lineno, const char* fmt, ...);
    void nndbgTPrintWarning_(const char* filename, int lineno, const char* fmt, ...);
}

#ifdef NN_DEBUG
    #ifdef __cplusplus
            #define NN_LOG_(...)           (void)nn::dbg::detail::Printf(__VA_ARGS__)
            #define NN_SLOG_(fmt, arg)     (void)nn::dbg::detail::Printf(__VA_ARGS__)
            #define NN_TLOG_(text, length)  (void)nn::dbg::detail::TPrintf(__VA_ARGS__)
        #else
            #define NN_LOG_(...)           (void)nndbgDetailPrintf(__VA_ARGS__)
            #define NN_LOGV_(fmt, arg)     (void)nndbgDetailVPrintf((fmt), (arg))
            #define NN_PUT_(text, length)  (void)nndbgDetailPutString((text), (length))
    #endif
#else

#define NN_LOG_(exp, ...)
#define NN_SLOG_(exp, ...)
#define NN_TLOG_(exp, ...)

#endif