// Filename: snd_DspFxManager.cpp
//
// Project: Horizon

#include <nn/snd/CTR/MPCore/snd_DspFxManager.h>
#include <nn/snd/CTR/MPCore/snd_OperateMaster.h>

namespace nn {
namespace snd {
namespace CTR {

void DspFxManager::Initialize() {
    this->mIsAttached[0][0] = false;
    this->mIsEnabled[0][0] = false;
    this->mChannelNum[0][0] = 0;
    this->mIsAttached[1][0] = false;
    this->mIsEnabled[1][0] = false;
    this->mChannelNum[1][0] = 0;
    this->mIsAttached[0][1] = false;
    this->mIsEnabled[0][1] = false;
    this->mChannelNum[0][1] = 0;
    this->mIsAttached[1][1] = false;
    this->mIsEnabled[1][1] = false;
    this->mChannelNum[1][1] = 0;
    DspFxManagerImpl::GetInstance().Initialize();
}



DspFxManagerImpl* DspFxManager::Finalize(){
    return DspFxManagerImpl::GetInstance().Finalize();
}

DspFxManager& DspFxManager::GetInstance() {
    static DspFxManager instance;
    return instance;
}

bool DspFxManagerImpl::SetDspReverbEffect(AuxBusId id, DspFxReverbParams* param) {
    return DspFxManagerImpl::SetDspReverbEffect(id,param);
}

bool DspFxManagerImpl::SetDspDelayEffect(AuxBusId id, DspFxDelayParams* param) {
    return DspFxManagerImpl::SetDspDelayEffect(id,param);
}

bool DspFxManager::Detach(DspEffectType type,AuxBusId id) {
    this->mIsAttached[type][id] = false;
    return true;
}

bool DspFxManager::Attach(DspEffectType type,AuxBusId id){
    if(this->mIsAttached[type][id])
        return false;
    this->mIsAttached[type][id] = true;
    return true;
}

s32 DspFxManager::GetDspCycles() {
    int cycle = 0;

    for(int i = 0; i < AUX_BUS_NUM; ++i){
        int newNum = cycle + 10000 * this->GetChannelNum(DSP_EFFECT_TYPE_DELAY,(AuxBusId)i);
        cycle = newNum + 40000 * this->GetChannelNum(DSP_EFFECT_TYPE_REVERB,(AuxBusId)i);
    }
    return cycle;
}



}
}
}