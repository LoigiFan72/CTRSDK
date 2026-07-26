#pragma once

#include <nn/types.h>
#include <nn/math/math_Matrix33.h>
#include <nn/math/math_Vec3.h>

#pragma push

namespace nn{
namespace math{
    
class QUAT;
class MTX34;

MTX34* MTX34Copy(MTX34* pOut, const MTX34* p);
MTX34* MTX34Scale(MTX34* pOut, const VEC3* pS);
bool   MTX34IsIdentity(const MTX34* p);

class MTX34{
public:
    union{
        float matrix[3][4];
        f32 a[12];
    };

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
        matrix[0][0] = x00; matrix[0][1] = x01; matrix[0][2] = x02; matrix[0][3] = x03;
        matrix[1][0] = x10; matrix[1][1] = x11; matrix[1][2] = x12; matrix[1][3] = x13;
        matrix[2][0] = x20; matrix[2][1] = x21; matrix[2][2] = x22; matrix[2][3] = x23;
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
        this->matrix[0][index] = column.x;
        this->matrix[1][index] = column.y;
        this->matrix[2][index] = column.z;
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
    return p->matrix[0][0] == 1.f && p->matrix[0][1] == 0.f && p->matrix[0][2] == 0.f && p->matrix[0][3] == 0.f &&
           p->matrix[1][0] == 0.f && p->matrix[1][1] == 1.f && p->matrix[1][2] == 0.f && p->matrix[1][3] == 0.f &&
           p->matrix[2][0] == 0.f && p->matrix[2][1] == 0.f && p->matrix[2][2] == 1.f && p->matrix[2][3] == 0.f;
}

inline MTX34* MTX34Copy(MTX34* pOut, const MTX34& m) { return MTX34Copy( pOut, &m ); }

//
/* ARMv6*/
//

namespace ARMv6{

/* VEC3 */

VEC3* VEC3TransformAsm(VEC3* pOut, const MTX34* __restrict pM, const VEC3* __restrict pV);
inline VEC3* VEC3TransformC(VEC3* pOut, const MTX34* __restrict pM, const VEC3* __restrict pV){
    VEC3 vTmp;
    VEC3* pDst = (pOut == pV) ? &vTmp : pOut;
    
    pDst->x = pM->matrix[0][0] * pV->x + pM->matrix[0][1] * pV->y + pM->matrix[0][2] * pV->z + pM->matrix[0][3];
    pDst->y = pM->matrix[1][0] * pV->x + pM->matrix[1][1] * pV->y + pM->matrix[1][2] * pV->z + pM->matrix[1][3];
    pDst->z = pM->matrix[2][0] * pV->x + pM->matrix[2][1] * pV->y + pM->matrix[2][2] * pV->z + pM->matrix[2][3];
    
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
MTX34* MTX34TransposeC(nn::math::MTX34 *,nn::math::MTX34 const*);

MTX34* MTX34LookAtC_FAST(MTX34* pOut, const VEC3* pCamPos, const VEC3* pCamUp, const VEC3* pTarget);
MTX34* MTX34LookAtC(MTX34* pOut, const VEC3* pCamPos, const VEC3* pCamUp, const VEC3* pTarget);

MTX34* MTX34RotXYZFIdxC(MTX34* pOut, f32 fIdxX, f32 fIdxY, f32 fIdxZ, bool isChangeTrans = true );
MTX34* MTX34RotXYZFIdxC_FAST(MTX34* pOut, f32 fIdxX, f32 fIdxY, f32 fIdxZ, bool isChangeTrans = true);

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

    m[0] =  p[0];  m[ 1] = f32_0;  m[ 2] = f32_0;  m[ 3] = f32_0;
    m[4] = f32_0;  m[ 5] =  p[1];  m[ 6] = f32_0;  m[ 7] = f32_0;
    m[8] = f32_0;  m[ 9] = f32_0;  m[10] =  p[2];  m[11] = f32_0;

    return pOut;
}

MTX34* QUATToMTX34C_FAST(MTX34* pOut, const QUAT* pQ, bool isChangeTrans = true);
MTX34* QUATToMTX34C(MTX34* pOut, const QUAT* pQ, bool isChangeTrans = true);

}
/* 
inline t name(){
    #ifdef NN_DEBUG
        return C
    #else
        return _FAST
    #endif
}
*/

inline VEC3* VEC3Transform(VEC3* pOut, const MTX34* __restrict pM, const VEC3* __restrict pV){
    #ifdef NN_BUILD_DEBUG // Unoptimized check.
        return ARMv6::VEC3TransformC(pOut,pM,pV);
    #else
        return ARMv6::VEC3TransformAsm(pOut, pM, pV);
    #endif
}

inline MTX34* MTX34Mult(MTX34* pOut, const MTX34* p1, const MTX34* p2){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34MultC(pOut,p1,p2);
    #else
        return ARMv6::MTX34MultAsm(pOut,p1,p2);
    #endif
}

inline MTX34* MTX34Mult(MTX34* pOut, const MTX34 p1, const MTX34 p2){
    return MTX34Mult(pOut, &p1, &p2);
}

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

} // math
} // nn

#pragma pop