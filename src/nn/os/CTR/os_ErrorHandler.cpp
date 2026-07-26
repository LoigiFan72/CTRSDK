// Filename: os_ErrorHandler.cpp
//
// Project: Horizon

#include <nn/os/CTR/os_ErrorHandler.h>
#include <nn/os/CTR/os_Environment.h>
#include <nn/err/CTR/err_Api.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/Assert.h>

namespace nn{
namespace os{
namespace CTR{
namespace detail{

static s8 sPreferFatal;

void SetInternalErrorHandlingMode(bool preferFatal){
    nn::os::CTR::detail::sPreferFatal = preferFatal;
}

void HandleInternalError(Result result, const char* fileName, int lineNo){
    Result::Level level = result.GetLevel();
    if((IsRunOnDevelopmentHardWare()) && (sPreferFatal == 0)){
        if(level == Result::LEVEL_FATAL) err::CTR::ThrowFatalErr(result, NN_ERR_FATAL_TYPE_SYSTEM_COMMON);

        nndbgBreakWithResultTMessage_(NN_DBG_BREAK_REASON_PANIC,(nnResult)result, fileName, lineNo, "Unexpected Result Failure.");
    
    }
    else if((level != Result::LEVEL_STATUS) && (level != Result::LEVEL_INFO)) err::CTR::ThrowFatalErr(result, NN_ERR_FATAL_TYPE_SYSTEM_COMMON);
}

void HandleInternalError(Result result){
    Result::Level level = result.GetLevel();
    if((IsRunOnDevelopmentHardWare()) && (sPreferFatal == 0)){
        if(level == Result::LEVEL_FATAL) err::CTR::ThrowFatalErr(result, NN_ERR_FATAL_TYPE_SYSTEM_COMMON, __return_address());

        NN_TPANIC_WITH_RESULT_(result, "");

    }
    else if((level != Result::LEVEL_STATUS) && (level != Result::LEVEL_INFO)) err::CTR::ThrowFatalErr(result, NN_ERR_FATAL_TYPE_SYSTEM_COMMON, __return_address());
}

}
}
}
}