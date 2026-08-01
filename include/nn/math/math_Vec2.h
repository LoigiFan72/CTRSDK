#pragma once

#include "nn/types.h"

namespace nn {
namespace math {

class VEC2_{
public:
    float x;
    float y;
};
    
class VEC2 : public VEC2_{
    typedef VEC2 self_type;
    typedef f32  value_type;
public:
    VEC2(){ }
    explicit VEC2(const f32* p) { x = p[0]; y = p[1]; }
    VEC2(const VEC2_& v) { x = v.x; y = v.y; }
    VEC2(f32 fx, f32 fy) { x = fx; y = fy; }

    operator f32*() { return &x; }
    operator const f32*() const { return &x; }
    self_type& operator += (const self_type& rhs) { x += rhs.x; y += rhs.y; return *this; }
    self_type& operator -= (const self_type& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    self_type& operator *= (f32 f) { x *= f; y *= f; return *this; }
    self_type& operator *= (const self_type& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
    self_type& operator /= (f32 f) { f32 r = 1.f / f; x *= r; y *= r; return *this; }
    self_type operator + () const { return *this; }
    self_type operator - () const { return self_type(-x, -y); }
    self_type operator + (const self_type& rhs) const { return self_type(x + rhs.x, y + rhs.y); }
    self_type operator - (const self_type& rhs) const { return self_type(x - rhs.x, y - rhs.y); }
    self_type operator * (f32 f) const { return self_type(f * x, f * y); }
    self_type operator / (f32 f) const { f32 r = 1.f / f; return self_type(r * x, r * y); }

    void Set(f32 fx, f32 fy) { x = fx; y = fy; }
};

static const VEC2& Zero(){

    static const VEC2 zero(0.0f, 0.0f);
    return zero;
}

    //
    static const VEC2& One(){

    static const VEC2 one(1.0f, 1.0f);
    return one;
}

}
}