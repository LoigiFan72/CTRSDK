#pragma once

#include <nn/types.h>
#include <nn/Result.h>

namespace nn{
namespace hio{
namespace CTR{

Result Initialize(void* pDeviceMemory);
Result Finalize();

}
}
}