// Filename: dbg_DebugString.cpp
//
// Project: Horizon

#include <nn/dbg/dbg_DebugString.h>

#include <nn/nstd/nstd_Printf.h>
#include <nn/module.h>
#include <nn/svc.h>

#include <string.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>

#define NN_DBG_PRINTF_BUFFER_LENGTH  256
#define NN_DBG_TPRINTF_BUFFER_LENGTH 128

NN_MAKE_MODULE(sUsePutDebugString, "NINTENDO", "DebugPrint");

namespace nn{
namespace dbg{
namespace detail{
    void PutString(const char* text, s32 length){
        NN_REFER_MODULE(sUsePutDebugString);
        nn::svc::OutputDebugString(text, length);
    }

    void PutString(const char* text){
        PutString(text, strlen(text));
    }

    void TVPrintf(const char* fmt, ::std::va_list arg){
        char buf[NN_DBG_TPRINTF_BUFFER_LENGTH];
        int length = nstd::TVSNPrintf(buf, sizeof(buf), fmt, arg);
        if (length >= NN_DBG_TPRINTF_BUFFER_LENGTH){
            length = NN_DBG_TPRINTF_BUFFER_LENGTH - 1;
        }
        detail::PutString(buf, length);
    }

    void TPrintf(const char* fmt, ...){
        va_list vlist;

        va_start(vlist, fmt);
        nn::dbg::detail::TVPrintf(fmt, vlist);
        va_end(vlist);
    }

    __weak void VPrintf(const char* fmt, ::std::va_list arg){
        char buf[NN_DBG_PRINTF_BUFFER_LENGTH];
        int length = ::std::vsnprintf(buf, sizeof(buf), fmt, arg);
        if (length >= NN_DBG_PRINTF_BUFFER_LENGTH){
            length = NN_DBG_PRINTF_BUFFER_LENGTH - 1;
        }
        PutString(buf, length);
    }
}
}
}

extern "C"{
    void nndbgPrintWarning_ (const char* filename, int lineno, const char* fmt, ...){
        va_list vlist;

        va_start(vlist, fmt);
        nn::dbg::detail::TPrintf("%s:%d [WARN] ", filename, lineno);
        nn::dbg::detail::VPrintf(fmt, vlist);
        nn::dbg::detail::TPrintf("\n");
        va_end(vlist);
    }
    void nndbgTPrintWarning_(const char* filename, int lineno, const char* fmt, ...){
        va_list vlist;

        va_start(vlist, fmt);
        nn::dbg::detail::TPrintf("%s:%d [WARN] ", filename, lineno);
        nn::dbg::detail::TVPrintf(fmt, vlist);
        nn::dbg::detail::TPrintf("\n");
        va_end(vlist);
    }
}

//#endif