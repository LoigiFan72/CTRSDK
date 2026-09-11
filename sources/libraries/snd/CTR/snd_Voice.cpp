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
    m_Id(id)
{}

Voice::~Voice(){}

void Voice::Initialize(){
    m_State = STATE_PAUSE;

    m_SampleRate = NN_SND_HW_I2S_CLOCK_32KHZ;
    m_Pitch = 1.0f;
    m_InterpolationType = INTERPOLATION_TYPE_POLYPHASE;

    mFilterType = FILTER_TYPE_NONE;
    ::std::memset(&this->mMonoFilterCoeffs, 0, sizeof(MonoFilterCoefficients));
    ::std::memset(&this->mBiquadFilterCoeffs, 0, sizeof(BiquadFilterCoefficients));

    m_Volume = 1.0f;
    MixParam mixParam;
    mixParam.mainBus[CHANNEL_INDEX_FRONT_LEFT ] = 1.0f;
    mixParam.mainBus[CHANNEL_INDEX_FRONT_RIGHT] = 1.0f;
    m_MixParam = mixParam;

    m_Priority = 0;
    m_PriorVoice = 0;
    m_InferiorVoice = 0;

    m_Callback = NULL;
    m_UserArg = 0;

    this->GetImpl()->Initialize();
}

void Voice::SetPriority(s32 priority){
    NN_TASSERT_(0 <= priority && priority <= VOICE_PRIORITY_NODROP);
    priority = math::Max(math::Min(priority, VOICE_PRIORITY_NODROP), 0);

    m_Priority = priority;

    VoiceManager::GetInstance().SetPriority(this, priority);
}

void Voice::SetPitch(f32 pitch){
    NN_TASSERT_(0.0f <= pitch);
    m_Pitch = math::Max(pitch, 0.0f);
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
    m_MixParam = mixParam;
    this->GetImpl()->SetMixParam(mixParam);
}

void Voice::SetSampleFormat(SampleFormat format){
    NN_TASSERT_(format == SAMPLE_FORMAT_PCM16 || format == SAMPLE_FORMAT_PCM8 || format == SAMPLE_FORMAT_ADPCM);
    this->GetImpl()->SetSampleFormat(format);
}

void Voice::SetSampleRate(s32 sampleRate){
    NN_TASSERT_(0 <= sampleRate);
    m_SampleRate = math::Max(sampleRate, 0);
    this->GetImpl()->SetSampleRate(sampleRate);
}

void Voice::SetState(State state){
    NN_TASSERT_(state == STATE_PLAY || state == STATE_STOP || state == STATE_PAUSE);
    ::std::memcpy(&this->mState, &state, 1);

    if(state == STATE_STOP)
    {
        this->GetImpl()->ReleaseWaveBuffer();
    }
    this->GetImpl()->SetState(state);
}

void Voice::SetVolume(f32 volume){
    m_Volume = volume;
    this->GetImpl()->SetVolume(volume);
}

void Voice::SetBiquadFilterCoefficients(const BiquadFilterCoefficients* pCoeff){
    m_BiquadFilterCoeffs = *pCoeff;
    this->GetImpl()->SetBiquadFilterCoefficients(m_BiquadFilterCoeffs);
}

void Voice::SetBiquadFilterCoefficients(const BiquadFilterCoefficients& coeff){
    m_BiquadFilterCoeffs = coeff;
    this->GetImpl()->SetBiquadFilterCoefficients(this->m_BiquadFilterCoeffs);
}

void Voice::SetMonoFilterCoefficients(const MonoFilterCoefficients* pCoeff){
    m_MonoFilterCoeffs = *pCoeff;
    this->GetImpl()->SetMonoFilterCoefficients(m_MonoFilterCoeffs);
}

void Voice::SetMonoFilterCoefficients(const MonoFilterCoefficients& coeff){
    m_MonoFilterCoeffs = coeff;
    this->GetImpl()->SetMonoFilterCoefficients(m_MonoFilterCoeffs);
}

void Voice::SetFrontBypassFlag(bool flag){
    this->GetImpl()->SetFrontBypassFlag(flag);
}

void Voice::SetInterpolationType(InterpolationType type){
    NN_TASSERT_(type == INTERPOLATION_TYPE_POLYPHASE || type == INTERPOLATION_TYPE_LINEAR || type == INTERPOLATION_TYPE_NONE);
    m_InterpolationType = type;
    this->GetImpl()->SetInterpolationType(type);
}

s32 Voice::GetPlayPosition() const{
    return this->GetImpl()->GetPlayPosition();
}

void Voice::EnableMonoFilter(bool enable){
    if (enable)
    {
        m_FilterType = static_cast<FilterType>(static_cast<bit32>(m_FilterType) | FILTER_TYPE_MONOPOLE);
    }

    else
    {
        mFilterType = static_cast<FilterType>(static_cast<bit32>(m_FilterType) & ~FILTER_TYPE_MONOPOLE);
    }
    
    this->GetImpl()->SetFilterType(this->m_FilterType);
}

void Voice::EnableBiquadFilter(bool enable){
    if (enable)
    {
        m_FilterType = static_cast<FilterType>(static_cast<bit32>(m_FilterType) | FILTER_TYPE_BIQUAD);
    }

    else
    {
        m_FilterType = static_cast<FilterType>(static_cast<bit32>(m_FilterType) & ~FILTER_TYPE_BIQUAD);
    }

    this->GetImpl()->SetFilterType(mFilterType);
}

}
}
}