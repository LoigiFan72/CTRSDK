#pragma once

#include <nn/math/math_Vec3.h>

namespace nn{
namespace math{

    enum PivotDirection{
        PIVOT_NONE,
        PIVOT_UPSIDE_TO_TOP,
        PIVOT_UPSIDE_TO_RIGHT,
        PIVOT_UPSIDE_TO_BOTTOM,
        PIVOT_UPSIDE_TO_LEFT,
        PIVOT_NUM
    };
class MTX44;

MTX44* MTX44Copy(MTX44* pOut, const MTX44* m);
inline MTX44* MTX44Copy(MTX44* pOut, const MTX44& m) { return MTX44Copy( pOut, &m ); }

class MTX44{
public:
    union{
        float matrix[4][4];
        f32 a[16];
    };

    operator f32*() { return this->a; }
    operator const f32*() const { return this->a; }

    static const int ROW_COUNT = 4; //
    static const int COLUMN_COUNT = 4; //
    MTX44() {}
    MTX44(f32 x00, f32 x01, f32 x02, f32 x03,f32 x10, f32 x11, f32 x12, f32 x13,f32 x20, f32 x21, f32 x22, f32 x23,f32 x30, f32 x31, f32 x32, f32 x33);
    MTX44(const MTX44& rhs) { (void)MTX44Copy(this, &rhs); }
    static const MTX44& Identity(){
        static const MTX44 identity(1.0f, 0.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f, 0.0f,0.0f, 0.0f, 1.0f, 0.0f,0.0f, 0.0f, 0.0f, 1.0f);
        return identity;
    }
};

inline MTX44* MTX44Identity(MTX44* pOut){
    MTX44Copy(pOut, MTX44::Identity());

    return pOut;
}

namespace ARMv6{
void MTX44MultAsm(nn::math::MTX44 *,nn::math::MTX44 const*,nn::math::MTX44 const*);

void MTX44MultScaleAsm(nn::math::MTX44 *,nn::math::MTX44 const*,nn::math::VEC3 const*);

void MTX44MultTranslateAsm(nn::math::MTX44 *,nn::math::VEC3 const*,nn::math::MTX44 const*);

namespace {
    inline void SwapF(f32 &a, f32 &b){
        f32 tmp;
        tmp = a;
        a = b;
        b = tmp;
    }
}

inline u32 MTX44InverseC(MTX44* pOut, const MTX44* p){
    MTX44 mTmp;
    f32 (*src)[4];
    f32 (*inv)[4];
    f32   w;

    MTX44Copy(&mTmp, p);
    MTX44Identity(pOut);
    
    src = mTmp.matrix;
    inv = pOut->matrix;
    
    for (int i = 0; i < 4; ++i){
        f32 max = 0.0f;
        s32 swp = i;

        for(int k = i ; k < 4 ; k++ ){
            f32 ftmp;
            ftmp = ::std::fabs(src[k][i]);
            if (ftmp > max){
                max = ftmp;
                swp = k;
            }
        }
        
        if (max == 0.0f){
            return 0;
        }

        if (swp != i){
            for (int k = 0; k < 4; k++){
                SwapF(src[i][k], src[swp][k]);
                SwapF(inv[i][k], inv[swp][k]);
            }
        }

        
        w = 1.0f / src[i][i];
        for (int j = 0; j < 4; ++j){
            src[i][j] *= w;
            inv[i][j] *= w;
        }
        
        for (int k = 0; k < 4; ++k )
        {
            if ( k == i )
                continue;
            
            w = src[k][i];
            for (int j = 0; j < 4; ++j)
            {
                src[k][j] -= src[i][j] * w;
                inv[k][j] -= inv[i][j] * w;
            }
        }
    }
    
    return 1;
}
inline u32 MTX44InverseC_FAST(MTX44* pOut, const MTX44* p){
    const f32 (*src)[4];
    f32 (*inv)[4];

    src = p->matrix;
    inv = pOut->matrix;

    f32 a11, a12, a13, a14, a21, a22, a23, a24, a31, a32, a33, a34, a41, a42, a43, a44;
    f32 b11, b12, b13, b14, b21, b22, b23, b24, b31, b32, b33, b34, b41, b42, b43, b44;
    f32 det;
    
    a11 = src[0][0];
    a12 = src[0][1];
    a13 = src[0][2];
    a14 = src[0][3];

    a21 = src[1][0];
    a22 = src[1][1];
    a23 = src[1][2];
    a24 = src[1][3];

    a31 = src[2][0];
    a32 = src[2][1];
    a33 = src[2][2];
    a34 = src[2][3];

    a41 = src[3][0];
    a42 = src[3][1];
    a43 = src[3][2];
    a44 = src[3][3];
    
    det = a11*(a22*a33*a44 + a23*a34*a42 + a24*a32*a43)
        + a12*(a21*a34*a43 + a23*a31*a44 + a24*a33*a41)
        + a13*(a21*a32*a44 + a22*a34*a41 + a24*a31*a42)
        + a14*(a21*a33*a42 + a22*a31*a43 + a23*a32*a41)
        - a11*(a22*a34*a43 + a23*a32*a44 + a24*a33*a42)
        - a12*(a21*a33*a44 + a23*a34*a41 + a24*a31*a43)
        - a13*(a21*a34*a42 + a22*a31*a44 + a24*a32*a41)
        - a14*(a21*a32*a43 + a22*a33*a41 + a23*a31*a42);
        
    if(det==0.0f)
        return 0;

    det = 1.0f / det;

    f32 a33xa44_a34xa43, a32xa44_a34xa42, a33xa42_a32xa43,
        a33xa41_a31xa43, a31xa44_a34xa41, a32xa41_a31xa42;
    
    a33xa44_a34xa43 = a33*a44 - a34*a43;
    a32xa44_a34xa42 = a32*a44 - a34*a42;
    a33xa42_a32xa43 = a33*a42 - a32*a43;
    a33xa41_a31xa43 = a33*a41 - a31*a43;
    a31xa44_a34xa41 = a31*a44 - a34*a41;
    a32xa41_a31xa42 = a32*a41 - a31*a42;
    
    f32 a23xa44_a24xa43, a24xa33_a23xa34, a24xa42_a22xa44, a22xa43_a23xa42,
        a22xa34_a24xa32, a23xa32_a22xa33, a21xa44_a24xa41, a23xa41_a21xa43,
        a24xa31_a21xa34, a21xa33_a23xa31, a21xa42_a22xa41, a22xa31_a21xa32;
    
    a23xa44_a24xa43 = a23*a44 - a24*a43;
    a24xa33_a23xa34 = a24*a33 - a23*a34;
    a24xa42_a22xa44 = a24*a42 - a22*a44;
    a22xa43_a23xa42 = a22*a43 - a23*a42;
    a22xa34_a24xa32 = a22*a34 - a24*a32;
    a23xa32_a22xa33 = a23*a32 - a22*a33;
    a21xa44_a24xa41 = a21*a44 - a24*a41;
    a23xa41_a21xa43 = a23*a41 - a21*a43;
    a24xa31_a21xa34 = a24*a31 - a21*a34;
    a21xa33_a23xa31 = a21*a33 - a23*a31;
    a21xa42_a22xa41 = a21*a42 - a22*a41;
    a22xa31_a21xa32 = a22*a31 - a21*a32;
    
    b11 =( a22*a33xa44_a34xa43) - (a23*a32xa44_a34xa42) - (a24*a33xa42_a32xa43);
    b12 =( a13*a32xa44_a34xa42) + (a14*a33xa42_a32xa43) - (a12*a33xa44_a34xa43);
    b13 =( a12*a23xa44_a24xa43) + (a13*a24xa42_a22xa44) + (a14*a22xa43_a23xa42);
    b14 =( a12*a24xa33_a23xa34) + (a13*a22xa34_a24xa32) + (a14*a23xa32_a22xa33);
    b21 =( a23*a31xa44_a34xa41) + (a24*a33xa41_a31xa43) - (a21*a33xa44_a34xa43);
    b22 =( a11*a33xa44_a34xa43) - (a13*a31xa44_a34xa41) - (a14*a33xa41_a31xa43);
    b23 =( a13*a21xa44_a24xa41) + (a14*a23xa41_a21xa43) - (a11*a23xa44_a24xa43);
    b24 =( a13*a24xa31_a21xa34) + (a14*a21xa33_a23xa31) - (a11*a24xa33_a23xa34);
    b31 =( a21*a32xa44_a34xa42) - (a22*a31xa44_a34xa41) - (a24*a32xa41_a31xa42);
    b32 =( a12*a31xa44_a34xa41) + (a14*a32xa41_a31xa42) - (a11*a32xa44_a34xa42);
    b33 =( a14*a21xa42_a22xa41) - (a11*a24xa42_a22xa44) - (a12*a21xa44_a24xa41);
    b34 =( a14*a22xa31_a21xa32) - (a11*a22xa34_a24xa32) - (a12*a24xa31_a21xa34);
    b41 =( a21*a33xa42_a32xa43) - (a22*a33xa41_a31xa43) + (a23*a32xa41_a31xa42);
    b42 =( a12*a33xa41_a31xa43) - (a13*a32xa41_a31xa42) - (a11*a33xa42_a32xa43);
    b43 =(-a13*a21xa42_a22xa41) - (a11*a22xa43_a23xa42) - (a12*a23xa41_a21xa43);
    b44 =(-a13*a22xa31_a21xa32) - (a11*a23xa32_a22xa33) - (a12*a21xa33_a23xa31);

    b11 = b11 * det;
    b12 = b12 * det;
    b13 = b13 * det;
    b14 = b14 * det;
    b21 = b21 * det;
    b22 = b22 * det;
    b23 = b23 * det;
    b24 = b24 * det;
    b31 = b31 * det;
    b32 = b32 * det;
    b33 = b33 * det;
    b34 = b34 * det;
    b41 = b41 * det;
    b42 = b42 * det;
    b43 = b43 * det;
    b44 = b44 * det;

    inv[0][0] = b11;
    inv[0][1] = b12;
    inv[0][2] = b13;
    inv[0][3] = b14;

    inv[1][0] = b21;
    inv[1][1] = b22;
    inv[1][2] = b23;
    inv[1][3] = b24;

    inv[2][0] = b31;
    inv[2][1] = b32;
    inv[2][2] = b33;
    inv[2][3] = b34;

    inv[3][0] = b41;
    inv[3][1] = b42;
    inv[3][2] = b43;
    inv[3][3] = b44;

    return 1;
}

MTX44* MTX44FrustumC_FAST(MTX44* pOut, f32 l, f32 r, f32 b, f32 t, f32 n, f32 f);
MTX44*  MTX44PivotC_FAST(MTX44* pOut, PivotDirection pivot );

MTX44* MTX44PerspectivePivotRadC(MTX44* pOut, f32 fovy, f32 aspect, f32 n, f32 f, PivotDirection pivot = PIVOT_NONE);
MTX44* MTX44PerspectivePivotRadC_FAST(MTX44* pOut, f32 fovy, f32 aspect, f32 n, f32 f, PivotDirection pivot = PIVOT_NONE);

MTX44* MTX44CopyAsm(nn::math::MTX44 *,nn::math::MTX44 const*);
MTX44* MTX44CopyC(MTX44* pOut, const MTX44* p);

}

inline u32 MTX44Inverse(MTX44* pOut, const MTX44* p){
    #ifdef NN_BUILD_DEBUG
        ARMv6::MTX44InverseC(pOut,p);
    #else
        ARMv6::MTX44InverseC_FAST(pOut,p);
    #endif
}

inline MTX44* MTX44Copy(MTX44* pOut, const MTX44* p){
    #ifdef NN_BUILD_DEBUG
        ARMv6::MTX44CopyC(pOut,p);
    #else
        ARMv6::MTX44CopyAsm(pOut,p);
    #endif
}

inline MTX44* MTX44FrustumPivot(MTX44* pOut, f32 l, f32 r, f32 b, f32 t, f32 n, f32 f, PivotDirection pivot = PIVOT_NONE){
    #ifdef NN_BUILD_DEBUG

    #else
        ARMv6::MTX44FrustumC_FAST(pOut, l, r, b, t, n, f);
        ARMv6::MTX44PivotC_FAST(pOut, pivot);
    #endif
}

inline MTX44* MTX44PerspectivePivotRad(MTX44* pOut, f32 fovy, f32 aspect, f32 n, f32 f, PivotDirection pivot = PIVOT_NONE){
    #ifdef NN_BUILD_DEBUG

    #else
        ARMv6::MTX44PerspectivePivotRadC_FAST(pOut, fovy, aspect, n, f);
        ARMv6::MTX44PivotC_FAST(pOut, pivot);
    #endif
}
}
}