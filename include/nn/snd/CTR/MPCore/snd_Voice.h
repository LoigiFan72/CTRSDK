#pragma once

#include <nn/Result.h>
#include <nn/Assert.h>
#include <nn/math.h>
#include <nn/snd/CTR/MPCore/snd_PrivateDefinition.h>
#include <nn/os/os_CriticalSection.h>

#include <string.h>

namespace nn {
namespace snd {
namespace CTR {

enum VoiceDropMode {
    VOICE_DROP_MODE_DEFAULT   = 0,
    VOICE_DROP_MODE_REAL_TIME = 1
};

typedef void (*VoiceDropCallbackFunc)(class Voice*, uptr userArg);

class VoiceImpl;

class Voice {
    friend class VoiceManager;

public:
    enum State {
        STATE_PLAY,
        STATE_STOP,
        STATE_PAUSE
    };

private:
    const s32 mId;
    s8 mState;
    s8 mInterpolationType;
    s8 rev;
    s8 mFilterType;
    MonoFilterCoefficients mMonoFilterCoeffs;
    BiquadFilterCoefficients mBiquadFilterCoeffs;
    s8 rev2[2];
    s32 mSampleRate;
    f32 mPitch;
    s32 mPriority;
    Voice* mPriorVoice;
    Voice* mInferiorVoice;
    VoiceDropCallbackFunc mCallback;
    uptr mUserArg;
    MixParam mMixParam;
    f32 mVolume;
    VoiceImpl* mpImpl;

    void Initialize();
    void UpdateStatus(const void* pVars);
    void UpdateWaveBufferList();
    s32 GetId() const { return mId; }
    VoiceImpl* GetImpl() const { return mpImpl; }

    explicit Voice(s32 id);
    ~Voice();

public:
    void AppendWaveBuffer(WaveBuffer* pBuffer);

    void SetChannelCount(s32 channelCount);
    void SetSampleFormat(SampleFormat format);
    void SetSampleRate(s32 sampleRate);
    void SetPitch(f32 pitch);
    void SetPriority(s32 priority);
    void SetState(State state);
    void SetMixParam(const MixParam& mixParam);
    void SetVolume(f32 volume);

    s32 GetPlayPosition() const;
    bool IsPlaying() const;

    bool SetupBcwav(uptr addrBcwav, WaveBuffer* pWaveBuffer0, WaveBuffer* pWaveBuffer1, Bcwav::ChannelIndex channelIndex = Bcwav::CHANNEL_INDEX_L);
};

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
    f32 CalcFsRatio(){ return (this->mSampleRate * this->mPitch) / 32728.0; }
    void CalculateDspCycle(); // big inline
    void ForceUpdateParams();
    void Initialize();
    void Pause(); // inline in cpp
    void ReleaseWaveBuffer();
    ushort SelectCoefficient(); // inline in cpp
    void SendWaveBuffer();
    void SetChannelCount(s32 channelCount){ // inline in cpp
        NN_TASSERT_(channelCount == 1 || channelCount == 2); 
        this->mSampleInfo &= 0xfffc | channelCount & 3; 
    }
    void SetFilterType(FilterType type){ this->mFilterType = type; this->mModifiedParamFlag |= 4; }
    void SetInterpolationType(InterpolationType type); // inline in cpp 
    void SetMixParam(const MixParam& mixParam); // inline in cpp
    void SetMixVolume();
    void SetPitch(f32 pitch); // inline in cpp
    void SetSampleFormat(SampleFormat format); // inline in cpp
    void SetSampleRate(s32 sampleRate); // inline cpp
    void SetState(Voice::State state);
    void SetSyncCount();
    void SetTimer();
    void SetVolume(f32 volume); //inline cpp
    void Start();
    void Stop();
    void UpdateInterpolationType(); // inline cpp
    void UpdateParams();
    void UpdateStatus(const void * ptr);
    void UpdateWaveBufferList();
    void UpdateWaveBufferStatus(ushort currentBufferId, ushort lastBufferId);
};

class VoiceManager{
public:
    bit32 mUsedVoiceBits;
    Voice* mMostPriorVoice;
    Voice* mMostInferiorVoice;
    short mAllocatedVoiceCount;
    util::SizedEnum1<VoiceDropMode> mVoiceDropMode;
    s8 pad1;
    os::CriticalSection mCriticalSection;
    u8 mVoiceBuffer[2592];
    u8 mVoiceImplBuffer[2976];
    Voice* mpVoice[24];

public:
    void AdjustVoicePlayState(s32 remain, s32 frame);
    Voice* AllocVoice(s32 priority, VoiceDropCallbackFunc callback, uptr userArg);
    void Finalize();
    void ForceUpdateParams();
    void FreeVoice(Voice* pVoice);
    Voice* GetAvaliableVoice();
    static VoiceManager& GetInstance();
    void Initialize();
    void InsertVoiceToPriorityList(Voice* pVoice, s32 priority);
    bool IsAllocated(Voice* pVoice){ return (1 << (pVoice->GetId() & 0xff) & this->mUsedVoiceBits) != 0; }
    void RemoveVoiceFromPriorityList(Voice* pVoice);
    void SetMostInferiorVoice(Voice* pVoice){ pVoice->mInferiorVoice = 0; this->mMostInferiorVoice = pVoice; }
    void SetMostPriorVoice(Voice* pVoice){ pVoice->mPriorVoice = 0; this->mMostPriorVoice = pVoice; }
    void SetPriority(Voice* pVoice, s32 priority);
    void SetVoiceDropMode(VoiceDropMode mode);
    void UpdateParams();
    void UpdateStatus(s32 id, const DspsndChannelPlayVars* pVars);
    void UpdateWaveBufferList();
    VoiceManager();
    ~VoiceManager(){ }
};

Voice* AllocVoice(s32 priority, VoiceDropCallbackFunc callback, uptr userArg);
void FreeVoice(Voice* pVoice);

void SetVoiceDropMode(VoiceDropMode mode);

namespace internal{
    extern VoiceManager sVoiceManager;
}

inline VoiceManager& VoiceManager::GetInstance(){
    return internal::sVoiceManager;
}

} // namespace CTR
} // namespace snd
} // namespace nn