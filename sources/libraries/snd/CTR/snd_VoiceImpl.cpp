// Filename: snd_VoiceImpl.cpp
//
// Project: Horizon

#include <string.h>

#include <nn/Result.h>
#include <nn/os.h>
#include <nn/snd.h>
#include <nn/math.h>

#include <nn/snd/CTR/MPCore/snd_Voice.h>
#include <nn/snd/CTR/MPCore/snd_OperateMaster.h>
#include <nn/os/ARM/os_MemoryBarrier.h>

#include "snd_VoiceImpl.h"
#include "snd_VoiceManager.h"

namespace nn{
namespace snd{
namespace CTR{
namespace{

WaveBuffer* SearchPlayingBuffer(ushort currentBufferId, ushort lastBufferId, WaveBuffer* pWaveBuffer, short& sentBufferCount)
{
    s32 nBuffersToBeReleased = 0;
    WaveBuffer* pBuffersToBeReleased[1 + NN_SND_NEXT_BUFFER_NUM];
    while (sentBufferCount)
    {
        if (currentBufferId == pWaveBuffer->bufferId)
        {
            pWaveBuffer->status = WaveBuffer::STATUS_PLAY;
            break;
        }
        else
        {
            if (--sentBufferCount)
            {
                NN_NULL_TASSERT_(pWaveBuffer->next);
            }
            pBuffersToBeReleased[nBuffersToBeReleased++] = pWaveBuffer;
            bool exit = ((currentBufferId == 0) && (lastBufferId == pWaveBuffer->bufferId || lastBufferId == 0));
            pWaveBuffer = pWaveBuffer->next;
            if (exit) break;
        }
    }

    if (currentBufferId == 0)
    {
        sentBufferCount = NULL;
    }

    NN_TASSERT_(nBuffersToBeReleased <= 1 + NN_SND_NEXT_BUFFER_NUM);
    os::ARM::DataMemoryBarrier();
    for (s32 i = 0; i < nBuffersToBeReleased; i++)
    {
        pBuffersToBeReleased[i]->status = WaveBuffer::STATUS_DONE;
    }

    return pWaveBuffer;
}

}

void VoiceImpl::AppendWaveBuffer(WaveBuffer* pBuffer)
{
    NN_TASSERT_(buffer->status == WaveBuffer::STATUS_FREE);
    NN_NULL_TASSERT_(buffer->bufferAddress);
    NN_TASSERTMSG_(reinterpret_cast<uptr>(pBuffer->bufferAddress) >= nn::os::GetDeviceMemoryAddress() && reinterpret_cast<uptr>(pBuffer->bufferAddress) < nn::os::GetDeviceMemoryAddress() + nn::os::GetDeviceMemorySize(), "pBuffer->bufferAddress must be in device memory area.");

    if (pBuffer->sampleLength == 0)
    {
        pBuffer->status = WaveBuffer::STATUS_DONE;
        return;
    }

    pBuffer->next = NULL;
    pBuffer->status = WaveBuffer::STATUS_WAIT;

    {
        os::InterCoreCriticalSection::ScopedLock lock(this->m_CriticalSection);

        WaveBuffer* pWaveBuffer = m_pWaveBuffer;

        if (pWaveBuffer)
        {
            NN_TASSERT_(pWaveBuffer != pBuffer);
            while (pWaveBuffer->next)
            {
                pWaveBuffer = pWaveBuffer->next;
                NN_TASSERT_(pWaveBuffer != pBuffer);
            }
            pWaveBuffer->next = pBuffer;
        }
        else
        {
            m_pWaveBuffer = pBuffer;
        }

        if (m_BufferId == 0) m_BufferId++;
        pBuffer->bufferId = m_BufferId++;
    }
}

void VoiceImpl::SetBiquadFilterCoefficients(const BiquadFilterCoefficients& coeff)
{
    m_BiquadFilterCoeffs = coeff;
    m_ModifiedParamFlag |= 16;
}

void VoiceImpl::SetMonoFilterCoefficients(const MonoFilterCoefficients& coeff)
{
    m_MonoFilterCoeffs = coeff;
    m_ModifiedParamFlag |= 8;
}

void VoiceImpl::SetFilterType(FilterType type)
{ 
    m_FilterType = type; 
    m_ModifiedParamFlag |= 4;
}

void VoiceImpl::SetFrontBypassFlag(bool flag)
{
    DspsndAudioInfo* pSampleInfo = reinterpret_cast<DspsndAudioInfo*>((u16*)&this->m_SampleInfo);
    pSampleInfo->isFrontBypass = flag;
}

void VoiceImpl::SetChannelCount(s32 channelCount)
{
    NN_TASSERT_(channelCount == 1 || channelCount == 2); 
    m_SampleInfo &= 0xfffc | channelCount & 3; 
}

f32 VoiceImpl::CalcFsRatio()
{ 
    return (m_SampleRate * m_Pitch) / 32728.0;
}

s32 VoiceImpl::GetCycle() const
{
    return m_DspCycles;
}

s32 VoiceImpl::GetPlayPosition() const
{
    return m_PlayPosition;
}

void VoiceImpl::CalculateDspCycle()
{
    m_ModifiedParamFlag = -1;
    this->UpdateParams();
}

void VoiceImpl::ForceUpdateParams()
{
    m_ModifiedParamFlag = 0xffff;
    this->UpdateParams();
}

void VoiceImpl::Initialize()
{
    m_State = Voice::STATE_PAUSE;
    m_Playing = false;
    m_PlayPosition = 0;
    m_IsFirstWaveBufferForAdpcm = false;
    m_WaveBufferModifiedFlag = 0;

    m_SampleInfo &= 0xfffc | 1;
    m_SampleInfo &= 0xfff3 | 4;
    m_SampleInfo &= 0xffef;
    m_SampleInfo &= 0xffdf;
    m_SampleInfo &= 0xffbf;

    this->SetVolume(1.0);
    MixParam mixParam;
    this->SetMixParam(mixParam);
    this->SetSampleRate(0x7fd8);
    this->SetPitch(1.0);
    this->SetInterpolationType(INTERPOLATION_TYPE_POLYPHASE);
    this->SetFilterType(FILTER_TYPE_NONE);
    memset(&this->m_MonoFilterCoeffs,0,4);
    memset(&this->m_BiquadFilterCoeffs,0,10);
    m_DspCycles = 0;
    m_pWaveBuffer = NULL;
    m_SentBufferCount = 0;
    m_NextBufferIndex = 0;
    m_BufferId = 0;
}

void VoiceImpl::ReleaseWaveBuffer()
{
    {
        os::InterCoreCriticalSection::ScopedLock lock(m_CriticalSection);

        WaveBuffer* pWaveBuffer = m_pWaveBuffer;

        while (pWaveBuffer)
        {
            pWaveBuffer->status = WaveBuffer::STATUS_DONE;
            pWaveBuffer = pWaveBuffer->next;
        }

        m_pWaveBuffer = NULL;
        m_SentBufferCount = 0;
        m_NextBufferIndex = 0;
    }

    m_SyncCount++;
    m_ModifiedParamFlag |= 0x8000;
}

void VoiceImpl::SendWaveBuffer()
{
    os::InterCoreCriticalSection::ScopedLock lock(this->m_CriticalSection);

    if (m_WaveBufferModifiedFlag)
    {
        Dspsnd::GetInstance().ResetChannelNextBuffer(this->m_Id);

        if (m_pWaveBuffer && m_pWaveBuffer->status == WaveBuffer::STATUS_TO_BE_DELETED)
        {
            m_SentBufferCount = 0;
        }
        else if (m_SentBufferCount > 0)
        {
            Dspsnd::GetInstance().UpdateChannelNextBuffer(this->m_Id, this->m_pWaveBuffer);
            m_SentBufferCount = 1;
        }
        m_NextBufferIndex = 0;

        if (m_WaveBufferModifiedFlag | 1){
            WaveBuffer* pWaveBuffer = m_pWaveBuffer;

            while (pWaveBuffer && pWaveBuffer->status == WaveBuffer::STATUS_TO_BE_DELETED)
            {
                WaveBuffer* pNext = pWaveBuffer->next;
                WaveBuffer* pTmp = pWaveBuffer;
                pWaveBuffer = pNext;
                os::ARM::DataMemoryBarrier();
                pTmp->status = WaveBuffer::STATUS_DONE;
            }

            m_pWaveBuffer = pWaveBuffer;

            while (pWaveBuffer)
            {
                WaveBuffer* pNext = pWaveBuffer->next;
                if (pNext && pNext->status == WaveBuffer::STATUS_TO_BE_DELETED)
                {
                    pWaveBuffer->next = pNext->next;
                    os::ARM::DataMemoryBarrier();
                    pNext->status = WaveBuffer::STATUS_DONE;
                }
                else
                {
                    pWaveBuffer = pNext;
                }
            }
        }

        m_WaveBufferModifiedFlag = 0;
    }

    WaveBuffer * pWaveBuffer = m_pWaveBuffer;

    for(s32 i = m_SentBufferCount ; i && pWaveBuffer != NULL ; --i)
    {
        pWaveBuffer = pWaveBuffer->next;
    }

    for(s32 i = m_SentBufferCount ; i < 1 + NN_SND_NEXT_BUFFER_NUM ; i++)
    {
        if(pWaveBuffer != NULL)
        {
            if (m_SentBufferCount == 0)
            {
                m_NextBufferIndex = 0;
                Dspsnd::GetInstance().ResetChannelNextBuffer(this->m_Id);

                DspsndAudioInfo* pSampleInfo = reinterpret_cast<DspsndAudioInfo*>((u16*)&this->m_SampleInfo);

                pWaveBuffer->status = WaveBuffer::STATUS_PLAY;

                if (pSampleInfo->format == 8)
                {
                    if (m_IsFirstWaveBufferForAdpcm == false && pWaveBuffer->pAdpcmContext == NULL)
                    {
                        NN_TASSERTMSG_(false, "AdpcmContext is required for the first WaveBuffer!!\n");
                    }

                    else
                    {
                        m_IsFirstWaveBufferForAdpcm = true;
                    }
                }

                Dspsnd::GetInstance().AssignPCM(this->m_Id,pWaveBuffer,*pSampleInfo);
            }
            else
            {
                Dspsnd::GetInstance().AppendChannelNextBuffer(this->m_Id,pWaveBuffer,this->m_NextBufferIndex);

                if (++m_NextBufferIndex >= NN_SND_NEXT_BUFFER_NUM)
                {
                    m_NextBufferIndex = 0;
                }
            }

            pWaveBuffer = pWaveBuffer->next;
            ++m_SentBufferCount;
        }
    }
}

void VoiceImpl::SetMixVolume()
{
    MixParam mix = m_MixParam;
    register f32 tmp[3][CHANNEL_INDEX_NUM];

    for (s32 i = 0; i < CHANNEL_INDEX_NUM; i++)
    {
        tmp[0][i] = mix.mainBus[i] * m_Volume;
        tmp[1][i] = mix.auxBusA[i] * m_Volume;
        tmp[2][i] = mix.auxBusB[i] * m_Volume;
    }
    for (s32 i = 0; i < CHANNEL_INDEX_NUM; i++)
    {
        mix.mainBus[i] = tmp[0][i];
        mix.auxBusA[i] = tmp[1][i];
        mix.auxBusB[i] = tmp[2][i];
    }

    Dspsnd::GetInstance().SetChannelMix(this->m_Id,&mix);
}

void VoiceImpl::SetState(Voice::State state)
{
    NN_TASSERT_(state == Voice::STATE_PLAY || state == Voice::STATE_STOP || state == Voice::STATE_PAUSE);
    m_State = state;
    switch (state)
    {
    case Voice::STATE_PLAY:
        break;

    case Voice::STATE_STOP:
        this->Stop();
        break;

    case Voice::STATE_PAUSE:
        this->Pause();
        break;
    }
}

void VoiceImpl::SetSyncCount()
{
    if(m_ModifiedParamFlag & 0x8000){
        Dspsnd::GetInstance().SetChannelSyncCount(this->m_Id, this->m_SyncCount);
        m_ModifiedParamFlag &= 0x7fff;
    }
}

void VoiceImpl::Start()
{
    Dspsnd::GetInstance().SetChannelPlayStart(this->m_Id);
    m_Playing = true;
}

void VoiceImpl::Stop()
{
    Dspsnd::GetInstance().SetChannelPlayStop(this->m_Id);
    m_Playing = false;
    Dspsnd::GetInstance().InitializeChannelParameters(this->m_Id);
}

void VoiceImpl::UpdateParams()
{
    bool isNeedToCalculateDspCycle = false;
    if(m_ModifiedParamFlag & 1)
    {
        this->SetMixVolume();
        isNeedToCalculateDspCycle = true;
    }
    if(m_ModifiedParamFlag & 2)
    {
        this->SetTimer();
        isNeedToCalculateDspCycle = true;
    }
    if(m_ModifiedParamFlag & 4)
    {
        Dspsnd::GetInstance().SetChannelIiRFilterType(this->m_Id,this->m_FilterType);
        isNeedToCalculateDspCycle = true;
    }
    if(m_ModifiedParamFlag & 8)
    {
        Dspsnd::GetInstance().SetChannelIIRFilter_Mono(this->m_Id,this->m_MonoFilterCoeffs.n0,this->m_MonoFilterCoeffs.d1);
    }
    if(m_ModifiedParamFlag & 0x10)
    {
        s16 d1 = m_BiquadFilterCoeffs.d1;
        s16 d2 = m_BiquadFilterCoeffs.d2;
        s16 n0 = m_BiquadFilterCoeffs.n0;
        s16 n1 = m_BiquadFilterCoeffs.n1;
        s16 n2 = m_BiquadFilterCoeffs.n2;
        Dspsnd::GetInstance().SetChannelIIRFilter_Biquad(this->m_Id, n0, n1, n2, d1, d2);
    }
    if(m_ModifiedParamFlag & 0x20)
    {
        this->UpdateInterpolationType();
        isNeedToCalculateDspCycle = true;
    }
    if(isNeedToCalculateDspCycle)
    {
        this->CalculateDspCycle();
    }
    m_ModifiedParamFlag &= 0x8000;
}

void VoiceImpl::UpdateStatus(const void * ptr){
    const DspsndChannelPlayVars* pVars = reinterpret_cast<const DspsndChannelPlayVars*>(ptr);

    if (pVars->syncCount == m_SyncCount)
    {
        m_PlayPosition = NN_DSP_32BIT_TO_ARM(pVars->plypos);

        if(pVars->isBufJumped)
        {
            this->UpdateWaveBufferStatus(pVars->currentBufferId, pVars->lastBufferId);
        }
    }

    m_Playing = (pVars->playState == 1);
}

void VoiceImpl::UpdateWaveBufferList(){
    if(m_State == Voice::STATE_PLAY)
    {
        this->SendWaveBuffer();
    }
}

void VoiceImpl::UpdateWaveBufferStatus(ushort currentBufferId, ushort lastBufferId){
    os::InterCoreCriticalSection::ScopedLock lock(this->m_CriticalSection);

    if (m_pWaveBuffer == NULL) return;

    WaveBuffer* pNext = SearchPlayingBuffer(currentBufferId, lastBufferId, m_pWaveBuffer, m_SentBufferCount);
    if (pNext == NULL) 
        NN_TASSERT_(m_SentBufferCount == 0);

    m_pWaveBuffer = pNext;
}

void VoiceImpl::Pause()
{
    Dspsnd::GetInstance().SetChannelPlayStop(m_Id);
}

ushort VoiceImpl::SelectCoefficient()
{
    if(m_SampleRateRatio == 1.3333334 || m_SampleRateRatio < 1.3333334 != (m_SampleRateRatio))
    {
        if(m_SampleRateRatio <= 1.0)
        {
            return 2;
        }
    }
    else
        return 0;
}

void VoiceImpl::SetInterpolationType(InterpolationType type)
{
    NN_TASSERT_(type == INTERPOLATION_TYPE_POLYPHASE || type == INTERPOLATION_TYPE_LINEAR || type ==  INTERPOLATION_TYPE_NONE);
    m_InterpolationType = type;
    m_ModifiedParamFlag |= 0x20;
}

void VoiceImpl::SetMixParam(const MixParam& mixParam)
{
    m_MixParam = mixParam;
    m_ModifiedParamFlag |= 1;
}

void VoiceImpl::SetPitch(f32 pitch)
{
    NN_TASSERT_(0.0f <= pitch);
    m_Pitch = math::Max(pitch,0.0);
    m_ModifiedParamFlag |= 2;
}

void VoiceImpl::SetSampleFormat(SampleFormat format)
{
    NN_TASSERT_(format == SAMPLE_FORMAT_PCM16 || format == SAMPLE_FORMAT_PCM8 || format == SAMPLE_FORMAT_ADPCM);
    m_SampleInfo &= 0xfff3 | (format & 3) << 2;
}

void VoiceImpl::SetSampleRate(s32 sampleRate)
{
    NN_TASSERT_(0 <= sampleRate);
    m_SampleRate = math::Max(sampleRate, 0);
    m_ModifiedParamFlag |= 2;
}

void VoiceImpl::SetVolume(f32 volume)
{
    m_Volume =          volume;
    m_ModifiedParamFlag |= 1;
}

void VoiceImpl::SetTimer()
{
    m_SampleRateRatio = this->CalcFsRatio();
    Dspsnd::GetInstance().SetChannelTimer(m_Id, m_SampleRateRatio);
    if(m_InterpolationType == INTERPOLATION_TYPE_POLYPHASE)
    {
        m_ModifiedParamFlag |= 0x20;
    }
}

void VoiceImpl::UpdateInterpolationType()
{
    u16 srcSelect = 2;
    u16 coefSelect = 1;

    switch (m_InterpolationType)
    {
    case INTERPOLATION_TYPE_POLYPHASE:
        srcSelect = 0;
        coefSelect = this->SelectCoefficient();
        break;

    case INTERPOLATION_TYPE_LINEAR:
        srcSelect = 1;
        break;
    }
    Dspsnd::GetInstance().SetChannelRIM(m_Id,srcSelect,coefSelect);
}

}
}
}