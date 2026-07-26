// Filename: snd_Api.cpp
//
// Project: Horizon

#include <nn/snd.h>
#include <nn/snd/CTR/snd_Result.h>
#include <nn/srv/srv_API.h>
#include <nn/dsp/CTR/MPCore/dsp_Api.h>
#include <nn/os/ARM/os_MemoryBarrier.h>
#include <nn/os/os_Thread.h>

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
    if((sInitialized & ~ sIsSleep) != 0){
        sSleepEvent.Initialize(true);
        sIsSleepPrepare = true;
        internal::sDspsnd.Finalize(true);
        sIsSleep = true;
    }
}

void OrderToWaitForFinalize(){
    if((sInitialized) && (sIsSleep)){
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

Voice* AllocVoice(s32 priority, VoiceDropCallbackFunc callback, uptr userArg){
    NN_TASSERT_(sInitialized);
    VoiceManager::GetInstance().AllocVoice(priority,callback,userArg);
}

void FreeVoice(Voice* pVoice){
    NN_TASSERT_(sInitialized);
    VoiceManager::GetInstance().FreeVoice(pVoice);
}

}
}
}