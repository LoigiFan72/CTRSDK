#pragma once

#include <nn/Result.h>
#include <nn/snd/CTR/Common/snd_Types.h>
#include <nn/snd/CTR/MPCore/snd_Voice.h>
#include <nn/os/os_LightEvent.h>

namespace nn {
namespace snd {
namespace CTR {
    static const s32 NN_SND_DSP_MAXIMUM_CYCLES = 622535;

    Result Initialize();
    void InitializeWaveBuffer(WaveBuffer * pWaveBuffer);
    Result Finalize();
    void WaitForDspSync();
    void WaitForDspSync(nn::os::Tick* pTick);
    void SendParameterToDsp();
    void Sleep();
    void WakeUp();
    void OrderToWaitForFinalize();
    bool GetHeadphoneStatus();
    void SetOutputBufferCount(s32 outputBufferCount);
    bool SetSurroundDepth(f32 depth);
    void SetSurroundSpeakerPosition(SurroundSpeakerPosition pos);
    f32 GetSystemMasterVolume();
    void SetMasterVolume(f32 fVolume);
    void SetAuxReturnVolume(AuxBusId id, f32 fVolume);
    void ClearEffect(AuxBusId busId);
    void DecodeAdpcmData(const u8* pInput, s16* pOutput, const AdpcmParam& param, AdpcmContext& context, s32 nSamples);
    Result StartSoundThread(const ThreadParameter* mainThreadParam,void (*mainThreadCallback)(uptr),uptr mainThreadArg,const ThreadParameter* userThreadParam,void (*userThreadCallback)(uptr),uptr userThreadArg,s32 coreNo);
    void FinalizeSoundThread();
    os::Tick GetSoundThreadTick();
    void EnableSoundThreadTickCounter(bool enable);
} // namespace CTR
} // namespace snd
} // namespace nn