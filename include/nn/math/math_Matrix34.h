#pragma once

#include <nn/types.h>
#include <nn/math/math_Triangular.h>
#include <nn/math/math_Quaternion.h>
#include <nn/math/math_Matrix33.h>
#include <nn/math/math_Vec3.h>
#include <nn/math/math_Vec4.h>

#pragma push

namespace nn{
namespace math{
    
class QUAT;
class MTX34;

MTX34* MTX34Copy(MTX34* pOut, const MTX34* p);
MTX34* MTX34Scale(MTX34* pOut, const VEC3* pS);
bool   MTX34IsIdentity(const MTX34* p);

class MTX34_{
public:
    struct BaseData{
        f32 _00;
        f32 _01;
        f32 _02;
        f32 _03;
        f32 _10;
        f32 _11;
        f32 _12;
        f32 _13;
        f32 _20;
        f32 _21;
        f32 _22;
        f32 _23;
    };
    union{
        BaseData f;
        float matrix[3][4];
        f32 a[12];
        VEC4_ v[3];
    };
};

class MTX34 : public MTX34_{
public:
    typedef MTX34 self_type;
    typedef f32   value_type;
public:
    static const int ROW_COUNT = 3;
    static const int COLUMN_COUNT = 4;

    operator f32*() { return this->a; }
    operator const f32*() const { return this->a; }
    
    /* Constructors */

    MTX34() {}
    explicit MTX34(const f32* p)   { MTX34Copy(this, (MTX34*)p); }
    MTX34(const MTX34& rhs)        { MTX34Copy(this, &rhs); }
    //explicit MTX34(const MTX33& rhs) { MTX33ToMTX34(this, &rhs); }
    MTX34(f32 x00, f32 x01, f32 x02, f32 x03,f32 x10, f32 x11, f32 x12, f32 x13,f32 x20, f32 x21, f32 x22, f32 x23){
        f._00 = x00; f._01 = x01; f._02 = x02; f._03 = x03;
        f._10 = x10; f._11 = x11; f._12 = x12; f._13 = x13;
        f._20 = x20; f._21 = x21; f._22 = x22; f._23 = x23;
    }

    VEC3 GetColumn(int index) const{
        VEC3 column;
        column.x = this->matrix[0][index];
        column.y = this->matrix[1][index];
        column.z = this->matrix[2][index];
        return column;
    }

    self_type& SetupScale(const VEC3& scale) { return *MTX34Scale(this, &scale); }

    void SetColumn(int index, const VEC3& column){
        matrix[0][index] = column.x;
        matrix[1][index] = column.y;
        matrix[2][index] = column.z;
    }

    /* Inlines */

    static const MTX34& Identity(){
        static const MTX34 identity(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,0.0f, 0.0f, 1.0f, 0.0f);
        return identity;
    }
    bool IsIdentity() const { return MTX34IsIdentity(this); }
};

inline bool MTX34IsIdentity(const MTX34& m) { return MTX34IsIdentity( &m ); }

inline bool MTX34IsIdentity(const MTX34* p) { 
    return p->f._00 == 1.f && p->f._01 == 0.f && p->f._02 == 0.f && p->f._03 == 0.f &&
           p->f._10 == 0.f && p->f._11 == 1.f && p->f._12 == 0.f && p->f._13 == 0.f &&
           p->f._20 == 0.f && p->f._21 == 0.f && p->f._22 == 1.f && p->f._23 == 0.f;
}

inline MTX34* MTX34Identity(MTX34* pOut) {
    NN_NULL_ASSERT_(pOut);

    MTX34Copy(pOut, &MTX34::Identity());

    return pOut;
}

inline MTX34* MTX34Copy(MTX34* pOut, const MTX34& m) { return MTX34Copy(pOut, &m); }

} // math
} // nn

