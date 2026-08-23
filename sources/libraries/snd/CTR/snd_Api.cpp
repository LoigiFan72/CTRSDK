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
    bool sInitialized;
    bool sIsSleep;
    bool sIsSleepPrepare;
    bool sIsWaitingForFinalize;
    os::LightEvent sSleepEvent;
    bool sIsHeadphoneConnected;
    enum SyncState{
        SYNC_STATE_WAIT,
        SYNC_STATE_SEND
    };
    SyncState sSyncState;
}

bool UpdateHeadphoneStatus(){
    sIsHeadphoneConnected = nn::os::GetWritableSharedInfo().isHeadphoneInserted;
    MasterManager::GetInstance().SetIsHeadsetConnected(sIsHeadphoneConnected);
    return sIsHeadphoneConnected;
}

Result Initialize(){
    if(sInitialized){
        return ResultAlreadyInitialized();
    }

    NN_UTIL_RETURN_IF_FAILED(srv::Initialize());
    NN_UTIL_RETURN_IF_FAILED(Dspsnd::GetInstance().Initialize(false));

    UpdateHeadphoneStatus();
    VoiceManager::GetInstance().Initialize();
    MasterManager::GetInstance().Initialize();
    DspFxManager::GetInstance().Initialize();
    dsp::CTR::RegisterSleepWakeUpCallback(Sleep,WakeUp,OrderToWaitForFinalize);
    sIsSleep = false;
    sIsSleepPrepare = false;
    sIsWaitingForFinalize = false;
    sSyncState = SYNC_STATE_SEND;
    sInitialized = true;
    return ResultSuccess();
}

Result Finalize(){
    if(!sInitialized){
        return ResultSuccess();
    }

    sInitialized = false;
    dsp::CTR::ClearSleepWakeUpCallback(Sleep,WakeUp,OrderToWaitForFinalize);
    DspFxManager::GetInstance().Finalize();
    MasterManager::GetInstance().Finalize();
    VoiceManager::GetInstance().Finalize();

    if(!sIsWaitingForFinalize){
        Dspsnd::GetInstance().Finalize(false);
    }

    return ResultSuccess();
}

void Sleep(){
    if(sInitialized && !sIsSleep){
        sSleepEvent.Initialize(true);
        sIsSleepPrepare = true;
        Dspsnd::GetInstance().Finalize(true);
        sIsSleep = true;
    }
}

void WakeUp(){
    if (sInitialized && sIsSleep){
        Dspsnd::GetInstance().Initialize(true);
        UpdateHeadphoneStatus();
        sIsSleep = false;
        sIsSleepPrepare = false;
        os::ARM::DataSynchronizationBarrier();
        sSleepEvent.Signal();
    }
}

void OrderToWaitForFinalize(){
    if(sInitialized && sIsSleep){
        sIsWaitingForFinalize = true;
        sIsSleep = false;
        sIsSleepPrepare = false;
        os::ARM::DataSynchronizationBarrier();
        sSleepEvent.Signal();
    }
}

bool GetHeadphoneStatus(){
    return sIsHeadphoneConnected;
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
    if (sIsSleepPrepare){
        if (Dspsnd::GetInstance().WaitPipe(nn::fnd::TimeSpan::FromMicroSeconds(NN_SND_USECS_PER_FRAME * 2))){
            Dspsnd::GetInstance().SyncFrameData();
            sSyncState = SYNC_STATE_WAIT;
            return;
        }
        else{
            sIsSleep = true;
        }
    }
    if (sIsSleep == true){
        sSleepEvent.Wait();
        sSleepEvent.ClearSignal();
        sSleepEvent.Finalize();
    }
    if (sIsWaitingForFinalize){
        nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMicroSeconds(NN_SND_USECS_PER_FRAME));
    }
    else{
        Dspsnd::GetInstance().WaitPipe();
        Dspsnd::GetInstance().SyncFrameData();
        sSyncState = SYNC_STATE_WAIT;
    }
}

void SendParameterToDsp(){
    NN_TASSERT_(sInitialized);
    if((!sIsSleep) && (!sIsWaitingForFinalize)){
        if(sSyncState){
            WaitForDspSync();
        }
        UpdateHeadphoneStatus();
        Dspsnd::GetInstance().SendParameter();
        sSyncState = SYNC_STATE_SEND;
    }
}

void WaitForDspSync(nn::os::Tick* pTick){
    if (sIsSleepPrepare){
        if (Dspsnd::GetInstance().WaitPipe(nn::fnd::TimeSpan::FromMicroSeconds(NN_SND_USECS_PER_FRAME*2))){
            nn::os::Tick tick = nn::os::Tick::GetSystemCurrent();
            Dspsnd::GetInstance().SyncFrameData();
            sSyncState = SYNC_STATE_WAIT;
            *pTick = nn::os::Tick::GetSystemCurrent() - tick;
            return;
        }
        else{
            sIsSleep = true;
        }
    }
    if (sIsSleep == true){
        sSleepEvent.Wait();
        sSleepEvent.ClearSignal();
        sSleepEvent.Finalize();
    }
    if (sIsWaitingForFinalize){
        nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMicroSeconds(NN_SND_USECS_PER_FRAME));
    }
    else{
        Dspsnd::GetInstance().WaitPipe();
        nn::os::Tick tick = nn::os::Tick::GetSystemCurrent();
        Dspsnd::GetInstance().SyncFrameData();
        sSyncState = SYNC_STATE_WAIT;
        *pTick = nn::os::Tick::GetSystemCurrent() - tick;
    }
}

Voice* AllocVoice(s32 priority, VoiceDropCallbackFunc callback, uptr userArg){
    NN_TASSERT_(sInitialized);
    return VoiceManager::GetInstance().AllocVoice(priority,callback,userArg);
}

void FreeVoice(Voice* pVoice){
    NN_TASSERT_(sInitialized);
    VoiceManager::GetInstance().FreeVoice(pVoice);
}

void InitializeWaveBuffer(WaveBuffer * pWaveBuffer){
#if 0
    NN_TASSERT_(pWaveBuffer->status != WaveBuffer::STATUS_WAIT && pWaveBuffer->status != WaveBuffer::STATUS_PLAY);
#endif

    ::std::memset(pWaveBuffer, 0, sizeof(WaveBuffer) );

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