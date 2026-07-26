// Filename: ucld_StereoCamera.cpp
//
// Project: Horizon

#include <nn/ulcd/CTR/ulcd_StereoCamera.h>
#include <nn/cfg/CTR/cfg_Api.h>
#include <nn/cfg/CTR/cfg_DetailApi.h>
#include <nn/math/math_Arithmetic.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/os/os_Types.h>

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
    StereoCamera::cfgdata sCfgData;
}

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

    mDepthLevel = depthLevel;
    f32 heightDiff = mLimitParallex;
    heightDiff *= math::FAbs(this->mBaseCamera.top - this->mBaseCamera.bottom) * mDepthLevel / (mBaseCamera.near * sCfgData.level);
    if (mBaseCamera.far > mDepthLevel) {
        mCameraInterval = heightDiff * (this->mBaseCamera.far / (this->mBaseCamera.far - this->mDepthLevel));
    } 
    else {
        this->mCameraInterval = 0.0f;
    }
    
    this->mCameraInterval *= factor;
    this->mCameraInterval *= GetSliderVolume() * 0.5f;

    infoL.left  = mBaseCamera.left +  mCameraInterval * mBaseCamera.near / mDepthLevel;
    infoL.right = mBaseCamera.right + mCameraInterval * mBaseCamera.near / mDepthLevel;

    infoR.right = mBaseCamera.right - mCameraInterval * mBaseCamera.near / mDepthLevel;
    infoR.left  = mBaseCamera.left  - mCameraInterval * mBaseCamera.near / mDepthLevel;

    infoL.bottom = infoR.bottom = mBaseCamera.bottom;
    infoL.top    = infoR.top    = mBaseCamera.top;
    infoL.near   = infoR.near   = mBaseCamera.near;
    infoL.far    = infoR.far    = mBaseCamera.far;

    nn::math::VEC3Scale(&(infoL.position), &this->mBaseCamera.posRight, this->mCameraInterval);
    nn::math::VEC3Sub(&infoL.position, &this->mBaseCamera.position, &infoL.position);
    nn::math::VEC3Add(&(infoL.posTarget), &(infoL.position), &this->mBaseCamera.posTarget);
    infoL.posRight = this->mBaseCamera.posRight;
    infoL.posUp    = this->mBaseCamera.posUp;

    nn::math::VEC3Scale(&infoR.position, &mBaseCamera.posRight, mCameraInterval);
    nn::math::VEC3Add(&infoR.position, &mBaseCamera.position, &infoR.position);
    nn::math::VEC3Add(&infoR.posTarget, &infoR.position, &mBaseCamera.posTarget);
    infoR.posRight = this->mBaseCamera.posRight;
    infoR.posUp = this->mBaseCamera.posUp;

    mDistanceToNearClip = mBaseCamera.near;
    mDistanceToFarClip  = mBaseCamera.far;

    mLevelWidth = nn::math::FAbs(this->mBaseCamera.right - this->mBaseCamera.left) * (this->mDepthLevel / this->mBaseCamera.near);

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
        f32 near = depthLevel / mBaseCamera.near;
        f32 levelWx = nn::math::FAbs(this->mBaseCamera.right - this->mBaseCamera.left) * near;
        f32 levelWy = nn::math::FAbs(this->mBaseCamera.top - this->mBaseCamera.bottom) * near;
        f32 r2vScale = levelWy / sCfgData.level;
        
        f32 newN, newF, newL, newR, newB, newT;

        mDepthLevel = sCfgData.far * r2vScale;

        newN = mDepthLevel - (depthLevel - mBaseCamera.near);
        newF = mDepthLevel + (mBaseCamera.far - depthLevel);

        if (newN <= 0.0f){
            newN = mDepthLevel * 0.01f;
        }

        if (newF <= newN){
            newF = newN * 2.0f;
        }

        near = newN / mDepthLevel;
        f32 nearWx = levelWx * near;
        f32 nearWy = levelWy * near;

        near = nearWy / nn::math::FAbs(this->mBaseCamera.top - this->mBaseCamera.bottom);
        newT = mBaseCamera.top * near;
        newB = mBaseCamera.bottom * near;
        newL = mBaseCamera.left * near;
        newR = mBaseCamera.right * near;

        mCameraInterval = sCfgData.far * r2vScale;  

        mCameraInterval *= factor;

        mCameraInterval *= GetSliderVolume() * 0.5f;
        
        infoL.left  = newL + mCameraInterval * newN / mDepthLevel;
        infoL.right = newR + mCameraInterval * newN / mDepthLevel;
        
        infoR.right = newR - mCameraInterval * newN / mDepthLevel;
        infoR.left  = newL - mCameraInterval * newN / mDepthLevel;
        
        infoL.bottom = infoR.bottom = newB;
        infoL.top    = infoR.top    = newT;
        infoL.near   = infoR.near   = newN;
        infoL.far    = infoR.far    = newF;
        
        nn::math::VEC3 movPose;
        near = mDepthLevel - depthLevel;
        nn::math::VEC3Scale(&movPose, &this->mBaseCamera.posTarget, near);
        nn::math::VEC3Sub(&movPose, &this->mBaseCamera.position, &movPose);

        nn::math::VEC3Scale(&(infoL.position), &this->mBaseCamera.posRight, this->mCameraInterval);
        nn::math::VEC3Sub(&(infoL.position), &movPose, &(infoL.position));
        nn::math::VEC3Add(&(infoL.posTarget), &(infoL.position), &this->mBaseCamera.posTarget);
        
        infoL.posRight = mBaseCamera.posRight;
        infoL.posUp    = mBaseCamera.posUp;

        nn::math::VEC3Scale(&(infoR.position), &this->mBaseCamera.posRight, this->mCameraInterval);
        nn::math::VEC3Add(&(infoR.position), &movPose, &(infoR.position));
        nn::math::VEC3Add(&(infoR.posTarget), &(infoR.position), &this->mBaseCamera.posTarget);

        infoR.posRight = mBaseCamera.posRight;
        infoR.posUp    = mBaseCamera.posUp;

        mDistanceToNearClip = newN;
        mDistanceToFarClip = newF;

        mLevelWidth = nearWx * (mDepthLevel / newN);

    }

    math::MTX44FrustumPivot(projL, infoL.left, infoL.right, infoL.bottom, infoL.top, infoL.near, infoL.far, pivot);
    math::MTX44FrustumPivot(projR, infoR.left, infoR.right, infoR.bottom, infoR.top, infoR.near, infoR.far, pivot);
    
    math::MTX34LookAt(viewL, &infoL.position, &infoL.posUp, &infoL.posTarget);
    math::MTX34LookAt(viewR, &infoR.position, &infoR.posUp, &infoR.posTarget);
}

