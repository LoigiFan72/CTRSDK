#pragma once

#include "nn/Result.h"

namespace nn {
namespace dsp {
namespace CTR {
    
NN_DEFINE_RESULT_CONST(ResultNotInitialized,Result::LEVEL_STATUS,Result::SUMMARY_INVALID_STATE,Result::MODULE_NN_DSP,Result::DESCRIPTION_NOT_INITIALIZED);

NN_DEFINE_RESULT_CONST(ResultAlreadyExists,Result::LEVEL_STATUS,Result::SUMMARY_INVALID_STATE,Result::MODULE_NN_DSP,Result::DESCRIPTION_ALREADY_EXISTS);
}
}
}