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
private:
    bit32  m_UsedVoiceBits;
    Voice* m_MostPriorVoice;
    Voice* m_MostInferiorVoice;
    short  m_AllocatedVoiceCount;
    util::SizedEnum1<VoiceDropMode> m_VoiceDropMode;
    s8 pad1;
    os::InterCoreCriticalSection    m_CriticalSection;
    u8 m_VoiceBuffer[2592];
    u8 m_VoiceImplBuffer[2976];
    Voice* mp_Voice[24];

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
    bool IsAllocated(Voice* pVoice){ return (1 << (pVoice->GetId() & 0xff) & m_UsedVoiceBits) != 0; }
    void RemoveVoiceFromPriorityList(Voice* pVoice);
    void SetMostInferiorVoice(Voice* pVoice);
    void SetMostPriorVoice(Voice* pVoice);
    void SetPriority(Voice* pVoice, s32 priority);
    void SetVoiceDropMode(VoiceDropMode mode);
    void UpdateParams();
    void UpdateStatus(s32 id, const DspsndChannelPlayVars* pVars);
    void UpdateWaveBufferList();
};

}
}
}