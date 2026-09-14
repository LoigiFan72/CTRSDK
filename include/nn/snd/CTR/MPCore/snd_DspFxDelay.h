#pragma once

#include <nn/snd/CTR/Common/snd_Types.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/util/util_SizedEnum.h>

namespace nn {
namespace snd {
namespace CTR {

class DspFxDelay : private nn::util::NonCopyable<DspFxDelay> {
public:
    struct Param 
    {
        u32  m_DelayTime;
        f32  m_FeedbackGain;
        f32  m_Damping;
        bool m_IsEnableSurround;
        u8 rev[3];

        Param(): 
            m_DelayTime(250), 
            m_FeedbackGain(0.4f), 
            m_Damping(0.5f), 
            m_IsEnableSurround(false) 
        {
        }
    };

public:
    DspFxDelay();
    ~DspFxDelay();
    static size_t GetRequiredMemorySize(const DspFxDelay::Param& param);
    bool Initialize(uptr buffer, size_t size);
    void Finalize();
    bool Attach(AuxBusId id);
    void Detach();
    bool SetParam(const DspFxDelay::Param& _param);
    bool Enable(bool enable = true);
    bool Disable(){return Enable(false); }
    bool IsEnabled() { return m_IsEnabled;  }
    bool IsBufferInUse();
private:
    bool AssignWorkBuffer(uptr buffer, size_t size);
    void ReleaseWorkBuffer();
    
protected:
    uptr m_Buffer;
    uptr m_BufferPhysical;
    size_t m_BufferSize;
    bool m_IsInitialized;
    util::SizedEnum1<AuxBusId> m_AuxBusId;
    bool m_IsEnabled;
    s8 m_ProcessCount;
};

class DspFxDelayParams{
public:
    ushort ctrl;
    ushort enable;
    ushort rev;
    short channels;
    uint delayBufferAddress;
    short delayFrames;
    ushort delayFeedbackGain;
    short aLpfCoefs[2];
};

} // namespace CTR
} // namespace snd
} // namespace nn