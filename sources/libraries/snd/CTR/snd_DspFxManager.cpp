// Filename: snd_DspFxManager.cpp
//
// Project: Horizon

#include "snd_DspFxManager.h"
#include <nn/snd/CTR/MPCore/snd_OperateMaster.h>

namespace nn {
namespace snd {
namespace CTR {

void DspFxManager::Initialize() {
    for (s32 i = 0; i < AUX_BUS_NUM; i++){
        for (s32 j = 0; j < DSP_EFFECT_TYPE_NUM; j++){
            AuxBusId id = static_cast<AuxBusId>(i);
            DspEffectType type = static_cast<DspEffectType>(j);

            mIsAttached[type][id] = false;
            mIsEnabled[type][id] = false;
            mChannelNum[type][id] = 0;
        }
    }
    DspFxManagerImpl::GetInstance().Initialize();
}



void DspFxManager::Finalize(){
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
    mIsAttached[type][id] = false;
    return true;
}

bool DspFxManager::Attach(DspEffectType type,AuxBusId id){
    if(mIsAttached[type][id])
        return false;
    mIsAttached[type][id] = true;
    return true;
}

s32 DspFxManager::GetDspCycles() {
    int cycle = 0;

    for(int id = 0; id < AUX_BUS_NUM; id++){
        int newNum = cycle + 10000 * this->GetChannelNum(DSP_EFFECT_TYPE_DELAY,static_cast<AuxBusId>(id));
        cycle = newNum + 40000 * this->GetChannelNum(DSP_EFFECT_TYPE_REVERB,static_cast<AuxBusId>(id));
    }
    return cycle;
}



}
}
}