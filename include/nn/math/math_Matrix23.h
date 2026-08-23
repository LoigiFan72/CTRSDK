#pragma once

#include <nn/math/math_Matrix22.h>

#pragma push
#pragma Otime

namespace nn{
namespace math{

class VEC2;
class MTX23;

inline MTX23* MTX23Copy(MTX23* pOut, const MTX23* p);
inline MTX23* MTX23Zero(MTX23* pOut);
inline MTX23* MTX23Identity(MTX23* pOut);
inline bool   MTX23IsIdentity(const MTX23* p);
inline MTX23* MTX23Add(MTX23* pOut, const MTX23* p1, const MTX23* p2);
inline MTX23* MTX23Sub(MTX23* pOut, const MTX23* p1, const MTX23* p2);
inline MTX23* MTX23Mult(MTX23* pOut, const MTX23* p, f32 f);
inline MTX23* MTX23Mult(MTX23* pOut, const MTX23* p1, const MTX23* p2);
inline MTX23* MTX22ToMTX23(MTX23* pOut, const MTX22* pM);
inline MTX23* MTX23Scale(MTX23* pOut, const MTX23* pM, const VEC2* pS);
inline MTX23* MTX23Translate(MTX23* pOut, const MTX23* pM, const VEC2* pT);

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
        f32 m[2][3];
        f32 a[6];
        VEC3_ v[2];
    };
};

class MTX23 : public MTX23_{
public:
    typedef MTX23 self_type;
    typedef f32   value_type;

    MTX23() {}
    explicit MTX23(const f32* p) { (void)MTX23Copy(this, reinterpret_cast<const MTX23*>(p)); }
    explicit MTX23(const MTX22& rhs) { MTX22ToMTX23(this, &rhs); }
    MTX23(f32 x00, f32 x01, f32 x02, f32 x10, f32 x11, f32 x12){
        f._00 = x00; f._01 = x01; f._02 = x02;
        f._10 = x10; f._11 = x11; f._12 = x12;
    }

    static const MTX23& Identity(){
        static const MTX23 identity(1.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f);
        
        return identity;
    }

    operator f32*() { return this->a; }
    operator const f32*() const { return this->a; }
};

inline MTX23* MTX23Copy(MTX23* pOut, const MTX23& m) { return MTX23Copy( pOut, &m ); }
inline bool   MTX23IsIdentity(const MTX23& m) { return MTX23IsIdentity( &m ); }
inline MTX23* MTX23Add(MTX23* pOut, const MTX23& m1, const MTX23& m2) { return MTX23Add(pOut, &m1, &m2); }
inline MTX23* MTX23Sub(MTX23* pOut, const MTX23& m1, const MTX23& m2) { return MTX23Sub(pOut, &m1, &m2); }
inline MTX23* MTX23Mult(MTX23* pOut, const MTX23& m, f32 f) { return MTX23Mult(pOut, &m, f); }
inline MTX23* MTX23Mult(MTX23* pOut, const MTX23& m1, const MTX23& m2) { return MTX23Mult(pOut, &m1, &m2); }
inline MTX23* MTX23Scale(MTX23* pOut, const MTX23& m, const VEC2& vS) { return MTX23Scale(pOut, &m, &vS); }

}
}

