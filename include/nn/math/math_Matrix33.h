#pragma once

#include <nn/math/math_Vec3.h>

#pragma push
#pragma Otime

namespace nn{
namespace math{
class VEC3;
class MTX33;
class MTX34;

inline MTX33* MTX33Copy(MTX33* pOut, const MTX33* p);
//inline MTX34* MTX33ToMTX34(MTX34* pOut, const MTX33* pM);
//inline MTX33* MTX34ToMTX33(MTX33* pOut, const MTX34* pM);

class MTX33_{
public:
    struct BaseData{
        f32 _00;
        f32 _01;
        f32 _02;
        f32 _10;
        f32 _11;
        f32 _12;
        f32 _20;
        f32 _21;
        f32 _22;
    };

    union{
        BaseData f;
        f32 matrix[3][3];
        f32 a[9];
        VEC3_ v[3];
    };
};

class MTX33 : public MTX33_{
public:
    MTX33() {}
    explicit MTX33(const f32* p) { MTX33Copy(this, reinterpret_cast<const MTX33*>(p)); }
    //explicit MTX33(const MTX34& rhs) { MTX34ToMTX33(this, &rhs); }
    MTX33(f32 x00, f32 x01, f32 x02,f32 x10, f32 x11, f32 x12,f32 x20, f32 x21, f32 x22){
        f._00 = x00; f._01 = x01; f._02 = x02;
        f._10 = x10; f._11 = x11; f._12 = x12;
        f._20 = x20; f._21 = x21; f._22 = x22;
    }
    static const int ROW_COUNT = 3;
    static const int COLUMN_COUNT = 3;
    
    static const MTX33& Identity(){
        static const MTX33 identity(1.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f,0.0f, 0.0f, 1.0f);
        return identity;
    }
};
namespace ARMv6{

MTX33* MTX33CopyC(MTX33* pOut, const MTX33* p);
MTX33* MTX33CopyAsm(MTX33* pOut, const MTX33* p);

template<typename TMatrix>
TMatrix* MTX33MultC(TMatrix* pOut, const TMatrix* p1, const TMatrix* p2);

template<typename TMatrix>
TMatrix* MTX33MultAsm(TMatrix* pOut, const TMatrix* p1, const TMatrix* p2);

VEC3* VEC3TransformAsm(VEC3* pOut, const MTX33* pM, const VEC3* pV);
VEC3* VEC3TransformC(VEC3* pOut, const MTX33* pM, const VEC3* pV);

}

inline VEC3* VEC3Transform(VEC3* pOut, const MTX33* __restrict pM, const VEC3* __restrict pV){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::VEC3TransformC(pOut,pM,pV);
    #else
        return ARMv6::VEC3TransformAsm(pOut, pM, pV);
    #endif
}

inline MTX33* MTX33Copy(MTX33* pOut, const MTX33* p){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX33CopyC(pOut,p);
    #else
        return ARMv6::MTX33CopyAsm(pOut,p);
    #endif
}

template<typename TMatrix>
inline TMatrix* MTX33Mult(TMatrix* pOut, const TMatrix* p1, const TMatrix* p2){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX33MultC(pOut,p1,p2);
    #else
        return ARMv6::MTX33MultAsm(pOut,p1,p2);
    #endif
}

template<typename TMatrix>
inline TMatrix* MTX33Mult(TMatrix* pOut, const TMatrix& m1, const TMatrix& m2) { return MTX33Mult( pOut, &m1, &m2 ); }

}
}