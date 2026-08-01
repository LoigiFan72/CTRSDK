// Filename: snd_FxReverb.cpp
//
// Project: Horizon

#include <nn/snd/CTR/MPCore/snd_FxReverb.h>
#include <nn/Assert.h>

#include <nn/snd/CTR/Common/snd_Types.h>

namespace nn{
namespace math{
    inline int Abs(int x){ if(x < 0) x = -x;  return x;}
}
namespace snd{
namespace CTR{

void FxReverb::UpdateBuffer(AuxBusData* data){
    NN_NULL_TASSERT_(data);
    if(!mIsActive) return;

    s32* input[CTR::CHANNEL_INDEX_NUM];
    input[CTR::CHANNEL_INDEX_FRONT_LEFT]  = data->frontLeft;
    input[CTR::CHANNEL_INDEX_FRONT_RIGHT] = data->frontRight;
    input[CTR::CHANNEL_INDEX_REAR_LEFT]   = data->rearLeft;
    input[CTR::CHANNEL_INDEX_REAR_RIGHT]  = data->rearRight;

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