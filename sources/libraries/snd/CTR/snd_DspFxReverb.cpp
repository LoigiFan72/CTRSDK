// Filename: snd_DspFxReverb.cpp
//
// Project: Horizon

#include <nn/snd/CTR/MPCore/snd_DspFxReverb.h>
#include <nn/snd/CTR/MPCore/snd_OperateMaster.h>
#include <nn/snd.h>
#include <nn/dsp.h>
#include <nn/dsp/CTR/Common/dsp_Types.h>
#include <nn/math.h>

// Native
#include "snd_DspFxManager.h"

namespace nn {
namespace snd {
namespace CTR {

DspFxReverb::FilterSize DspFxReverb::sDefaultFilterSize ={
    19 * NN_SND_SAMPLES_PER_FRAME,
    23 * NN_SND_SAMPLES_PER_FRAME,
    13 * NN_SND_SAMPLES_PER_FRAME
};

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

bool DspFxReverb::SetParam(const DspFxReverb::Param& _param) {
    NN_TASSERTMSG_((mAuxBusId == AUX_BUS_A || mAuxBusId == AUX_BUS_B) && mBuffer != NULL && mBufferSize > 0, "DspFxReverb is not initialized\n");
    if ((mAuxBusId != AUX_BUS_A) && (mAuxBusId != AUX_BUS_B)){
        return false;
    }

    if (mBuffer == NULL || mBufferSize == 0){
        return false;
    }

    NN_TASSERT_(_param.mColoration >= 0.0f && _param.mColoration <= 1.0f);
    if (_param.mColoration < 0.0f || _param.mColoration > 1.0f){
        return false;
    }

    NN_TASSERT_(_param.mDamping >= 0.0f && _param.mDamping <= 1.0f);
    if (_param.mDamping < 0.0f || _param.mDamping > 1.0f){
        return false;
    }

    NN_TASSERT_(_param.mEarlyGain >= 0.0f && _param.mEarlyGain <= 1.0f);
    if (_param.mEarlyGain < 0.0f || _param.mEarlyGain > 1.0f){
        return false;
    }

    NN_TASSERT_(_param.mFusedGain >= 0.0f && _param.mFusedGain <= 1.0f);
    if (_param.mFusedGain < 0.0f || _param.mFusedGain > 1.0f){
        return false;
    }

    DspFxReverbParams params;

    s32 earlyDelayFrames = (_param.mEarlyReflectionTime * 1000) / NN_SND_USECS_PER_FRAME;
    earlyDelayFrames = math::Max(earlyDelayFrames, 1);

    s32 preDelayFrames = (_param.mPreDelayTime * 1000) / NN_SND_USECS_PER_FRAME;
    preDelayFrames = math::Max(preDelayFrames, 1);

    s32 channels = 2;

    params.earlyDelayFrames = earlyDelayFrames;
    params.preDelayFrames = preDelayFrames;
    params.channels = channels;

    f32 fused_time_sec = static_cast<f32>(_param.mFusedTime) / 1000.f;
    NN_TASSERT_(fused_time_sec != 0.f);

    FilterSize* pFilterSize = (_param.mpFilterSize ? _param.mpFilterSize : &sDefaultFilterSize);
    params.combFrames[0] = static_cast<s32>(pFilterSize->mComb0 / NN_SND_SAMPLES_PER_FRAME);
    params.combFrames[1] = static_cast<s32>(pFilterSize->mComb1 / NN_SND_SAMPLES_PER_FRAME);

    for(s32 i = 0; i < 2; i++){
        f32 comb_coef = std::powf(10.0f, (-3.0f * static_cast<f32>(params.combFrames[i] * NN_SND_SAMPLES_PER_FRAME) / (fused_time_sec * NN_SND_HW_I2S_CLOCK_32KHZ_F32)));
        params.aCombCoefs[i] = static_cast<s32>( static_cast<f32>(0x80L) * comb_coef );
    }

    params.earlyGain = 0x80L * _param.mEarlyGain;
    params.fusedGain = 0x80L * _param.mFusedGain;

    params.allPassCoef = 0x80L * _param.mColoration;
    params.allPassFrames = pFilterSize->mAllPass / NN_SND_SAMPLES_PER_FRAME;

    f32 lpf_coef = _param.mDamping;
    if (lpf_coef > 0.95f){
        lpf_coef = 0.95f;
    }

    f32 lpf_coef_0, lpf_coef_1;
    if( _param.mUseHpfDamping == true ){
        lpf_coef_0 = lpf_coef - 1.f;
        lpf_coef_1 = -lpf_coef;
    }
    else{
        lpf_coef_0 = 1.f - lpf_coef;
        lpf_coef_1 = lpf_coef;
    }

    params.aLpfCoefs[0] = static_cast<s16>(static_cast<s32>(0x80L) * lpf_coef_0);
    params.aLpfCoefs[1] = static_cast<s16>(static_cast<s32>(0x80L) * lpf_coef_1);

    params.ctrl = 4;

    uptr buffer = mBuffer;
    size_t size = 0;
    bool ret = true;
    if (ret){
        const size_t thisSize = channels * NN_SND_SAMPLES_PER_FRAME * earlyDelayFrames * sizeof(s32);
        params.earlyDelayBufferAddress = NN_DSP_32BIT_TO_DSP(os::ConvertAddressForDevice(buffer + size, thisSize));
        size += thisSize;
        ret = (size <= mBufferSize);
    }
    if (ret){
        const size_t thisSize = channels * NN_SND_SAMPLES_PER_FRAME * preDelayFrames * sizeof(s32);
        params.preDelayBufferAddress = NN_DSP_32BIT_TO_DSP(os::ConvertAddressForDevice(buffer + size, thisSize));
        size += thisSize;
        ret = (size <= mBufferSize);
    }
    if (ret){
        for (s32 i = 0; i < 2; i++){
            const size_t thisSize = channels * NN_SND_SAMPLES_PER_FRAME * params.combFrames[i] * sizeof(s32);
            params.combBufferAddress[i] = NN_DSP_32BIT_TO_DSP(os::ConvertAddressForDevice(buffer + size, thisSize));
            size += thisSize;
            ret = (size <= mBufferSize);
        }
    }
    if (ret){
        const size_t thisSize = channels * NN_SND_SAMPLES_PER_FRAME * params.allPassFrames * sizeof(s32);
        params.allPassBufferAddress = NN_DSP_32BIT_TO_DSP(os::ConvertAddressForDevice(buffer + size, thisSize));
        size += thisSize;
        ret = (size <= mBufferSize);
    }
    NN_TASSERTMSG_(ret, "Invalid parameter (requires larger memory than allocated)\n");

    params.ctrl |= 2;

    ::std::memset(reinterpret_cast<void*>(mBuffer), 0, mBufferSize);
    nn::dsp::CTR::FlushDataCache(mBuffer, mBufferSize);

    return DspFxManager::GetInstance().SetDspReverbEffect(mAuxBusId, &params);
}

size_t DspFxReverb::GetRequiredMemorySize(const DspFxReverb::Param& _param) {
    s32 earlyDelayFrames = (_param.mEarlyReflectionTime * 1000) / NN_SND_USECS_PER_FRAME;
    earlyDelayFrames = math::Max(earlyDelayFrames, 1);

    s32 preDelayFrames = (_param.mPreDelayTime * 1000) / NN_SND_USECS_PER_FRAME;
    preDelayFrames = math::Max(preDelayFrames, 1);

    FilterSize* pFilterSize = (_param.mpFilterSize ? _param.mpFilterSize : &sDefaultFilterSize);
    s32 delayLength  = (earlyDelayFrames + preDelayFrames) * NN_SND_SAMPLES_PER_FRAME;
    s32 filterLength = pFilterSize->mComb0 + pFilterSize->mComb1 + pFilterSize->mAllPass;

    s32 channels = 2;

    return (sizeof(s32) * channels * (delayLength + filterLength));
}

bool DspFxReverb::AssignWorkBuffer(uptr buffer, size_t size) {
    NN_TASSERTMSG_(mBuffer == NULL, "DspFxReverb is already initialized\n");
    if (mBuffer){
        return false;
    }

    uptr deviceAddress = os::ConvertAddressForDevice(buffer, size);
    if(deviceAddress == NULL){
        NN_TASSERTMSG_(false, "Reverb buffer must be in device memory area\n");
        return false;
    }

    mBuffer = buffer;
    mBufferPhysical = deviceAddress;
    mBufferSize = size;

    return true;
}

void DspFxReverb::ReleaseWorkBuffer(){ 
    mBuffer = 0; 
    mBufferPhysical = 0; 
    mBufferSize = 0; 
}

bool DspFxReverb::IsBufferInUse() {
    if (mIsEnabled){
        return true;
    }
    if (!Dspsnd::GetInstance().mIsInitialized){
        return false;
    }
    s8 diff = Dspsnd::GetInstance().mProcessCount - mProcessCount;
    if (diff > 2){
        return false;
    }
    return true;
}

} // namespace CTR
} // namespace snd
} // namespace nn