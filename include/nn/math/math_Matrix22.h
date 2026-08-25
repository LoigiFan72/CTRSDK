#pragma once

#include <cstring>
#include <nn/math/math_Vec2.h>

#pragma push
#pragma Otime

namespace nn { 
namespace math {

class MTX22;
class MTX23;

inline MTX22* MTX22Identity(MTX22* pOut);
inline bool   MTX22IsIdentity(const MTX22* p);
inline MTX22* MTX22Copy(MTX22* pOut, const MTX22* p);
inline MTX22* MTX22Zero(MTX22* pOut);
inline MTX22* MTX23ToMTX22(MTX22* pOut, const MTX23* pM);

struct MTX22_{
    struct BaseData{
        f32 _00, _01;
        f32 _10, _11;
    };

    union{
        BaseData f;
        f32 m[2][2];
        f32 a[4];
        VEC2_ v[2];
    };
};

struct MTX22 : public MTX22_{
public:
    typedef MTX22 self_type;
    typedef f32   value_type;
    MTX22() {}
    explicit MTX22(const f32* p){ MTX22Copy(this, reinterpret_cast<const MTX22*>(p)); }
    explicit MTX22(const MTX23& rhs){ MTX23ToMTX22(this, &rhs); }
    MTX22(f32 x00, f32 x01, f32 x10, f32 x11){
        f._00 = x00; f._01 = x01;
        f._10 = x10; f._11 = x11;
    }

    operator f32*() { return this->a; }
    operator const f32*() const { return this->a; }
    
    bool IsIdentity() const { return MTX22IsIdentity(this); }

    static const MTX22& Identity(){
        static const MTX22 identity(1.0f, 0.0f,0.0f, 1.0f);
        
        return identity;
    }
};

inline bool   MTX22IsIdentity(const MTX22& m) { return MTX22IsIdentity(&m); }
inline MTX22* MTX22Copy(MTX22* pOut, const MTX22& m) { return MTX22Copy(pOut, &m); }
inline MTX22* MTX23ToMTX22(MTX22* pOut, const MTX23& m) { return MTX23ToMTX22(pOut, &m); }

}
}

namespace nn{
namespace math{

inline bool MTX22IsIdentity(const MTX22* p){
    return p->f._00 == 1.f && p->f._01 == 0.f &&
           p->f._10 == 0.f && p->f._11 == 1.f;
}

inline MTX22* MTX22Copy(MTX22* pOut, const MTX22* p){
    NN_NULL_ASSERT_(pOut);
    NN_NULL_ASSERT_(p);
    
    if (pOut != p){
        *pOut = *p;
    }
    
    return pOut;
}

inline MTX22* MTX22Zero(MTX22* pOut){
    NN_NULL_ASSERT_(pOut);
    
    pOut->f._00 = pOut->f._01 = 
    pOut->f._10 = pOut->f._11 = 0.f;
    return pOut;
}

inline MTX22* MTX22Identity(MTX22* pOut){
    NN_NULL_ASSERT_(pOut);
    
    MTX22Copy(pOut, MTX22::Identity());
    
    return pOut;
}

}
}