#include <nn/snd/CTR/MPCore/snd_MasterManager.h>
#include <nn/dsp.h>
#include <nn/math/math_Utility.h>
#include <nn/Assert.h>

namespace nn {
namespace snd {
namespace CTR {
namespace internal{
    CTR::MasterManagerImpl sMasterManagerImpl;
}
// flagged
void MasterManagerImpl::AuxUserCallback(AuxBusId busId, uptr data){
    NN_TASSERT_( busId == AUX_BUS_A || busId == AUX_BUS_B );

    if (!mInitialized){
        return ;
    }
    os::CriticalSection::ScopedLock lock(this->mCriticalSection);

    if (mAuxCallback[busId]){
        s32 * pData = reinterpret_cast<s32*>(data);
        AuxBusData auxBusData ={
            pData,
            pData + NN_SND_SAMPLES_PER_FRAME,
            pData + NN_SND_SAMPLES_PER_FRAME * 2,
            pData + NN_SND_SAMPLES_PER_FRAME * 3
        };

        mAuxCallback[busId]( &auxBusData, NN_SND_SAMPLES_PER_FRAME, mAuxUserData[busId] );
    }
}

void MasterManagerImpl::Finalize(){
    if(mInitialized){
        this->mCriticalSection.Finalize();
        mInitialized = false;
    }
}

void MasterManagerImpl::ForceUpdateParams(){
    AuxBusId id;
    bool flag;
    OutputMode outmode;
    ClippingMode clippingmode;
    SurroundSpeakerPosition pos;
    SyncMode syncmode;
    Dspsnd::GetInstance().SetMasterVolume(this->mMasterVolume * this->mSystemMasterVolume);
    for(int i = 0; i < AUX_BUS_NUM; i++){
        Dspsnd::GetInstance().SetAuxReturnVolume(id,this->mAuxVolume[id]);
        if((!mAuxCallback[id]) && (!mFxEnabled[id])){
            flag = false;
        }
        else{
            flag = true;
        }
        Dspsnd::GetInstance().EnableAuxBus(id,flag);
        Dspsnd::GetInstance().SetAuxFrontBypass(id,this->mAuxFrontBypass[id]);
    }
    outmode = mOutputMode;
    Dspsnd::GetInstance().SetSoundOutputMode(outmode);

    clippingmode = mClippingMode;
    Dspsnd::GetInstance().SetClippingMode(clippingmode);

    Dspsnd::GetInstance().SetSurroundDepth(this->mSurroundDepth);

    pos = mSpeakerPosition;
    Dspsnd::GetInstance().SetSurroundSpeakerPosition(pos);

    Dspsnd::GetInstance().SetRearRatio(this->mRearRatio);
    Dspsnd::GetInstance().SetOutputBufferCount(this->mOutputBufferCount);
    this->mDroppedFrameCount = 0;

    syncmode = this->mSyncMode;
    Dspsnd::GetInstance().SetSyncMode(syncmode);
}

void MasterManagerImpl::Initialize(){
    if(!mInitialized){
        this->mCriticalSection.Initialize();
        mInitialized = true;
    }
}

void MasterManagerImpl::InitializeParam(){
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

    for(int i = 0; i < AUX_BUS_NUM; i++){
        this->mFxEnabled[i] = false;
    }

    this->SetOutputBufferCount(2);
    this->SetSyncMode(SYNC_MODE_STRICT);
}

void MasterManagerImpl::SetIsHeadsetConnected(bool flag){
    Dspsnd::GetInstance().SetIsHeadsetConnected(flag);
}

void MasterManagerImpl::RegisterAuxCallback(AuxBusId busId, AuxCallback callback, uptr userData){
    NN_TASSERT_(busId == AUX_BUS_A || busId == AUX_BUS_B);
    os::CriticalSection::ScopedLock lock(this->mCriticalSection);
    this->mAuxCallback[busId] = callback;
    this->mAuxUserData[busId] = userData;
    if(!callback){
        Dspsnd::GetInstance().EnableAuxBus(busId,this->mFxEnabled[busId]);
    }
    else{
        Dspsnd::GetInstance().EnableAuxBus(busId,true);
    }
}

void MasterManagerImpl::SetAuxReturnVolume(AuxBusId busId, f32 fVolume){
    NN_TASSERT_(busId == AUX_BUS_A || busId == AUX_BUS_B);
    if(mInitialized){
        mAuxVolume[busId] = fVolume;
        Dspsnd::GetInstance().SetAuxReturnVolume(busId,fVolume);
    }
}

void MasterManagerImpl::SetSurroundSpeakerPosition(SurroundSpeakerPosition pos){
    if(pos < 2){
        mSpeakerPosition = pos;
        Dspsnd::GetInstance().SetSurroundSpeakerPosition(pos);
    }
}

bool MasterManagerImpl::SetClippingMode(ClippingMode mode){
    if(dsp::CTR::IsComponentLoaded()){
        NN_TASSERT_(mode == CLIPPING_MODE_NORMAL || mode == CLIPPING_MODE_SOFT);
        this->mClippingMode = mode;
        Dspsnd::GetInstance().SetClippingMode(mode);
    }
    else{
        return false;
    }
}

void MasterManagerImpl::SetMasterVolume(f32 fVolume){
    if(mInitialized){
        mMasterVolume = fVolume;
        Dspsnd::GetInstance().SetMasterVolume(this->mMasterVolume * this->mSystemMasterVolume);
    }
}

void MasterManagerImpl::SetOutputBufferCount(s32 outputBufferCount){
    s32 newCnt = math::Max(outputBufferCount,2);
    newCnt = math::Min(newCnt, 3);
    mOutputBufferCount = newCnt;
    Dspsnd::GetInstance().SetOutputBufferCount(newCnt);
}

bool MasterManagerImpl::SetRearRatio(f32 ratio){
    mRearRatio = ((0.0 < ratio * 32768.0) * (ratio * 32768.0));
    return Dspsnd::GetInstance().SetRearRatio(this->mRearRatio);
}

bool MasterManagerImpl::SetSoundOutputMode(OutputMode mode){
    if (dsp::CTR::IsComponentLoaded() == false){
        return false;
    }

    NN_TASSERT_(mode == OUTPUT_MODE_MONO || mode == OUTPUT_MODE_STEREO || mode == OUTPUT_MODE_3DSURROUND);

    mOutputMode = mode;
    return Dspsnd::GetInstance().SetSoundOutputMode(mode);
}

bool MasterManagerImpl::SetSurroundDepth(f32 depth){
    if(depth < 0.0)
        depth = 0.0;
    if(depth != 1.0 && depth < 1.0 == (depth))
        depth = 1.0;
    this->mSurroundDepth = (0.0 < depth * 32767.0) * (depth * 32767.0);
    return internal::sDspsnd.SetSurroundDepth(this->mSurroundDepth);
}

void MasterManagerImpl::SetSyncMode(SyncMode mode){
    this->mSyncMode = mode;
    Dspsnd::GetInstance().SetSyncMode(mode);
}

void MasterManagerImpl::SetSystemMasterVolume(f32 volume){
    if(mInitialized){
        mSystemMasterVolume = volume;
        Dspsnd::GetInstance().SetMasterVolume(this->mMasterVolume * this->mSystemMasterVolume);
    }
}

}
}
}