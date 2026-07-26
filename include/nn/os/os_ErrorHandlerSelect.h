#pragma once

#include <nn/assert.h>
#include <nn/os/CTR/os_ErrorHandler.h>

#if defined(NN_BUILD_DEBUG) || defined(NN_BUILD_DEVELOPMENT)
    #define NN_OS_ERROR_IF_FAILED(result)           \
        do                                          \
        {                                           \
            ::Result nn_os_result = (result);   \
            if ( nn_os_result.IsFailure() )         \
            {                                       \
                ::nn::os::CTR::detail::HandleInternalError(nn_os_result, NN_FILE_NAME, __LINE__); \
            }                                       \
        } while(0)
#else

#define NN_OS_ERROR_IF_FAILED(result)           \
    do                                          \
    {                                           \
        ::Result nn_os_result = (result);   \
        if ( nn_os_result.IsFailure() )         \
        {                                       \
            ::nn::os::CTR::detail::HandleInternalError(nn_os_result); \
        }                                       \
    } while(0)

#endif