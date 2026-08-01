// Filename: hid_AnalogStickClamper.cpp
//
// Project: Horizon

#include <nn/hid/CTR/hid_AnalogStickClamper.h>
#include <nn/hidlow/hidlow_Utils.h>
#include <nn/math.h>

namespace nn{
namespace hid{
namespace CTR{

AnalogStickClamper::AnalogStickClamper() : 
    mMinOfStickClampCircle(MIN_OF_STICK_CLAMP_MODE_CIRCLE),
    mMinOfStickClampCross(MIN_OF_STICK_CLAMP_MODE_CROSS),
    mMinOfStickClampMinimum(MIN_OF_STICK_CLAMP_MODE_CIRCLE),
    mMaxOfStickClampCircle(LIMIT_OF_STICK_CLAMP_MAX),
    mMaxOfStickClampCross(LIMIT_OF_STICK_CLAMP_MAX),
    mMaxOfStickClampMinimum(LIMIT_OF_STICK_CLAMP_MAX),
    mStickClampMode(STICK_CLAMP_MODE_CIRCLE){
        
    mThreshold = DEFAULT_THRESHOLD_OF_NORMALIZE_STICK;
    mStrokeVelocity = 0.0f;
    mLastLength = 0.0f;
    mLastDiff = 0.0f;
    mScale = DEFAULT_SCALE_OF_NORMALIZE_STICK;
    mStroke = 141.0f;
}

void AnalogStickClamper::SetStickClampFree(s16 min, s16 max){
    if(mStickClampMode == STICK_CLAMP_MODE_CIRCLE){
        mMinOfStickClampCircle = min;
        mMaxOfStickClampCircle = max;
    }

    else if(mStickClampMode == STICK_CLAMP_MODE_CROSS){
        mMinOfStickClampCross = min;
        mMaxOfStickClampCross = max;
    }

    else{
        mMaxOfStickClampMinimum = max;
        mMinOfStickClampMinimum = min;
    }
}

void AnalogStickClamper::SetNormalizeStickScaleSettings(f32 scale, s16 threshold){
    if(LIMIT_OF_STICK_CLAMP_MAX < threshold) threshold = LIMIT_OF_STICK_CLAMP_MAX;
    mScale = scale;
    mThreshold = threshold;
}

void AnalogStickClamper::ClampCore(short* pOutX, short* pOutY, s32 x, s32 y){
    switch (this->mStickClampMode) {
    case STICK_CLAMP_MODE_CIRCLE:
        hidlow::ClampStickCircle(pOutX, pOutY, x, y, this->mMinOfStickClampCircle, this->mMaxOfStickClampCircle);
        break;
    case STICK_CLAMP_MODE_CROSS:
        hidlow::ClampStickCross(pOutX, pOutY, x, y, this->mMinOfStickClampCross, this->mMaxOfStickClampCross);
        break;
    case STICK_CLAMP_MODE_MINIMUM:
        hidlow::ClampStickMinimum(pOutX, pOutY, x, y, this->mMinOfStickClampMinimum, this->mMaxOfStickClampMinimum);
        break;
    }
}

void AnalogStickClamper::ClampValueOfClamp() {
  if (mMinOfStickClampCircle < MIN_OF_STICK_CLAMP_MODE_CIRCLE)
    mMinOfStickClampCircle = MIN_OF_STICK_CLAMP_MODE_CIRCLE;
    
  if (mMinOfStickClampCross < MIN_OF_STICK_CLAMP_MODE_CROSS)
    mMinOfStickClampCross = MIN_OF_STICK_CLAMP_MODE_CROSS;
    
  if (mMaxOfStickClampCircle > LIMIT_OF_STICK_CLAMP_MAX)
    mMaxOfStickClampCircle = LIMIT_OF_STICK_CLAMP_MAX;
    
  if (mMaxOfStickClampCross > LIMIT_OF_STICK_CLAMP_MAX)
    mMaxOfStickClampCross = LIMIT_OF_STICK_CLAMP_MAX;
    
  if (mMaxOfStickClampMinimum > LIMIT_OF_STICK_CLAMP_MAX)
    mMaxOfStickClampMinimum = LIMIT_OF_STICK_CLAMP_MAX;
}

f32 AnalogStickClamper::NormalizeStick(s16 x){
    f32 fx = (f32)x;
    s16 threshold;

    switch (mStickClampMode){
    case STICK_CLAMP_MODE_CIRCLE:
        threshold = mMaxOfStickClampCircle - mMinOfStickClampCircle;
        break;

    case STICK_CLAMP_MODE_CROSS:
        threshold = mMaxOfStickClampCross - mMinOfStickClampCross;
        break;

    case STICK_CLAMP_MODE_MINIMUM:
        threshold = LIMIT_OF_STICK_CLAMP_MAX - MIN_OF_STICK_CLAMP_MODE_CIRCLE;
        break;
    }

    if (0 == x)
        return 0.0f;
    else if(threshold <= x)
        return 1.0f;

    return fx / threshold;
}


void AnalogStickClamper::SetStickClamp(short min, short max) {
    NN_TASSERT_(0 <= min);
    NN_TASSERT_(min < max);
    
    if (LIMIT_OF_STICK_CLAMP_MAX < max){
        max = LIMIT_OF_STICK_CLAMP_MAX;
    }

    if (mStickClampMode == STICK_CLAMP_MODE_CIRCLE){
        if (min < MIN_OF_STICK_CLAMP_MODE_CIRCLE){
            min = MIN_OF_STICK_CLAMP_MODE_CIRCLE;
        }
        mMinOfStickClampCircle = min;
        mMaxOfStickClampCircle = max;
    
    }
    else if (mStickClampMode == STICK_CLAMP_MODE_CROSS){
        if (min < MIN_OF_STICK_CLAMP_MODE_CROSS){
            min = MIN_OF_STICK_CLAMP_MODE_CROSS;
        }
        mMinOfStickClampCross = min;
        mMaxOfStickClampCross = max;
    }
    else{
        mMaxOfStickClampMinimum = max;
    }
}

}
}
}