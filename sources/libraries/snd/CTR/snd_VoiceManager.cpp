// Filename: snd_VoiceManager.cpp
//
// Project: Horizon

#include "snd_VoiceManager.h"
#include "snd_VoiceImpl.h"
#include <nn/snd/CTR/Common/snd_Const.h>

namespace nn{
namespace snd{
namespace CTR{
namespace internal{
    CTR::VoiceManager s_VoiceManager;
}

VoiceManager::VoiceManager()
{
    u8* pBuffer = m_VoiceBuffer;
    u8* pImplBuffer = m_VoiceImplBuffer;
    for (int i = 0; i < NN_SND_VOICE_NUM; i++)
    {
        m_pVoice[i] = new (pBuffer) Voice(i);
        NN_TASSERT_(reinterpret_cast<uptr>(this->m_pVoice[i]) == reinterpret_cast<uptr>(pBuffer));
        pBuffer += sizeof(Voice);

        m_pVoice[i]->m_pImpl = new (pImplBuffer) VoiceImpl(i);
        NN_TASSERT_(reinterpret_cast<uptr>(this->m_pVoice[i]->m_pImpl) == reinterpret_cast<uptr>(pImplBuffer));
        pImplBuffer += sizeof(VoiceImpl);
    }
}

void VoiceManager::Initialize()
{
    m_MostPriorVoice      = NULL;
    m_MostInferiorVoice   = NULL;
    m_UsedVoiceBits       = 0;
    m_AllocatedVoiceCount = 0;

    this->SetVoiceDropMode(VOICE_DROP_MODE_DEFAULT);
    this->m_CriticalSection.Initialize();
}

void VoiceManager::Finalize()
{
    this->m_CriticalSection.Finalize();
}

void VoiceManager::AdjustVoicePlayState(s32 remain, s32 frame)
{
    if (m_VoiceDropMode == VOICE_DROP_MODE_REAL_TIME)
    {
        static const int DSP_CYCLES = 622535 * 100 / 95;
        int delayCycles = frame - DSP_CYCLES;
        if (delayCycles > 0)
        {
            remain -= delayCycles;
        }
    }

    os::InterCoreCriticalSection::ScopedLock lock(this->m_CriticalSection);

    Voice* pVoice = m_MostPriorVoice;

    if (!pVoice) return;

    NN_NULL_TASSERT_(m_MostInferiorVoice);

    while (pVoice)
    {
        if (pVoice->GetImpl()->GetState() == Voice::STATE_PLAY && pVoice->GetImpl()->m_pWaveBuffer)
        {
            s32 cyclesVoice = pVoice->GetImpl()->GetCycle();

            if ((remain >= cyclesVoice) || (pVoice->GetPriority() == VOICE_PRIORITY_NODROP))
            {

                pVoice->GetImpl()->SetSyncCount();

                if (!pVoice->GetImpl()->IsPlaying())
                {
                    pVoice->GetImpl()->Start();
                }

                remain -= cyclesVoice;
            }
            else
            {
                this->FreeVoice(pVoice);

                if (pVoice->m_Callback)
                {
                    (*pVoice->m_Callback)(pVoice, pVoice->m_UserArg);
                }
            }
        }

        pVoice = pVoice->m_InferiorVoice;
    }
}

Voice* VoiceManager::AllocVoice(s32 priority, VoiceDropCallbackFunc callback, uptr userArg)
{
    Voice* pVoice = NULL;
    NN_TASSERT_(0 <= priority && priority <= VOICE_PRIORITY_NODROP);
    if(!(0 <= priority && priority <= VOICE_PRIORITY_NODROP))
        return NULL;

    this->m_CriticalSection.Enter();

    if(math::CntBit1(this->m_UsedVoiceBits) == NN_SND_VOICE_NUM)
    {
        NN_TASSERT_(m_MostInferiorVoice != NULL);

        if((m_MostInferiorVoice->m_Priority == VOICE_PRIORITY_NODROP || m_MostInferiorVoice->m_Priority > priority))
        {
            this->m_CriticalSection.Leave();
            return NULL;
        }
        else
        {
            Voice* voice = m_MostInferiorVoice;
            VoiceDropCallbackFunc _callback = voice->m_Callback;
            uptr _userArg = voice->m_UserArg;
            FreeVoice(this->m_MostInferiorVoice);
            if (_callback)
            {
                (_callback)(voice, _userArg);
            }
        }
    }

    pVoice = GetAvaliableVoice();
    NN_TASSERT_(pVoice != NULL);

    pVoice->m_Priority = priority;

    this->InsertVoiceToPriorityList(pVoice,priority);
    this->m_CriticalSection.Leave();

    pVoice->GetImpl()->SetState(Voice::STATE_PAUSE);
    pVoice->GetImpl()->m_SyncCount;

    pVoice->m_Callback = callback;
    pVoice->m_UserArg = userArg;

    return pVoice;
}

void VoiceManager::ForceUpdateParams()
{
    for(int i = 0; i < NN_SND_VOICE_NUM; i++)
    {
        m_pVoice[i]->GetImpl()->ForceUpdateParams();
    }
}

void VoiceManager::SetMostInferiorVoice(Voice* pVoice)
{
    pVoice->m_InferiorVoice = 0; 
    m_MostInferiorVoice = pVoice;
}

void VoiceManager::SetMostPriorVoice(Voice* pVoice)
{ 
    pVoice->m_PriorVoice = 0; 
    m_MostPriorVoice     = pVoice; 
}

void VoiceManager::FreeVoice(Voice* pVoice)
{
    NN_TASSERT_(m_pVoice[0] <= pVoice && pVoice <= m_pVoice[NN_SND_VOICE_NUM-1]);
    NN_TASSERTMSG_(this->IsAllocated(pVoice), "Cannot free voice which is not allocated\n");
}

Voice* VoiceManager::GetAvaliableVoice()
{
    s32 varVoice = m_UsedVoiceBits;
    Voice * pVoice = reinterpret_cast<Voice*>(NULL);
}

void VoiceManager::InsertVoiceToPriorityList(Voice* pVoice, s32 priority)
{
    NN_TASSERT_(0 <= priority && priority <= VOICE_PRIORITY_NODROP);
    Voice* pVoiceList = m_MostPriorVoice;

    if(pVoiceList)
    {
        while(true){

            if(priority >= pVoiceList->GetPriority())
            {
                Voice* pPriorVoice = pVoiceList->m_PriorVoice;

                pVoice->m_PriorVoice = pPriorVoice;
                pVoice->m_InferiorVoice = pVoiceList;

                if(pPriorVoice)
                {
                    pPriorVoice->m_InferiorVoice = pVoice;
                }

                else
                {
                    this->SetMostPriorVoice(pVoice);
                }

                pVoiceList->m_PriorVoice = pVoice;
                break;
            }

            else
            {
                if(pVoiceList->m_InferiorVoice)
                {
                    pVoiceList = pVoiceList->m_InferiorVoice;
                }

                else
                {
                    pVoiceList->m_InferiorVoice = pVoice;

                    pVoice->m_PriorVoice = pVoiceList;
                    this->SetMostInferiorVoice(pVoice);
                    break;
                }
            }
        }
    }
    else
    {
        NN_TASSERT_(mMostPriorVoice == NULL && mMostInferiorVoice == NULL );

        this->SetMostPriorVoice(pVoice);
        this->SetMostInferiorVoice(pVoice);
    }
}

void VoiceManager::RemoveVoiceFromPriorityList(Voice* pVoice)
{
    Voice *pPriorVoice = pVoice->m_PriorVoice;
    Voice *pInfVoice = pVoice->m_InferiorVoice;

    if(pPriorVoice == NULL && pInfVoice == NULL)
    {
        m_MostPriorVoice = NULL;
        m_MostInferiorVoice = NULL;
        return;
    }

    if(pInfVoice != NULL)
    {
        pInfVoice->m_PriorVoice = pVoice->m_PriorVoice;

        if (pInfVoice->m_PriorVoice == NULL)
        {
            m_MostPriorVoice = pInfVoice;
        }
    }

    if(pPriorVoice != NULL)
    {
        pPriorVoice->m_InferiorVoice = pVoice->m_InferiorVoice;

        if (pPriorVoice->m_InferiorVoice == NULL)
        {
            m_MostInferiorVoice = pPriorVoice;
        }
    }
}

void VoiceManager::SetPriority(Voice* pVoice, s32 priority)
{
    NN_TASSERT_(m_pVoice[0] <= pVoice && pVoice <= m_pVoice[NN_SND_VOICE_NUM-1]);
    os::InterCoreCriticalSection::ScopedLock lock(m_CriticalSection);
    this->RemoveVoiceFromPriorityList(pVoice);
    this->InsertVoiceToPriorityList(pVoice,priority);
}

inline void VoiceManager::SetVoiceDropMode(VoiceDropMode mode)
{
    NN_TASSERT_(mode == VOICE_DROP_MODE_DEFAULT || mode == VOICE_DROP_MODE_REAL_TIME);
    m_VoiceDropMode = mode;
}

void VoiceManager::UpdateParams()
{
    for(int i = 0; i < NN_SND_VOICE_NUM; i++)
    {
        m_pVoice[i]->GetImpl()->UpdateParams();
    }
}

void VoiceManager::UpdateStatus(s32 id, const DspsndChannelPlayVars* pVars)
{
    m_pVoice[id]->GetImpl()->UpdateStatus(pVars);
}

void VoiceManager::UpdateWaveBufferList()
{
    for(int i = 0; i < NN_SND_VOICE_NUM; i++)
    {
        m_pVoice[i]->GetImpl()->UpdateWaveBufferList();
    }
}
}
}
}