namespace nn{
namespace math{

inline MTX23* MTX23Copy(MTX23* pOut, const MTX23* p){
    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(p);
    
    if (pOut != p){
        *pOut = *p;
    }
    
    return pOut;
}

inline MTX23* MTX23Zero(MTX23* pOut){
    NN_NULL_ASSERT_(pOut);
    
    pOut->f._00 = pOut->f._01 = pOut->f._02 = 
    pOut->f._10 = pOut->f._11 = pOut->f._12 = 0.f;
    
    return pOut;
}

inline MTX23* MTX23Identity(MTX23* pOut){
    NN_NULL_ASSERT_(pOut);
    
    MTX23Copy(pOut, MTX23::Identity());
    
    return pOut;
}

inline bool MTX23IsIdentity(const MTX23* p){
    NN_NULL_ASSERT_(p);
    
    return p->f._00 == 1.f && p->f._01 == 0.f && p->f._02 == 0.f &&
           p->f._10 == 0.f && p->f._11 == 1.f && p->f._12 == 0.f;
}

inline MTX23* MTX23Add(MTX23* pOut, const MTX23* p1, const MTX23* p2){
    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(p1);
    NN_NULL_ASSERT_(p2);
    
    pOut->f._00 = p1->f._00 + p2->f._00;
    pOut->f._01 = p1->f._01 + p2->f._01;
    pOut->f._02 = p1->f._02 + p2->f._02;

    pOut->f._10 = p1->f._10 + p2->f._10;
    pOut->f._11 = p1->f._11 + p2->f._11;
    pOut->f._12 = p1->f._12 + p2->f._12;
    
    return pOut;
}

inline MTX23* MTX23Sub(MTX23* pOut, const MTX23* p1, const MTX23* p2){
    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(p1);
    NN_NULL_ASSERT_(p2);
    
    pOut->f._00 = p1->f._00 - p2->f._00;
    pOut->f._01 = p1->f._01 - p2->f._01;
    pOut->f._02 = p1->f._02 - p2->f._02;

    pOut->f._10 = p1->f._10 - p2->f._10;
    pOut->f._11 = p1->f._11 - p2->f._11;
    pOut->f._12 = p1->f._12 - p2->f._12;
    
    return pOut;
}

inline MTX23* MTX23Mult(MTX23* pOut, const MTX23* p, f32 f){
    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(p);
    
    pOut->f._00 = p->f._00 * f;
    pOut->f._01 = p->f._01 * f;
    pOut->f._02 = p->f._02 * f;

    pOut->f._10 = p->f._10 * f;
    pOut->f._11 = p->f._11 * f;
    pOut->f._12 = p->f._12 * f;
    
    return pOut;
}

inline MTX23* MTX23Mult(MTX23* pOut, const MTX23* __restrict p1, const MTX23* __restrict p2){
    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(p1);
    NN_NULL_ASSERT_(p2);
    
    MTX23  tmp;
    MTX23* __restrict pMtx;
    
    if ((pOut == p1) || (pOut == p2)){
        pMtx = &tmp;
    }
    else{
        pMtx = pOut;
    }
    
    pMtx->f._00 = p1->f._00 * p2->f._00 + p1->f._01 * p2->f._10;
    pMtx->f._01 = p1->f._00 * p2->f._01 + p1->f._01 * p2->f._11;
    pMtx->f._02 = p1->f._00 * p2->f._02 + p1->f._01 * p2->f._12 + p1->f._02;

    pMtx->f._10 = p1->f._10 * p2->f._00 + p1->f._11 * p2->f._10;
    pMtx->f._11 = p1->f._10 * p2->f._01 + p1->f._11 * p2->f._11;
    pMtx->f._12 = p1->f._10 * p2->f._02 + p1->f._11 * p2->f._12 + p1->f._12;
    
    if(pMtx == &tmp){
        MTX23Copy(pOut, &tmp);
    }
    
    return pOut;
}

inline MTX23* MTX23Scale(MTX23* pOut, const MTX23* __restrict pM, const VEC2* __restrict pS){
    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(pM);
    NN_NULL_ASSERT_(pS);
    
    pOut->f._00 = pM->f._00 * pS->x;
    pOut->f._10 = pM->f._10 * pS->x;

    pOut->f._01 = pM->f._01 * pS->y;
    pOut->f._11 = pM->f._11 * pS->y;
    
    if (pOut != pM){
        pOut->f._02 = pM->f._02;
        pOut->f._12 = pM->f._12;
    }
    return pOut;
}

inline MTX23* MTX23Translate(MTX23* pOut, const MTX23* pM, const VEC2* pT){
    if (pOut != pM){
        (void)MTX23Copy(pOut, pM);
    }

    VEC2 tmp;
    VEC2Transform(&tmp, pM, pT);

    pOut->f._02 = tmp.x;
    pOut->f._12 = tmp.y;
    return pOut;
}

inline MTX23* MTX23RotFIdx(MTX23* pOut, f32 fIdx){
    NN_NULL_ASSERT_(pOut);
    
    f32 sin, cos;
    
    SinCosFIdx(&sin, &cos, fIdx);
    
    pOut->f._00 = pOut->f._11 = cos;
    pOut->f._01 = sin;
    pOut->f._10 = -sin;
    pOut->f._02 = pOut->f._12 = 0.f;
    
    return pOut;
}

inline MTX23* MTX22ToMTX23(MTX23* pOut, const MTX22* pM){
    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(pM);
    
    pOut->f._00 = pM->f._00;
    pOut->f._01 = pM->f._01;
    pOut->f._10 = pM->f._10;
    pOut->f._11 = pM->f._11;

    pOut->f._02 = pOut->f._12 = 0.f;
    return pOut;
}

}
}