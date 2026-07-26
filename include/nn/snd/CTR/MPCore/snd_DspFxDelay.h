#pragma once

#include <nn/snd/CTR/Common/snd_Types.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/util/util_SizedEnum.h>

namespace nn {
namespace snd {
namespace CTR {

class DspFxDelay : private nn::util::NonCopyable<DspFxDelay> {
public:
    struct Param {
        u32  mDelayTime;
        f32  mFeedbackGain;
        f32  mDamping;
        bool mIsEnableSurround;
        u8 rev[3];

        Param() : mDelayTime(250), mFeedbackGain(0.4f), mDamping(0.5f), mIsEnableSurround(false) {}
    };

public:
    DspFxDelay();
    ~DspFxDelay();
    static size_t GetRequiredMemorySize(const DspFxDelay::Param& param);
    bool Initialize(uptr buffer, size_t size);
    void Finalize();
    bool Attach(AuxBusId id);
    void Detach();
    bool SetParam(const DspFxDelay::Param& param);
    bool Enable(bool enable = true);
    bool Disable(){return Enable(false); }
    bool IsEnabled() { return mIsEnabled;  }
    bool IsBufferInUse();
private:
    bool AssignWorkBuffer(uptr buffer, size_t size);
    void ReleaseWorkBuffer(){ this->mBuffer = 0; this->mBufferPhysical = 0; this->mBufferSize = 0; }
    
protected:
    uptr mBuffer;
    uptr mBufferPhysical;
    size_t mBufferSize;
    bool mIsInitialized;
    util::SizedEnum1<AuxBusId> mAuxBusId;
    bool mIsEnabled;
    s8 mProcessCount;
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