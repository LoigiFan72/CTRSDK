// Filename: snd_Voice.cpp
//
// Project: Horizon

#include <nn/snd/CTR/MPCore/snd_Voice.h>
#include <cstring>

namespace nn {
namespace snd {
namespace CTR {

void Voice::AppendWaveBuffer(WaveBuffer* pBuffer){
    this->mpImpl->AppendWaveBuffer(pBuffer);
}

void Voice::SetChannelCount(s32 channelCount){
    NN_TASSERT_(channelCount == 1 || channelCount == 2);
    this->mpImpl->SetChannelCount(channelCount);
}

void Voice::SetMixParam(const MixParam& mixParam){
    memcpy(&this->mMixParam, &mixParam, 0x30);
    this->mpImpl->SetMixParam(mixParam);
}

void Voice::SetSampleFormat(SampleFormat format){
    NN_TASSERT_(format == SAMPLE_FORMAT_PCM16 || format == SAMPLE_FORMAT_PCM8 || format == SAMPLE_FORMAT_ADPCM);
    this->mpImpl->SetSampleFormat(format);
}

void Voice::SetState(State state){
    NN_TASSERT_(state == STATE_PLAY || state == STATE_STOP || state == STATE_PAUSE);
    ::std::memcpy(&this->mState, &state, 1);
    if(state == STATE_STOP){
        this->mpImpl->ReleaseWaveBuffer();
    }
    this->mpImpl->SetState(state);
}

void Voice::SetVolume(f32 volume){
    mVolume = volume;
    this->mpImpl->SetVolume(volume);
}

}
}
}