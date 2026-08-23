#pragma once

#include <nn/types.h>
#include <nn/snd/CTR/Common/snd_Types.h>
#include <nn/snd/CTR/Common/snd_Const.h>

namespace nn{
namespace snd{
namespace CTR{

class FxReverb{
public:
    struct FilterSize{
        u32  mComb0;
        u32  mComb1;
        u32  mAllPass;
        FilterSize(): 
            mComb0(19 * NN_SND_SAMPLES_PER_FRAME), 
            mComb1(23 * NN_SND_SAMPLES_PER_FRAME), 
            mAllPass(13 * NN_SND_SAMPLES_PER_FRAME)
        {}
    };
    
    struct Param{
        u32 mEarlyReflectionTime;
        u32 mFusedTime;
        u32 mPreDelayTime;
        f32 mColoration;
        f32 mDamping;
        FilterSize* mpFilterSize;
        f32 mEarlyGain;
        f32 mFusedGain;
        bool mUseHpfDamping;

        Param(): 
            mEarlyReflectionTime(60),
            mFusedTime(4000),
            mPreDelayTime(100),
            mDamping(0.4f),
            mpFilterSize(&sDefaultFilterSize),
            mEarlyGain(0.6f),
            mFusedGain(0.4f),
            mUseHpfDamping(false)
        {}
    };

    FxReverb();
    virtual ~FxReverb();
    bool SetParam(const Param& param);
    const Param& GetParam() const{ return mParam; }
    size_t GetRequiredMemSize();
    bool AssignWorkBuffer(uptr buffer, size_t size);
    void ReleaseWorkBuffer();
    bool Initialize();
    void Finalize();
    void UpdateBuffer(uptr data);
private:
    struct WorkBuffer{
        s32* mEarlyReflection[4];
        s32* mPreDelay[4];
        s32* mCombFilter[4][2];
        s32* mAllPassFilter[4];
        s32 mLpf[4];
    };

    static FilterSize sDefaultFilterSize;

    void AllocBuffer();
    void FreeBuffer();
    void InitializeParam();

    Param mParam;
    uptr mpBuffer;
    size_t mBufferSize;
    FilterSize mFilterSize;
    WorkBuffer mWorkBuffer;
    s32 mEarlyLength;
    s32 mEarlyPos;
    s32 mPreDelayLength;
    s32 mPreDelayPos;
    s32 mCombFilterLength[2];
    s32 mCombFilterPos[2];
    s32 mCombFilterCoef[2];
    s32 mAllPassFilterLength;
    s32 mAllPassFilterPos;
    s32 mAllPassFilterCoef;
    s32 mLastLpfOut[4];
    s32 mEarlyGain;
    s32 mFusedGain;
    s32 mLpfCoef1;
    s32 mLpfCoef2;
    u32 mEarlyReflectionTimeAtInitialize;
    u32 mPreDelayTimeAtInitialize;
    FilterSize mFilterSizeAtInitialize;
    bool mIsActive;
};

}
}
}