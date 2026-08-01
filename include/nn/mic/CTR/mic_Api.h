#pragma once

#include <nn/types.h>
#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/mic/CTR/mic_Types.h>

#define NN_MIC_AMP_GAIN_MAX              119
#define NN_MIC_AMP_GAIN_MIN                0
#define NN_MIC_BUFFER_ALIGNMENT_SIZE    4096
#define NN_MIC_BUFFER_ALIGNMENT_ADDRESS 4096

namespace nn{
namespace mic{
namespace CTR{
namespace detail{

Result InitializeBase(Handle* pSession, const char* name);
Result FinalizeBase(Handle* pSession);

} // detail

Result Initialize();
Result Finalize();
Result SetBuffer(void* p, size_t size);
Result StartSampling(CTR::SamplingType type, CTR::SamplingRate rate, int, uint);
Result StopSampling();
Result ResetBuffer();
Result GetSamplingBufferSize(uint size);

}
}
}