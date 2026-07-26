#pragma once

#include <nn/Assert.h>

#ifdef NN_DEBUG
    #define NN_DBG_PRINT_RESULT(exp)    ::nn::dbg::detail::PrintResult(exp)
    #define NN_DBG_CHECK_RESULT(exp)    NN_PANIC_IF_FAILED(exp)
#else
    #define NN_DBG_PRINT_RESULT(exp)    ((void)(exp))
    #define NN_DBG_CHECK_RESULT(exp)    ((void)(exp))
#endif