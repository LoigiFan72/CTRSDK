#pragma once

#include <nn/Assert.h>

namespace nn{
namespace dbg{
/* Version > 2 */
namespace detail{
    void PrintResult(Result result);
    void TPrintResult(Result result);
}

    void PrintResult(Result result);
    void TPrintResult(Result result);
}
}

#ifdef NN_DEBUG
    #define NN_DBG_PRINT_RESULT(exp)    ::nn::dbg::PrintResult(exp)
    #define NN_DBG_PRINT_TRESULT(exp)    ::nn::dbg::TPrintResult(exp)
    #define NN_DBG_CHECK_RESULT(exp)    NN_PANIC_IF_FAILED(exp)
#else
    #define NN_DBG_PRINT_RESULT(exp)    ((void)(exp))
    #define NN_DBG_CHECK_RESULT(exp)    ((void)(exp))
#endif