f32 StereoCamera::GetCoefficientForParallax(void) const{
    this->mCameraInterval / this->mLevelWidth;
}

void StereoCamera::SetBaseCamera(const nn::math::MTX34 *view){
    NN_NULL_ASSERT_(view);
    Direction direction;
    GetLookPose(view, &this->mBaseCamera.position, &direction);
    
    mBaseCamera.posRight = direction.right;
    mBaseCamera.posUp = direction.up;
    mBaseCamera.posTarget = direction.target;
}

void StereoCamera::SetBaseFrustum(const nn::math::MTX44 *proj){
    mBaseCamera.near = proj->matrix[2][3] / proj->matrix[2][2];
    mBaseCamera.far = proj->matrix[2][3] / (proj->matrix[2][2] - 1.0f);

    f32 inverseProjX = proj->matrix[2][3] / (proj->matrix[0][0] * proj->matrix[2][2]);
    f32 inverseProjY = proj->matrix[2][3] / (proj->matrix[1][1] * proj->matrix[2][2]);

    mBaseCamera.left = (proj->matrix[0][2] - 1.0f) * inverseProjX;
    mBaseCamera.right = (proj->matrix[0][2] + 1.0f) * inverseProjX;
    mBaseCamera.top = (proj->matrix[1][2] + 1.0f) * inverseProjY;
    mBaseCamera.bottom = (proj->matrix[1][2] - 1.0f) * inverseProjY;
}

void StereoCamera::Initialize(){
    Result res;
    if(sIsInitialized == 0){
        cfg::CTR::Initialize();
        res = cfg::CTR::detail::GetConfig(&sCfgData,0x20,0x50005);
        NN_PANIC_IF_FAILED(res);
        cfg::CTR::Finalize();
        sIsInitialized = true;
    }
    this->mLimitParallex = sCfgData.limit;
    this->mLevelWidth = 0.0;
    this->mDepthLevel = 0.0;
    this->mDistanceToNearClip = 0.0;
    this->mDistanceToFarClip = 0.0;
    this->mCameraInterval = 0.0;
    this->mBaseCamera.left = 0.0;
    this->mBaseCamera.right = 0.0;
    this->mBaseCamera.bottom = 0.0;
    this->mBaseCamera.top = 0.0;
    this->mBaseCamera.near = 0.0;
    this->mBaseCamera.far = 0.0;
    this->mBaseCamera.position = math::VEC3(0.0f, 0.0f, 0.0f);
    this->mBaseCamera.posRight = math::VEC3(0.0f, 0.0f, 0.0f);
    this->mBaseCamera.posUp = nn::math::VEC3(0.0f, 0.0f, 0.0f);
    this->mBaseCamera.posTarget = math::VEC3(0.0f, 0.0f, 0.0f);
}

void StereoCamera::Finalize(){ }

StereoCamera::StereoCamera(){
    mDepthLevel = 0.0;
    mCameraInterval = 0.0;
}

StereoCamera::~StereoCamera(){ this->Finalize(); }

}
}
}