namespace nn{
namespace math{
namespace ARMv6{

VEC3* VEC3TransformAsm(VEC3* pOut, const MTX34* __restrict pM, const VEC3* __restrict pV);
inline VEC3* VEC3TransformC(VEC3* pOut, const MTX34* __restrict pM, const VEC3* __restrict pV){
    VEC3 vTmp;
    VEC3* pDst = (pOut == pV) ? &vTmp : pOut;
    
    pDst->x = pM->f._00 * pV->x + pM->f._01 * pV->y + pM->f._02 * pV->z + pM->f._03;
    pDst->y = pM->f._10 * pV->x + pM->f._11 * pV->y + pM->f._12 * pV->z + pM->f._13;
    pDst->z = pM->f._20 * pV->x + pM->f._21 * pV->y + pM->f._22 * pV->z + pM->f._23;
    
    if (pDst == &vTmp){
        pOut->x = pDst->x;
        pOut->y = pDst->y;
        pOut->z = pDst->z;
    }
    return pOut;
}

/* MTX C / ASM */

MTX34* MTX34CopyAsm(nn::math::MTX34 *,nn::math::MTX34 const*);
MTX34* MTX34CopyC(nn::math::MTX34 *,nn::math::MTX34 const*);

MTX34* MTX34MultAsm(nn::math::MTX34 *,nn::math::MTX34 const*,math::MTX34 const*);
MTX34* MTX34MultC(nn::math::MTX34 *,nn::math::MTX34 const*,math::MTX34 const*);

MTX34* MTX34MultAsm(nn::math::MTX34 *,nn::math::MTX34 const*,float);

MTX34* MTX34AddAsm(nn::math::MTX34 *,nn::math::MTX34 const*,nn::math::MTX34 const*);

MTX34* MTX34MultScaleAsm(MTX34* pOut, const MTX34* pM, const VEC3* pS);
MTX34* MTX34MultScaleC(MTX34* pOut, const MTX34* pM, const VEC3* pS);

u32 MTX34InverseAsm(nn::math::MTX34 *,nn::math::MTX34 const*);
u32 MTX34InverseC(MTX34* pOut, const MTX34* p);

u32 MTX34InvTransposeC(nn::math::MTX34 *,nn::math::MTX34 const*);
u32 MTX34InvTransposeAsm(nn::math::MTX34 *,nn::math::MTX34 const*);

MTX34* MTX34MultTranslateC(nn::math::MTX34 *,nn::math::VEC3 const*,nn::math::MTX34 const*);
MTX34* MTX34MultTranslateAsm(nn::math::MTX34 *,nn::math::VEC3 const*,nn::math::MTX34 const*);

MTX34* MTX34MultTranslateC(nn::math::MTX34 *,nn::math::MTX34 const*,nn::math::VEC3 const*);
MTX34* MTX34MultTranslateAsm(nn::math::MTX34 *,nn::math::MTX34 const*,nn::math::VEC3 const*);

MTX33* MTX34ToMTX33Asm(MTX33* pOut, const MTX34* pM);
inline  MTX33* MTX34ToMTX33C(MTX33* pOut, const MTX34* pM){
    pOut->matrix[0][0] = pM->matrix[0][0]; pOut->matrix[0][1] = pM->matrix[0][1]; pOut->matrix[0][2] = pM->matrix[0][2];
    pOut->matrix[1][0] = pM->matrix[1][0]; pOut->matrix[1][1] = pM->matrix[1][1]; pOut->matrix[1][2] = pM->matrix[1][2];
    pOut->matrix[2][0] = pM->matrix[2][0]; pOut->matrix[2][1] = pM->matrix[2][1]; pOut->matrix[2][2] = pM->matrix[2][2];

    return pOut;
}

MTX34* MTX34TransposeAsm(nn::math::MTX34 *,nn::math::MTX34 const*);
inline MTX34* MTX34TransposeC(nn::math::MTX34 * pOut,nn::math::MTX34 const* p){
    MTX34 mTmp;

    NN_NULL_ASSERT_(p);
    NN_NULL_ASSERT_(pOut);

    const f32 (*const src)[4] = p->matrix;
    f32 (*m)[4];

    if (p == pOut){
        m = mTmp.matrix;
    }
    else{
        m = pOut->matrix;
    }

    m[0][0] = src[0][0];   m[0][1] = src[1][0];      m[0][2] = src[2][0];     m[0][3] = 0.0f;
    m[1][0] = src[0][1];   m[1][1] = src[1][1];      m[1][2] = src[2][1];     m[1][3] = 0.0f;
    m[2][0] = src[0][2];   m[2][1] = src[1][2];      m[2][2] = src[2][2];     m[2][3] = 0.0f;

    if (m == mTmp.matrix){
        MTX34Copy(pOut, &mTmp);
    }
    
    return pOut;
}

MTX34* MTX34LookAtC_FAST(MTX34* pOut, const VEC3* pCamPos, const VEC3* pCamUp, const VEC3* pTarget);
inline MTX34* MTX34LookAtC(MTX34* pOut, const VEC3* pCamPos, const VEC3* pCamUp, const VEC3* pTarget){
    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(pCamPos);
    NN_NULL_ASSERT_(pCamUp);
    NN_NULL_ASSERT_(pTarget);

    f32 (*const m)[4] = pOut->matrix;

    VEC3 vLook;
    VEC3Sub(&vLook, pCamPos, pTarget);
    VEC3Normalize(&vLook, &vLook);

    VEC3 vRight;
    VEC3Cross(&vRight, pCamUp, &vLook);
    VEC3Normalize(&vRight, &vRight);

    VEC3 vUp;
    VEC3Cross(&vUp, &vLook, &vRight);

    m[0][0] = vRight.x;
    m[0][1] = vRight.y;
    m[0][2] = vRight.z;
    m[0][3] = -(pCamPos->x * vRight.x + pCamPos->y * vRight.y + pCamPos->z * vRight.z);

    m[1][0] = vUp.x;
    m[1][1] = vUp.y;
    m[1][2] = vUp.z;
    m[1][3] = -(pCamPos->x * vUp.x + pCamPos->y * vUp.y + pCamPos->z * vUp.z);

    m[2][0] = vLook.x;
    m[2][1] = vLook.y;
    m[2][2] = vLook.z;
    m[2][3] = -(pCamPos->x * vLook.x + pCamPos->y * vLook.y + pCamPos->z * vLook.z);
    
    return pOut;
}

MTX34* MTX34RotXYZFIdxC_FAST(MTX34* pOut, f32 fIdxX, f32 fIdxY, f32 fIdxZ, bool isChangeTrans = true); // math_Matrix34.ipp
inline MTX34* MTX34RotXYZFIdxC(MTX34* pOut, f32 fIdxX, f32 fIdxY, f32 fIdxZ, bool isChangeTrans = true){
    f32 sinx, cosx;
    f32 siny, cosy;
    f32 sinz, cosz;
    f32 f1, f2;

    SinCosFIdx(&sinx, &cosx, fIdxX);
    SinCosFIdx(&siny, &cosy, fIdxY);
    SinCosFIdx(&sinz, &cosz, fIdxZ);

    pOut->f._20 = -siny;
    pOut->f._00 = cosz * cosy;
    pOut->f._10 = sinz * cosy;
    pOut->f._21 = cosy * sinx;
    pOut->f._22 = cosy * cosx;

    f1 = cosx * sinz;
    f2 = sinx * cosz;

    pOut->f._01 = f2 * siny - f1;
    pOut->f._12 = f1 * siny - f2;

    f1 = sinx * sinz;
    f2 = cosx * cosz;
    pOut->f._02 = f2 * siny + f1;
    pOut->f._11 = f1 * siny + f2;

    pOut->f._03 = 0.f;
    pOut->f._13 = 0.f;
    pOut->f._23 = 0.f;

    return pOut;
}

MTX34* MTX34ScaleAsm(MTX34* pOut, const VEC3* pS);
inline MTX34* MTX34ScaleC(MTX34* pOut, const VEC3* pS){
    f32 (*const m)[4] = pOut->matrix;

    m[0][0] = pS->x;    m[0][1] = 0.0f;  m[0][2] = 0.0f;  m[0][3] = 0.0f;
    m[1][0] = 0.0f;     m[1][1] = pS->y; m[1][2] = 0.0f;  m[1][3] = 0.0f;
    m[2][0] = 0.0f;     m[2][1] = 0.0f;  m[2][2] = pS->z; m[2][3] = 0.0f;

    return pOut;
}

inline MTX34* MTX34ScaleC_FAST(MTX34* pOut, const VEC3* pS){
    f32 f0 = 0.0f;
    const unsigned int f32_0 = *(reinterpret_cast<unsigned int*>(&f0));
    unsigned int *m = reinterpret_cast<unsigned int *>(pOut->matrix);
    const unsigned int *p = reinterpret_cast<const unsigned int*>(pS);

    m[0] =  p[0];  m[1] = f32_0;  m[ 2] = f32_0;  m[3] = f32_0;
    m[4] = f32_0;  m[5] =  p[1];  m[ 6] = f32_0;  m[7] = f32_0;
    m[8] = f32_0;  m[9] = f32_0;  m[10] =  p[2];  m[11] = f32_0;

    return pOut;
}

MTX34* QUATToMTX34C_FAST(MTX34* pOut, const QUAT* pQ, bool isChangeTrans = true);
inline MTX34* QUATToMTX34C(MTX34* pOut, const QUAT* pQ, bool isChangeTrans = true){
    f32 s, xs, ys, zs;
    f32 wx, wy, wz, xx, xy, xz, yy, yz, zz;

    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(pQ);
    NN_ASSERT_(pQ->x || pQ->y || pQ->z || pQ->w);

    f32 (*const m)[4] = pOut->matrix;

    s = 2.0f / ( (pQ->x * pQ->x) + (pQ->y * pQ->y) + (pQ->z * pQ->z) + (pQ->w * pQ->w) );

    xs = pQ->x *  s;     ys = pQ->y *  s;  zs = pQ->z *  s;
    wx = pQ->w * xs;     wy = pQ->w * ys;  wz = pQ->w * zs;
    xx = pQ->x * xs;     xy = pQ->x * ys;  xz = pQ->x * zs;
    yy = pQ->y * ys;     yz = pQ->y * zs;  zz = pQ->z * zs;

    m[0][0] = 1.0f - (yy + zz);
    m[0][1] = xy   - wz;
    m[0][2] = xz   + wy;
    m[0][3] = 0.0f;

    m[1][0] = xy   + wz;
    m[1][1] = 1.0f - (xx + zz);
    m[1][2] = yz   - wx;
    m[1][3] = 0.0f;

    m[2][0] = xz   - wy;
    m[2][1] = yz   + wx;
    m[2][2] = 1.0f - (xx + yy);
    m[2][3] = 0.0f;
    
    return pOut;
}

}

inline VEC3* VEC3Transform(VEC3* pOut, const MTX34* __restrict pM, const VEC3* __restrict pV){
    #ifdef NN_BUILD_DEBUG // Unoptimized check.
        return ARMv6::VEC3TransformC(pOut,pM,pV);
    #else
        return ARMv6::VEC3TransformAsm(pOut, pM, pV);
    #endif
}

inline MTX34* MTX34Copy(MTX34* pOut, const MTX34* pM){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34CopyC(pOut, pM);
    #else
        return ARMv6::MTX34CopyAsm(pOut, pM);
    #endif
}

inline MTX34* MTX34Mult(MTX34* pOut, const MTX34* p1, const MTX34* p2){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34MultC(pOut,p1,p2);
    #else
        return ARMv6::MTX34MultAsm(pOut,p1,p2);
    #endif
}

inline MTX34* MTX34Mult(MTX34* pOut, const MTX34 p1, const MTX34 p2){ return MTX34Mult(pOut, &p1, &p2);}

inline u32 MTX34Inverse(MTX34* pOut, const MTX34* p){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34InverseC(pOut,p);
    #else
        return ARMv6::MTX34InverseAsm(pOut,p);
    #endif
}

inline u32 MTX34InvTranspose(nn::math::MTX34* pOut,nn::math::MTX34 const* pM){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34InvTransposeC(pOut,pM);
    #else
        return ARMv6::MTX34InvTransposeAsm(pOut,pM);
    #endif
}

inline u32 MTX34Inverse(MTX34* pOut, const MTX34& m) { return MTX34Inverse(pOut, &m); }

inline MTX34* MTX34MultScale(MTX34* pOut, const MTX34* pM, const VEC3* pS){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34MultScaleC(pOut, pM, pS);
    #else
        return ARMv6::MTX34MultScaleAsm(pOut, pM, pS);
    #endif
}

inline MTX34* MTX34MultScale(MTX34* pOut, const MTX34& m, const VEC3& vS) { return MTX34MultScale(pOut, &m, &vS); }

inline MTX34* MTX34MultTranslate(MTX34* pOut, const VEC3* pT, const MTX34* pM){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34MultTranslateC(pOut, pT, pM);
    #else
        return ARMv6::MTX34MultTranslateAsm(pOut, pT, pM);
    #endif
}

inline MTX34* MTX34MultTranslate(MTX34* pOut, const MTX34* pM, const VEC3* pT){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34MultTranslateC(pOut, pM, pT);
    #else
        return ARMv6::MTX34MultTranslateAsm(pOut, pM, pT);
    #endif
}

inline MTX33* MTX34ToMTX33(MTX33* pOut, const MTX34* pM){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34ToMTX33C(pOut, pM);
    #else
        return ARMv6::MTX34ToMTX33Asm(pOut, pM);
    #endif
}

inline MTX34* MTX34Transpose(nn::math::MTX34* pOut,nn::math::MTX34 const* pM){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34TransposeC(pOut,pM);
    #else
        return ARMv6::MTX34TransposeAsm(pOut,pM);
    #endif
}

inline MTX34* MTX34LookAt(MTX34* pOut, const VEC3* pCamPos, const VEC3* pCamUp, const VEC3* pTarget){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34LookAtC(pOut, pCamPos, pCamUp, pTarget);
    #else
        return ARMv6::MTX34LookAtC_FAST(pOut, pCamPos, pCamUp, pTarget);
    #endif
}

inline MTX34* MTX34RotXYZRad(MTX34* pOut, f32 fRadX, f32 fRadY, f32 fRadZ){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34RotXYZFIdxC(pOut, fRadX, fRadY, fRadZ);
    #else
        return ARMv6::MTX34RotXYZFIdxC_FAST(pOut, fRadX, fRadY, fRadZ);
    #endif
}

inline MTX34* MTX34Scale(MTX34* pOut, const VEC3* pS){
    #if defined(NN_BUILD_DEBUG)
        return ARMv6::MTX34ScaleC(pOut,pS);
    #elif defined(NN_BUILD_DEVELOPMENT)
        return ARMv6::MTX34ScaleC_FAST(pOut,pS);
    #else
        return ARMv6::MTX34ScaleAsm(pOut,pS);
    #endif
}

inline MTX34* QUATToMTX34(MTX34* pOut, const QUAT* pQ){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::QUATToMTX34C(pOut,pQ);
    #else
        return ARMv6::QUATToMTX34C_FAST(pOut,pQ);
    #endif
}

}
}

#pragma pop