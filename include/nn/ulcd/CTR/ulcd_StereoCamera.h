#pragma once

#include <nn/WithInitialize.h>
#include <nn/math/math_Matrix44.h>
#include <nn/math/math_Matrix34.h>

using namespace nn::math;

namespace nn{
namespace ulcd{
namespace CTR{
namespace{
struct Direction{
    VEC3 right;
    VEC3 up;
    VEC3 target;
};

}

class StereoCamera{
public:
    StereoCamera();
    StereoCamera(const WithInitialize&){ this->Initialize(); }
    ~StereoCamera();
    void Initialize();
    void Finalize();
    void CalculateMatrices(MTX44 *projL,MTX34 *viewL,MTX44 *projR,MTX34 *viewR, MTX44 *projOriginal,MTX34 *viewOriginal,const f32 depthLevel,const f32 factor, const nn::math::PivotDirection pivot = nn::math::PIVOT_UPSIDE_TO_TOP);
    void CalculateMatricesReal(nn::math::MTX44* projL, nn::math::MTX34* viewL,nn::math::MTX44* projR, nn::math::MTX34* viewR, const f32 depthLevel, const f32 factor, const nn::math::PivotDirection pivot = nn::math::PIVOT_UPSIDE_TO_TOP);
    void SetBaseFrustum(const MTX44 *proj);
    void SetBaseCamera(const MTX34 *view);
    f32 GetCoefficientForParallax() const;
    f32 GetMaxParallax() const;
    f32 GetLimitParallax() const { return m_LimitParallax; }
    f32 GetDistanceToLevel() const { return m_DepthLevel; }
    f32 GetDistanceToNearClip() const { return m_DistanceToNearClip; }
    f32 GetDistanceToFarClip() const { return m_DistanceToFarClip; }
private:
    struct CameraInfo{
        f32 left;
        f32 right;
        f32 bottom;
        f32 top;
        f32 near;
        f32 far;

        VEC3 position;
        VEC3 posRight;
        VEC3 posUp;
        VEC3 posTarget;
    };

    CameraInfo m_BaseCamera;
    f32 m_LimitParallax;
    f32 m_LevelWidth;
    f32 m_DepthLevel;
    f32 m_DistanceToNearClip;
    f32 m_DistanceToFarClip;
    f32 m_CameraInterval;
};

}
}
}