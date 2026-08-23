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
    CTR::VoiceManager sVoiceManager;
}

VoiceManager::VoiceManager(){
    u8* pBuffer = mVoiceBuffer;
    u8* pImplBuffer = mVoiceImplBuffer;
    for (int i = 0; i < NN_SND_VOICE_NUM; i++){

        mpVoice[i] = new (pBuffer) Voice(i);
        NN_TASSERT_(reinterpret_cast<uptr>(this->mpVoice[i]) == reinterpret_cast<uptr>(pBuffer));
        pBuffer += sizeof(Voice);

        mpVoice[i]->mpImpl = new (pImplBuffer) VoiceImpl(i);
        NN_TASSERT_(reinterpret_cast<uptr>(this->mpVoice[i]->mpImpl) == reinterpret_cast<uptr>(pImplBuffer));
        pImplBuffer += sizeof(VoiceImpl);
    }
}

void VoiceManager::Initialize(){
    mMostPriorVoice = NULL;
    mMostInferiorVoice = NULL;
    mUsedVoiceBits = 0;
    mAllocatedVoiceCount = 0;
    this->SetVoiceDropMode(VOICE_DROP_MODE_DEFAULT);
    this->mCriticalSection.Initialize();
}

void VoiceManager::Finalize(){
    this->mCriticalSection.Finalize();
}

void VoiceManager::AdjustVoicePlayState(s32 remain, s32 frame){
    if (mVoiceDropMode == VOICE_DROP_MODE_REAL_TIME){
        static const int DSP_CYCLES = 622535 * 100 / 95;
        int delayCycles = frame - DSP_CYCLES;
        if (delayCycles > 0){
            remain -= delayCycles;
        }
    }

    os::CriticalSection::ScopedLock lock(this->mCriticalSection);

    Voice* pVoice = mMostPriorVoice;

    if (!pVoice) return;

    NN_NULL_TASSERT_(mMostInferiorVoice);

    while (pVoice){
        if (pVoice->GetImpl()->GetState() == Voice::STATE_PLAY && pVoice->GetImpl()->mpWaveBuffer){
            s32 cyclesVoice = pVoice->GetImpl()->GetCycle();

            if ((remain >= cyclesVoice) || (pVoice->GetPriority() == VOICE_PRIORITY_NODROP)){

                pVoice->GetImpl()->SetSyncCount();

                if (!pVoice->GetImpl()->IsPlaying()){
                    pVoice->GetImpl()->Start();
                }

                remain -= cyclesVoice;
            }
            else{
                this->FreeVoice(pVoice);

                if (pVoice->mCallback){
                    (*pVoice->mCallback)(pVoice, pVoice->mUserArg);
                }
            }
        }

        pVoice = pVoice->mInferiorVoice;
    }
}

Voice* VoiceManager::AllocVoice(s32 priority, VoiceDropCallbackFunc callback, uptr userArg){
    Voice* pVoice = NULL;
    NN_TASSERT_(0 <= priority && priority <= VOICE_PRIORITY_NODROP);
    if(!(0 <= priority && priority <= VOICE_PRIORITY_NODROP))
        return NULL;

    this->mCriticalSection.Enter();

    if(math::CntBit1(this->mUsedVoiceBits) == NN_SND_VOICE_NUM){
        NN_TASSERT_(mMostInferiorVoice != NULL);

        if((mMostInferiorVoice->mPriority == VOICE_PRIORITY_NODROP || mMostInferiorVoice->mPriority > priority)){
            this->mCriticalSection.Leave();
            return NULL;
        }
        
        else{
            Voice* voice = mMostInferiorVoice;
            VoiceDropCallbackFunc _callback = voice->mCallback;
            uptr _userArg = voice->mUserArg;
            FreeVoice(this->mMostInferiorVoice);
            if (_callback){
                (_callback)(voice, _userArg);
            }
        }
    }

    pVoice = GetAvaliableVoice();
    NN_TASSERT_(pVoice != NULL);

    pVoice->mPriority = priority;

    this->InsertVoiceToPriorityList(pVoice,priority);
    this->mCriticalSection.Leave();

    pVoice->GetImpl()->SetState(Voice::STATE_PAUSE);
    pVoice->GetImpl()->mSyncCount;

    pVoice->mCallback = callback;
    pVoice->mUserArg = userArg;

    return pVoice;
}

