#pragma once

#include <nn/Result.h>
#include <nn/Assert.h>
#include <nn/math.h>
#include <nn/snd/CTR/MPCore/snd_PrivateDefinition.h>
#include <nn/os/os_InterCoreCriticalSection.h>

#include <string.h>

namespace nn {
namespace snd {
namespace CTR {

enum VoiceDropMode 
{
    VOICE_DROP_MODE_DEFAULT   = 0,
    VOICE_DROP_MODE_REAL_TIME = 1
};

typedef void (*VoiceDropCallbackFunc)(class Voice*, uptr userArg);

class VoiceImpl;

class Voice 
{
    friend class VoiceManager;

public:
    enum State 
    {
        STATE_PLAY,
        STATE_STOP,
        STATE_PAUSE
    };

private:
    const s32 m_Id;
    util::SizedEnum1<Voice::State> m_State;
    util::SizedEnum1<InterpolationType> m_InterpolationType;
    s8 rev;
    util::SizedEnum1<FilterType> m_FilterType;
    MonoFilterCoefficients m_MonoFilterCoeffs;
    BiquadFilterCoefficients m_BiquadFilterCoeffs;
    s8 rev2[2];
    s32 m_SampleRate;
    f32 m_Pitch;
    s32 m_Priority;
    Voice* m_PriorVoice;
    Voice* m_InferiorVoice;
    VoiceDropCallbackFunc m_Callback;
    uptr m_UserArg;
    MixParam m_MixParam;
    f32 m_Volume;
    VoiceImpl* m_pImpl;

    void Initialize();
    void UpdateStatus(const void* pVars);
    void UpdateWaveBufferList();

    explicit Voice(s32 id);
    ~Voice();

public:
    void AppendWaveBuffer(WaveBuffer* pBuffer);

    void SetChannelCount(s32 channelCount);
    void SetSampleFormat(SampleFormat format);
    void SetSampleRate(s32 sampleRate);
    void SetPitch(f32 pitch);
    void SetInterpolationType(InterpolationType type);
    void SetPriority(s32 priority);
    void SetFrontBypassFlag(bool flag);
    void SetBiquadFilterCoefficients(const BiquadFilterCoefficients* pCoeff);
    void SetBiquadFilterCoefficients(const BiquadFilterCoefficients& coeff);
    void SetMonoFilterCoefficients(const MonoFilterCoefficients* pCoeff);
    void SetMonoFilterCoefficients(const MonoFilterCoefficients& coeff);
    void SetState(State state);
    void SetMixParam(const MixParam& mixParam);
    void SetVolume(f32 volume);

    void EnableBiquadFilter(bool enable);
    void EnableMonoFilter(bool enable);

    s32 GetPlayPosition() const;
    bool IsPlaying() const;

    bool SetupBcwav(uptr addrBcwav, WaveBuffer* pWaveBuffer0, WaveBuffer* pWaveBuffer1, Bcwav::ChannelIndex channelIndex = Bcwav::CHANNEL_INDEX_L);
public:
    s32 GetId() const { return m_Id; }
    VoiceImpl* GetImpl() const { return m_pImpl; }
    s32 GetPriority() const { return m_Priority; }
    Voice::State GetState() const { return m_State; }
};

Voice* AllocVoice(s32 priority, VoiceDropCallbackFunc callback, uptr userArg);
void FreeVoice(Voice* pVoice);

void SetVoiceDropMode(VoiceDropMode mode);


} // namespace CTR
} // namespace snd
} // namespace nn