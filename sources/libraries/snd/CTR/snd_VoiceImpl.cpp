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

namespace nn{
namespace snd{
namespace CTR{
namespace{

WaveBuffer* SearchPlayingBuffer(ushort currentBufferId, ushort lastBufferId, WaveBuffer* pWaveBuffer, short& sentBufferCount){

    s32 nBuffersToBeReleased = 0;
    WaveBuffer* pBuffersToBeReleased[1 + NN_SND_NEXT_BUFFER_NUM];
    while (sentBufferCount){
        if (currentBufferId == pWaveBuffer->bufferId){
            pWaveBuffer->status = WaveBuffer::STATUS_PLAY;
            break;
        }
        else{
            if (--sentBufferCount){
                NN_NULL_TASSERT_(pWaveBuffer->next);
            }
            pBuffersToBeReleased[nBuffersToBeReleased++] = pWaveBuffer;
            bool exit = ((currentBufferId == 0) && (lastBufferId == pWaveBuffer->bufferId || lastBufferId == 0));
            pWaveBuffer = pWaveBuffer->next;
            if (exit) break;
        }
    }

    if (currentBufferId == 0){
        sentBufferCount = NULL;
    }

    NN_TASSERT_(nBuffersToBeReleased <= 1 + NN_SND_NEXT_BUFFER_NUM);
    os::ARM::DataMemoryBarrier();
    for (s32 i = 0; i < nBuffersToBeReleased; i++){
        pBuffersToBeReleased[i]->status = WaveBuffer::STATUS_DONE;
    }

    return pWaveBuffer;
}

}

void VoiceImpl::AppendWaveBuffer(WaveBuffer* buffer){
    NN_TASSERT_(buffer->status == WaveBuffer::STATUS_FREE);
    NN_NULL_TASSERT_(buffer->bufferAddress);

}

void VoiceImpl::CalculateDspCycle(){
    mModifiedParamFlag = -1;
    this->UpdateParams();
}

void VoiceImpl::ForceUpdateParams(){
    mModifiedParamFlag = 0xffff;
    this->UpdateParams();
}

void VoiceImpl::Initialize(){
    mState = Voice::STATE_PAUSE;
    mPlaying = false;
    mPlayPosition = 0;
    mIsFirstWaveBufferForAdpcm = false;
    mWaveBufferModifiedFlag = 0;

    mSampleInfo &= 0xfffc | 1;
    mSampleInfo &= 0xfff3 | 4;
    mSampleInfo &= 0xffef;
    mSampleInfo &= 0xffdf;
    mSampleInfo &= 0xffbf;

    this->SetVolume(1.0);
    MixParam mixParam;
    this->SetMixParam(mixParam);
    this->SetSampleRate(0x7fd8);
    this->SetPitch(1.0);
    this->SetInterpolationType(INTERPOLATION_TYPE_POLYPHASE);
    this->SetFilterType(FILTER_TYPE_NONE);
    memset(&this->mMonoFilterCoeffs,0,4);
    memset(&this->mBiquadFilterCoeffs,0,10);
    mDspCycles = 0;
    mpWaveBuffer = NULL;
    mSentBufferCount = 0;
    mNextBufferIndex = 0;
    mBufferId = 0;
}

void VoiceImpl::ReleaseWaveBuffer(){
    {
        os::CriticalSection::ScopedLock lock(mCriticalSection);

        WaveBuffer* pWaveBuffer = mpWaveBuffer;

        while (pWaveBuffer){
            pWaveBuffer->status = WaveBuffer::STATUS_DONE;
            pWaveBuffer = pWaveBuffer->next;
        }

        mpWaveBuffer = NULL;
        mSentBufferCount = 0;
        mNextBufferIndex = 0;
    }

    mSyncCount++;
    mModifiedParamFlag |= 0x8000;
}

void VoiceImpl::SendWaveBuffer(){
    os::CriticalSection::ScopedLock lock(this->mCriticalSection);

    if (mWaveBufferModifiedFlag){
        Dspsnd::GetInstance().ResetChannelNextBuffer(this->mId);

        if (mpWaveBuffer && mpWaveBuffer->status == WaveBuffer::STATUS_TO_BE_DELETED){
            mSentBufferCount = 0;
        }
        else if (mSentBufferCount > 0){
            Dspsnd::GetInstance().UpdateChannelNextBuffer(this->mId, this->mpWaveBuffer);
            mSentBufferCount = 1;
        }
        mNextBufferIndex = 0;

        if (mWaveBufferModifiedFlag | 1){
            WaveBuffer* pWaveBuffer = mpWaveBuffer;

            while (pWaveBuffer && pWaveBuffer->status == WaveBuffer::STATUS_TO_BE_DELETED){
                WaveBuffer* pNext = pWaveBuffer->next;
                WaveBuffer* pTmp = pWaveBuffer;
                pWaveBuffer = pNext;
                os::ARM::DataMemoryBarrier();
                pTmp->status = WaveBuffer::STATUS_DONE;
            }

            mpWaveBuffer = pWaveBuffer;

            while (pWaveBuffer){
                WaveBuffer* pNext = pWaveBuffer->next;
                if (pNext && pNext->status == WaveBuffer::STATUS_TO_BE_DELETED){
                    pWaveBuffer->next = pNext->next;
                    os::ARM::DataMemoryBarrier();
                    pNext->status = WaveBuffer::STATUS_DONE;
                }
                else{
                    pWaveBuffer = pNext;
                }
            }
        }

        mWaveBufferModifiedFlag = 0;
    }

    WaveBuffer * pWaveBuffer = mpWaveBuffer;

    for(s32 i = mSentBufferCount ; i && pWaveBuffer != NULL ; --i){
        pWaveBuffer = pWaveBuffer->next;
    }

    for(s32 i = mSentBufferCount ; i < 1 + NN_SND_NEXT_BUFFER_NUM ; i++){
        if(pWaveBuffer != NULL){
            if (mSentBufferCount == 0){
                mNextBufferIndex = 0;
                Dspsnd::GetInstance().ResetChannelNextBuffer(this->mId);

                DspsndAudioInfo* pSampleInfo = reinterpret_cast<DspsndAudioInfo*>((u16*)&this->mSampleInfo);

                pWaveBuffer->status = WaveBuffer::STATUS_PLAY;

                if (pSampleInfo->format == 8){
                    if (mIsFirstWaveBufferForAdpcm == false && pWaveBuffer->pAdpcmContext == NULL){
                        NN_TASSERTMSG_(false, "AdpcmContext is required for the first WaveBuffer!!\n");
                    }

                    else{
                        mIsFirstWaveBufferForAdpcm = true;
                    }
                }

                Dspsnd::GetInstance().AssignPCM(this->mId,pWaveBuffer,*pSampleInfo);
            }
            else{
                Dspsnd::GetInstance().AppendChannelNextBuffer(this->mId,pWaveBuffer,this->mNextBufferIndex);

                if (++mNextBufferIndex >= NN_SND_NEXT_BUFFER_NUM){
                    mNextBufferIndex = 0;
                }
            }

            pWaveBuffer = pWaveBuffer->next;
            ++mSentBufferCount;
        }
    }
}

void VoiceImpl::SetMixVolume(){
    MixParam mix = mMixParam;
    register f32 tmp[3][CHANNEL_INDEX_NUM];

    for (s32 i = 0; i < CHANNEL_INDEX_NUM; i++){
        tmp[0][i] = mix.mainBus[i] * mVolume;
        tmp[1][i] = mix.auxBusA[i] * mVolume;
        tmp[2][i] = mix.auxBusB[i] * mVolume;
    }
    for (s32 i = 0; i < CHANNEL_INDEX_NUM; i++){
        mix.mainBus[i] = tmp[0][i];
        mix.auxBusA[i] = tmp[1][i];
        mix.auxBusB[i] = tmp[2][i];
    }

    Dspsnd::GetInstance().SetChannelMix(this->mId,&mix);
}

void VoiceImpl::SetState(Voice::State state){
    NN_TASSERT_(state == Voice::STATE_PLAY || state == Voice::STATE_STOP || state == Voice::STATE_PAUSE);
    mState = state;
    switch (state){
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

void VoiceImpl::SetSyncCount(){
    if(mModifiedParamFlag & 0x8000){
        Dspsnd::GetInstance().SetChannelSyncCount(this->mId, this->mSyncCount);
        mModifiedParamFlag &= 0x7fff;
    }
}

void VoiceImpl::Start(){
    Dspsnd::GetInstance().SetChannelPlayStart(this->mId);
    mPlaying = true;
}

void VoiceImpl::Stop(){
    Dspsnd::GetInstance().SetChannelPlayStop(this->mId);
    mPlaying = false;
    Dspsnd::GetInstance().InitializeChannelParameters(this->mId);
}

void VoiceImpl::UpdateParams(){
    bool isNeedToCalculateDspCycle = false;
    if(mModifiedParamFlag & 1){
        this->SetMixVolume();
        isNeedToCalculateDspCycle = true;
    }
    if(mModifiedParamFlag & 2){
        this->SetTimer();
        isNeedToCalculateDspCycle = true;
    }
    if(mModifiedParamFlag & 4){
        Dspsnd::GetInstance().SetChannelIiRFilterType(this->mId,this->mFilterType);
        isNeedToCalculateDspCycle = true;
    }
    if(mModifiedParamFlag & 8){
        Dspsnd::GetInstance().SetChannelIIRFilter_Mono(this->mId,this->mMonoFilterCoeffs.n0,this->mMonoFilterCoeffs.d1);
    }
    if(mModifiedParamFlag & 0x10){
        s16 d1 = mBiquadFilterCoeffs.d1;
        s16 d2 = mBiquadFilterCoeffs.d2;
        s16 n0 = mBiquadFilterCoeffs.n0;
        s16 n1 = mBiquadFilterCoeffs.n1;
        s16 n2 = mBiquadFilterCoeffs.n2;
        Dspsnd::GetInstance().SetChannelIIRFilter_Biquad(this->mId, n0, n1, n2, d1, d2);
    }
    if(mModifiedParamFlag & 0x20){
        this->UpdateInterpolationType();
        isNeedToCalculateDspCycle = true;
    }
    if(isNeedToCalculateDspCycle){
        this->CalculateDspCycle();
    }
    mModifiedParamFlag &= 0x8000;
}

void VoiceImpl::UpdateStatus(const void * ptr){
    const DspsndChannelPlayVars* pVars = reinterpret_cast<const DspsndChannelPlayVars*>(ptr);

    if (pVars->syncCount == mSyncCount){
        mPlayPosition = NN_DSP_32BIT_TO_ARM(pVars->plypos);

        if(pVars->isBufJumped){
            this->UpdateWaveBufferStatus(pVars->currentBufferId, pVars->lastBufferId);
        }
    }

    mPlaying = (pVars->playState == 1);
}

void VoiceImpl::UpdateWaveBufferList(){
    if(mState == Voice::STATE_PLAY){
        this->SendWaveBuffer();
    }
}

void VoiceImpl::UpdateWaveBufferStatus(ushort currentBufferId, ushort lastBufferId){
    os::CriticalSection::ScopedLock lock(this->mCriticalSection);

    if (mpWaveBuffer == NULL) return;

    WaveBuffer* pNext = SearchPlayingBuffer(currentBufferId, lastBufferId, mpWaveBuffer, mSentBufferCount);
    if (pNext == NULL) 
        NN_TASSERT_(mSentBufferCount == 0);

    mpWaveBuffer = pNext;
}

void VoiceImpl::Pause(){
    Dspsnd::GetInstance().SetChannelPlayStop(this->mId);
}

ushort VoiceImpl::SelectCoefficient(){
    if(mSampleRateRatio == 1.3333334 || mSampleRateRatio < 1.3333334 != (mSampleRateRatio)){
        if(mSampleRateRatio <= 1.0){
            return 2;
        }
    }
    else
        return 0;
}

void VoiceImpl::SetInterpolationType(InterpolationType type){
    NN_TASSERT_(type == INTERPOLATION_TYPE_POLYPHASE || type == INTERPOLATION_TYPE_LINEAR || type ==  INTERPOLATION_TYPE_NONE);
    mInterpolationType = type;
    mModifiedParamFlag |= 0x20;
}

void VoiceImpl::SetMixParam(const MixParam& mixParam){
    memcpy(&this->mMixParam, &mixParam, 0x30);
    mModifiedParamFlag |= 1;
}

void VoiceImpl::SetPitch(f32 pitch){
    NN_TASSERT_(0.0f <= pitch);
    mPitch = math::Max(pitch,0.0);
    mModifiedParamFlag |= 2;
}

void VoiceImpl::SetSampleFormat(SampleFormat format){
    NN_TASSERT_(format == SAMPLE_FORMAT_PCM16 || format == SAMPLE_FORMAT_PCM8 || format == SAMPLE_FORMAT_ADPCM);
    mSampleInfo &= 0xfff3 | (format & 3) << 2;
}

void VoiceImpl::SetSampleRate(s32 sampleRate){
    NN_TASSERT_(0 <= sampleRate);
    mSampleRate = math::Max(sampleRate, 0);
    mModifiedParamFlag |= 2;
}

void VoiceImpl::SetVolume(f32 volume){
    mVolume = volume;
    mModifiedParamFlag |= 1;
}

void VoiceImpl::SetTimer(){
    mSampleRateRatio = this->CalcFsRatio();
    Dspsnd::GetInstance().SetChannelTimer(this->mId, this->mSampleRateRatio);
    if(mInterpolationType == INTERPOLATION_TYPE_POLYPHASE){
        mModifiedParamFlag |= 0x20;
    }
}

void VoiceImpl::UpdateInterpolationType(){
    DSPWord method;
    ushort coefSelect;
    if(mInterpolationType == INTERPOLATION_TYPE_POLYPHASE){
        method = 0;
        coefSelect = this->SelectCoefficient();
    }
    else if(mInterpolationType == INTERPOLATION_TYPE_LINEAR){
        method = 1;
    }
    Dspsnd::GetInstance().SetChannelRIM(this->mId,method,coefSelect);
}

}
}
}