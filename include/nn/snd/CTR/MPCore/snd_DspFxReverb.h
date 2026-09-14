#pragma once

#include <nn/snd/CTR/Common/snd_Types.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/util/util_SizedEnum.h>

namespace nn {
namespace snd {
namespace CTR {

class DspFxReverb : private nn::util::NonCopyable<DspFxReverb> 
{
public:
    struct FilterSize 
    {
        u32 m_Comb0;
        u32 m_Comb1;
        u32 m_AllPass;
    };

    struct Param 
    {
        u32         m_EarlyReflectionTime;
        u32         m_FusedTime;
        u32         m_PreDelayTime;
        f32         m_Coloration;
        f32         m_Damping;
        FilterSize* m_pFilterSize;
        f32         m_EarlyGain;
        f32         m_FusedGain;
        bool        m_UseHpfDamping;
        s8 rev[3];

        Param();
    };

public:
    DspFxReverb();
    ~DspFxReverb();

    static size_t GetRequiredMemorySize(const DspFxReverb::Param& param);
    bool Initialize(uptr buffer, size_t size);
    void Finalize();
    bool Attach(AuxBusId id);
    void Detach();
    bool SetParam(const DspFxReverb::Param& param);
    bool Enable(bool enable = true);

    bool Disable(){ return Enable(false); }
    bool IsEnabled() { return m_IsEnabled;  }

    bool IsBufferInUse();

private:
    bool AssignWorkBuffer(uptr buffer, size_t size);
    void ReleaseWorkBuffer();

    static FilterSize s_DefaultFilterSize;

    uptr m_Buffer;
    uptr m_BufferPhysical;
    size_t m_BufferSize;
    bool m_IsInitialized;
    util::SizedEnum1<AuxBusId> m_AuxBusId;
    bool m_IsEnabled;
    s8 m_ProcessCount;
};

struct DspFxReverbParams {
    u16  ctrl;                    // 0x00
    u16  enable;                  // 0x02
    s16  padding;                 // 0x04
    s16  channels;                // 0x06
    u32  earlyDelayBufferAddress; // 0x08
    u32  preDelayBufferAddress;   // 0x0c
    u32  combBufferAddress[2];    // 0x10
    u32  allPassBufferAddress;    // 0x18
    s16  earlyDelayFrames;        // 0x1c
    s16  preDelayFrames;          // 0x1e
    s16  combFrames[2];           // 0x20
    s16  allPassFrames;           // 0x24
    u16  earlyGain;               // 0x26
    u16  fusedGain;               // 0x28
    u16  allPassCoef;             // 0x2a
    u16  aCombCoefs[2];           // 0x2c
    s16  aLpfCoefs[2];            // 0x30
};                                // size: 0x34

} // namespace CTR
} // namespace snd
} // namespace nn