// Filename: snd_MasterManager.cpp
//
// Project: Horizon

#include "snd_MasterManager.h"
#include <nn/snd/CTR/MPCore/snd_Api.h>
#include <nn/cfg/CTR/cfg_Api.h>
#include <nn/cfg/CTR/cfg_DetailApi.h>
#include <nn/cfg/CTR/cfg_Sound.h>
#include <nn/Assert.h>

namespace nn {
namespace snd {
namespace CTR {
namespace internal{
    CTR::MasterManager s_MasterManager;
}

void MasterManager::Initialize()
{
    if(m_Initialized) 
        return;
    m_Initialized = true;
    this->GetImpl()->Initialize();

    m_MasterVolume = 1.0f;
    m_SystemMasterVolume = 1.0f;
    m_AuxVolume[0] = 1.0f;
    m_AuxVolume[1] = 1.0f;
    m_AuxCallback[AUX_BUS_A] = NULL;
    m_AuxCallback[AUX_BUS_B] = NULL;
    m_AuxUserData[AUX_BUS_A] = 0;
    m_AuxUserData[AUX_BUS_B] = 0;
    m_AuxFrontBypass[AUX_BUS_A] = false;
    m_AuxFrontBypass[AUX_BUS_B] = false;
    m_RearRadio = 1.0f;
    m_SurroundDepth = 1.0f;
    m_ClippingMode = CLIPPING_MODE_SOFT;

    this->GetImpl()->InitializeParam();

    nn::cfg::CTR::detail::SoundSettingCfgData soundSettingCfgData;
    cfg::CTR::Initialize();
    Result res = cfg::CTR::detail::GetConfig(&soundSettingCfgData,1,0x70001);
    cfg::CTR::Finalize();

    OutputMode mode = OUTPUT_MODE_STEREO;
    if(res.IsSuccess())
    {
        nn::cfg::CTR::CfgSoundOutputMode nandMode = static_cast<nn::cfg::CTR::CfgSoundOutputMode>(soundSettingCfgData.soundOutputMode);
        if (nandMode == nn::cfg::CTR::CFG_SOUND_OUTPUT_MODE_MONO)
        {
            mode = OUTPUT_MODE_MONO;
        }
        if (nandMode == nn::cfg::CTR::CFG_SOUND_OUTPUT_MODE_STEREO)
        {
            mode = OUTPUT_MODE_STEREO;
        }
        if (nandMode == nn::cfg::CTR::CFG_SOUND_OUTPUT_MODE_SURROUND)
        {
            mode = OUTPUT_MODE_3DSURROUND;
        }
    }
    else
    {
        mode = OUTPUT_MODE_STEREO;
    }
    m_OutputMode = mode;
    this->GetImpl()->SetSoundOutputMode(mode);
    m_DroppedFrameCount = 0;
    for(int i = 0; i < AUX_BUS_NUM; i++)
    {
        m_FxSet[i].m_pFxDelay = NULL;
        m_FxSet[i].m_pFxReverb = NULL;
    }
    this->m_FxCriticalSection.Initialize();
}

void MasterManager::Finalize()
{
    if(m_Initialized)
    {
        this->GetImpl()->Finalize();
        this->m_FxCriticalSection.Finalize();
        m_Initialized = false;
    }
}

void MasterManager::AuxUserCallback(AuxBusId busId, uptr data)
{
    NN_TASSERT_((busId != AUX_BUS_A) && (busId != AUX_BUS_B));
    if(this->m_Initialized)
    {
        MasterManagerImpl::GetInstance().AuxUserCallback(busId,data);
    }
}

void MasterManager::ExecuteEffect(AuxBusId busId, uptr data)
{
    os::CriticalSection::ScopedLock lock(this->m_FxCriticalSection);

    s32* pData = reinterpret_cast<s32*>(data);
    AuxBusData auxBusData =
    {
        pData,
        pData + NN_SND_SAMPLES_PER_FRAME,
        pData + NN_SND_SAMPLES_PER_FRAME * 2,
        pData + NN_SND_SAMPLES_PER_FRAME * 3
    };
    if (m_FxSet[busId].m_pFxDelay != NULL)
    {
        this->m_FxSet[busId].m_pFxDelay->UpdateBuffer(reinterpret_cast<uptr>(&auxBusData));
    }
    else if (m_FxSet[busId].m_pFxReverb != NULL)
    {
        this->m_FxSet[busId].m_pFxReverb->UpdateBuffer(reinterpret_cast<uptr>(&auxBusData));
    }
}

void MasterManager::RegisterAuxCallback( AuxBusId busId, AuxCallback callback, uptr userData )
{
    NN_TASSERT_(busId == AUX_BUS_A || busId == AUX_BUS_B);

    m_AuxCallback[busId] = callback;
    m_AuxUserData[busId] = userData;

    GetImpl()->RegisterAuxCallback(busId, callback, userData);
}

bool MasterManager::SetEffect(AuxBusId busId, FxDelay* fx)
{
    if (fx == NULL)
    {
        return false;
    }

    this->ClearEffect(busId);

    {
        os::CriticalSection::ScopedLock lock(this->m_FxCriticalSection);
        m_FxSet[busId].m_pFxDelay = fx;
        fx->Initialize();

        this->GetImpl()->EnableFx(busId, true);
    }

    return true;
}

bool MasterManager::SetEffect(AuxBusId busId, FxReverb* fx)
{
    if (fx == NULL)
    {
        return false;
    }

    this->ClearEffect(busId);

    {
        os::CriticalSection::ScopedLock lock(this->m_FxCriticalSection);
        m_FxSet[busId].m_pFxReverb = fx;
        fx->Initialize();

        this->GetImpl()->EnableFx(busId, true);
    }

    return true;
}

void MasterManager::ClearEffect(AuxBusId busId)
{
    nn::os::CriticalSection::ScopedLock lock(this->m_FxCriticalSection);

    if (m_FxSet[busId].m_pFxDelay != NULL)
    {
        this->m_FxSet[busId].m_pFxDelay->Finalize();
    }

    if (m_FxSet[busId].m_pFxReverb != NULL)
    {
        this->m_FxSet[busId].m_pFxReverb->Finalize();
    }
    m_FxSet[busId].m_pFxDelay = NULL;
    m_FxSet[busId].m_pFxReverb = NULL;

    this->GetImpl()->EnableFx(busId, false);
}

bool MasterManager::SetSoundOutputMode(OutputMode mode)
{
    NN_TASSERT_(mode == OUTPUT_MODE_MONO ||mode == OUTPUT_MODE_STEREO ||mode == OUTPUT_MODE_3DSURROUND);

    m_OutputMode = mode;

    return GetImpl()->SetSoundOutputMode(mode);
}

void MasterManager::ClearAuxCallback(AuxBusId busId){
    m_AuxCallback[busId] = 0;
    m_AuxUserData[busId] = 0;
    GetImpl()->RegisterAuxCallback(busId, 0, 0);
}

s32 MasterManager::GetDspCycles()
{
    s32 cycle = 0xcd78;
    switch(this->GetSoundOutputMode())
    {
    case OUTPUT_MODE_MONO:
        cycle = 0xdf0c;
        break;
    case OUTPUT_MODE_STEREO:
        cycle = 0xd930;
        break;
    case OUTPUT_MODE_3DSURROUND:
        if(GetHeadphoneStatus()){
            cycle = 0x30BB0;
        }
        else{
            cycle = 0x37140;
        }
        break;
    }

    switch(m_ClippingMode)
    {
    case CLIPPING_MODE_NORMAL:
        cycle += 0x400 + 0x1DC;
        break;
    case CLIPPING_MODE_SOFT:
        cycle += 0x2000 + 0x710;
        break;
    }
    return cycle;
}

void MasterManager::GetAuxCallback(AuxBusId busId, AuxCallback* pCallback, uptr* pUserData)
{
    *pCallback = m_AuxCallback[busId];
    *pUserData = m_AuxUserData[busId];
}

void MasterManager::SetOutputBufferCount(s32 outputBufferCount)
{
    return this->GetImpl()->SetOutputBufferCount(outputBufferCount);
}

void MasterManager::SetMasterVolume(float fVolume)
{
    if(m_Initialized)
    {
        this->GetImpl()->SetMasterVolume(fVolume);
    }
}

void MasterManager::SetSurroundSpeakerPosition(SurroundSpeakerPosition pos)
{
    m_SpeakerPosition = pos;
    return this->GetImpl()->SetSurroundSpeakerPosition(pos);
}
bool MasterManager::SetSurroundDepth(f32 depth)
{
    m_SurroundDepth = depth;
    return this->GetImpl()->SetSurroundDepth(depth);
}

void MasterManager::SetIsHeadphoneConnected(bool flag)
{
    m_IsHeadsetConnected = flag;
    this->GetImpl()->SetIsHeadphoneConnected(flag);
}

OutputMode MasterManager::GetSoundOutputMode()
{
    return m_OutputMode;
}

bool MasterManager::SetClippingMode(ClippingMode mode)
{
    m_ClippingMode = mode;

    return this->GetImpl()->SetClippingMode(mode);
}

void MasterManager::UpdateDroppedSoundFrameCount()
{
    s32 frameCnt = Dspsnd::GetInstance().GetDroppedFrameCount();
    if(frameCnt >= 0 && m_DroppedFrameCount + frameCnt <= 0x7fffffff)
    {
        m_DroppedFrameCount += frameCnt;
    }
}

void MasterManager::SetAuxReturnVolume(AuxBusId busId, f32 volume)
{
    if (!m_Initialized) 
        return;
    m_AuxVolume[busId] = volume;

    this->GetImpl()->SetAuxReturnVolume(busId, volume);
}

}
}
}