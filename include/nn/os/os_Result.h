#pragma once

#include <nn/Result.h>

namespace nn {
namespace os {
        enum Description{
                DESCRIPTION_FAILED_TO_ALLOCATE_MEMORY          = 1,
                DESCRIPTION_FAILED_TO_ALLOCATE_SHARED_MEMORY   = 2,
                DESCRIPTION_FAILED_TO_ALLOCATE_THREAD          = 3,
                DESCRIPTION_FAILED_TO_ALLOCATE_MUTEX           = 4,
                DESCRIPTION_FAILED_TO_ALLOCATE_SEMAPHORE       = 5,
                DESCRIPTION_FAILED_TO_ALLOCATE_EVENT           = 6,
                DESCRIPTION_FAILED_TO_ALLOCATE_TIMER           = 7,
                DESCRIPTION_FAILED_TO_ALLOCATE_PORT            = 8,
                DESCRIPTION_FAILED_TO_ALLOCATE_SESSION         = 9,
                DESCRIPTION_EXCEED_MEMORY_LIMIT                = 10,
                DESCRIPTION_EXCEED_SHARED_MEMORY_LIMIT         = 11,
                DESCRIPTION_EXCEED_THREAD_LIMIT                = 12,
                DESCRIPTION_EXCEED_MUTEX_LIMIT                 = 13,
                DESCRIPTION_EXCEED_SEMAPHORE_LIMIT             = 14,
                DESCRIPTION_EXCEED_EVENT_LIMIT                 = 15,
                DESCRIPTION_EXCEED_TIMER_LIMIT                 = 16,
                DESCRIPTION_EXCEED_PORT_LIMIT                  = 17,
                DESCRIPTION_EXCEED_SESSION_LIMIT               = 18,
                DESCRIPTION_MAX_HANDLE                         = 19,
                DESCRIPTION_INACCESSIBLE_PAGE                  = 20,
                DESCRIPTION_ABANDONED                          = 21,
                DESCRIPTION_INVALID_PROCESS_ID                 = 24,
                DESCRIPTION_INVALID_THREAD_ID                  = 25,
                DESCRIPTION_SESSION_CLOSED                     = 26,
                DESCRIPTION_INVALID_MESSAGE                    = 28,
                DESCRIPTION_MANUAL_RESET_EVENT_REQUIRED        = 29,
                DESCRIPTION_TOO_LONG_NAME                      = 30,
                DESCRIPTION_NOT_OWNED                          = 31,
                DESCRIPTION_PROCESS_TERMINATED                 = 32,
                DESCRIPTION_INVALID_TLS_INDEX                  = 33,
                DESCRIPTION_NO_RUNNABLE_PROCESSOR              = 34,
                DESCRIPTION_NO_SESSION                         = 35,
                DESCRIPTION_USING_REGION                       = 36,
                DESCRIPTION_ALREADY_RECEIVED                   = 37,
                DESCRIPTION_CANCEL_REQUESTD                    = 38,
                DESCRIPTION_NOT_RECEIVED                       = 39,
                DESCRIPTION_DELIVER_ARG_NOT_READY              = 41,
                DESCRIPTION_DELIVER_ARG_OVER_SIZE              = 42,
                DESCRIPTION_INVALID_DELIVER_ARG                = 43,
                DESCRIPTION_I_AM_OWNER                         = 44,
                DESCRIPTION_EXCEEDS_SHARED_LIMIT               = 45,
                DESCRIPTION_UNEXPECTED_PERMISSION              = 46,
                DESCRIPTION_INVALID_TAG                        = 47,
                DESCRIPTION_INVALID_FORMAT                     = 48,
                DESCRIPTION_OTHER_HANDLE                       = 49,
                DESCRIPTION_FAILED_TO_ALLOCATE_ADDRESS_ARBITER = 50,
                DESCRIPTION_EXCEED_ADDRESS_ARBITER_LIMIT       = 51,
                DESCRIPTION_OVER_PORT_CAPACITY                 = 52,
                DESCRIPTION_NOT_MAPPED                         = 53,
                DESCRIPTION_NO_ADDRESS_SPACE                   = 55,
                DESCRIPTION_EXCEED_TLS_LIMIT                   = 56,
                DESCRIPTION_OBSOLETE_RESULT                    = 1023
        };

NN_DEFINE_RESULT_CONST(ResultOverPortCapacity,Result::LEVEL_TEMPORARY, Result::SUMMARY_WOULD_BLOCK, Result::MODULE_NN_OS, DESCRIPTION_OVER_PORT_CAPACITY);

NN_DEFINE_RESULT_CONST(ResultNoAddressSpace,Result::LEVEL_PERMANENT, Result::SUMMARY_OUT_OF_RESOURCE, Result::MODULE_NN_OS, DESCRIPTION_NO_ADDRESS_SPACE);

NN_DEFINE_RESULT_CONST(ResultAlreadyInitialized,Result::LEVEL_INFO, Result::SUMMARY_INVALID_STATE, Result::MODULE_NN_OS, nn::Result::DESCRIPTION_ALREADY_INITIALIZED);

NN_DEFINE_RESULT_CONST(ResultMisalignedAddress,Result::LEVEL_USAGE, Result::SUMMARY_INVALID_ARGUMENT, Result::MODULE_NN_OS, nn::Result::DESCRIPTION_MISALIGNED_ADDRESS);

NN_DEFINE_RESULT_CONST(ResultMisalignedSize,Result::LEVEL_USAGE, Result::SUMMARY_INVALID_ARGUMENT, Result::MODULE_NN_OS, nn::Result::DESCRIPTION_MISALIGNED_SIZE);

}
}