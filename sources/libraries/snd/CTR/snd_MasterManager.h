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
    bool m_Initialized;
    SizedEnum1<ClippingMode> m_ClippingMode;
    SizedEnum1<OutputMode> m_OutputMode;
    SizedEnum1<SyncMode> m_SyncMode;
    f32 m_MasterVolume;
    f32 m_SystemMasterVolume;
    f32 m_AuxVolume[2];
    AuxCallback m_AuxCallback[2];
    uptr m_AuxUserData[2];
    bool m_AuxFrontBypass[2];
    bool m_IsHeadsetConnected;
    SizedEnum1<SurroundSpeakerPosition> m_SpeakerPosition;
    u16 m_SurroundDepth;
    u16 m_RearRatio;
    s32 m_DroppedFrameCount;
    bool m_FxEnabled[2];
    u16 m_OutputBufferCount;
    CriticalSection m_CriticalSection;
public:

    MasterManagerImpl() : m_Initialized(0){ }
    ~MasterManagerImpl(){ }
    void AuxUserCallback(AuxBusId busId, uptr data);
    void ClearAuxCallback(AuxBusId busId){ return this->RegisterAuxCallback(busId, 0, 0); }
    void Finalize();
    void ForceUpdateParams();
    void EnableFx(AuxBusId busId, bool enable);
    void Initialize();
    void InitializeParam();
    void RegisterAuxCallback(AuxBusId busId, AuxCallback callback, uptr userData);
    bool SetAuxFrontBypass(AuxBusId busId, bool flag){ this->m_AuxFrontBypass[busId] = flag; return Dspsnd::GetInstance().SetAuxFrontBypass(busId, flag); }
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

class MasterManager
{
public:
    struct FxSet
    {
        FxDelay* m_pFxDelay;
        FxReverb* m_pFxReverb;
    };

    bool m_Initialized;
    s8 buf;
    ClippingMode m_ClippingMode;
    OutputMode m_OutputMode;
    f32 m_MasterVolume;
    f32 m_SystemMasterVolume;
    f32 m_AuxVolume[2];
    AuxCallback m_AuxCallback[2];
    uptr m_AuxUserData[2];
    bool m_AuxFrontBypass[2];
    bool m_IsHeadsetConnected;
    SurroundSpeakerPosition m_SpeakerPosition;
    f32 m_SurroundDepth;
    f32 m_RearRadio;
    s32 m_DroppedFrameCount;
    FxSet m_FxSet[2];
    CriticalSection m_FxCriticalSection;

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
    bool SetSoundOutputMode(OutputMode mode);
    void SetSurroundSpeakerPosition(SurroundSpeakerPosition pos);
    void SetIsHeadsetConnected(bool flag);
    void ClearAuxCallback(AuxBusId busId);
    void RegisterAuxCallback(AuxBusId busId, AuxCallback callback, uptr userData);
    void UpdateDroppedSoundFrameCount();

    MasterManagerImpl* GetImpl() { return &(MasterManagerImpl::GetInstance()); }
    static MasterManager& GetInstance(){ return internal::s_MasterManager; }
};

}
}
}