void VoiceManager::ForceUpdateParams(){
    for(int i = 0; i < NN_SND_VOICE_NUM; i++){
        this->mpVoice[i]->GetImpl()->ForceUpdateParams();
    }
}

void VoiceManager::FreeVoice(Voice* pVoice){
    NN_TASSERT_(mpVoice[0] <= pVoice && pVoice <= this->mpVoice[NN_SND_VOICE_NUM-1]);
    NN_TASSERTMSG_(this->IsAllocated(pVoice), "Cannot free voice which is not allocated\n");
}

Voice* VoiceManager::GetAvaliableVoice(){
    s32 varVoice = this->mUsedVoiceBits;
    Voice * pVoice = reinterpret_cast<Voice*>(NULL);
}

void VoiceManager::InsertVoiceToPriorityList(Voice* pVoice, s32 priority){
    NN_TASSERT_(0 <= priority && priority <= VOICE_PRIORITY_NODROP);
    Voice* pVoiceList = mMostPriorVoice;

    if(pVoiceList){
        while(true){
            if(priority >= pVoiceList->GetPriority()){
                Voice* pPriorVoice = pVoiceList->mPriorVoice;

                pVoice->mPriorVoice = pPriorVoice;
                pVoice->mInferiorVoice = pVoiceList;

                if(pPriorVoice){
                    pPriorVoice->mInferiorVoice = pVoice;
                }

                else{
                    this->SetMostPriorVoice(pVoice);
                }

                pVoiceList->mPriorVoice = pVoice;
                break;
            }
            else{
                if(pVoiceList->mInferiorVoice){
                    pVoiceList = pVoiceList->mInferiorVoice;
                }
                else{
                    pVoiceList->mInferiorVoice = pVoice;

                    pVoice->mPriorVoice = pVoiceList;
                    this->SetMostInferiorVoice(pVoice);
                    break;
                }
            }
        }
    }
    else{
        NN_TASSERT_(mMostPriorVoice == NULL && mMostInferiorVoice == NULL );

        this->SetMostPriorVoice(pVoice);
        this->SetMostInferiorVoice(pVoice);
    }
}

void VoiceManager::RemoveVoiceFromPriorityList(Voice* pVoice){
    Voice *pPriorVoice = pVoice->mPriorVoice;
    Voice *pInfVoice = pVoice->mInferiorVoice;

    if(pPriorVoice == NULL && pInfVoice == NULL){
        mMostPriorVoice = NULL;
        mMostInferiorVoice = NULL;
        return;
    }
    if(pInfVoice != NULL){
        pInfVoice->mPriorVoice = pVoice->mPriorVoice;

        if (pInfVoice->mPriorVoice == NULL){
            mMostPriorVoice = pInfVoice;
        }
    }

    if(pPriorVoice != NULL){
        pPriorVoice->mInferiorVoice = pVoice->mInferiorVoice;

        if (pPriorVoice->mInferiorVoice == NULL){
            mMostInferiorVoice = pPriorVoice;
        }
    }
}

void VoiceManager::SetPriority(Voice* pVoice, s32 priority){
    NN_TASSERT_(mpVoice[0] <= pVoice && pVoice <= this->mpVoice[NN_SND_VOICE_NUM-1]);
    os::CriticalSection::ScopedLock lock(this->mCriticalSection);
    this->RemoveVoiceFromPriorityList(pVoice);
    this->InsertVoiceToPriorityList(pVoice,priority);
}

inline void VoiceManager::SetVoiceDropMode(VoiceDropMode mode){
    NN_TASSERT_(mode == VOICE_DROP_MODE_DEFAULT || mode == VOICE_DROP_MODE_REAL_TIME);
    memcpy(&this->mVoiceDropMode,&mode,1);
}

void VoiceManager::UpdateParams(){
    for(int i = 0; i < NN_SND_VOICE_NUM; i++){
        this->mpVoice[i]->GetImpl()->UpdateParams();
    }
}

void VoiceManager::UpdateStatus(s32 id, const DspsndChannelPlayVars* pVars){
    this->mpVoice[id]->GetImpl()->UpdateStatus(pVars);
}

void VoiceManager::UpdateWaveBufferList(){
    for(int i = 0; i < NN_SND_VOICE_NUM; i++){
        this->mpVoice[i]->GetImpl()->UpdateWaveBufferList();
    }
}
}
}
}