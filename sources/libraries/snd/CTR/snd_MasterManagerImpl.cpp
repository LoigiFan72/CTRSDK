// Filename: snd_MasterManagerImpl.cpp
//
// Project: Horizon

#include "snd_MasterManager.h"
#include <nn/dsp.h>
#include <nn/math/math_Utility.h>
#include <nn/Assert.h>

namespace nn {
namespace snd {
namespace CTR {
namespace internal{
    CTR::MasterManagerImpl s_MasterManagerImpl;
}

void MasterManagerImpl::AuxUserCallback(AuxBusId busId, uptr data)
{
    NN_TASSERT_(busId == AUX_BUS_A || busId == AUX_BUS_B);

    if (!m_Initialized)
    {
        return ;
    }
    os::CriticalSection::ScopedLock lock(this->m_CriticalSection);

    if (m_AuxCallback[busId])
    {
        s32 * pData = reinterpret_cast<s32*>(data);
        AuxBusData auxBusData =
        {
            pData,
            pData + NN_SND_SAMPLES_PER_FRAME,
            pData + NN_SND_SAMPLES_PER_FRAME * 2,
            pData + NN_SND_SAMPLES_PER_FRAME * 3
        };

        m_AuxCallback[busId](&auxBusData, NN_SND_SAMPLES_PER_FRAME, m_AuxUserData[busId]);
    }
}

void MasterManagerImpl::Finalize()
{
    if(m_Initialized)
    {
        this->m_CriticalSection.Finalize();
        m_Initialized = false;
    }
}

void MasterManagerImpl::ForceUpdateParams()
{
    Dspsnd::GetInstance().SetMasterVolume(this->m_MasterVolume * this->m_SystemMasterVolume);
    for(int i = 0; i < AUX_BUS_NUM; i++)
    {
        AuxBusId busId = static_cast<AuxBusId>(i);
        Dspsnd::GetInstance().SetAuxReturnVolume(busId, this->m_AuxVolume[busId]);
        Dspsnd::GetInstance().EnableAuxBus(busId, (this->m_AuxCallback[busId] != NULL) || this->m_FxEnabled[busId]);
        Dspsnd::GetInstance().SetAuxFrontBypass(busId, this->m_AuxFrontBypass[busId]);
    }
    Dspsnd::GetInstance().SetSoundOutputMode(this->m_OutputMode);
    Dspsnd::GetInstance().SetClippingMode(this->m_ClippingMode);
    Dspsnd::GetInstance().SetSurroundDepth(this->m_SurroundDepth);
    Dspsnd::GetInstance().SetSurroundSpeakerPosition(this->m_SpeakerPosition);
    Dspsnd::GetInstance().SetRearRatio(this->m_RearRatio);
    Dspsnd::GetInstance().SetOutputBufferCount(this->m_OutputBufferCount);
    m_DroppedFrameCount = 0;

    Dspsnd::GetInstance().SetSyncMode(this->m_SyncMode);
}

void MasterManagerImpl::EnableFx(AuxBusId busId, bool enable)
{
    os::CriticalSection::ScopedLock lock(this->m_CriticalSection);
    m_FxEnabled[busId] = enable;
    if (enable)
    {
        Dspsnd::GetInstance().EnableAuxBus(busId, true);
    }
    else
    {
        Dspsnd::GetInstance().EnableAuxBus(busId, (this->m_AuxCallback[busId] != NULL));
    }
}

void MasterManagerImpl::Initialize()
{
    if(m_Initialized) return;

    this->m_CriticalSection.Initialize();
    m_Initialized = true;
}

void MasterManagerImpl::InitializeParam()
{
    this->SetMasterVolume(1.0);
    this->SetSystemMasterVolume(1.0);
    this->SetAuxReturnVolume(AUX_BUS_A,1.0);
    this->SetAuxReturnVolume(AUX_BUS_B,1.0);
    this->ClearAuxCallback(AUX_BUS_A);
    this->ClearAuxCallback(AUX_BUS_B);
    this->SetAuxFrontBypass(AUX_BUS_A,false);
    this->SetAuxFrontBypass(AUX_BUS_B,false);
    this->SetRearRatio(1.0);
    this->SetSurroundDepth(1.0);
    this->SetClippingMode(CLIPPING_MODE_SOFT);

    for(int i = 0; i < AUX_BUS_NUM; i++)
    {
        this->m_FxEnabled[i] = false;
    }

    this->SetOutputBufferCount(2);
    this->SetSyncMode(SYNC_MODE_STRICT);
}

void MasterManagerImpl::SetIsHeadsetConnected(bool flag){
    Dspsnd::GetInstance().SetIsHeadsetConnected(flag);
}

void MasterManagerImpl::RegisterAuxCallback(AuxBusId busId, AuxCallback callback, uptr userData)
{
    NN_TASSERT_(busId == AUX_BUS_A || busId == AUX_BUS_B);
    os::CriticalSection::ScopedLock lock(this->m_CriticalSection);
    this->m_AuxCallback[busId] = callback;
    this->m_AuxUserData[busId] = userData;
    if(!callback)
    {
        Dspsnd::GetInstance().EnableAuxBus(busId,this->m_FxEnabled[busId]);
    }
    else
    {
        Dspsnd::GetInstance().EnableAuxBus(busId,true);
    }
}

void MasterManagerImpl::SetAuxReturnVolume(AuxBusId busId, f32 fVolume)
{
    NN_TASSERT_(busId == AUX_BUS_A || busId == AUX_BUS_B);
    if(m_Initialized)
    {
        m_AuxVolume[busId] = fVolume;
        Dspsnd::GetInstance().SetAuxReturnVolume(busId,fVolume);
    }
}

void MasterManagerImpl::SetSurroundSpeakerPosition(SurroundSpeakerPosition pos)
{
    if(pos < 2)
    {
        m_SpeakerPosition = pos;
        Dspsnd::GetInstance().SetSurroundSpeakerPosition(pos);
    }
}

bool MasterManagerImpl::SetClippingMode(ClippingMode mode)
{
    if(dsp::CTR::IsComponentLoaded())
    {
        NN_TASSERT_(mode == CLIPPING_MODE_NORMAL || mode == CLIPPING_MODE_SOFT);
        this->m_ClippingMode = mode;
        Dspsnd::GetInstance().SetClippingMode(mode);
    }

    return false;
}

void MasterManagerImpl::SetMasterVolume(f32 fVolume)
{
    if(m_Initialized)
    {
        m_MasterVolume = fVolume;
        Dspsnd::GetInstance().SetMasterVolume(this->m_MasterVolume * this->m_SystemMasterVolume);
    }
}

void MasterManagerImpl::SetOutputBufferCount(s32 outputBufferCount)
{
    outputBufferCount = math::Max(outputBufferCount,2);
    outputBufferCount = math::Min(outputBufferCount, 3);
    m_OutputBufferCount = outputBufferCount;
    Dspsnd::GetInstance().SetOutputBufferCount(outputBufferCount);
}

bool MasterManagerImpl::SetRearRatio(f32 ratio)
{
    m_RearRatio = ((0.0 < ratio * 32768.0) * (ratio * 32768.0));
    return Dspsnd::GetInstance().SetRearRatio(this->m_RearRatio);
}

bool MasterManagerImpl::SetSoundOutputMode(OutputMode mode)
{
    if (dsp::CTR::IsComponentLoaded() == false)
    {
        return false;
    }

    NN_TASSERT_(mode == OUTPUT_MODE_MONO || mode == OUTPUT_MODE_STEREO || mode == OUTPUT_MODE_3DSURROUND);

    m_OutputMode = mode;
    return Dspsnd::GetInstance().SetSoundOutputMode(mode);
}

bool MasterManagerImpl::SetSurroundDepth(f32 depth)
{
    if(depth < 0.0) depth = 0.0;
    if(depth != 1.0 && depth < 1.0 == (depth)) depth = 1.0;
    this->m_SurroundDepth = (0.0 < depth * 32767.0) * (depth * 32767.0);
    return Dspsnd::GetInstance().SetSurroundDepth(this->m_SurroundDepth);
}

void MasterManagerImpl::SetSyncMode(SyncMode mode)
{
    this->m_SyncMode = mode;
    Dspsnd::GetInstance().SetSyncMode(mode);
}

void MasterManagerImpl::SetSystemMasterVolume(f32 volume)
{
    if(m_Initialized){
        m_SystemMasterVolume = volume;
        Dspsnd::GetInstance().SetMasterVolume(this->m_MasterVolume * this->m_SystemMasterVolume);
    }
}

}
}
}