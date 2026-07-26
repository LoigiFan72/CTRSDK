// Filename: snd_DspFxManagerImpl.cpp
//
// Project: Horizon

#include <nn/snd/CTR/MPCore/snd_DspFxManager.h>
#include <nn/snd/CTR/MPCore/snd_OperateMaster.h>

namespace nn {
namespace snd {
namespace CTR {
namespace{
    enum{
        CTRL_ENABLE     =   1,
        CTRL_ADDRESSES  =   2,
        CTRL_COEFS      =   4
    };
}

void DspFxManagerImpl::Initialize() {
    DspFxReverbParams params;

    for(int i = 0; i < AUX_BUS_NUM; i++){
        params.combFrames[1] = 0;
        params.combFrames[0] = 1;
        this->SetDspDelayEffect((AuxBusId)i,(DspFxDelayParams*)&params);
        params.enable = 0;
        params.ctrl = 1;
        this->SetDspReverbEffect((AuxBusId)i,&params);
    }
}

DspFxManagerImpl* DspFxManagerImpl::Finalize(){ }

void DspFxManagerImpl::ForceUpdateParams() {
    for(int i = 0; i < AUX_BUS_NUM; i++){
        AuxBusId busId = (AuxBusId)i;
        mDspFxDelayParams[busId].ctrl= 0xffff;
        Dspsnd::GetInstance().SetDspDelayEffect(busId, &this->mDspFxDelayParams[busId]);
        mDspFxReverbParams[busId].ctrl = 0xffff;
        Dspsnd::GetInstance().SetDspReverbEffect(busId, &this->mDspFxReverbParams[busId]);
    }
}

DspFxManagerImpl& DspFxManagerImpl::GetInstance() {
    static DspFxManagerImpl instance;
    return instance;
}

bool DspFxManagerImpl::SetDspDelayEffect(AuxBusId id, DspFxDelayParams* param) {
    if((param->ctrl & CTRL_ENABLE) != 0){
        mDspFxDelayParams[id].enable = param->enable;
    }
    if((param->ctrl & CTRL_COEFS) != 0){
        mDspFxDelayParams[id].channels = param->channels;
        mDspFxDelayParams[id].delayFrames = param->delayFrames;
        mDspFxDelayParams[id].delayFeedbackGain = param->delayFeedbackGain;

        for(int i = 0; i < AUX_BUS_NUM; i += 1){
            mDspFxDelayParams[id].aLpfCoefs[i] = param->aLpfCoefs[i];
        }

    }
    if((param->ctrl & CTRL_ADDRESSES) != 0){
        mDspFxDelayParams[id].delayBufferAddress = param->delayBufferAddress;
    }
    return Dspsnd::GetInstance().SetDspDelayEffect(id,param);
}

bool DspFxManagerImpl::SetDspReverbEffect(AuxBusId id, DspFxReverbParams* param) {
    if((param->ctrl & CTRL_ENABLE) != 0){
        this->mDspFxReverbParams[id].enable = param->enable;
    }
    if((param->ctrl & CTRL_COEFS) != 0){
        mDspFxReverbParams[id].channels = param->channels;
        mDspFxReverbParams[id].earlyDelayFrames = param->earlyDelayFrames;
        mDspFxReverbParams[id].preDelayFrames = param->preDelayFrames;

        for(int i = 0; i < AUX_BUS_NUM; i++){
            mDspFxReverbParams[id].combFrames[i] = param->combFrames[i];
        }

        mDspFxReverbParams[id].allPassFrames = param->allPassFrames;
        mDspFxReverbParams[id].earlyGain = param->earlyGain;
        mDspFxReverbParams[id].fusedGain = param->fusedGain;
        mDspFxReverbParams[id].allPassCoef = param->allPassCoef;

        for(int coefi = 0; coefi < AUX_BUS_NUM; coefi++){
            mDspFxReverbParams[id].aCombCoefs[coefi] = param->aCombCoefs[coefi];
        }

        for(int lpfco = 0; lpfco < AUX_BUS_NUM; lpfco++){
            mDspFxReverbParams[id].aLpfCoefs[lpfco] = param->aLpfCoefs[lpfco];
        }
    }
    if((param->ctrl & CTRL_ADDRESSES) != 0){
        mDspFxReverbParams[id].earlyDelayBufferAddress = param->earlyDelayBufferAddress;
        mDspFxReverbParams[id].preDelayBufferAddress = param->preDelayBufferAddress;
        mDspFxReverbParams[id].combBufferAddress[0] = param->combBufferAddress[0];
        mDspFxReverbParams[id].combBufferAddress[1] = param->combBufferAddress[1];
        mDspFxReverbParams[id].allPassBufferAddress = param->allPassBufferAddress;
    }
    Dspsnd::GetInstance().SetDspReverbEffect(id,param);
}

}
}
}