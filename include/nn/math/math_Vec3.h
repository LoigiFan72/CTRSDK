#pragma once

#include <nn/types.h>
#include <nn/Assert.h>
#include <cmath>

namespace nn {
namespace math {

class VEC3;

inline VEC3* VEC3Add(VEC3* pOut, const VEC3& v1, const VEC3& v2);
inline VEC3* VEC3Sub(VEC3* pOut, const VEC3& v1, const VEC3& v2);
inline VEC3* VEC3Mult(VEC3* pOut, const VEC3& v1, const VEC3& v2);
inline VEC3* VEC3Scale(VEC3* pOut, const VEC3* p, f32 scale);
inline VEC3* VEC3Normalize(VEC3* pOut, const VEC3* p);
inline f32   VEC3SquareLen(const VEC3* p);
inline bool  VEC3IsZero(const VEC3* p);
inline f32   VEC3Len(const VEC3* p);

struct VEC3_{
    f32 x;
    f32 y;
    f32 z;
};
    
class VEC3 : public VEC3_{
public:
    static const VEC3& Zero(){
        static const VEC3 zero(0.0f, 0.0f, 0.0f);
            
        return zero;
    }
        
    static const VEC3& One(){
        static const VEC3 one(1.0f, 1.0f, 1.0f);
            
        return one;
    }

    typedef VEC3 self_type; //
    typedef f32  value_type; //
public:
    VEC3() {}
    explicit VEC3(const f32* p) { x = p[0]; y = p[1]; z = p[2]; }
    VEC3(const VEC3_& v) { x = v.x; y = v.y; z = v.z; }
    VEC3(f32 fx, f32 fy, f32 fz) { x = fx; y = fy; z = fz; }

    operator f32*() { return &x; }

    operator const f32*() const { return &x; }

    self_type& operator *= (f32 f) { (void)VEC3Scale(this, this, f); return *this; }

    void Set(f32 fx, f32 fy, f32 fz) { x = fx; y = fy; z = fz; }
    void Set(const self_type& value) { x = value.x; y = value.y; z = value.z; }
    
    self_type& Normalize(){
        return *VEC3Normalize(this, this);
    }

    f32 LengthSquare() const { return VEC3SquareLen(this); }
    f32 Length() const { return VEC3Len(this); }

    bool IsZero() const { return VEC3IsZero(this); }

    float x;
    float y;
    float z;
};

/* Inlines */

inline VEC3* VEC3Add(VEC3* pOut, const VEC3* p1, const VEC3* p2){
    pOut->x = p1->x + p2->x;
    pOut->y = p1->y + p2->y;
    pOut->z = p1->z + p2->z;
    return pOut;
}

inline VEC3* VEC3Sub(VEC3* pOut, const VEC3* p1, const VEC3* p2){
    pOut->x = p1->x - p2->x;
    pOut->y = p1->y - p2->y;
    pOut->z = p1->z - p2->z;
    return pOut;
}

inline VEC3* VEC3Mult(VEC3* pOut, const VEC3* p1, const VEC3* p2){
    pOut->x = p1->x * p2->x;
    pOut->y = p1->y * p2->y;
    pOut->z = p1->z * p2->z;
    return pOut;
}

inline VEC3* VEC3Scale(VEC3* pOut, const VEC3* p, f32 scale){
    pOut->x = scale * p->x;
    pOut->y = scale * p->y;
    pOut->z = scale * p->z;
    return pOut;
}

inline VEC3* VEC3Lerp(VEC3* pOut, const VEC3* p1, const VEC3* p2, f32 t){
    pOut->x = p1->x + t * (p2->x - p1->x);
    pOut->y = p1->y + t * (p2->y - p1->y);
    pOut->z = p1->z + t * (p2->z - p1->z);
    return pOut;
}

inline f32 VEC3Dot(const VEC3* p1, const VEC3* p2){
    return p1->x * p2->x + p1->y * p2->y + p1->z * p2->z;
}

inline f32 VEC3Len(const VEC3* p){
    NN_NULL_ASSERT_(p);
    
    return ::std::sqrtf( VEC3SquareLen( p ) );
}

inline f32 VEC3SquareLen(const VEC3* p){
    return p->x * p->x + p->y * p->y + p->z * p->z;
}

inline VEC3* VEC3Cross(VEC3* pOut, const VEC3* p1, const VEC3* p2){

    VEC3 tmpVec;

    tmpVec.x = ( p1->y * p2->z ) - ( p1->z * p2->y );
    tmpVec.y = ( p1->z * p2->x ) - ( p1->x * p2->z );
    tmpVec.z = ( p1->x * p2->y ) - ( p1->y * p2->x );

    pOut->x = tmpVec.x;
    pOut->y = tmpVec.y;
    pOut->z = tmpVec.z;

    return pOut;
}

/* Misc Inlines */

inline bool VEC3IsZero(const VEC3* p){
    return p->x == 0.f && p->y == 0.f && p->z == 0.f;
}

/* Converter */

inline VEC3* VEC3Add(VEC3* pOut, const VEC3& v1, const VEC3& v2) { return VEC3Add( pOut, &v1, &v2 ); }
inline VEC3* VEC3Sub(VEC3* pOut, const VEC3& v1, const VEC3& v2) { return VEC3Sub( pOut, &v1, &v2 ); }
inline VEC3* VEC3Mult(VEC3* pOut, const VEC3& v1, const VEC3& v2) { return VEC3Mult( pOut, &v1, &v2 ); }
inline VEC3* VEC3Scale(VEC3* pOut, const VEC3& v, f32 scale) { return VEC3Scale( pOut, &v, scale ); }
inline VEC3* VEC3Cross(VEC3* pOut, const VEC3& v1, const VEC3& v2) { return VEC3Cross(pOut, &v1, &v2 );}

namespace ARMv6{

inline VEC3* VEC3NormalizeC(VEC3* pOut, const VEC3* p){
    f32 mag = (p->x * p->x) + (p->y * p->y) + (p->z * p->z);

    mag = 1.0f / ::std::sqrtf(mag);

    pOut->x = p->x * mag;
    pOut->y = p->y * mag;
    pOut->z = p->z * mag;
    
    return pOut;
}

inline VEC3* VEC3NormalizeC_FAST(VEC3* pOut, const VEC3* p){

    register f32 x, y, z, mag;
        
    x = p->x;
    y = p->y;
    z = p->z;
        
    mag = (x * x) + (y * y) + (z * z);

    mag = 1.0f / ::std::sqrtf(mag);
        
    x *= mag;
    y *= mag;
    z *= mag;

    pOut->x = x;
    pOut->y = y;
    pOut->z = z;
    
    return pOut;
}

}

inline VEC3* VEC3Normalize(VEC3* pOut, const VEC3& v) { return VEC3Normalize(pOut, &v); }

inline VEC3* VEC3Normalize(VEC3* pOut, const VEC3* p){
    #ifdef NN_BUILD_DEBUG // Unoptimized check.
        return ARMv6::VEC3NormalizeC(pOut, p);
    #else  
        return ARMv6::VEC3NormalizeC_FAST(pOut, p);
    #endif
}


}
}