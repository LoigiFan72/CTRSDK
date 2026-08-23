#pragma once

#include <nn/snd/CTR/MPCore/snd_Voice.h>

namespace nn{
namespace snd{
namespace CTR{

class VoiceImpl{
public:
    s32 mId;
    s16 mSyncCount;
    u16 mBufferId;
    s32 mPlayPosition;
    bool mPlaying;
    Voice::State mState;
    InterpolationType mInterpolationType;
    FilterType mFilterType;
    MonoFilterCoefficients mMonoFilterCoeffs;
    BiquadFilterCoefficients mBiquadFilterCoeffs;
    u16 mSampleInfo;
    s32 mSampleRate;
    f32 mPitch;
    f32 mSampleRateRatio;
    u32 mDspCycles;
    WaveBuffer* mpWaveBuffer;
    s16 mSentBufferCount;
    s16 mNextBufferIndex;
    MixParam mMixParam;
    f32 mVolume;
    bit16 mModifiedParamFlag;
    bool mIsFirstWaveBufferForAdpcm;
    bit8 mWaveBufferModifiedFlag;
    os::InterCoreCriticalSection mCriticalSection;

public:
    VoiceImpl(){ }
    VoiceImpl(s32 id);
    void AppendWaveBuffer(WaveBuffer* buffer);
    f32 CalcFsRatio();
    void CalculateDspCycle();
    void ForceUpdateParams();
    void Initialize();
    void Pause();
    void ReleaseWaveBuffer();
    ushort SelectCoefficient();
    s32 GetPlayPosition() const;
    void SetBiquadFilterCoefficients(const BiquadFilterCoefficients& coeff);
    void SetMonoFilterCoefficients(const MonoFilterCoefficients& coeff);
    void SendWaveBuffer();
    void SetChannelCount(s32 channelCount);
    void SetFilterType(FilterType type);
    void SetFrontBypassFlag(bool flag);
    void SetInterpolationType(InterpolationType type);
    void SetMixParam(const MixParam& mixParam);
    void SetMixVolume();
    void SetPitch(f32 pitch);
    void SetSampleFormat(SampleFormat format);
    void SetSampleRate(s32 sampleRate);
    void SetState(Voice::State state);
    void SetSyncCount();
    void SetTimer();
    void SetVolume(f32 volume);
    void Start();
    void Stop();
    void UpdateInterpolationType();
    void UpdateParams();
    void UpdateStatus(const void * ptr);
    void UpdateWaveBufferList();
    void UpdateWaveBufferStatus(ushort currentBufferId, ushort lastBufferId);
public:
    s32 GetCycle() const { return mDspCycles; }
    bool IsPlaying () const{ return mPlaying; }
    Voice::State VoiceImpl::GetState() const{ return mState; }
};

}
}
}