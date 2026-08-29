// Filename: snd_Api.cpp
//
// Project: Horizon

#include <nn/snd.h>
#include <nn/snd/CTR/snd_Result.h>
#include <nn/srv/srv_API.h>
#include <nn/dsp/CTR/MPCore/dsp_Api.h>
#include <nn/os/ARM/os_MemoryBarrier.h>
#include <nn/os/os_Thread.h>

// Private
#include "snd_VoiceManager.h"
#include "snd_VoiceImpl.h"
#include "snd_MasterManager.h"
#include "snd_DspFxManager.h"
#include "snd_ThreadManager.h"

namespace nn{
namespace snd{
namespace CTR{
namespace{
    bool s_Initialized;
    bool s_IsSleeping;
    bool s_IsSleepPrepare;
    bool s_IsWaitingForFinalize;
    os::LightEvent s_SleepEvent;
    bool s_IsHeadphoneConnected;
    enum SyncState{
        SYNC_STATE_WAIT,
        SYNC_STATE_SEND
    };
    SyncState s_SyncState;
}

bool UpdateHeadphoneStatus(){
    s_IsHeadphoneConnected = nn::os::GetWritableSharedInfo().isHeadphoneInserted;
    MasterManager::GetInstance().SetIsHeadsetConnected(s_IsHeadphoneConnected);
    return s_IsHeadphoneConnected;
}

Result Initialize(){
    if(s_Initialized){
        return ResultAlreadyInitialized();
    }

    NN_UTIL_RETURN_IF_FAILED(srv::Initialize());
    NN_UTIL_RETURN_IF_FAILED(Dspsnd::GetInstance().Initialize(false));

    UpdateHeadphoneStatus();
    VoiceManager::GetInstance().Initialize();
    MasterManager::GetInstance().Initialize();
    DspFxManager::GetInstance().Initialize();
    dsp::CTR::RegisterSleepWakeUpCallback(Sleep,WakeUp,OrderToWaitForFinalize);
    s_IsSleeping = false;
    s_IsSleepPrepare = false;
    s_IsWaitingForFinalize = false;
    s_SyncState = SYNC_STATE_SEND;
    s_Initialized = true;
    return ResultSuccess();
}

Result Finalize(){
    if(!s_Initialized){
        return ResultSuccess();
    }

    s_Initialized = false;
    dsp::CTR::ClearSleepWakeUpCallback(Sleep,WakeUp,OrderToWaitForFinalize);
    DspFxManager::GetInstance().Finalize();
    MasterManager::GetInstance().Finalize();
    VoiceManager::GetInstance().Finalize();

    if(!s_IsWaitingForFinalize){
        Dspsnd::GetInstance().Finalize(false);
    }

    return ResultSuccess();
}

void Sleep(){
    if(s_Initialized && !s_IsSleeping){
        s_SleepEvent.Initialize(true);
        s_IsSleepPrepare = true;
        Dspsnd::GetInstance().Finalize(true);
        s_IsSleeping = true;
    }
}

void WakeUp(){
    if (s_Initialized && s_IsSleeping){
        Dspsnd::GetInstance().Initialize(true);
        UpdateHeadphoneStatus();
        s_IsSleeping = false;
        s_IsSleepPrepare = false;
        os::ARM::DataSynchronizationBarrier();
        s_SleepEvent.Signal();
    }
}

void OrderToWaitForFinalize(){
    if(s_Initialized && s_IsSleeping){
        s_IsWaitingForFinalize = true;
        s_IsSleeping = false;
        s_IsSleepPrepare = false;
        os::ARM::DataSynchronizationBarrier();
        s_SleepEvent.Signal();
    }
}

bool GetHeadphoneStatus(){
    return s_IsHeadphoneConnected;
}

f32 GetSystemMasterVolume(){
    return MasterManager::GetInstance().mMasterVolume;
}

bool SetSurroundDepth(f32 depth){
    return MasterManager::GetInstance().SetSurroundDepth(depth);
}

void SetAuxReturnVolume(AuxBusId id, f32 fVolume){
    MasterManager::GetInstance().SetAuxReturnVolume(id, fVolume);
}

void ClearEffect(AuxBusId busId){
    MasterManager::GetInstance().ClearEffect(busId);
}

void SetOutputBufferCount(s32 outputBufferCount){
    return MasterManager::GetInstance().SetOutputBufferCount(outputBufferCount);
}

void SetMasterVolume(f32 fVolume){
    return MasterManager::GetInstance().SetMasterVolume(fVolume);
}

void SetSurroundSpeakerPosition(SurroundSpeakerPosition pos){
    return MasterManager::GetInstance().SetSurroundSpeakerPosition(pos);
}

void WaitForDspSync(){
    NN_TASSERT_(sInitialized);
    NN_TASSERT_(sSyncState == SYNC_STATE_SEND);
    if (s_IsSleepPrepare){
        if (Dspsnd::GetInstance().WaitPipe(nn::fnd::TimeSpan::FromMicroSeconds(NN_SND_USECS_PER_FRAME * 2))){
            Dspsnd::GetInstance().SyncFrameData();
            s_SyncState = SYNC_STATE_WAIT;
            return;
        }
        else{
            s_IsSleeping = true;
        }
    }
    if (s_IsSleeping == true){
        s_SleepEvent.Wait();
        s_SleepEvent.ClearSignal();
        s_SleepEvent.Finalize();
    }
    if (s_IsWaitingForFinalize){
        nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMicroSeconds(NN_SND_USECS_PER_FRAME));
    }
    else{
        Dspsnd::GetInstance().WaitPipe();
        Dspsnd::GetInstance().SyncFrameData();
        s_SyncState = SYNC_STATE_WAIT;
    }
}

void SendParameterToDsp(){
    NN_TASSERT_(s_Initialized);
    if((!s_IsSleeping) && (!s_IsWaitingForFinalize)){
        if(s_SyncState){
            WaitForDspSync();
        }
        UpdateHeadphoneStatus();
        Dspsnd::GetInstance().SendParameter();
        s_SyncState = SYNC_STATE_SEND;
    }
}

void WaitForDspSync(nn::os::Tick* pTick){
    if (s_IsSleepPrepare){
        if (Dspsnd::GetInstance().WaitPipe(nn::fnd::TimeSpan::FromMicroSeconds(NN_SND_USECS_PER_FRAME*2))){
            nn::os::Tick tick = nn::os::Tick::GetSystemCurrent();
            Dspsnd::GetInstance().SyncFrameData();
            s_SyncState = SYNC_STATE_WAIT;
            *pTick = nn::os::Tick::GetSystemCurrent() - tick;
            return;
        }
        else{
            s_IsSleeping = true;
        }
    }
    if (s_IsSleeping == true){
        s_SleepEvent.Wait();
        s_SleepEvent.ClearSignal();
        s_SleepEvent.Finalize();
    }
    if (s_IsWaitingForFinalize){
        nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMicroSeconds(NN_SND_USECS_PER_FRAME));
    }
    else{
        Dspsnd::GetInstance().WaitPipe();
        nn::os::Tick tick = nn::os::Tick::GetSystemCurrent();
        Dspsnd::GetInstance().SyncFrameData();
        s_SyncState = SYNC_STATE_WAIT;
        *pTick = nn::os::Tick::GetSystemCurrent() - tick;
    }
}

Voice* AllocVoice(s32 priority, VoiceDropCallbackFunc callback, uptr userArg){
    NN_TASSERT_(s_Initialized);
    return VoiceManager::GetInstance().AllocVoice(priority,callback,userArg);
}

void FreeVoice(Voice* pVoice){
    NN_TASSERT_(s_Initialized);
    VoiceManager::GetInstance().FreeVoice(pVoice);
}

void InitializeWaveBuffer(WaveBuffer * pWaveBuffer){
    ::std::memset(pWaveBuffer, 0, sizeof(WaveBuffer));

    pWaveBuffer->status = WaveBuffer::STATUS_FREE;
}

void GetAuxCallback(AuxBusId busId, AuxCallback* pcb, uptr* pUserData){
    return MasterManager::GetInstance().GetAuxCallback(busId, pcb, pUserData);
}

Result StartSoundThread(const ThreadParameter* mainThreadParam,void (*mainThreadCallback)(uptr),uptr mainThreadArg,const ThreadParameter* userThreadParam,void (*userThreadCallback)(uptr),uptr userThreadArg,s32 coreNo){
    return ThreadManager::GetInstance().StartSoundThread(mainThreadParam, mainThreadCallback, mainThreadArg,userThreadParam, userThreadCallback, userThreadArg,coreNo);
}

void FinalizeSoundThread(){
    ThreadManager::GetInstance().FinalizeSoundThread();
}

void EnableSoundThreadTickCounter(bool enable){
    ThreadManager::GetInstance().EnableSoundThreadTickCounter(enable);
}

os::Tick GetSoundThreadTick(){
    return ThreadManager::GetInstance().GetSoundThreadTick();
}

}
}
}