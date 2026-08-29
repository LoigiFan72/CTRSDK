#pragma once

#include <nn/Assert.h>

#define NN_UTIL_RETURN_IF_FAILED_BASE(result, s1, s2, s3, s4, s5) \
    do \
    { \
        ::Result nn_util_return_if_failure_result = (result); \
        if (nn_util_return_if_failure_result.IsFailure()) \
        { \
            s1; s2; s3; s4; s5; \
            return nn_util_return_if_failure_result; \
        } \
    } while(0)

#define NN_UTIL_RETURN_IF_FAILED(result) NN_UTIL_RETURN_IF_FAILED_BASE(result,,,,,)

#define NN_UTIL_RETURN_IF_FAILED_0(result) NN_UTIL_RETURN_IF_FAILED_BASE(result,,,,,)
#define NN_UTIL_RETURN_IF_FAILED_1(result, c1) NN_UTIL_RETURN_IF_FAILED_BASE(result,c1,,,,)
#define NN_UTIL_RETURN_IF_FAILED_2(result, c1, c2) NN_UTIL_RETURN_IF_FAILED_BASE(result,c1,c2,,,)
#define NN_UTIL_RETURN_IF_FAILED_3(result, c1, c2, c3) NN_UTIL_RETURN_IF_FAILED_BASE(result,c1,c2,c3,,)
#define NN_UTIL_RETURN_IF_FAILED_4(result, c1, c2, c3, c4) NN_UTIL_RETURN_IF_FAILED_BASE(result,c1,c2,c3,c4,)
#define NN_UTIL_RETURN_IF_FAILED_5(result, c1, c2, c3, c4, c5) NN_UTIL_RETURN_IF_FAILED_BASE(result,c1,c2,c3,c4,c5)

#if NN_VERSION_MAJOR > 2

#define NN_UTIL_PANIC_IF_FAILED(result) NN_PANIC_IF_FAILED(result)

#else

#define NN_UTIL_BEGIN_CHECK_RESULT(result) \
    { \
        ::nn::Result nn_util_result_try_result = (result); \
        if (nn_util_result_try_result.IsFailure()) \
        { \
            do {

#define NN_UTIL_ADD_RESULT_MESSAGE(expected, ...) \
                if (nn_util_result_try_result == (expected)) \
                { \
                    ::nnResultFailureHandler(nn_util_result_try_result, NN_FILE_NAME, __LINE__, __VA_ARGS__); \
                    break; \
                }

#define NN_UTIL_END_CHECK_RESULT \
                ::nnResultTFailureHandler(nn_util_result_try_result, NN_FILE_NAME, __LINE__, "Unexpected Result Failure."); \
            } while (0); \
            ::nn::dbg::Break(::nn::dbg::BREAK_REASON_PANIC); \
        } \
    }

#define NN_UTIL_PANIC_IF_FAILED(result)                     \
    do {                                                    \
        NN_UTIL_BEGIN_CHECK_RESULT(result)                  \
        NN_TLOG_("RESULT FAILURE: result = %s\n", #result);   \
        NN_UTIL_END_CHECK_RESULT                            \
    } while (0)

#endif