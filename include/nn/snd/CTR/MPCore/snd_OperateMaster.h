#pragma once

#include <nn/os.h>
#include <nn/snd/CTR/MPCore/snd_PrivateDefinition.h>
#include <nn/snd/CTR/Common/snd_Const.h>
#include <cstring>

namespace nn {
namespace snd {
namespace CTR {
    class Dspsnd;
namespace internal{
    extern Dspsnd sDspsnd;
}

typedef ushort DSPWord;
class Dspsnd
{
private:
    enum COM_PAGE
    {
        COM_PAGE_0 = 0,
        COM_PAGE_1 = 1,
        COM_PAGE_NUM = 2
    };

public:
    Event m_EventInterrupt;
    Event m_EventSemaphore;
    CriticalSection m_CriticalSection;
    OutputCapture* m_pOutputCapture;
    u8 m_SaveData[4224];
    ushort* m_pDirectIdOnShare[COM_PAGE_NUM];
    DspsndMasterDirect* m_pMasterDirectOnShare[COM_PAGE_NUM];
    DspsndMasterStatus* m_pMasterStatusOnShare[COM_PAGE_NUM];
    DspsndChannelDirect* m_pChannelDirectOnShare[COM_PAGE_NUM][NN_SND_VOICE_NUM];
    DspsndChannelStatus* m_pChannelStatusOnShare[COM_PAGE_NUM][NN_SND_VOICE_NUM];
    DspsndChannelOption* m_pChannelOptionOnShare[COM_PAGE_NUM][NN_SND_VOICE_NUM];
    s32* m_pAuxBusOnShare[COM_PAGE_NUM][AUX_BUS_NUM];
    short* m_pMixBusOnShare[COM_PAGE_NUM];
    ushort* m_pCompressorTableOnShare[COM_PAGE_NUM];
    DspsndDspCycles* m_pDspCyclesOnShare[COM_PAGE_NUM];
    bool m_IsInitialized;
    u8 m_ProcessCount;
    ushort m_DirectId;
    ushort m_ReadPage;
    ushort m_WritePage;
    s32 m_DspCyclesLimit;
    DspsndDspCycles m_DspCycles;
    short* m_pSpacialCoeffsOnShare[COM_PAGE_NUM];
    short* m_pDirectionCoeffsSpOnShare[COM_PAGE_NUM];
    short* m_pDirectionCoeffsHpOnShare[COM_PAGE_NUM];
    s32* m_pSurroundIirCoeffsSpOnShare[COM_PAGE_NUM];
    s32* m_pSurroundIirCoeffsHpOnShare[COM_PAGE_NUM];
    bool m_IsAuxCallbackInSendParameterEnabled;
public:
    Dspsnd(){ }
    ~Dspsnd();
    bool AppendChannelNextBuffer(u8 ch_no, WaveBuffer* pWaveBuffer, s32 index);
    bool AssignPCM(u8 ch_no, WaveBuffer* pWaveBuffer, DspsndAudioInfo i);
    bool EnableAuxBus(AuxBusId busId, bool flag);
    void Finalize(bool isSleep);
    int GetDroppedFrameCount();
    Result Initialize(bool isWakeup);
    bool InitializeChannelParameters(u8 ch_no);
    void InitializeVariables(bool isWakeup);
    bool ResetChannelNextBuffer(u8 channelId);
    void SendParameter();
    bool SetAuxFrontBypass(AuxBusId busId, bool flag);
    void SetAuxReturnVolume(AuxBusId busId, f32 volume);
    bool SetChannelAdpcmParam(u8 ch_no, AdpcmParam* pParam);
    bool SetChannelIIRFilter_Biquad(u8 ch_no,s16 n0,s16 n1,s16 n2,s16 d1,s16 d2);
    bool SetChannelIIRFilter_Mono(u8 ch_no,s16 n0,s16 d1);
    bool SetChannelIiRFilterType(u8 ch, FilterType type);
    bool SetChannelMix(u8 ch_no,MixParam *p_channelMix);
    bool SetChannelPlayStart(u8 ch_no);
    bool SetChannelPlayStop(u8 ch_no);
    bool SetChannelRIM(u8 ch_no,DSPWord method,DSPWord coef);
    bool SetChannelSyncCount(u8 ch_no,short sync_count);
    bool SetChannelTimer(u8 ch_no, f32 timer);
    bool SetClippingMode(ClippingMode mode);
    bool SetDspDelayEffect(AuxBusId busId, DspFxDelayParams* params);
    bool SetDspReverbEffect(AuxBusId busId, DspFxReverbParams* params);
    bool SetIsHeadsetConnected(bool isConnected);
    void SetMasterVolume(f32 fVolume);
    void SetOutputBufferCount(s32 n);
    bool SetRearRatio(ushort ratio);
    bool SetSoundOutputMode(OutputMode mode);
    bool SetSurroundDepth(ushort depth);
    bool SetSurroundSpeakerPosition(SurroundSpeakerPosition pos);
    void SetSyncMode(SyncMode mode);
    void SyncFrameData();
    bool UpdateChannelNextBuffer(u8 channelId, WaveBuffer* pWaveBuffer);
    void WaitPipe();
    bool WaitPipe(fnd::TimeSpan timeSpan);

    /* Inlines */

    bool ChangeState(u8 ch_no, bool state);
    bool ChangePlayState(u8 ch_no, bool state);
    s32* GetAuxBusAddr(AuxBusId id){ return this->m_pAuxBusOnShare[this->m_ReadPage][id]; }
    DspsndChannelDirect* GetChannelDirectAddr(u8 ch){ u32 page = this->getCurrentPage(); return this->m_pChannelDirectOnShare[page][ch]; }
    DspsndChannelOption* GetChannelOptionAddr(u8 ch){ return this->m_pChannelOptionOnShare[this->m_WritePage][ch]; }
    DspsndChannelStatus* GetChannelStatusAddr(u8 ch){ return this->m_pChannelStatusOnShare[this->m_ReadPage][ch]; }
    u32 getCurrentPage(){ m_DirectId & 1; }
    ushort* GetDirectIdAddrOnShared(int page){ this->m_pDirectIdOnShare[page]; }
    DspsndDspCycles* GetDspCyclesAddr(){ return this->m_pDspCyclesOnShare[this->m_ReadPage]; }
    s32 GetDspCyclesFrame(){ return this->m_DspCycles.ch0.frame; }
    static Dspsnd& GetInstance(){ return internal::sDspsnd; }
    DspsndMasterDirect* GetMasterDirectAddr(){ return this->m_pMasterDirectOnShare[this->m_WritePage]; }
    DspsndMasterStatus* GetMasterStatusAddr(){ return this->m_pMasterStatusOnShare[this->m_ReadPage]; }
    short* GetMixBusAddr(){ return this->m_pMixBusOnShare[this->m_ReadPage]; }
    s32 GetDspCyclesWhole() { return m_DspCycles.ch0.whole; }
    u32 getNextPage(){ u32 page = this->getCurrentPage(); return page + 1 & 1; }
    bool UpdateSlotId(ushort recvid);
    bool SetIsHeadSet(bool flag);
    void EnableAuxCallbackInSendParameter(bool param){ m_IsAuxCallbackInSendParameterEnabled = param; }
};

inline bool Dspsnd::UpdateSlotId(ushort recvid){
    bool ret = true;
    if(!recvid){
        return ret;
    }
    else{
        m_DirectId = recvid++;
        if(recvid == 0xffff)
            m_DirectId = 2;
        m_ReadPage = m_DirectId & 1;
    }
    return ret;
}

} // namespace CTR
} // namespace snd
} // namespace nn