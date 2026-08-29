// Filename: snd_MasterManager.cpp
//
// Project: Horizon

#include "snd_MasterManager.h"
#include <nn/snd/CTR/MPCore/snd_Api.h>
#include <nn/cfg/CTR/cfg_Api.h>
#include <nn/cfg/CTR/cfg_DetailApi.h>
#include <nn/cfg/CTR/cfg_Sound.h>
#include <nn/Assert.h>

namespace nn {
namespace snd {
namespace CTR {
namespace internal{
    CTR::MasterManager s_MasterManager;
}

void MasterManager::AuxUserCallback(AuxBusId busId, uptr data){
    NN_TASSERT_((busId != AUX_BUS_A) && (busId != AUX_BUS_B));
    if(this->mInitialized){
        MasterManagerImpl::GetInstance().AuxUserCallback(busId,data);
    }
}

void MasterManager::ExecuteEffect(AuxBusId busId, uptr data){
    os::CriticalSection::ScopedLock lock(this->mFxCriticalSection);

    s32* pData = reinterpret_cast<s32*>(data);
    AuxBusData auxBusData ={
        pData,
        pData + NN_SND_SAMPLES_PER_FRAME,
        pData + NN_SND_SAMPLES_PER_FRAME * 2,
        pData + NN_SND_SAMPLES_PER_FRAME * 3
    };
    if (mFxSet[busId].mpFxDelay != NULL){
        this->mFxSet[busId].mpFxDelay->UpdateBuffer(reinterpret_cast<uptr>(&auxBusData));
    }
    else if (mFxSet[busId].mpFxReverb != NULL){
        this->mFxSet[busId].mpFxReverb->UpdateBuffer(reinterpret_cast<uptr>(&auxBusData));
    }
}

bool MasterManager::SetEffect(AuxBusId busId, FxDelay* fx){
    if (fx == NULL){
        return false;
    }

    this->ClearEffect(busId);

    {
        os::CriticalSection::ScopedLock lock(this->mFxCriticalSection);
        mFxSet[busId].mpFxDelay = fx;
        fx->Initialize();

        this->GetImpl()->EnableFx(busId, true);
    }

    return true;
}

bool MasterManager::SetEffect(AuxBusId busId, FxReverb* fx){
    if (fx == NULL){
        return false;
    }

    this->ClearEffect(busId);

    {
        os::CriticalSection::ScopedLock lock(this->mFxCriticalSection);
        mFxSet[busId].mpFxReverb = fx;
        fx->Initialize();

        this->GetImpl()->EnableFx(busId, true);
    }

    return true;
}

void MasterManager::ClearEffect(AuxBusId busId){
    nn::os::CriticalSection::ScopedLock lock(this->mFxCriticalSection);

    if (mFxSet[busId].mpFxDelay != NULL){
        this->mFxSet[busId].mpFxDelay->Finalize();
    }

    if (mFxSet[busId].mpFxReverb != NULL){
        this->mFxSet[busId].mpFxReverb->Finalize();
    }
    mFxSet[busId].mpFxDelay = NULL;
    mFxSet[busId].mpFxReverb = NULL;

    this->GetImpl()->EnableFx(busId, false);
}

void MasterManager::Finalize(){
    if(this->mInitialized){
        this->GetImpl()->Finalize();
        this->mFxCriticalSection.Finalize();
        mInitialized = false;
    }
}

s32 MasterManager::GetDspCycles(){
    s32 cycle = 0xcd78;
    switch(this->GetSoundOutputMode()){
    case OUTPUT_MODE_MONO:
        cycle = 0xdf0c;
        break;
    case OUTPUT_MODE_STEREO:
        cycle = 0xd930;
        break;
    case OUTPUT_MODE_3DSURROUND:
        if(GetHeadphoneStatus()){
            cycle = 0x30BB0;
        }
        else{
            cycle = 0x37140;
        }
        break;
    }

    switch(mClippingMode){
    case CLIPPING_MODE_NORMAL:
        cycle += 0x400 + 0x1DC;
        break;
    case CLIPPING_MODE_SOFT:
        cycle += 0x2000 + 0x710;
        break;
    }
    return cycle;
}

void MasterManager::GetAuxCallback( AuxBusId busId, AuxCallback* pCallback, uptr* pUserData ){
    *pCallback = mAuxCallback[busId];
    *pUserData = mAuxUserData[busId];
}

void MasterManager::Initialize(){
    if(mInitialized) 
        return;
    mInitialized = true;
    this->GetImpl()->Initialize();

    mMasterVolume = 1.0f;
    mSystemMasterVolume = 1.0f;
    mAuxVolume[0] = 1.0f;
    mAuxVolume[1] = 1.0f;
    mAuxCallback[AUX_BUS_A] = NULL;
    mAuxCallback[AUX_BUS_B] = NULL;
    mAuxUserData[AUX_BUS_A] = 0;
    mAuxUserData[AUX_BUS_B] = 0;
    mAuxFrontBypass[AUX_BUS_A] = false;
    mAuxFrontBypass[AUX_BUS_B] = false;
    mRearRadio = 1.0f;
    mSurroundDepth = 1.0f;
    mClippingMode = CLIPPING_MODE_SOFT;

    this->GetImpl()->InitializeParam();

    nn::cfg::CTR::detail::SoundSettingCfgData soundSettingCfgData;
    cfg::CTR::Initialize();
    Result res = cfg::CTR::detail::GetConfig(&soundSettingCfgData,1,0x70001);
    cfg::CTR::Finalize();

    OutputMode mode = OUTPUT_MODE_STEREO;
    if(res.IsSuccess()){
        nn::cfg::CTR::CfgSoundOutputMode nandMode = static_cast<nn::cfg::CTR::CfgSoundOutputMode>(soundSettingCfgData.soundOutputMode);
        if (nandMode == nn::cfg::CTR::CFG_SOUND_OUTPUT_MODE_MONO){
            mode = OUTPUT_MODE_MONO;
        }
        if (nandMode == nn::cfg::CTR::CFG_SOUND_OUTPUT_MODE_STEREO){
            mode = OUTPUT_MODE_STEREO;
        }
        if (nandMode == nn::cfg::CTR::CFG_SOUND_OUTPUT_MODE_SURROUND){
            mode = OUTPUT_MODE_3DSURROUND;
        }
    }
    else{
        mode = OUTPUT_MODE_STEREO;
    }
    mOutputMode = mode;
    this->GetImpl()->SetSoundOutputMode(mode);
    mDroppedFrameCount = 0;
    for(int i = 0; i < AUX_BUS_NUM; i++){
        mFxSet[i].mpFxDelay = NULL;
        mFxSet[i].mpFxReverb = NULL;
    }
    this->mFxCriticalSection.Initialize();
}

void MasterManager::SetOutputBufferCount(s32 outputBufferCount){
    return this->GetImpl()->SetOutputBufferCount(outputBufferCount);
}

void MasterManager::SetMasterVolume(float fVolume){
    if(mInitialized){
        this->GetImpl()->SetMasterVolume(fVolume);
    }
}

void MasterManager::SetSurroundSpeakerPosition(SurroundSpeakerPosition pos){
    mSpeakerPosition = pos;
    return this->GetImpl()->SetSurroundSpeakerPosition(pos);
}
bool MasterManager::SetSurroundDepth(f32 depth){
    mSurroundDepth = depth;
    return this->GetImpl()->SetSurroundDepth(depth);
}

void MasterManager::SetIsHeadphoneConnected(bool flag){
    mIsHeadsetConnected = flag;
    this->GetImpl()->SetIsHeadphoneConnected(flag);
}

OutputMode MasterManager::GetSoundOutputMode(){
    return mOutputMode;
}

bool MasterManager::SetClippingMode(ClippingMode mode){
    mClippingMode = mode;

    return this->GetImpl()->SetClippingMode(mode);
}

void MasterManager::UpdateDroppedSoundFrameCount(){
    s32 frameCnt = Dspsnd::GetInstance().GetDroppedFrameCount();
    if(frameCnt >= 0 && mDroppedFrameCount + frameCnt <= 0x7fffffff){
        mDroppedFrameCount += frameCnt;
    }
}

void MasterManager::SetAuxReturnVolume(AuxBusId busId, f32 volume){
    if (!mInitialized) 
        return;
    mAuxVolume[busId] = volume;

    this->GetImpl()->SetAuxReturnVolume(busId, volume);
}

}
}
}