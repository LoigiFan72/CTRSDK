#pragma once

#include <nn/math/math_Vec3.h>
#include <nn/math/math_Matrix34.h>

namespace nn{
namespace math{
inline VEC3* VEC3TransformNormal(VEC3* pOut, const MTX34* pM, const VEC3* pV){
    VEC3 tmp;
    tmp.x = pM->matrix[0][0] * pV->x + pM->matrix[0][1] * pV->y + pM->matrix[0][2] * pV->z;
    tmp.y = pM->matrix[1][0] * pV->x + pM->matrix[1][1] * pV->y + pM->matrix[1][2] * pV->z;
    tmp.z = pM->matrix[2][0] * pV->x + pM->matrix[2][1] * pV->y + pM->matrix[2][2] * pV->z;

    pOut->x = tmp.x;
    pOut->y = tmp.y;
    pOut->z = tmp.z;
    
    return pOut;
}
}
}