// Filename: snd_DspFxDelay.cpp
//
// Project: Horizon

#include <nn/dsp.h>
#include <nn/snd.h>
#include <nn/Assert.h>

// Native
#include "snd_DspFxManager.h"

namespace nn{
namespace snd{
namespace CTR{
    
DspFxDelay::DspFxDelay(): 
    m_Buffer(0),
    m_BufferPhysical(0),
    m_BufferSize(0),
    m_IsInitialized(false),
    m_AuxBusId(AUX_BUS_NULL),
    m_IsEnabled(false),
    m_ProcessCount(0)
{
}

DspFxDelay::~DspFxDelay()
{
}

bool DspFxDelay::Initialize(uptr buffer, size_t size)
{
    if (m_IsInitialized || buffer == NULL || size == 0)
        return false;

    bool ret = this->AssignWorkBuffer(buffer, size);

    if (ret)
        m_IsInitialized = true;
    return ret;
}

void DspFxDelay::Finalize() 
{
    if(m_IsInitialized)
    {
        this->Detach();
        this->ReleaseWorkBuffer();
        m_IsInitialized = false;
    }
}

bool DspFxDelay::Attach(AuxBusId id)
{
    NN_TASSERTMSG_(m_IsInitialized,"DspFxDelay is not initialized\n");
    NN_TASSERTMSG_(m_AuxBusId, "DspFxDelay is already attached\n");
    NN_TASSERT_(id == AUX_BUS_A || id == AUX_BUS_B);

    if((!m_IsInitialized) || m_AuxBusId != AUX_BUS_NULL || (id != AUX_BUS_A && id != AUX_BUS_B))
        return false;
    if(!DspFxManager::GetInstance().Attach(DspFxManager::DSP_EFFECT_TYPE_DELAY ,id))
        return false;

    DspFxDelayParams params;
    params.delayBufferAddress = __ror(m_BufferPhysical, 16);
    params.ctrl = 2;
    bool ret = DspFxManager::GetInstance().SetDspDelayEffect(id, &params);
    if(ret)
        m_AuxBusId = id;
    return ret;
}

void DspFxDelay::ReleaseWorkBuffer()
{ 
    m_Buffer = 0; 
    m_BufferPhysical = 0; 
    m_BufferSize = 0; 
}

void DspFxDelay::Detach() 
{
    if((!m_IsInitialized) && ((m_AuxBusId != AUX_BUS_A)  && (m_AuxBusId != AUX_BUS_B)))
    {
        return;
    }
    this->Disable();
    DspFxManager::GetInstance().Detach(DspFxManager::DSP_EFFECT_TYPE_DELAY, this->m_AuxBusId);
    m_AuxBusId = AUX_BUS_NULL;
}

bool DspFxDelay::Enable(bool enable)
{
    NN_TASSERTMSG_((mAuxBusId == AUX_BUS_A || mAuxBusId == AUX_BUS_B) &&mBuffer != NULL && mBufferSize > 0, "DspFxDelay is not initialized\n");

    if ((m_AuxBusId != AUX_BUS_A) && (m_AuxBusId != AUX_BUS_B))
        return false;
    if(m_Buffer == NULL || m_BufferSize == 0)
        return false;

    DspFxDelayParams params;
    params.enable = enable;
    params.ctrl = 1;
    bool ret = DspFxManager::GetInstance().SetDspDelayEffect(m_AuxBusId,&params);

    if (ret || enable == false)
        m_IsEnabled = enable;
    if (m_IsEnabled == false)
        m_ProcessCount = Dspsnd::GetInstance().m_ProcessCount;
    return ret;
}

size_t DspFxDelay::GetRequiredMemorySize(const DspFxDelay::Param& param) 
{
    u32 delayFrames = (param.m_DelayTime * 1000) / NN_SND_USECS_PER_FRAME;
    if (delayFrames == 0)
        delayFrames = 1;

    s32 channels = param.m_IsEnableSurround ? 4 : 2;

    return (sizeof(s32) * NN_SND_SAMPLES_PER_FRAME * channels * delayFrames);
}

bool DspFxDelay::SetParam(const DspFxDelay::Param& _param)
{
    NN_TASSERTMSG_((mAuxBusId == AUX_BUS_A || mAuxBusId == AUX_BUS_B) &&mBuffer != NULL && mBufferSize > 0,"DspFxDelay is not initialized\n");

    if ((m_AuxBusId != AUX_BUS_A) && (m_AuxBusId != AUX_BUS_B))
        return false;
    if (m_Buffer == NULL || m_BufferSize == 0)
        return false;

    NN_TASSERT_(_param.m_Damping >= 0.0f && _param.m_Damping <= 1.0f);
    if (_param.m_Damping < 0.0f || _param.m_Damping > 1.0f)
        return false;

    NN_TASSERT_(param.m_FeedbackGain >= 0.0f && _param.m_FeedbackGain <= 1.0f);
    if (_param.m_FeedbackGain < 0.0f || _param.m_FeedbackGain > 1.0f)
        return false;

    DspFxDelayParams params;

    u32 delayFrames = (_param.m_DelayTime * 1000) / NN_SND_USECS_PER_FRAME;
    if (delayFrames == 0)
        delayFrames = 1;

    s32 channels = _param.m_IsEnableSurround ? 4 : 2;

    NN_TASSERTMSG_(sizeof(s32) * NN_SND_SAMPLES_PER_FRAME * channels * delayFrames <= mBufferSize,"mDelayTime is too large\n");

    if (sizeof(s32) * NN_SND_SAMPLES_PER_FRAME * channels * delayFrames > m_BufferSize)
        return false;

    params.delayFrames = delayFrames;
    params.channels = channels;

    params.delayFeedbackGain = 0x80L * _param.m_FeedbackGain;

    f32 lpf_coef = _param.m_Damping;

    if (lpf_coef > 0.95f)
        lpf_coef = 0.95f;

    f32 lpf_coef_0 = 1.f - lpf_coef;
    f32 lpf_coef_1 = lpf_coef;

    params.aLpfCoefs[0] = 0x80L * lpf_coef_0;
    params.aLpfCoefs[1] = 0x80L * lpf_coef_1;

    params.ctrl = 4;

    ::std::memset(reinterpret_cast<void*>(m_Buffer), 0, m_BufferSize);
    dsp::CTR::FlushDataCache(m_Buffer, m_BufferSize);

    return DspFxManager::GetInstance().SetDspDelayEffect(m_AuxBusId, &params);
}

}
}
}