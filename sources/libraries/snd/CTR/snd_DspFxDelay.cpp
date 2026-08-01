// Filename: snd_DspFxDelay.cpp
//
// Project: Horizon

#include <nn/dsp.h>
#include <nn/snd.h>
#include <nn/Assert.h>

namespace nn{
namespace snd{
namespace CTR{
    
DspFxDelay::DspFxDelay(): 
    mBuffer(0),
    mBufferPhysical(0),
    mBufferSize(0),
    mIsInitialized(false),
    mAuxBusId(AUX_BUS_NULL),
    mIsEnabled(false),
    mProcessCount(0)
{}

DspFxDelay::~DspFxDelay(){ }

bool DspFxDelay::Initialize(uptr buffer, size_t size){
    if (mIsInitialized || buffer == NULL || size == 0)
        return false;

    bool ret = this->AssignWorkBuffer(buffer, size);

    if (ret)
        mIsInitialized = true;
    return ret;
}

void DspFxDelay::Finalize() {
    if(mIsInitialized){
        this->Detach();
        this->ReleaseWorkBuffer();
        mIsInitialized = false;
    }
}

bool DspFxDelay::Attach(AuxBusId id){
    NN_TASSERTMSG_(mIsInitialized,"DspFxDelay is not initialized\n");
    NN_TASSERTMSG_(mAuxBusId, "DspFxDelay is already attached\n");
    NN_TASSERT_(id == AUX_BUS_A || id == AUX_BUS_B);

    if((!mIsInitialized) || mAuxBusId != AUX_BUS_NULL || (id != AUX_BUS_A && id != AUX_BUS_B))
        return false;
    if(!DspFxManager::GetInstance().Attach(DspFxManager::DSP_EFFECT_TYPE_DELAY ,id))
        return false;

    DspFxDelayParams params;
    params.delayBufferAddress = __ror(mBufferPhysical, 16);
    params.ctrl = 2;
    bool ret = DspFxManager::GetInstance().SetDspDelayEffect(id, &params);
    if(ret)
        mAuxBusId = id;
    return ret;
}

void DspFxDelay::ReleaseWorkBuffer(){ 
    mBuffer = 0; 
    mBufferPhysical = 0; 
    mBufferSize = 0; 
}

void DspFxDelay::Detach() {
    if((!mIsInitialized) && ((mAuxBusId != AUX_BUS_A)  && (mAuxBusId != AUX_BUS_B))){
        return;
    }
    this->Disable();
    DspFxManager::GetInstance().Detach(DspFxManager::DSP_EFFECT_TYPE_DELAY, this->mAuxBusId);
    mAuxBusId = AUX_BUS_NULL;
}

bool DspFxDelay::Enable(bool enable){
    NN_TASSERTMSG_((mAuxBusId == AUX_BUS_A || mAuxBusId == AUX_BUS_B) &&mBuffer != NULL && mBufferSize > 0, "DspFxDelay is not initialized\n");

    if ((mAuxBusId != AUX_BUS_A) && (mAuxBusId != AUX_BUS_B))
        return false;
    if(mBuffer == NULL || mBufferSize == 0)
        return false;

    DspFxDelayParams params;
    params.enable = enable;
    params.ctrl = 1;
    bool ret = DspFxManager::GetInstance().SetDspDelayEffect(mAuxBusId,&params);

    if (ret || enable == false)
        mIsEnabled = enable;
    if (mIsEnabled == false)
        mProcessCount = Dspsnd::GetInstance().mProcessCount;
    return ret;
}

size_t DspFxDelay::GetRequiredMemorySize(const DspFxDelay::Param& param) {
    u32 delayFrames = (param.mDelayTime * 1000) / NN_SND_USECS_PER_FRAME;
    if (delayFrames == 0)
        delayFrames = 1;

    s32 channels = param.mIsEnableSurround ? 4 : 2;

    return (sizeof(s32) * NN_SND_SAMPLES_PER_FRAME * channels * delayFrames);
}

bool DspFxDelay::SetParam(const DspFxDelay::Param& param){
    NN_TASSERTMSG_((mAuxBusId == AUX_BUS_A || mAuxBusId == AUX_BUS_B) &&mBuffer != NULL && mBufferSize > 0,"DspFxDelay is not initialized\n");

    if ((mAuxBusId != AUX_BUS_A) && (mAuxBusId != AUX_BUS_B))
        return false;
    if (mBuffer == NULL || mBufferSize == 0)
        return false;

    NN_TASSERT_(param.mDamping >= 0.0f && param.mDamping <= 1.0f);
    if (param.mDamping < 0.0f || param.mDamping > 1.0f)
        return false;

    NN_TASSERT_(param.mFeedbackGain >= 0.0f && param.mFeedbackGain <= 1.0f);
    if (param.mFeedbackGain < 0.0f || param.mFeedbackGain > 1.0f)
        return false;

    DspFxDelayParams params;

    u32 delayFrames = (param.mDelayTime * 1000) / NN_SND_USECS_PER_FRAME;
    if (delayFrames == 0)
        delayFrames = 1;

    s32 channels = param.mIsEnableSurround ? 4 : 2;

    NN_TASSERTMSG_(sizeof(s32) * NN_SND_SAMPLES_PER_FRAME * channels * delayFrames <= mBufferSize,"mDelayTime is too large\n");

    if (sizeof(s32) * NN_SND_SAMPLES_PER_FRAME * channels * delayFrames > mBufferSize)
        return false;

    params.delayFrames = delayFrames;
    params.channels = channels;

    params.delayFeedbackGain = 0x80L * param.mFeedbackGain;

    f32 lpf_coef = param.mDamping;

    if (lpf_coef > 0.95f)
        lpf_coef = 0.95f;

    f32 lpf_coef_0 = 1.f - lpf_coef;
    f32 lpf_coef_1 = lpf_coef;

    params.aLpfCoefs[0] = 0x80L * lpf_coef_0;
    params.aLpfCoefs[1] = 0x80L * lpf_coef_1;

    params.ctrl = 4;

    ::std::memset(reinterpret_cast<void*>(mBuffer), 0, mBufferSize);
    dsp::CTR::FlushDataCache(mBuffer, mBufferSize);

    return DspFxManager::GetInstance().SetDspDelayEffect(mAuxBusId, &params);
}

}
}
}