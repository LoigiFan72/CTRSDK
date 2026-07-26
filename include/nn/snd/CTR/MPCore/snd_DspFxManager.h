#pragma once

#include "nn/types.h"
#include "nn/snd/CTR/MPCore/snd_DspFxDelay.h"
#include "nn/snd/CTR/MPCore/snd_DspFxReverb.h"

namespace nn{
namespace snd{
namespace CTR{

class DspFxManagerImpl;

class DspFxManager{
protected:
    bool mIsAttached[2][2];
    bool mIsEnabled[2][2];
    s8 mChannelNum[2][2];

public:
    enum DspEffectType{
        DSP_EFFECT_TYPE_DELAY = 0,
        DSP_EFFECT_TYPE_REVERB = 1,
        DSP_EFFECT_TYPE_NUM = 2
    };
public:
    void Initialize();
    DspFxManagerImpl* Finalize();
    static DspFxManager& GetInstance();
    bool Detach(DspEffectType type,AuxBusId id);
    bool Attach(DspEffectType,AuxBusId);
    s32 GetDspCycles();
    bool SetDspDelayEffect(AuxBusId id, DspFxDelayParams* param);
    bool SetDspReverbEffect(AuxBusId id, DspFxReverbParams* param);

    inline s32 GetChannelNum(DspEffectType type, AuxBusId id){
        if(this->mIsEnabled[type][id]){
            return this->mChannelNum[type][id];
        } 
        else{
            return 0;
        }
    }
};

class DspFxManagerImpl{
protected:
    DspFxDelayParams mDspFxDelayParams[2];
    DspFxReverbParams mDspFxReverbParams[2];

public:
    void Initialize();
    DspFxManagerImpl* Finalize();
    void ForceUpdateParams();
    static DspFxManagerImpl& GetInstance();
    bool SetDspDelayEffect(AuxBusId id, DspFxDelayParams* param);
    bool SetDspReverbEffect(AuxBusId id, DspFxReverbParams* param);
};
}
}
}

//extern nn::snd::CTR::DspFxManager instance;
//extern nn::snd::CTR::DspFxManagerImpl instance;