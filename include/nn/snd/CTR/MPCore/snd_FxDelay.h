#pragma once

#include <nn/snd/CTR/Common/snd_Types.h>

namespace nn{
namespace snd{
namespace CTR{

class FxDelay{
public:
    FxDelay();
    virtual ~FxDelay();
    
    struct Param
    {
        u32 m_DelayTime;
        f32 m_FeedbackGain;  
        f32 m_Damping;
        bool m_IsEnableSurround;
        Param(): 
            m_DelayTime(250), 
            m_FeedbackGain(0.4f), 
            m_Damping(0.5f),
            m_IsEnableSurround(false)
        {
        }
    };

    void UpdateBuffer(uptr data);
    bool SetParam(const FxDelay::Param& param);
    size_t GetRequiredMemSize();
    bool AssignWorkBuffer(uptr buffer, size_t size);
    void ReleaseWorkBuffer();
    bool Initialize();
    void Finalize();

    const Param& GetParam() const
    {
        return m_Param;
    }
private:
    void AllocBuffer();
    void FreeBuffer();
    void InitializeParam();

    struct WorkBuffer
    {
        s32* m_Delay[4];
        s32  m_Lpf[4];
    };
    
    Param m_Param;
    uptr m_pBuffer;
    size_t m_BufferSize;
    WorkBuffer m_WorkBuffer;
    u32 m_DelayFrames;
    u32 m_CurFrame;
    s32 m_FeedbackGain;
    s32 m_LpfCoef1;
    s32 m_LpfCoef2;
    u32 m_DelayTimeAtInitialize;
    bool m_IsEnableSurroundAtInitialize;
    u8 m_ProcessChannelCount;
    bool m_IsActive;

};

}
}
}