#pragma once

#include <nn/types.h>
#include <nn/Result.h>
#include <nn/os/CTR/MPCore/os_MemoryMap.h>

namespace nn{
namespace os{
namespace detail{
    inline uptr GetHeapAddressWithoutCheck(){
        return 0x08000000;
    }
}
    uptr GetDeviceMemoryAddress();
    size_t GetAppMemorySize();

    size_t GetDeviceMemorySize();
    Result SetDeviceMemorySize(size_t size);
    void SetupHeapForMemoryBlock(size_t size);
    size_t GetHeapSize();
    uptr GetDeviceMemoryAddress();
    uptr GetCodeRegionAddress();
    size_t GetCodeRegionSize();
    size_t GetUsingMemorySize();

}
}