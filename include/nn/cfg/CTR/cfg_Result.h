#pragma once

#include <nn/Result.h>

namespace nn {
namespace cfg{ 
namespace CTR{

NN_DEFINE_RESULT_CONST(ResultCancelRequested, Result::LEVEL_PERMANENT, Result::SUMMARY_CANCELLED, Result::MODULE_NN_CFG, Result::DESCRIPTION_CANCEL_REQUESTED);

}
}
}