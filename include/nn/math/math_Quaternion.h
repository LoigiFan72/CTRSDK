#pragma once

#include <cmath>

namespace nn{
namespace math{

class MTX34;

struct QUAT_{
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

class QUAT : public QUAT_{
public:
    typedef QUAT self_type;
    typedef f32  value_type;
public:
    QUAT() {}
    explicit QUAT(const f32* p) { x = p[0]; y = p[1]; z = p[2]; w = p[3]; }
    QUAT(const QUAT_& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w; }
    QUAT(f32 fx, f32 fy, f32 fz, f32 fw) { x = fx; y = fy; z = fz; w = fw; }

    operator f32*() { return &x; }
    operator const f32*() const { return &x; }
};

namespace ARMv6{
    QUAT* MTX34ToQUATC_FAST(QUAT* pOut, const MTX34* pMtx);
    QUAT* MTX34ToQUATC(QUAT* pOut, const MTX34* pMtx);
    
    inline QUAT* QUATNormalizeC(QUAT* pOut, const QUAT* __restrict q){
        f32 mag;

        NN_NULL_ASSERT_(q);
        NN_NULL_ASSERT_(pOut);

        mag = (q->x * q->x) + (q->y * q->y) + (q->z * q->z) + (q->w * q->w);

        if (mag >= 0.00001F){
            mag = 1.0F / ::std::sqrtf(mag);
            
            pOut->x = q->x * mag;
            pOut->y = q->y * mag;
            pOut->z = q->z * mag;
            pOut->w = q->w * mag;
        }
        else
        {
            pOut->x = pOut->y = pOut->z = pOut->w = 0.0F;
        }
        
        return pOut;
    }
}

/* Inlines */

#define NN_QUAT_EPSILON        0.00001F

inline QUAT* QUATSlerp(QUAT* pOut, const QUAT* __restrict q1, const QUAT* __restrict q2, f32 t){
    f32 theta, sin_th, cos_th, tp, tq;

    cos_th = q1->x * q2->x + q1->y * q2->y + q1->z * q2->z + q1->w * q2->w;
    tq     = 1.0F;

    if (cos_th < 0.0F){
        cos_th = -cos_th;
        tq     = -tq;
    }

    if (cos_th <= 1.0F - NN_QUAT_EPSILON ){
        theta  = ::std::acosf(cos_th);
        sin_th = ::std::sinf(theta);
        tp     = ::std::sinf((1.0F - t) * theta) / sin_th;
        tq    *= ::std::sinf( t * theta ) / sin_th;
    }
    else{
        tp = 1.0F - t;
        tq = tq * t;
    }

    pOut->x = tp * q1->x + tq * q2->x;
    pOut->y = tp * q1->y + tq * q2->y;
    pOut->z = tp * q1->z + tq * q2->z;
    pOut->w = tp * q1->w + tq * q2->w;

    return pOut;
}

inline QUAT* QUATSlerp(QUAT* pOut, const QUAT& q1, const QUAT& q2, f32 t) { return QUATSlerp( pOut, &q1, &q2, t ); }

inline QUAT* QUATNormalize(QUAT* pOut, const QUAT* q){

}

inline QUAT* MTX34ToQUAT(QUAT* pOut, const MTX34* pMtx){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34ToQUATC(pOut, pMtx);
    #else
        return ARMv6::MTX34ToQUATC_FAST(pOut, pMtx);
    #endif
}

QUAT* MTX34ToQUAT(QUAT* pOut, const MTX34* pMtx);
}
}