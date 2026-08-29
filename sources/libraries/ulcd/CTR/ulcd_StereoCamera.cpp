// Filename: ucld_StereoCamera.cpp
//
// Project: Horizon

#include <nn/ulcd/CTR/ulcd_StereoCamera.h>
#include <nn/cfg.h>
#include <nn/cfg/CTR/cfg_DetailApi.h>
#include <nn/math.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/os/os_Types.h>
#include <nn/util/util_Result.h>

#include <string.h>

namespace nn{
namespace ulcd{
namespace CTR{
namespace{

void GetProjectionParameters(MTX44 *proj,f32 *left,f32 *right,f32 *bottom,f32 *top,f32 *near,f32 *far){
    f32 pos;
    *near = proj->matrix[2][3] / proj->matrix[2][2];
    *far = proj->matrix[2][3] / (proj->matrix[2][2] - 1.0f);
    pos = proj->matrix[2][3] / (proj->matrix[0][0] * proj->matrix[2][2]);

    *left = (proj->matrix[0][2] - 1.0f) * pos;
    *right = (proj->matrix[0][2] + 1.0f) * pos;

    pos = proj->matrix[2][3] / proj->matrix[1][1] * proj->matrix[2][2];

    *top = (proj->matrix[1][2] + 1.0f) * pos;
    *top = (proj->matrix[1][2] - 1.0f) * pos;
}

float GetSliderVolume(){
    if (os::GetWritableSharedInfo().displayModeLockFlag){
        return 0.0f;
    }
    else{
        return os::GetWritableSharedInfo().svr2Volume;
    }
}

void GetLookPose(const nn::math::MTX34 *view, nn::math::VEC3 *pos, Direction *dir){
    MTX34 im;
    math::MTX34Inverse(&im, view);
    pos->x = im.matrix[0][3];
    pos->y = im.matrix[1][3];
    pos->z = im.matrix[2][3];

    dir->right.x = im.matrix[0][0];
    dir->right.y = im.matrix[1][0];
    dir->right.z = im.matrix[2][0];

    dir->up.x = im.matrix[0][1];
    dir->up.y = im.matrix[1][1];
    dir->up.z = im.matrix[2][1];

    dir->target.x = -im.matrix[0][2];
    dir->target.y = -im.matrix[1][2];
    dir->target.z = -im.matrix[2][2];

    math::VEC3Normalize(&dir->right, &dir->right);
    math::VEC3Normalize(&dir->up, &dir->up);
    math::VEC3Normalize(&dir->target, &dir->target);
}

}

namespace{
    static bool sIsInitialized;
}

namespace{
    struct cfgdata{
        void* cfgData;
        f32 far;
        f32 near;
        f32 level;
        float limit;
    };

