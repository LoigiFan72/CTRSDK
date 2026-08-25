// Filename: snd_Voice.cpp
//
// Project: Horizon

#include <nn/snd/CTR/MPCore/snd_Voice.h>
#include <nn/snd/CTR/Common/snd_Const.h>
#include <cstring>

#include "snd_VoiceImpl.h"
#include "snd_VoiceManager.h"

namespace nn {
namespace snd {
namespace CTR {

Voice::Voice(s32 id): 
    mId(id)
{}

Voice::~Voice(){}

void Voice::Initialize(){
    mState = STATE_PAUSE;

    mSampleRate = NN_SND_HW_I2S_CLOCK_32KHZ;
    mPitch = 1.0f;
    mInterpolationType = INTERPOLATION_TYPE_POLYPHASE;

    mFilterType = FILTER_TYPE_NONE;
    ::std::memset(&this->mMonoFilterCoeffs, 0, sizeof(MonoFilterCoefficients));
    ::std::memset(&this->mBiquadFilterCoeffs, 0, sizeof(BiquadFilterCoefficients));

    mVolume = 1.0f;
    MixParam mixParam;
    mixParam.mainBus[CHANNEL_INDEX_FRONT_LEFT ] = 1.0f;
    mixParam.mainBus[CHANNEL_INDEX_FRONT_RIGHT] = 1.0f;
    mMixParam = mixParam;

    mPriority = 0;
    mPriorVoice = 0;
    mInferiorVoice = 0;

    mCallback = NULL;
    mUserArg = 0;

    this->GetImpl()->Initialize();
}

void Voice::SetPriority(s32 priority){
    NN_TASSERT_(0 <= priority && priority <= VOICE_PRIORITY_NODROP);
    priority = math::Max(math::Min(priority, VOICE_PRIORITY_NODROP), 0);

    mPriority = priority;

    VoiceManager::GetInstance().SetPriority(this, priority);
}

void Voice::SetPitch(f32 pitch){
    NN_TASSERT_(0.0f <= pitch);
    mPitch = math::Max(pitch, 0.0f);
    this->GetImpl()->SetPitch(pitch);
}

void Voice::AppendWaveBuffer(WaveBuffer* pBuffer){
    this->GetImpl()->AppendWaveBuffer(pBuffer);
}

void Voice::SetChannelCount(s32 channelCount){
    NN_TASSERT_(channelCount == 1 || channelCount == 2);
    this->GetImpl()->SetChannelCount(channelCount);
}

void Voice::SetMixParam(const MixParam& mixParam){
    mMixParam = mixParam;
    this->GetImpl()->SetMixParam(mixParam);
}

void Voice::SetSampleFormat(SampleFormat format){
    NN_TASSERT_(format == SAMPLE_FORMAT_PCM16 || format == SAMPLE_FORMAT_PCM8 || format == SAMPLE_FORMAT_ADPCM);
    this->GetImpl()->SetSampleFormat(format);
}

void Voice::SetSampleRate(s32 sampleRate){
    NN_TASSERT_(0 <= sampleRate);
    mSampleRate = math::Max(sampleRate, 0);
    this->GetImpl()->SetSampleRate(sampleRate);
}

void Voice::SetState(State state){
    NN_TASSERT_(state == STATE_PLAY || state == STATE_STOP || state == STATE_PAUSE);
    ::std::memcpy(&this->mState, &state, 1);
    if(state == STATE_STOP){
        this->GetImpl()->ReleaseWaveBuffer();
    }
    this->GetImpl()->SetState(state);
}

void Voice::SetVolume(f32 volume){
    mVolume = volume;
    this->GetImpl()->SetVolume(volume);
}

void Voice::SetBiquadFilterCoefficients(const BiquadFilterCoefficients* pCoeff){
    mBiquadFilterCoeffs = *pCoeff;
    this->GetImpl()->SetBiquadFilterCoefficients(mBiquadFilterCoeffs);
}

void Voice::SetBiquadFilterCoefficients(const BiquadFilterCoefficients& coeff){
    mBiquadFilterCoeffs = coeff;
    this->GetImpl()->SetBiquadFilterCoefficients(this->mBiquadFilterCoeffs);
}

void Voice::SetMonoFilterCoefficients(const MonoFilterCoefficients* pCoeff){
    mMonoFilterCoeffs = *pCoeff;
    this->GetImpl()->SetMonoFilterCoefficients(mMonoFilterCoeffs);
}

void Voice::SetMonoFilterCoefficients(const MonoFilterCoefficients& coeff){
    mMonoFilterCoeffs = coeff;
    this->GetImpl()->SetMonoFilterCoefficients(mMonoFilterCoeffs);
}

void Voice::SetFrontBypassFlag(bool flag){
    this->GetImpl()->SetFrontBypassFlag(flag);
}

void Voice::SetInterpolationType(InterpolationType type){
    NN_TASSERT_(type == INTERPOLATION_TYPE_POLYPHASE || type == INTERPOLATION_TYPE_LINEAR || type == INTERPOLATION_TYPE_NONE);
    mInterpolationType = type;
    this->GetImpl()->SetInterpolationType(type);
}

s32 Voice::GetPlayPosition() const{
    return this->GetImpl()->GetPlayPosition();
}

void Voice::EnableMonoFilter(bool enable){
    if (enable){
        mFilterType = static_cast<FilterType>(static_cast<bit32>(mFilterType) | FILTER_TYPE_MONOPOLE);
    }

    else{
        mFilterType = static_cast<FilterType>(static_cast<bit32>(mFilterType) & ~FILTER_TYPE_MONOPOLE);
    }
    
    this->GetImpl()->SetFilterType(this->mFilterType);
}

void Voice::EnableBiquadFilter(bool enable){
    if (enable){
        mFilterType = static_cast<FilterType>(static_cast<bit32>(mFilterType) | FILTER_TYPE_BIQUAD);
    }

    else{
        mFilterType = static_cast<FilterType>(static_cast<bit32>(mFilterType) & ~FILTER_TYPE_BIQUAD);
    }

    this->GetImpl()->SetFilterType(mFilterType);
}

}
}
}