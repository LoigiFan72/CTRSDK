#pragma once

#include <nn/snd/CTR/MPCore/snd_FxDelay.h>
#include <nn/snd/CTR/MPCore/snd_FxReverb.h>
#include <nn/snd/CTR/MPCore/snd_OperateMaster.h>
#include <nn/os.h>
#include <nn/math.h>
#include <nn/util.h>

#include <nn/snd/CTR/MPCore/snd_OperateMaster.h>

namespace nn{
namespace snd{
namespace CTR{

class MasterManager;
class MasterManagerImpl;

namespace internal{
    extern CTR::MasterManager s_MasterManager;
    extern CTR::MasterManagerImpl s_MasterManagerImpl;
}

class MasterManagerImpl{
public:
    bool mInitialized;
    SizedEnum1<ClippingMode> mClippingMode;
    SizedEnum1<OutputMode> mOutputMode;
    SizedEnum1<SyncMode> mSyncMode;
    f32 mMasterVolume;
    f32 mSystemMasterVolume;
    f32 mAuxVolume[2];
    AuxCallback mAuxCallback[2];
    uptr mAuxUserData[2];
    bool mAuxFrontBypass[2];
    bool mIsHeadsetConnected;
    SizedEnum1<SurroundSpeakerPosition> mSpeakerPosition;
    u16 mSurroundDepth;
    u16 mRearRatio;
    s32 mDroppedFrameCount;
    bool mFxEnabled[2];
    u16 mOutputBufferCount;
    CriticalSection mCriticalSection;
public:

    MasterManagerImpl() : mInitialized(0){ }
    ~MasterManagerImpl(){ }
    void AuxUserCallback(AuxBusId busId, uptr data);
    void ClearAuxCallback(AuxBusId busId){ return this->RegisterAuxCallback(busId, 0, 0); }
    void Finalize();
    void ForceUpdateParams();
    void EnableFx(AuxBusId busId, bool enable);
    void Initialize();
    void InitializeParam();
    void RegisterAuxCallback(AuxBusId busId, AuxCallback callback, uptr userData);
    bool SetAuxFrontBypass(AuxBusId busId, bool flag){ this->mAuxFrontBypass[busId] = flag; return Dspsnd::GetInstance().SetAuxFrontBypass(busId, flag); }
    void SetAuxReturnVolume(AuxBusId busId, f32 fVolume);
    bool SetClippingMode(ClippingMode mode);
    void SetIsHeadphoneConnected(bool flag){ internal::sDspsnd.SetIsHeadsetConnected(flag); }
    void SetMasterVolume(f32 fVolume);
    void SetOutputBufferCount(s32 outputBufferCount);
    bool SetRearRatio(f32 ratio);
    bool SetSoundOutputMode(OutputMode mode);
    bool SetSurroundDepth(f32 depth);
    void SetSyncMode(SyncMode mode);
    void SetSystemMasterVolume(f32 volume);
    void SetSurroundSpeakerPosition(SurroundSpeakerPosition pos);
    void SetIsHeadsetConnected(bool flag);
    static MasterManagerImpl& GetInstance(){ return internal::s_MasterManagerImpl; }
};

class MasterManager{
public:
    struct FxSet{
        FxDelay* mpFxDelay;
        FxReverb* mpFxReverb;
    };

    bool mInitialized;
    s8 buf;
    ClippingMode mClippingMode;
    OutputMode mOutputMode;
    f32 mMasterVolume;
    f32 mSystemMasterVolume;
    f32 mAuxVolume[2];
    AuxCallback mAuxCallback[2];
    uptr mAuxUserData[2];
    bool mAuxFrontBypass[2];
    bool mIsHeadsetConnected;
    SurroundSpeakerPosition mSpeakerPosition;
    f32 mSurroundDepth;
    f32 mRearRadio;
    s32 mDroppedFrameCount;
    FxSet mFxSet[2];
    CriticalSection mFxCriticalSection;

    MasterManager(){ }
    ~MasterManager(){ }
    
    void AuxUserCallback(AuxBusId busId, uptr data);
    void GetAuxCallback(AuxBusId busId, AuxCallback* pCallback, uptr* pUserData);
    void ClearEffect(AuxBusId busId);
    void ExecuteEffect(AuxBusId busId, uptr data);
    bool SetEffect(AuxBusId busId, FxDelay* fx);
    bool SetEffect(AuxBusId busId, FxReverb* fx);
    void Finalize();
    s32 GetDspCycles();
    void Initialize();
    OutputMode GetSoundOutputMode(void);
    void SetIsHeadphoneConnected(bool flag);
    bool SetClippingMode(ClippingMode mode);
    void SetAuxReturnVolume(AuxBusId busId, f32 volume);
    void SetMasterVolume(float fVolume);
    void SetOutputBufferCount(s32 outputBufferCount);
    bool SetSurroundDepth(f32 depth);
    void UpdateDroppedSoundFrameCount();
    void SetSurroundSpeakerPosition(SurroundSpeakerPosition pos);
    void SetIsHeadsetConnected(bool flag);

    MasterManagerImpl* GetImpl() { return &(MasterManagerImpl::GetInstance()); }
    static MasterManager& GetInstance(){ return internal::s_MasterManager; }
};

}
}
}