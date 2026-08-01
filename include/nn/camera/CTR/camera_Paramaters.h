#pragma once

namespace nn{
namespace camera{
namespace CTR{
namespace detail{
    enum CameraSelect {
        SELECT_NONE,
        SELECT_OUT1,
        SELECT_IN1,
        SELECT_IN1_OUT1,
        SELECT_OUT2,
        SELECT_OUT1_OUT2,
        SELECT_IN1_OUT2,
        SELECT_ALL
    };

    enum Port{
        PORT_NONE = 0,
        PORT_CAM1 = (1<<0),
        PORT_CAM2 = (1<<1),
        PORT_BOTH = (PORT_CAM1 | PORT_CAM2)
    };

}
}
}
}