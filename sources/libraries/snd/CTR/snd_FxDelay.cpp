// Filename: snd_FxDelay.cpp
//
// Project: Horizon

#include <nn/snd.h>
#include <nn/math.h>
#include <cstring>

namespace nn {
namespace snd {
namespace CTR {

FxDelay::FxDelay(): 
    mpBuffer(NULL),
    mBufferSize(0),
    mCurFrame(0),
    mFeedbackGain(0x0),
    mLpfCoef1(0x10000),
    mLpfCoef2(0x0),
    mProcessChannelCount(4),
    mIsActive(false){
    this->FreeBuffer();

    for (int ch = 0; ch < mProcessChannelCount; ch++){
        mWorkBuffer.mLpf[ch] = 0;
    }
}

FxDelay::~FxDelay(){
    if (mIsActive){
        this->Finalize();
    }
    if (mpBuffer != NULL){
        this->ReleaseWorkBuffer();
    }
}
    
bool FxDelay::SetParam(const FxDelay::Param& param){
    {
        if (param.mDamping < 0.0f || param.mDamping > 1.0f){
            return false;
        }

        if (param.mFeedbackGain < 0.0f || param.mFeedbackGain > 1.0f){
            return false;
        }
    }

    {
        if (mIsActive == true){
            if (param.mDelayTime > mDelayTimeAtInitialize){
                return false;
            }
            if (mIsEnableSurroundAtInitialize == false && param.mIsEnableSurround == true){
                return false;
            }
        }

        mDelayFrames = (param.mDelayTime * 1000 ) / NN_SND_USECS_PER_FRAME;
        if (mDelayFrames == 0){
            mDelayFrames = 1;
        }

        if (param.mIsEnableSurround == false){
            mProcessChannelCount = 2;
        }
        else{
            mProcessChannelCount = 4;
        }

    }

    {
        mFeedbackGain = static_cast<s32>( static_cast<s32>(0x80L) * param.mFeedbackGain );

        f32 lpf_coef = param.mDamping;
        if (lpf_coef > 0.95f){
            lpf_coef = 0.95f;
        }
        f32 lpf_coef_1 = 1.f - lpf_coef;
        f32 lpf_coef_2 = lpf_coef;

        mLpfCoef1 = static_cast<s32>( static_cast<s32>(0x80L) * lpf_coef_1 );
        mLpfCoef2 = static_cast<s32>( static_cast<s32>(0x80L) * lpf_coef_2 );
    }

    mParam = param;
    return true;
}

size_t FxDelay::GetRequiredMemSize(){
    size_t result = (sizeof(s32) * NN_SND_SAMPLES_PER_FRAME * mDelayFrames) * mProcessChannelCount;
    result += 32;
    return result;
}

bool FxDelay::AssignWorkBuffer( uptr buffer, size_t size ){
    if (buffer == NULL){
        return false;
    }

    mpBuffer     = buffer;
    mBufferSize = size;

    return true;
}

void FxDelay::ReleaseWorkBuffer(){
    mpBuffer = NULL;
}

bool FxDelay::Initialize(){
    if (mIsActive){
        return false;
    }

    mDelayTimeAtInitialize = mParam.mDelayTime;
    mIsEnableSurroundAtInitialize = mParam.mIsEnableSurround;

    this->AllocBuffer();
    this->InitializeParam();

    mIsActive = true;

    return true;
}

void FxDelay::Finalize(){
    if(!mIsActive){
        return;
    }

    mIsActive = false;
    ::std::memset( reinterpret_cast<void*>(&this->mParam), 0, sizeof(Param) );

    this->FreeBuffer();
}

void FxDelay::UpdateBuffer(uptr data){
    if(!mIsActive){
        return;
    }

    NN_NULL_ASSERT_(data);
    NN_ASSERT_(mDelayFrames != 0);

    AuxBusData* auxData = reinterpret_cast<AuxBusData*>(data);
    s32* input[ CHANNEL_INDEX_NUM ];
    input[ CHANNEL_INDEX_FRONT_LEFT ]  = auxData->frontLeft;
    input[ CHANNEL_INDEX_FRONT_RIGHT ] = auxData->frontRight;
    input[ CHANNEL_INDEX_REAR_LEFT ]   = auxData->rearLeft;
    input[ CHANNEL_INDEX_REAR_RIGHT ]  = auxData->rearRight;

    const u32 start_pos = NN_SND_SAMPLES_PER_FRAME * mCurFrame;

    for (u32 ch = 0; ch < mProcessChannelCount; ch++){
        s32* pInput = &input[ch][0];
        s32* pDelay = &mWorkBuffer.mDelay[ch][0] + start_pos;
        s32 lpfData = mWorkBuffer.mLpf[ch];

        for (u32 samp = 0; samp < NN_SND_SAMPLES_PER_FRAME; samp++){
            s32 delay_out = *pDelay;

            s32 feedback = (math::Abs(delay_out) * mFeedbackGain) >> 7;
            if (delay_out < 0) feedback = -feedback;

            feedback = *pInput - feedback;

            s32 lpf_out = mLpfCoef1 * feedback + mLpfCoef2 * lpfData;
            lpf_out >>= 7;


            lpfData = lpf_out;

            *pDelay++ = lpf_out;

            *pInput++ = delay_out;
        }

        mWorkBuffer.mLpf[ch] = lpfData;
    }

    if (++mCurFrame >= mDelayFrames){
        mCurFrame = 0;
    }
}

void FxDelay::AllocBuffer(){
    const size_t ch_buffer_size = sizeof(s32) * NN_SND_SAMPLES_PER_FRAME * mDelayFrames;

    uptr ptr = math::RoundUp(mpBuffer, 32 );

    for(int ch = 0; ch < mProcessChannelCount; ch++){
        mWorkBuffer.mDelay[ch] = reinterpret_cast<s32*>(ptr);
        ptr += ch_buffer_size;
    }
}

void FxDelay::FreeBuffer(){
    for (int ch = 0; ch < mProcessChannelCount; ch++){
        mWorkBuffer.mDelay[ch] = NULL;
    }
}

void FxDelay::InitializeParam(){
    mCurFrame = 0;

    ::std::memset( reinterpret_cast<void*>(mpBuffer), 0, mBufferSize);

    for (int ch = 0; ch < mProcessChannelCount; ch++){
        mWorkBuffer.mLpf[ch] = 0;
    }
}

}
}
}