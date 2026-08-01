#pragma once

#include <cmath>
#include <nn/assert.h>
#include <nn/math/math_Vec3.h>

namespace nn{
namespace math{

inline VEC3* VEC3SafeNormalize(VEC3* pOut, const VEC3* p, const VEC3& alt);
inline VEC3* VEC3SafeNormalize(VEC3* pOut, const VEC3& v, const VEC3& alt) { return VEC3SafeNormalize(pOut, &v, alt); }

inline VEC3* VEC3SafeNormalize(VEC3* pOut, const VEC3* p, const VEC3& alt){
    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(p);

    register f32 x, y, z, mag;

    x = p->x;
    y = p->y;
    z = p->z;

    mag = (x * x) + (y * y) + (z * z);

    if (mag == 0){
        *pOut = alt;

        return pOut;
    }

    mag = 1.0f / ::std::sqrtf(mag);

    x *= mag;
    y *= mag;
    z *= mag;

    pOut->x = x;
    pOut->y = y;
    pOut->z = z;

    return pOut;
}

}
}