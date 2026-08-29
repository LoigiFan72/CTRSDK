#pragma once

#include <nn/types.h>

typedef enum nnerrFatalErrType
{
    NN_ERR_FATAL_TYPE_SYSTEM_COMMON,
    NN_ERR_FATAL_TYPE_NAND_CORRUPTION,
    NN_ERR_FATAL_TYPE_CARD_EJECTION,
    NN_ERR_FATAL_TYPE_EXCEPTION,
    NN_ERR_FATAL_TYPE_RESULT_FAILURE,
    NN_ERR_FATAL_TYPE_LOG_ONLY
} nnerrFatalErrType;