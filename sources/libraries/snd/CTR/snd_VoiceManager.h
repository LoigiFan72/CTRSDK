#pragma once

#include <nn/snd/CTR/MPCore/snd_Voice.h>

#include <string.h>

namespace nn{
namespace snd{
namespace CTR{

class VoiceManager;

namespace internal{
    extern VoiceManager sVoiceManager;
}



class VoiceManager{
public:
    static VoiceManager& GetInstance(){ return internal::sVoiceManager; }
public:
    bit32 mUsedVoiceBits;
    Voice* mMostPriorVoice;
    Voice* mMostInferiorVoice;
    short mAllocatedVoiceCount;
    util::SizedEnum1<VoiceDropMode> mVoiceDropMode;
    s8 pad1;
    os::CriticalSection mCriticalSection;
    u8 mVoiceBuffer[2592];
    u8 mVoiceImplBuffer[2976];
    Voice* mpVoice[24];

public:
    VoiceManager();
    ~VoiceManager(){ }

    void AdjustVoicePlayState(s32 remain, s32 frame);
    Voice* AllocVoice(s32 priority, VoiceDropCallbackFunc callback, uptr userArg);
    void Finalize();
    void ForceUpdateParams();
    void FreeVoice(Voice* pVoice);
    Voice* GetAvaliableVoice();
    void Initialize();
    void InsertVoiceToPriorityList(Voice* pVoice, s32 priority);
    bool IsAllocated(Voice* pVoice){ return (1 << (pVoice->GetId() & 0xff) & this->mUsedVoiceBits) != 0; }
    void RemoveVoiceFromPriorityList(Voice* pVoice);
    void SetMostInferiorVoice(Voice* pVoice){ pVoice->mInferiorVoice = 0; this->mMostInferiorVoice = pVoice; }
    void SetMostPriorVoice(Voice* pVoice){ pVoice->mPriorVoice = 0; this->mMostPriorVoice = pVoice; }
    void SetPriority(Voice* pVoice, s32 priority);
    void SetVoiceDropMode(VoiceDropMode mode);
    void UpdateParams();
    void UpdateStatus(s32 id, const DspsndChannelPlayVars* pVars);
    void UpdateWaveBufferList();
};

}
}
}