#pragma once

#include <nn/snd/CTR/Common/snd_Types.h>

namespace nn{
namespace snd{
namespace CTR{

class FxDelay{
public:
    FxDelay();
    virtual ~FxDelay();
    
    struct Param{
        u32 mDelayTime;
        f32 mFeedbackGain;  
        f32 mDamping;
        bool mIsEnableSurround;
        Param(): mDelayTime(250), mFeedbackGain(0.4f), mDamping(0.5f),mIsEnableSurround(false){}
    };

    void UpdateBuffer(uptr data);
    bool SetParam(const FxDelay::Param& param);
    size_t GetRequiredMemSize();
    bool AssignWorkBuffer(uptr buffer, size_t size);
    void ReleaseWorkBuffer();
    bool Initialize();
    void Finalize();

    const Param& GetParam() const{
        return mParam;
    }
private:
    void AllocBuffer();
    void FreeBuffer();
    void InitializeParam();

    struct WorkBuffer{
        s32* mDelay[4];
        s32  mLpf[4];
    };
    
    Param mParam;
    uptr mpBuffer;
    size_t mBufferSize;
    WorkBuffer mWorkBuffer;
    u32 mDelayFrames;
    u32 mCurFrame;
    s32 mFeedbackGain;
    s32 mLpfCoef1;
    s32 mLpfCoef2;
    u32 mDelayTimeAtInitialize;
    bool mIsEnableSurroundAtInitialize;
    u8 mProcessChannelCount;
    bool mIsActive;

};

}
}
}