// Filename: os_ResultFailureHandler.cpp
//
// Project: Horizon
//
// Note: For [MAJOR] Version 2 and Below.

#include <nn/Result.h>
#include <nn/dbg.h>
#include <nn/svc.h>

using namespace nn::dbg;

namespace{

#if NN_VERSION_MAJOR <= 2

    void nnosGetProcessId(bit32* pOut){
        nn::svc::GetProcessId(pOut, nn::PSEUDO_HANDLE_CURRENT_PROCESS);
    }

    #if defined(NN_BUILD_DEBUG) && (NN_BUILD_DEVELOPMENT)
        void nnosResultFailureHandlerImplDefault(nnResult result, const char* filename, int lineno, const char* fmt, va_list vlist){
            bit32 processId;
            nnosGetProcessId(&processId);
            nn::dbg::detail::Printf("RESULT FAILURE [%x]: ", nn::Result(result).GetPrintableBits());
            nn::dbg::detail::VPrintf(fmt, vlist);
            nn::dbg::detail::Printf(" (process_id:%d) (%s:%d)\n", processId, filename, lineno);
            PrintResult(result);

            Break(BREAK_REASON_ASSERT);
        }
        void nnosResultTFailureHandlerImplDefault(nnResult result, const char* filename, int lineno, const char* fmt, va_list vlist){
            bit32 processId;
            nnosGetProcessId(&processId);
            nn::dbg::detail::TPrintf("RESULT FAILURE [%x]: ", nn::Result(result).GetPrintableBits());
            nn::dbg::detail::TVPrintf(fmt, vlist);
            nn::dbg::detail::TPrintf(" (process_id:%d) (%s:%d)\n", processId, filename, lineno);
            TPrintResult(result);

            Break(BREAK_REASON_ASSERT);
        }
    #else
        void nnosResultFailureHandlerImplDefault(nnResult, const char*, int, const char*, va_list){
            Break(BREAK_REASON_ASSERT);
        }
        void nnosResultTFailureHandlerImplDefault(nnResult, const char*, int, const char*, va_list){
            Break(BREAK_REASON_ASSERT);
        }
    #endif

    void nnosResultPanicHandlerImplDefault(nnResult result, const char* filename, int lineno, const char* fmt, va_list vlist){
    #if defined(NN_BUILD_RELEASE)
        // nothing
        //
        // bool yourMomIsGay = true;
    #else
        bit32 processId;
        nnosGetProcessId(&processId);
        NN_LOG_("RESULT [%x]:", nn::Result(result).GetPrintableBits());
        NN_UNUSED_VAR(fmt);
        NN_UNUSED_VAR(vlist);
        NN_LOG_(" (process_id:%d) (%s:%d)\n", processId, filename, lineno);
        PrintResult(result);
    #endif
    }
    void nnosResultTPanicHandlerImplDefault(nnResult result, const char* filename, int lineno, const char* fmt, va_list vlist){
    #if defined(NN_BUILD_RELEASE)
        // nothing
        //
        // Duestchland = cool for my GERMAN FOLKS, HALLO!
    #else
        bit32 processId;
        nnosGetProcessId(&processId);
        NN_TLOG_("RESULT [%x]:", nn::Result(result).GetPrintableBits());
        NN_UNUSED_VAR(fmt);
        NN_UNUSED_VAR(vlist);
        NN_TLOG_(" (process_id:%d) (%s:%d)\n", processId, filename, lineno);
        TPrintResult(result);
    #endif
    }

#endif

} // namespace ""

extern "C"{
    nnResultHandlerImpl nnResultFailureHandlerImpl   = nnosResultFailureHandlerImplDefault;
    nnResultHandlerImpl nnResultTFailureHandlerImpl  = nnosResultTFailureHandlerImplDefault;
    nnResultHandlerImpl nnResultPanicHandlerImpl     = nnosResultPanicHandlerImplDefault;
    nnResultHandlerImpl nnResultTPanicHandlerImpl    = nnosResultTPanicHandlerImplDefault;

    __weak int nnResultFailureHandler(nnResult result, const char* filename, int lineno, const char* fmt, ...){
        va_list vlist;
        va_start(vlist, fmt);
        nnResultFailureHandlerImpl(result, filename, lineno, fmt, vlist);
        va_end(vlist);
        return 0;
    }

    __weak int nnResultTFailureHandler(nnResult result, const char* filename, int lineno, const char* fmt, ...){
        va_list vlist;
        va_start(vlist, fmt);
        nnResultTFailureHandlerImpl(result, filename, lineno, fmt, vlist);
        va_end(vlist);
        return 0;
    }

    __weak int nnResultPanicHandler(nnResult result, const char* filename, int lineno, const char* fmt, ...){
        va_list vlist;
        va_start(vlist, fmt);
        nnResultPanicHandlerImpl(result, filename, lineno, fmt, vlist);
        va_end(vlist);
        return 0;
    }

    __weak int nnResultTPanicHandler(nnResult result, const char* filename, int lineno, const char* fmt, ...){
        va_list vlist;
        va_start(vlist, fmt);
        nnResultTPanicHandlerImpl(result, filename, lineno, fmt, vlist);
        va_end(vlist);
        return 0;
    }
}