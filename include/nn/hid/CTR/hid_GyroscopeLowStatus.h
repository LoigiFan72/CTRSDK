#pragma once

#include <nn/types.h>

namespace nn {
namespace hid {
namespace CTR {

typedef struct{
    s16         x;
    s16         y;
    s16         z;
}GyroscopeLowStatus;

struct GyroscopeLowCalibrateAxisParam{
    s16 rpm0;
    s16 rpmPositive;
    s16 rpmNegative;
};

struct GyroscopeLowCalibrateParam{
    GyroscopeLowCalibrateAxisParam x;
    GyroscopeLowCalibrateAxisParam y;
    GyroscopeLowCalibrateAxisParam z;
};


struct GyroscopeLowCalibrateAxisScaleParam{
    s16 rpm0;
    u8 padding[6];
    f64 scale;
};


struct GyroscopeLowCalibrateScaleParam{
    GyroscopeLowCalibrateAxisScaleParam x;
    GyroscopeLowCalibrateAxisScaleParam y;
    GyroscopeLowCalibrateAxisScaleParam z;
};


}
}
}