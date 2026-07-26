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

CameraInfo mBaseCamera;
f32 mLimitParallex;
f32 mLevelWidth;
f32 mDepthLevel;
f32 mDistanceToNearClip;
f32 mDistanceToFarClip;
f32 mCameraInterval;

StereoCamera();
StereoCamera(const WithInitialize&){ this->Initialize(); }
~StereoCamera();
void CalculateMatrices(MTX44 *projL,MTX34 *viewL,MTX44 *projR,MTX34 *viewR, MTX44 *projOriginal,MTX34 *viewOriginal,const f32 depthLevel,const f32 factor, const nn::math::PivotDirection pivot = nn::math::PIVOT_UPSIDE_TO_TOP);
void CalculateMatricesReal(nn::math::MTX44* projL, nn::math::MTX34* viewL,nn::math::MTX44* projR, nn::math::MTX34* viewR, const f32 depthLevel, const f32 factor, const nn::math::PivotDirection pivot = nn::math::PIVOT_UPSIDE_TO_TOP);
f32 GetCoefficientForParallax(void) const;
void SetBaseFrustum(const MTX44 *proj);
void SetBaseCamera(const MTX34 *view);
void Initialize();
void Finalize();

struct cfgdata{
    void* cfgData;
    f32 far;
    f32 near;
    f32 level;
    float limit;
};

};

}
}
}