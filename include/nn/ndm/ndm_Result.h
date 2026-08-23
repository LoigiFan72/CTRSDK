#pragma once

#include <nn/Result.h>
#include <nn/util/util_Result.h>

namespace nn{
namespace ndm{

NN_DEFINE_RESULT_FUNC_LSM(MakeInvalidArgumentResult, Result::LEVEL_PERMANENT, Result::SUMMARY_INVALID_ARGUMENT, Result::MODULE_NN_NDM);
NN_DEFINE_RESULT_CONST_LSM(ResultInvalidEnumValue,MakeInvalidArgumentResult, Result::DESCRIPTION_INVALID_ENUM_VALUE);

}
}