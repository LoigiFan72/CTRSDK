// Filename: os_Memory.cpp
//
// Project: Horizon

#include <nn/os.h>
#include <nn/assert.h>
#include <nn/svc.h>
#include <nn/dbg.h>
#include <nn/util.h>
#include <nn/init.h>
#include <nn/svc.h>
#include <cstdint>

extern "C" bit8 Load$$LR$$TEXT_SECTION$$Base[];
extern "C" bit8 Load$$LR$$TEXT_SECTION$$Length[];

namespace nn{
namespace os{
namespace {
    uptr s_DeviceMemoryAddress = 0;
    size_t s_DeviceMemorySize = 0;
    size_t s_HeapSize = 0;
}

uptr GetDeviceMemoryAddress(){
    NN_TASSERTMSG_(s_DeviceMemoryAddress != NULL, "Device Memory is NOT Initialized.");
    return s_DeviceMemoryAddress;
}

Result SetDeviceMemorySize(size_t size){
    if ((size % NN_OS_MEMORY_PAGE_SIZE) != 0){
        return ResultMisalignedSize();
    }
    Result res;

    if(size > s_DeviceMemorySize){
        if (s_DeviceMemorySize != 0){
            if (!(s_DeviceMemorySize % (1024 * 1024) == 0 && size % (1024 * 1024) == 0)){
                    return ResultMisalignedSize();
            }
        }
        uptr addr;
        const size_t mapSize = size - s_DeviceMemorySize;
        const uptr requestAddress = (s_DeviceMemorySize == 0) ? NULL: (s_DeviceMemoryAddress + s_DeviceMemorySize);

        res = nn::svc::ControlMemory(&addr,requestAddress,NULL,mapSize, (nn::os::MEMORY_OPERATION_COMMIT | nn::os::MEMORY_OPERATION_FLAG_LINEAR),nn::os::MEMORY_PERMISSION_READ_WRITE);

        if(res.IsSuccess() ){
            NN_TASSERT_(sDeviceMemorySize == 0 || addr == requestAddress);

            if(s_DeviceMemorySize == 0 ){
                s_DeviceMemoryAddress   = addr;
            }

            s_DeviceMemorySize  = size;
        }
    }
    else{
        uptr addr;
        const size_t unmapSize = s_DeviceMemorySize - size;
        const uptr freeAddress = s_DeviceMemoryAddress + size;

        res = nn::svc::ControlMemory(&addr,freeAddress,NULL,unmapSize,MEMORY_OPERATION_FREE,MEMORY_PERMISSION_NONE);

        if(res.IsSuccess()){
            if(size == 0){
                s_DeviceMemoryAddress   = NULL;
            }

            s_DeviceMemorySize  = size;
        }
    }

    return res;
}

size_t GetDeviceMemorySize(){
    return s_DeviceMemorySize;
}

Result SetHeapSize(size_t size){
    NN_TASSERTMSG_(!nninitIsStartUpDefaultUsing(),"Using default nninitStartUp.\nPlease override nninitStartUp.");

    if ((size % NN_OS_MEMORY_PAGE_SIZE) != 0){
        return ResultMisalignedSize();
    }
    Result res;

    if(size > s_HeapSize){
        uptr addr;
        const size_t mapSize = size - s_HeapSize;
        const uptr requestAddress = 0x08000000 + s_HeapSize;

        res = nn::svc::ControlMemory(&addr,requestAddress,NULL,mapSize,nn::os::MEMORY_OPERATION_COMMIT,nn::os::MEMORY_PERMISSION_READ_WRITE);

        if(res.IsSuccess()){
            NN_TASSERT_(addr == requestAddress);
            s_HeapSize  = size;
        }
    }
    else{
        uptr addr;
        const size_t unmapSize = s_HeapSize - size;
        const uptr freeAddress = 0x08000000 + size;

        res = nn::svc::ControlMemory(&addr,freeAddress,NULL,unmapSize,nn::os::MEMORY_OPERATION_FREE,nn::os::MEMORY_PERMISSION_NONE);

        if(res.IsSuccess()){
            s_HeapSize  = size;
        }
    }

    return res;
}

uptr GetHeapAddress(){
    NN_TASSERTMSG_(!nninitIsStartUpDefaultUsing(),"Using default nninitStartUp.\nPlease override nninitStartUp.");
    return nn::os::detail::GetHeapAddressWithoutCheck();
}

size_t GetHeapSize(){
    NN_TASSERTMSG_(!nninitIsStartUpDefaultUsing(),"Using default nninitStartUp.\nPlease override nninitStartUp.");
    return s_HeapSize;
}

uptr GetCodeRegionAddress(){
    return reinterpret_cast<uptr>(Load$$LR$$TEXT_SECTION$$Base);
}

size_t GetCodeRegionSize(){
    return reinterpret_cast<size_t>(Load$$LR$$TEXT_SECTION$$Length);
}

void SetupHeapForMemoryBlock(size_t heapSize){
    Result result;

    result = SetHeapSize(heapSize);
    NN_OS_ERROR_IF_FAILED(result);

    InitializeMemoryBlock(GetHeapAddress(), GetHeapSize());

    SetDefaultAutoStackManager();
}

}
}