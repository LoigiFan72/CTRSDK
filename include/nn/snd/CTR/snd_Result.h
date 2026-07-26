#pragma once

#include <nn/Result.h>

namespace nn{
namespace snd{
namespace CTR{

NN_DEFINE_RESULT_CONST(
    ResultAlreadyInitialized,Result::LEVEL_INFO,Result::SUMMARY_NOTHING_HAPPENED,Result::MODULE_NN_SND,Result::DESCRIPTION_ALREADY_INITIALIZED
);
}
}
}