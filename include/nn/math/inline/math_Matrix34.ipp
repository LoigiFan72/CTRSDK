#pragma once

#include <cmath>
#include <nn/math/math_Matrix34.h>
#include <nn/math/math_Vec3.h>
#include <nn/Assert.h>

namespace nn {
namespace math {

inline MTX34* MTX34TextureProjectionFrustum(MTX34* pOut, f32 l, f32 r, f32 b, f32 t, f32 n, f32 scaleS, f32 scaleT, f32 translateS, f32 translateT){

    f32 reverseWidth = 1.0f / (r - l);

    f32 (*const mtx)[4] = pOut->matrix;

    mtx[0][0] = ((2.0f * n) * reverseWidth) * scaleS;
    mtx[0][1] = 0.0f;
    mtx[0][2] = (((r + l) * reverseWidth) * scaleS) - translateS;
    mtx[0][3] = 0.0f;

    f32 reverseHeight = 1.0f / (t - b);
    mtx[1][0] = 0.0f;
    mtx[1][1] = ((2.0f * n) * reverseHeight) * scaleT;
    mtx[1][2] = (((t+ b) * reverseHeight) * scaleT) - translateT;
    mtx[1][3] = 0.0f;

    mtx[2][0] = 0.0f;
    mtx[2][1] = 0.0f;
    mtx[2][2] = -1.0f;
    mtx[2][3] = 0.0f;
    return pOut;
}

inline MTX34* MTX34TextureProjectionPerspective(MTX34* pOut, f32 fovy, f32 aspect, f32 scaleS, f32 scaleT, f32 translateS, f32 translateT){
    f32 angle = fovy * 0.5f;
    f32 cot = 1.0f / math::TanRad(angle);

    f32 (*const mtx)[4] = pOut->matrix;

    mtx[0][0] = (cot / aspect) * scaleS;
    mtx[0][1] = 0.0f;
    mtx[0][2] = -translateS;
    mtx[0][3] = 0.0f;

    mtx[1][0] = 0.0f;
    mtx[1][1] = cot * scaleT;
    mtx[1][2] = -translateT;
    mtx[1][3] = 0.0f;

    mtx[2][0] = 0.0f;
    mtx[2][1] = 0.0f;
    mtx[2][2] = -1.0f;
    mtx[2][3] = 0.0f;

    return pOut;
}

namespace ARMv6{

MTX34* MTX34LookAtC_FAST(MTX34* pOut, const VEC3* pCamPos, const VEC3* pCamUp, const VEC3* pTarget){
    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(pCamPos);
    NN_NULL_ASSERT_(pCamUp);
    NN_NULL_ASSERT_(pTarget);

    f32 (*const m)[4] = pOut->matrix;

    f32 vLookx, vLooky, vLookz;{
        vLookx = pCamPos->x - pTarget->x;
        vLooky = pCamPos->y - pTarget->y;
        vLookz = pCamPos->z - pTarget->z;
    }

    {
        f32 mag = (vLookx * vLookx) + (vLooky * vLooky) + (vLookz * vLookz);
    
        NN_ASSERTMSG_(mag != 0, "MATHNormalize3():  zero magnitude vector");
    
        mag = 1.0f / ::std::sqrtf(mag);
    
    
        vLookx = vLookx * mag;
        vLooky = vLooky * mag;
        vLookz = vLookz * mag;
    }

    f32 vRightx, vRighty, vRightz;

    {
        vRightx = ( pCamUp->y * vLookz ) - ( pCamUp->z * vLooky );
        vRighty = ( pCamUp->z * vLookx ) - ( pCamUp->x * vLookz );
        vRightz = ( pCamUp->x * vLooky ) - ( pCamUp->y * vLookx );
    }

    {
        f32 mag = (vRightx * vRightx) + (vRighty * vRighty) + (vRightz * vRightz);
    
        NN_ASSERTMSG_(mag != 0, "MATHNormalize3():  zero magnitude vector");
    
        mag = 1.0f / ::std::sqrtf(mag);

        vRightx = vRightx * mag;
        vRighty = vRighty * mag;
        vRightz = vRightz * mag;
    }

    f32 vUpx, vUpy, vUpz;
    {
        vUpx = (vLooky * vRightz) - (vLookz * vRighty);
        vUpy = (vLookz * vRightx) - (vLookx * vRightz);
        vUpz = (vLookx * vRighty) - (vLooky * vRightx);
    }

    f32 tmp1, tmp2, tmp3;

    tmp1 = -(pCamPos->x * vRightx + pCamPos->y * vRighty + pCamPos->z * vRightz);
    tmp2 = -(pCamPos->x * vUpx + pCamPos->y * vUpy + pCamPos->z * vUpz);
    tmp3 = -(pCamPos->x * vLookx + pCamPos->y * vLooky + pCamPos->z * vLookz);
    
    m[0][0] = vRightx;
    m[0][1] = vRighty;
    m[0][2] = vRightz;

    m[1][0] = vUpx;
    m[1][1] = vUpy;
    m[1][2] = vUpz;

    m[2][0] = vLookx;
    m[2][1] = vLooky;
    m[2][2] = vLookz;

    m[0][3] = tmp1;
    m[1][3] = tmp2;
    m[2][3] = tmp3;

    return pOut;
}

}
}
}