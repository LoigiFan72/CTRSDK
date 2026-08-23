// Filename: snd_FxReverb.cpp
//
// Project: Horizon

#include <nn/snd.h>
#include <nn/math.h>
#include <nn/Assert.h>

#include <nn/snd/CTR/Common/snd_Types.h>

namespace nn{
namespace snd{
namespace CTR{
namespace{

const f32 MSEC_PER_FRAME = NN_SND_USECS_PER_FRAME / 1000.f;
const int FX_SAMPLE_RATE = NN_SND_HW_I2S_CLOCK_32KHZ;   // 32728 Hz

inline f32 RoundUpToMsecPerFrame( u32 msec ){
    return ( msec > MSEC_PER_FRAME ) ? msec : MSEC_PER_FRAME;
}

inline u32 ConvertMsecToSamples( f32 msec ){
    return static_cast<u32>( msec / MSEC_PER_FRAME ) * NN_SND_SAMPLES_PER_FRAME;
}

}

FxReverb::FilterSize FxReverb::sDefaultFilterSize;

FxReverb::FxReverb(): 
    mpBuffer(NULL),
    mFilterSize(sDefaultFilterSize),
    mEarlyGain(0),
    mFusedGain(0),
    mLpfCoef1(0),
    mLpfCoef2(0),
    mIsActive(false){

    mEarlyLength = NN_SND_SAMPLES_PER_FRAME;
    mEarlyPos = 0;

    mPreDelayLength = NN_SND_SAMPLES_PER_FRAME;
    mPreDelayPos = 0;

    for (int i = 0; i < 2; i++){
        mCombFilterLength[i] = NN_SND_SAMPLES_PER_FRAME;
        mCombFilterPos[i] = 0;
        mCombFilterCoef[i] = 0;
    }

    mAllPassFilterLength = NN_SND_SAMPLES_PER_FRAME;
    mAllPassFilterPos = 0;
    mAllPassFilterCoef = 0;

    for (int ch = 0; ch < 4; ch++){
        mWorkBuffer.mEarlyReflection[ch] = NULL;
        mWorkBuffer.mPreDelay[ch] = NULL;

        for (int i = 0; i < 2; i++){
            mWorkBuffer.mCombFilter[ch][i] = NULL;
        }
        mWorkBuffer.mAllPassFilter[ch] = NULL;
        mWorkBuffer.mLpf[ch] = 0;
        mLastLpfOut[ch] = 0;
    }
}

FxReverb::~FxReverb(){
    if (mIsActive){
        this->Finalize();
    }
    if (mpBuffer != NULL){
        this->ReleaseWorkBuffer();
    }
}

bool FxReverb::Initialize(){
    mEarlyReflectionTimeAtInitialize = mParam.mEarlyReflectionTime;
    mPreDelayTimeAtInitialize = mParam.mPreDelayTime;
    mFilterSizeAtInitialize = mFilterSize;

    this->AllocBuffer();
    this->InitializeParam();
    mIsActive = true;

    return true;
}

void FxReverb::Finalize(){
    if (!mIsActive){
        return;
    }
    mIsActive = false;
    FreeBuffer();
}

bool FxReverb::SetParam(const FxReverb::Param& param){
    {
        if (param.mColoration < 0.0f || param.mColoration > 1.f){
            return false;
        }

        if (param.mDamping < 0.0f || param.mDamping > 1.0){
            return false;
        }

        if (param.mEarlyGain < 0.0f || param.mEarlyGain > 1.f){
            return false;
        }

        if (param.mFusedGain < 0.0f || param.mFusedGain > 1.0f){
            return false;
        }

        if (param.mpFilterSize != NULL){
            if (param.mpFilterSize->mComb0 == 0 || param.mpFilterSize->mComb1 == 0 || param.mpFilterSize->mAllPass == 0 ){
                return false;
            }
        }
    }

    {
        if (mIsActive == true){

            if (param.mEarlyReflectionTime > mEarlyReflectionTimeAtInitialize){
                return false;
            }

            if (param.mPreDelayTime > mPreDelayTimeAtInitialize){
                return false;
            }

            if (param.mpFilterSize != NULL){

                if (param.mpFilterSize->mComb0 > mFilterSizeAtInitialize.mComb0){
                    return false;
                }
                if (param.mpFilterSize->mComb1 > mFilterSizeAtInitialize.mComb1){
                    return false;
                }
                if (param.mpFilterSize->mAllPass > mFilterSizeAtInitialize.mAllPass){
                    return false;
                }
            }
        }

    }

    mParam = param; 

    if (mParam.mpFilterSize != NULL ){
        mFilterSize = *mParam.mpFilterSize;
        mParam.mpFilterSize = &mFilterSize;
    }

    if (mIsActive == true){
        InitializeParam();
    }
    return true;
}

bool FxReverb::AssignWorkBuffer(uptr buffer, size_t size){
    if (buffer == NULL){
        return false;
    }

    mpBuffer = buffer;
    mBufferSize = size;
    return true;
}

void FxReverb::ReleaseWorkBuffer(){
    mpBuffer = NULL;
}

size_t FxReverb::GetRequiredMemSize(){
    const size_t bufSizeForEarlyReflection = sizeof(s32) * ConvertMsecToSamples(RoundUpToMsecPerFrame(mParam.mEarlyReflectionTime));
    const size_t bufSizeForPreDelay = sizeof(s32) * ConvertMsecToSamples(RoundUpToMsecPerFrame(mParam.mPreDelayTime));
    const size_t bufSizeForFilterComp0 = sizeof(s32) * mFilterSize.mComb0;
    const size_t bufSizeForFilterComp1 = sizeof(s32) * mFilterSize.mComb1;
    const size_t bufSizeForFilterAllPass = sizeof(s32) * mFilterSize.mAllPass;

    size_t result = (bufSizeForEarlyReflection + bufSizeForPreDelay + bufSizeForFilterComp0 + bufSizeForFilterComp1 + bufSizeForFilterAllPass ) * 2;

    result += 32;
    return result;
}

void FxReverb::AllocBuffer(){
    const size_t bufSizeForEarlyReflection = sizeof(s32) * ConvertMsecToSamples(RoundUpToMsecPerFrame(mParam.mEarlyReflectionTime));
    const size_t bufSizeForPreDelay = sizeof(s32) * ConvertMsecToSamples(RoundUpToMsecPerFrame(mParam.mPreDelayTime));
    const size_t bufSizeForFilterComp0 = sizeof(s32) * mFilterSize.mComb0;
    const size_t bufSizeForFilterComp1 = sizeof(s32) * mFilterSize.mComb1;
    const size_t bufSizeForFilterAllPass = sizeof(s32) * mFilterSize.mAllPass;

    uptr ptr = math::RoundUp(mpBuffer, 32);

    for (int ch = 0; ch < 2; ch++){
        mWorkBuffer.mEarlyReflection[ch] = reinterpret_cast<s32*>(ptr);
        ptr += bufSizeForEarlyReflection;

        mWorkBuffer.mPreDelay[ch] = reinterpret_cast<s32*>(ptr);
        ptr += bufSizeForPreDelay;

        mWorkBuffer.mCombFilter[ch][0] = reinterpret_cast<s32*>(ptr);
        ptr += bufSizeForFilterComp0;

        mWorkBuffer.mCombFilter[ch][1] = reinterpret_cast<s32*>(ptr);
        ptr += bufSizeForFilterComp1;

        mWorkBuffer.mAllPassFilter[ch] = reinterpret_cast<s32*>(ptr);
        ptr += bufSizeForFilterAllPass;
    }
}

void FxReverb::FreeBuffer(){
    for ( int ch = 0; ch < 2; ch++ ){
        mWorkBuffer.mEarlyReflection[ch] = NULL;
        mWorkBuffer.mPreDelay[ch]        = NULL;
        mWorkBuffer.mCombFilter[ch][0]   = NULL;
        mWorkBuffer.mCombFilter[ch][1]   = NULL;
        mWorkBuffer.mAllPassFilter[ch]   = NULL;
    }
}

void FxReverb::InitializeParam(){
    f32 early_time = RoundUpToMsecPerFrame(mParam.mEarlyReflectionTime);
    mEarlyLength = ConvertMsecToSamples(early_time);
    mEarlyPos = 0;

    f32 pre_delay_time = RoundUpToMsecPerFrame(mParam.mPreDelayTime);
    mPreDelayLength = ConvertMsecToSamples(pre_delay_time);
    mPreDelayPos = 0;

    f32 fused_time_sec = static_cast<f32>(mParam.mFusedTime) / 1000.f;

    mCombFilterLength[0] = static_cast<s32>(mFilterSize.mComb0);
    mCombFilterLength[1] = static_cast<s32>(mFilterSize.mComb1);

    for (s32 i = 0; i < 2; i++){
        mCombFilterPos[i] = 0;

        f32 comb_coef = ::std::powf(10.f, (-3.f * static_cast<f32>(mCombFilterLength[i]) / (fused_time_sec * FX_SAMPLE_RATE)));
        mCombFilterCoef[i] = static_cast<s32>( static_cast<f32>(0x80L) * comb_coef );
    }

    mAllPassFilterLength = static_cast<s32>(mFilterSize.mAllPass);
    mAllPassFilterPos = 0;

    f32 all_pass_coef = mParam.mColoration;
    mAllPassFilterCoef = static_cast<s32>(static_cast<f32>(0x80L) * all_pass_coef);

    mEarlyGain = static_cast<s32>(static_cast<f32>(0x80L) * mParam.mEarlyGain);
    mFusedGain = static_cast<s32>(static_cast<f32>(0x80L) * mParam.mFusedGain);

    f32 lpf_coef = mParam.mDamping;
    if (lpf_coef > 0.95f) lpf_coef = 0.95f;

    if(mParam.mUseHpfDamping == true){
        mLpfCoef1 = static_cast<s32>(static_cast<s32>(0x80L) * (lpf_coef - 1.f ));
        mLpfCoef2 = static_cast<s32>(static_cast<s32>(0x80L) * (-1.f) * lpf_coef);
    }
    else{
        mLpfCoef1 = static_cast<s32>(static_cast<s32>(0x80L) * (1.f - lpf_coef));
        mLpfCoef2 = static_cast<s32>(static_cast<s32>(0x80L) * lpf_coef);
    }

    ::std::memset(reinterpret_cast<void*>(mpBuffer), 0, mBufferSize);
}

void FxReverb::UpdateBuffer(uptr data){
    if(!mIsActive) 
        return;
    NN_NULL_TASSERT_(data);

    AuxBusData* auxData = reinterpret_cast<AuxBusData*>(data);
    s32* input[CHANNEL_INDEX_NUM];
    input[CHANNEL_INDEX_FRONT_LEFT]  = auxData->frontLeft;
    input[CHANNEL_INDEX_FRONT_RIGHT] = auxData->frontRight;
    input[CHANNEL_INDEX_REAR_LEFT]   = auxData->rearLeft;
    input[CHANNEL_INDEX_REAR_RIGHT]  = auxData->rearRight;

    u32 early_pos;
    u32 pre_delay_pos;
    u32 comb_filter_pos0;
    u32 comb_filter_pos1;
    u32 allpass_filter_pos;

    for (int ch = 0; ch < 2; ch++){
        s32* early_reflection = mWorkBuffer.mEarlyReflection[ch] + mEarlyPos;
        s32* pre_delay      = mWorkBuffer.mPreDelay[ch] + mPreDelayPos;

        s32* pCombFilterLine0 = mWorkBuffer.mCombFilter[ch][0] + mCombFilterPos[0];
        s32* pCombFilterLine1 = mWorkBuffer.mCombFilter[ch][1] + mCombFilterPos[1];
        s32* pAllpassLine   = mWorkBuffer.mAllPassFilter[ch] + mAllPassFilterPos;

        s32* pInput = input[ch];

        s32 lastLpfOut = mLastLpfOut[ch];

        for (s32 samp = 0; samp < NN_SND_SAMPLES_PER_FRAME; samp++){
            s32 indata = *pInput;
            s32 pre_delay_out = *pre_delay;
            *pre_delay++ = indata;

            s32 filter_out = 0;
            s32 out_tmp = *pCombFilterLine0;

            s32 comb_fb_0 = ( math::Abs(out_tmp) * this->mCombFilterCoef[0] ) >> 7;
            if (out_tmp < 0) comb_fb_0 = -comb_fb_0;

            *pCombFilterLine0++ = pre_delay_out + comb_fb_0;
            filter_out += out_tmp;

            out_tmp = *pCombFilterLine1;

            s32 comb_fb_1 = ( math::Abs(out_tmp) * mCombFilterCoef[1] ) >> 7;
            if (out_tmp < 0) comb_fb_1 = -comb_fb_1;

            *pCombFilterLine1++ = pre_delay_out + comb_fb_1;
            filter_out -= out_tmp;

            out_tmp = *pAllpassLine;
            s32 allpass_coef = mAllPassFilterCoef;

            s32 allpass_in = ( math::Abs(out_tmp) * allpass_coef ) >> 7;
            if (out_tmp < 0) allpass_in = -allpass_in;
            allpass_in += filter_out;

            *pAllpassLine++ = allpass_in;

            s32 fo_2 = ( math::Abs(allpass_in) * allpass_coef ) >> 7;
            if (allpass_in < 0) fo_2 = -fo_2;
            filter_out = out_tmp - fo_2;

            s32 tmp = filter_out * mLpfCoef1 + lastLpfOut * mLpfCoef2;
            s32 fused_out = tmp >> 7;
            lastLpfOut = fused_out;

            s32 early_out = *early_reflection * mEarlyGain;

            *early_reflection++ = indata;

            fused_out *= mFusedGain;
            fused_out += early_out;
            fused_out >>= 7;
            *pInput++ = fused_out;
        }

        mLastLpfOut[ch] = lastLpfOut;
    }

    mEarlyPos += NN_SND_SAMPLES_PER_FRAME;
    if (mEarlyPos >= mEarlyLength){
        mEarlyPos = 0;
    }

    mPreDelayPos += NN_SND_SAMPLES_PER_FRAME;
    if (mPreDelayPos >= mPreDelayLength){
        mPreDelayPos = 0;
    }

    mCombFilterPos[0] += NN_SND_SAMPLES_PER_FRAME;
    if (mCombFilterPos[0] >= mCombFilterLength[0]){
        mCombFilterPos[0] = 0;
    }

    mCombFilterPos[1] += NN_SND_SAMPLES_PER_FRAME;
    if (mCombFilterPos[1] >= mCombFilterLength[1]){
        mCombFilterPos[1] = 0;
    }

    mAllPassFilterPos += NN_SND_SAMPLES_PER_FRAME;
    if (mAllPassFilterPos >= mAllPassFilterLength){
        mAllPassFilterPos = 0;
    }
}

}
}
}