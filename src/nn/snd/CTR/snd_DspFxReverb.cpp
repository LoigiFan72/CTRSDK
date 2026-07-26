// Filename: snd_DspFxReverb.cpp
//
// Project: Horizon Decompilation

#include <nn/snd/CTR/MPCore/snd_DspFxReverb.h>
#include <nn/snd/CTR/MPCore/snd_DspFxManager.h>
#include <nn/snd/CTR/MPCore/snd_OperateMaster.h>

namespace nn {
namespace snd {
namespace CTR {

DspFxReverb::FilterSize DspFxReverb::sDefaultFilterSize;

DspFxReverb::Param::Param(): 
    mEarlyReflectionTime(60),
    mFusedTime(4000),
    mPreDelayTime(100),
    mColoration(0.5f),
    mDamping(0.4f),
    mpFilterSize(&sDefaultFilterSize ),
    mEarlyGain(0.6f),
    mFusedGain(0.4f),
    mUseHpfDamping(false)
{}

DspFxReverb::DspFxReverb(): 
    mBuffer(NULL), 
    mBufferPhysical(NULL), 
    mBufferSize(0),
    mIsInitialized(false), 
    mAuxBusId(AUX_BUS_NULL), 
    mIsEnabled(false),
    mProcessCount(0)
{}

DspFxReverb::~DspFxReverb(){}

size_t DspFxReverb::GetRequiredMemorySize(const DspFxReverb::Param& param) {

}

bool DspFxReverb::Initialize(uptr buffer, size_t size) {
    if (mIsInitialized || buffer == NULL || size == 0)
        return false;

    bool ret = this->AssignWorkBuffer(buffer, size);

    if (ret)
        mIsInitialized = true;
    return ret;
}

void DspFxReverb::Finalize(){
    if(mIsInitialized){
        this->Detach();
        this->ReleaseWorkBuffer();
        mIsInitialized = false;
    }
}

bool DspFxReverb::Attach(AuxBusId id) {
    NN_TASSERTMSG_(mIsInitialized == true, "DspFxReverb is not initialized\n");
    NN_TASSERTMSG_(mAuxBusId == AUX_BUS_NULL, "DspFxReverb is already attached\n");
    NN_TASSERT_(id == AUX_BUS_A || id == AUX_BUS_B);

    if (!mIsInitialized || mAuxBusId != AUX_BUS_NULL || (id != AUX_BUS_A && id != AUX_BUS_B))
        return false;

    if (!DspFxManager::GetInstance().Attach(DspFxManager::DSP_EFFECT_TYPE_REVERB, id))
        return false;

    mAuxBusId = id;

    return true;
}

void DspFxReverb::Detach() {
    if((!mIsInitialized) && ((mAuxBusId != AUX_BUS_A)  && (mAuxBusId != AUX_BUS_B))){
        return;
    }
    this->Disable();
    DspFxManager::GetInstance().Detach(DspFxManager::DSP_EFFECT_TYPE_REVERB, this->mAuxBusId);
    mAuxBusId = AUX_BUS_NULL;
}

bool DspFxReverb::Enable(bool enable) {
    NN_TASSERTMSG_((mAuxBusId == AUX_BUS_A || mAuxBusId == AUX_BUS_B) &&mBuffer != NULL && mBufferSize > 0,"DspFxReverb is not initialized\n");

    if ((mAuxBusId != AUX_BUS_A) && (mAuxBusId != AUX_BUS_B))
        return false;
    if (mBuffer == NULL || mBufferSize == 0)
        return false;

    DspFxReverbParams params;
    params.enable = enable;
    params.ctrl = 1;

    bool ret = DspFxManager::GetInstance().SetDspReverbEffect(this->mAuxBusId, &params);

    if (ret || enable == false)
        mIsEnabled = enable;
    if (mIsEnabled == false)
        mProcessCount = Dspsnd::GetInstance().mProcessCount;
    return ret;
}

bool DspFxReverb::SetParam(const DspFxReverb::Param& param) {

}

bool DspFxReverb::IsBufferInUse() {

}

bool DspFxReverb::AssignWorkBuffer(uptr buffer, size_t size) {

}

} // namespace CTR
} // namespace snd
} // namespace nn