#pragma once

#include <nn/types.h>
#include <nn/snd/CTR/MPCore/snd_DspFxDelay.h>
#include <nn/snd/CTR/MPCore/snd_DspFxReverb.h>

namespace nn{
namespace snd{
namespace CTR{

class DspFxManagerImpl;

class DspFxManagerImpl{
protected:
    DspFxDelayParams mDspFxDelayParams[2];
    DspFxReverbParams mDspFxReverbParams[2];

public:
    void Initialize();
    void Finalize();
    void ForceUpdateParams();
    static DspFxManagerImpl& GetInstance();
    bool SetDspDelayEffect(AuxBusId id, DspFxDelayParams* param);
    bool SetDspReverbEffect(AuxBusId id, DspFxReverbParams* param);
};

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
    void Finalize();
    static DspFxManager& GetInstance();
    bool Detach(DspEffectType type,AuxBusId id);
    bool Attach(DspEffectType,AuxBusId);
    s32 GetDspCycles();
    bool SetDspDelayEffect(AuxBusId id, DspFxDelayParams* param);
    bool SetDspReverbEffect(AuxBusId id, DspFxReverbParams* param);
    s32 GetChannelNum(DspEffectType type, AuxBusId id);

    DspFxManagerImpl* GetImpl(){ return &DspFxManagerImpl::GetInstance(); }
};

}
}
}

//extern nn::snd::CTR::DspFxManager instance;
//extern nn::snd::CTR::DspFxManagerImpl instance;