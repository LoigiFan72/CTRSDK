#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/mic/CTR/mic_Types.h>



namespace nn {
namespace mic {
namespace CTR {
namespace detail {

class Mic{
public:
    static Handle sSession;

    static Result AllocateBuffer(Handle handle, size_t size);
    static Result FreeBuffer();
    static Result StartSampling(mic::CTR::SamplingType type, mic::CTR::SamplingRate rate, s32 offset, size_t size, bool loop);
    static Result StopSampling();
    static Result IsSampling(bool* pSampling);
    static Result SetPGAB(u8 gain);
    static Result GetPGAB(u8* pGain);
    static Result SetMicBias(bool isOn);
    static Result GetMicBias(bool* pIsOn);
    static Result SetClamp(bool isOn);
    static Result SetIirFilterMic(const u8 pParam[], size_t filterSize);
};

}
}
}
}