// Filename: dbg_Break.cpp
//
// Project: Horizon

#include <nn/dbg/dbg_Break.h>
#include <nn/dbg/dbg_DebugString.h>
#include <nn/svc.h>
#include <cstdarg>

using namespace nn::dbg;

namespace{
    void PrintErrorMessageHeader(nndbgBreakReason reason, const char* filename, int lineno){
        detail::TPrintf("----\n");
        if(reason == NN_DBG_BREAK_REASON_ASSERT){
            detail::TPrintf("Assertion failure at %s:%d\n  ", filename, lineno);
        }
        else{
            detail::TPrintf("Panic at %s:%d\n  ", filename, lineno);
        }
    }
}

//#endif

namespace{

static nn::dbg::BreakHandler spBreakHandler = NULL;

/* nn::dbg::Break Call Handlers */

void CallBreakHandler(nn::dbg::BreakReason reason, Result* pResult, const char* filename, int lineno, const char* fmt, std::va_list args){
    nn::dbg::BreakHandler pBreakHandler = spBreakHandler;

    if (pBreakHandler != NULL){
        spBreakHandler = NULL;
        pBreakHandler(reason, pResult, filename, lineno, fmt, args);
    }
}

void CallBreakHandler(nndbgBreakReason reason, nnResult result, const char* filename, int lineno, const char* fmt, std::va_list args){
    Result resultCpp = result;
    CallBreakHandler(static_cast<nn::dbg::BreakReason>(reason), &resultCpp, filename, lineno, fmt, args);
}

void CallBreakHandler(nndbgBreakReason reason, const char* filename, int lineno, const char* fmt, std::va_list args){
    CallBreakHandler(static_cast<nn::dbg::BreakReason>(reason), NULL, filename, lineno, fmt, args);
}

void CallBreakHandler(nn::dbg::BreakReason reason){
    std::va_list args;
    CallBreakHandler(reason, NULL, NULL, 0, NULL, args);
}

}
namespace nn{
namespace dbg{

/* dbg::Break */

NN_NOINLINE Result Break(BreakReason reason){
    CallBreakHandler(reason);
    return nn::svc::Break(reason, NULL, 0);
}

/* dbg::Panic */

void Panic(){
    Break(BREAK_REASON_PANIC);
}

namespace detail{

/* nn::ro things */

Result NotifyDllLoadedToDebugger(const void* pDllInfo, size_t size){
    return nn::svc::Break(BREAK_REASON_UNLOAD_RO, pDllInfo, size);
}

Result NotifyDllUnloadingToDebugger(const void* pDllInfo, size_t size){
    return nn::svc::Break(BREAK_REASON_UNLOAD_RO, pDllInfo, size);
}

}
}
}

extern "C" {

//#if defined(NN_BUILD_DEBUG) || defined(NN_BUILD_DEVELOPMENT)

void nndbgBreakWithMessage_ (nndbgBreakReason reason, const char* filename, int lineno, const char* fmt, ...){
    va_list arg;
    va_start(arg, fmt);
    PrintErrorMessageHeader(reason, filename, lineno);
    detail::VPrintf(fmt, arg);
    detail::TPrintf("\n");
    va_end(arg);

    va_start(arg, fmt);
    CallBreakHandler(reason, filename, lineno, fmt, arg);
    va_end(arg);

    Break(static_cast<BreakReason>(reason));
}

void nndbgBreakWithTMessage_(nndbgBreakReason reason, const char* filename, int lineno, const char* fmt, ...){
    va_list arg;
    va_start(arg, fmt);
    PrintErrorMessageHeader(reason, filename, lineno);
    detail::TVPrintf(fmt, arg);
    detail::TPrintf("\n");
    va_end(arg);

    va_start(arg, fmt);
    CallBreakHandler(reason, filename, lineno, fmt, arg);
    va_end(arg);

    Break(static_cast<BreakReason>(reason));
}

void nndbgBreakWithResultMessage_ (nndbgBreakReason reason, nnResult result, const char* filename, int lineno, const char* fmt, ...){
    va_list arg;
    va_start(arg, fmt);
    PrintErrorMessageHeader(reason, filename, lineno);
    detail::VPrintf(fmt, arg);
    detail::TPrintf("\n");
    detail::PrintResult(result);
    va_end(arg);

    va_start(arg, fmt);
    CallBreakHandler(reason, result, filename, lineno, fmt, arg);
    va_end(arg);

    Break(static_cast<BreakReason>(reason));
}

void nndbgBreakWithResultTMessage_(nndbgBreakReason reason, nnResult result, const char* filename, int lineno, const char* fmt, ...){
    va_list arg;
    va_start(arg, fmt);
    PrintErrorMessageHeader(reason, filename, lineno);
    detail::TVPrintf(fmt, arg);
    detail::TPrintf("\n");
    detail::PrintResult(result);
    va_end(arg);

    va_start(arg, fmt);
    CallBreakHandler(reason, result, filename, lineno, fmt, arg);
    va_end(arg);

    Break(static_cast<BreakReason>(reason));
}

void nndbgPanic(){
    nn::dbg::Panic();
}

Result nndbgBreak(nn::dbg::BreakReason reason){
    nn::dbg::Break(reason);
}

}