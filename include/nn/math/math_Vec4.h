#pragma once

#include <nn/math/math_Vec3.h>

namespace nn {
namespace math {

class VEC4_{
public:
    float x;
    float y;
    float z;
    float w;
};
    
class VEC4 : public VEC4_{
    typedef VEC4 self_type;
    typedef f32  value_type;
public:
    VEC4() {}
    explicit VEC4(const f32* p) { x = p[0]; y = p[1]; z = p[2]; w = p[3]; }
    VEC4(const VEC4_& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
    VEC4(f32 fx, f32 fy, f32 fz, f32 fw) { x = fx; y = fy; z = fz; w = fw; }
    explicit VEC4(const VEC3& v) { x = v.x; y = v.y; z = v.z; w = 0.0f; }

    operator f32*() { return &x; }

    operator const f32*() const { return &x; }
    self_type operator - () const { return self_type(-x, -y, -z, -w); }

    static const VEC4& Zero(){
        static const VEC4 zero(0.0f, 0.0f, 0.0f, 0.0f);

        return zero;
    }

    static const VEC4& One(){
        static const VEC4 one(1.0f, 1.0f, 1.0f, 1.0f);

        return one;
    }

    void Set(f32 fx, f32 fy, f32 fz, f32 fw) { x = fx; y = fy; z = fz; w = fw; }
};

}
}