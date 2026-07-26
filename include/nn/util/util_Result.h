#pragma once

#include <nn/Result.h>
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

#define NN_UTIL_PANIC_IF_FAILED(result) NN_PANIC_IF_FAILED(result)