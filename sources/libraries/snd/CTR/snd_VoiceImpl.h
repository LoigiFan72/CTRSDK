#pragma once

#include <nn/snd/CTR/MPCore/snd_Voice.h>
#include <nn/os/os_InterCoreCriticalSection.h>

namespace nn{
namespace snd{
namespace CTR{

class VoiceImpl
{
public:
    s32 m_Id;
    s16 m_SyncCount;
    u16 m_BufferId;
    s32 m_PlayPosition;
    bool m_Playing;
    Voice::State m_State;
    InterpolationType m_InterpolationType;
    FilterType m_FilterType;
    MonoFilterCoefficients m_MonoFilterCoeffs;
    BiquadFilterCoefficients m_BiquadFilterCoeffs;
    u16 m_SampleInfo;
    s32 m_SampleRate;
    f32 m_Pitch;
    f32 m_SampleRateRatio;
    u32 m_DspCycles;
    WaveBuffer* m_pWaveBuffer;
    s16 m_SentBufferCount;
    s16 m_NextBufferIndex;
    MixParam m_MixParam;
    f32 m_Volume;
    bit16 m_ModifiedParamFlag;
    bool m_IsFirstWaveBufferForAdpcm;
    bit8 m_WaveBufferModifiedFlag;
    os::InterCoreCriticalSection m_CriticalSection;

public:
    VoiceImpl()
    { 
    }
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
    s32 GetCycle() const;
    bool IsPlaying () const{ return m_Playing; }
    Voice::State VoiceImpl::GetState() const{ return m_State; }
};

}
}
}