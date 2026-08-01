#pragma once

#include <nn/math/math_Vec3.h>

#pragma push
#pragma Otime

namespace nn{
namespace math{

class MTX23_{
public:
    struct BaseData{
        f32 _00;
        f32 _01;
        f32 _02;
        f32 _10;
        f32 _11;
        f32 _12;
    };
    union{
        BaseData f;
        float matrix[2][3];
        f32 a[6];
        VEC3_ v[2];
    };
};

class MTX23 : public MTX23_{
public:
};

}
}