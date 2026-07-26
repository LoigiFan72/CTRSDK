#pragma once

#include <nn/types.h>
#include <nn/Result.h>
#include <nn/mic/CTR/mic_Types.h>

namespace nn{
namespace mic{
namespace CTR{
namespace detail{

Result InitializeBase();
Result FinalizeBase();

} // detail

Result SetBuffer(void* buffer, uint);
Result StartSampling(CTR::SamplingType type, CTR::SamplingRate rate, int, uint);
u8* ResetBuffer();
u8* GetSamplingBufferSize(uint size);

}
}
}