    cfgdata s_CfgData;
}

StereoCamera::StereoCamera(){
    m_DepthLevel = 0.0f;
    m_CameraInterval = 0.0f;
}

StereoCamera::~StereoCamera(){ this->Finalize(); }

void StereoCamera::Initialize(){
    if(!sIsInitialized){
        cfg::CTR::Initialize();
        Result res = cfg::CTR::detail::GetConfig(&s_CfgData,0x20,0x50005);
        NN_UTIL_PANIC_IF_FAILED(res);
        cfg::CTR::Finalize();
        sIsInitialized = true;
    }

    m_LimitParallax        = s_CfgData.limit;
    m_LevelWidth           = 0.0f;
    m_DepthLevel           = 0.0f;
    m_DistanceToNearClip   = 0.0f;
    m_DistanceToFarClip    = 0.0f;
    m_CameraInterval       = 0.0f;
    m_BaseCamera.left      = 0.0f;
    m_BaseCamera.right     = 0.0f;
    m_BaseCamera.bottom    = 0.0f;
    m_BaseCamera.top       = 0.0f;
    m_BaseCamera.near      = 0.0f;
    m_BaseCamera.far       = 0.0f;
    m_BaseCamera.position  = math::VEC3(0.0f, 0.0f, 0.0f);
    m_BaseCamera.posRight  = math::VEC3(0.0f, 0.0f, 0.0f);
    m_BaseCamera.posUp     = math::VEC3(0.0f, 0.0f, 0.0f);
    m_BaseCamera.posTarget = math::VEC3(0.0f, 0.0f, 0.0f);
}

void StereoCamera::Finalize(){ }

void StereoCamera::CalculateMatrices(nn::math::MTX44 *projL,nn::math::MTX34 *viewL,nn::math::MTX44 *projR,nn::math::MTX34 *viewR, nn::math::MTX44 *projOriginal,nn::math::MTX34 *viewOriginal,const f32 depthLevel,const f32 factor,const math::PivotDirection pivot){
    NN_ASSERT_(sIsInitialized);
    NN_NULL_ASSERT_(projL);
    NN_NULL_ASSERT_(viewL);
    NN_NULL_ASSERT_(projR);
    NN_NULL_ASSERT_(viewR);
    NN_ASSERT_(nn::math::PIVOT_NONE <= pivot && pivot < nn::math::PIVOT_NUM);
    if (!(0.0f <= factor && factor <= 1.0f)) {
        NN_TPANIC_("factor must be [0,1].");
    }
    CameraInfo infoL, infoR;
    
    {

        m_DepthLevel = depthLevel;
        f32 heightDiff = m_LimitParallax;
        heightDiff *= math::FAbs(this->m_BaseCamera.top - this->m_BaseCamera.bottom) * m_DepthLevel / (m_BaseCamera.near * s_CfgData.level);
        if (m_BaseCamera.far > m_DepthLevel) {
            m_CameraInterval = heightDiff * (this->m_BaseCamera.far / (this->m_BaseCamera.far - this->m_DepthLevel));
        } 
        else {
            m_CameraInterval = 0.0f;
        }
        
        m_CameraInterval *= factor;
        m_CameraInterval *= GetSliderVolume() * 0.5f;

        infoL.left  = m_BaseCamera.left +  m_CameraInterval * m_BaseCamera.near / m_DepthLevel;
        infoL.right = m_BaseCamera.right + m_CameraInterval * m_BaseCamera.near / m_DepthLevel;

        infoR.right = m_BaseCamera.right - m_CameraInterval * m_BaseCamera.near / m_DepthLevel;
        infoR.left  = m_BaseCamera.left  - m_CameraInterval * m_BaseCamera.near / m_DepthLevel;

        infoL.bottom = infoR.bottom = m_BaseCamera.bottom;
        infoL.top    = infoR.top    = m_BaseCamera.top;
        infoL.near   = infoR.near   = m_BaseCamera.near;
        infoL.far    = infoR.far    = m_BaseCamera.far;

        nn::math::VEC3Scale(&(infoL.position), &this->m_BaseCamera.posRight, this->m_CameraInterval);
        nn::math::VEC3Sub(&infoL.position, &this->m_BaseCamera.position, &infoL.position);
        nn::math::VEC3Add(&(infoL.posTarget), &(infoL.position), &this->m_BaseCamera.posTarget);
        infoL.posRight = m_BaseCamera.posRight;
        infoL.posUp    = m_BaseCamera.posUp;

        nn::math::VEC3Scale(&infoR.position, &m_BaseCamera.posRight, m_CameraInterval);
        nn::math::VEC3Add(&infoR.position, &m_BaseCamera.position, &infoR.position);
        nn::math::VEC3Add(&infoR.posTarget, &infoR.position, &m_BaseCamera.posTarget);
        infoR.posRight = m_BaseCamera.posRight;
        infoR.posUp =    m_BaseCamera.posUp;

        m_DistanceToNearClip = m_BaseCamera.near;
        m_DistanceToFarClip  = m_BaseCamera.far;

        m_LevelWidth = nn::math::FAbs(this->m_BaseCamera.right - this->m_BaseCamera.left) * (this->m_DepthLevel / this->m_BaseCamera.near);

    }

    math::MTX44FrustumPivot(projL, infoL.left, infoL.right, infoL.bottom, infoL.top,infoL.near, infoL.far, pivot);
    math::MTX44FrustumPivot(projR, infoR.left, infoR.right, infoR.bottom, infoR.top,infoR.near, infoR.far, pivot);
    
    math::MTX34LookAt(viewL, &infoL.position, &infoL.posUp, &infoL.posTarget);
    math::MTX34LookAt(viewR, &infoR.position, &infoR.posUp, &infoR.posTarget);
}

void StereoCamera::CalculateMatricesReal(nn::math::MTX44* projL, nn::math::MTX34* viewL,nn::math::MTX44* projR, nn::math::MTX34* viewR, const f32 depthLevel, const f32 factor, const nn::math::PivotDirection pivot){
    NN_ASSERT_(sIsInitialized);
    NN_NULL_ASSERT_(projL);
    NN_NULL_ASSERT_(viewL);
    NN_NULL_ASSERT_(projR);
    NN_NULL_ASSERT_(viewR);
    NN_ASSERT_(nn::math::PIVOT_NONE <= pivot && pivot < nn::math::PIVOT_NUM);
    if (!(0.0f <= factor && factor <= 1.0f)) {
        NN_TPANIC_("factor must be [0,1].");
    }
    CameraInfo infoL, infoR;
    
    {
        f32 near = depthLevel / m_BaseCamera.near;
        f32 levelWx = nn::math::FAbs(this->m_BaseCamera.right - this->m_BaseCamera.left) * near;
        f32 levelWy = nn::math::FAbs(this->m_BaseCamera.top - this->m_BaseCamera.bottom) * near;
        f32 r2vScale = levelWy / s_CfgData.level;
        
        f32 newN, newF, newL, newR, newB, newT;

        m_DepthLevel = s_CfgData.far * r2vScale;

        newN = m_DepthLevel - (depthLevel - m_BaseCamera.near);
        newF = m_DepthLevel + (m_BaseCamera.far - depthLevel);

        if (newN <= 0.0f){
            newN = m_DepthLevel * 0.01f;
        }

        if (newF <= newN){
            newF = newN * 2.0f;
        }

        near = newN / m_DepthLevel;
        f32 nearWx = levelWx * near;
        f32 nearWy = levelWy * near;

        near = nearWy / nn::math::FAbs(this->m_BaseCamera.top - this->m_BaseCamera.bottom);
        newT = m_BaseCamera.top * near;
        newB = m_BaseCamera.bottom * near;
        newL = m_BaseCamera.left * near;
        newR = m_BaseCamera.right * near;

        m_CameraInterval = s_CfgData.far * r2vScale;  

        m_CameraInterval *= factor;

        m_CameraInterval *= GetSliderVolume() * 0.5f;
        
        infoL.left  = newL + m_CameraInterval * newN / m_DepthLevel;
        infoL.right = newR + m_CameraInterval * newN / m_DepthLevel;
        
        infoR.right = newR - m_CameraInterval * newN / m_DepthLevel;
        infoR.left  = newL - m_CameraInterval * newN / m_DepthLevel;
        
        infoL.bottom = infoR.bottom = newB;
        infoL.top    = infoR.top    = newT;
        infoL.near   = infoR.near   = newN;
        infoL.far    = infoR.far    = newF;
        
        nn::math::VEC3 movPose;
        near = m_DepthLevel - depthLevel;
        nn::math::VEC3Scale(&movPose, &this->m_BaseCamera.posTarget, near);
        nn::math::VEC3Sub(&movPose, &this->m_BaseCamera.position, &movPose);

        nn::math::VEC3Scale(&(infoL.position), &this->m_BaseCamera.posRight, this->m_CameraInterval);
        nn::math::VEC3Sub(&(infoL.position), &movPose, &(infoL.position));
        nn::math::VEC3Add(&(infoL.posTarget), &(infoL.position), &this->m_BaseCamera.posTarget);
        
        infoL.posRight = m_BaseCamera.posRight;
        infoL.posUp    = m_BaseCamera.posUp;

        nn::math::VEC3Scale(&(infoR.position), &this->m_BaseCamera.posRight, this->m_CameraInterval);
        nn::math::VEC3Add(&(infoR.position), &movPose, &(infoR.position));
        nn::math::VEC3Add(&(infoR.posTarget), &(infoR.position), &this->m_BaseCamera.posTarget);

        infoR.posRight = m_BaseCamera.posRight;
        infoR.posUp    = m_BaseCamera.posUp;

        m_DistanceToNearClip = newN;
        m_DistanceToFarClip = newF;

        m_LevelWidth = nearWx * (m_DepthLevel / newN);

    }

    math::MTX44FrustumPivot(projL, infoL.left, infoL.right, infoL.bottom, infoL.top, infoL.near, infoL.far, pivot);
    math::MTX44FrustumPivot(projR, infoR.left, infoR.right, infoR.bottom, infoR.top, infoR.near, infoR.far, pivot);
    
    math::MTX34LookAt(viewL, &infoL.position, &infoL.posUp, &infoL.posTarget);
    math::MTX34LookAt(viewR, &infoR.position, &infoR.posUp, &infoR.posTarget);
}

f32 StereoCamera::GetCoefficientForParallax(void) const{
    m_CameraInterval / m_LevelWidth;
}

f32 StereoCamera::GetMaxParallax(void) const{
    return m_LimitParallax / s_CfgData.near * 0.5f * GetSliderVolume();
}

void StereoCamera::SetBaseCamera(const nn::math::MTX34 *view){
    NN_NULL_ASSERT_(view);
    Direction direction;
    GetLookPose(view, &this->m_BaseCamera.position, &direction);
    
    m_BaseCamera.posRight  = direction.right;
    m_BaseCamera.posUp     = direction.up;
    m_BaseCamera.posTarget = direction.target;
}

void StereoCamera::SetBaseFrustum(const nn::math::MTX44 *proj){
    m_BaseCamera.near = proj->matrix[2][3] / proj->matrix[2][2];
    m_BaseCamera.far = proj->matrix[2][3] / (proj->matrix[2][2] - 1.0f);

    f32 inverseProjX = proj->matrix[2][3] / (proj->matrix[0][0] * proj->matrix[2][2]);
    f32 inverseProjY = proj->matrix[2][3] / (proj->matrix[1][1] * proj->matrix[2][2]);

    m_BaseCamera.left = (proj->matrix[0][2] - 1.0f) * inverseProjX;
    m_BaseCamera.right = (proj->matrix[0][2] + 1.0f) * inverseProjX;
    m_BaseCamera.top = (proj->matrix[1][2] + 1.0f) * inverseProjY;
    m_BaseCamera.bottom = (proj->matrix[1][2] - 1.0f) * inverseProjY;
}

}
}
}