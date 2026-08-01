#pragma once

#include <nn/Result.h>

namespace nn {
namespace camera {
namespace CTR {

enum Description{
    DESCRIPTION_CAMERA_IS_SLEEPING = 1,
    DESCRIPTION_CAMERA_FATAL_ERROR
};

NN_DEFINE_RESULT_CONST(ResultIsSleeping,Result::LEVEL_STATUS, Result::SUMMARY_STATUS_CHANGED, Result::MODULE_NN_CAMERA, DESCRIPTION_CAMERA_IS_SLEEPING);

}
}
}