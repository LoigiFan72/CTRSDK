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
    uptr sDeviceMemoryAddress = 0;
    size_t sDeviceMemorySize = 0;
    size_t sHeapSize = 0;
}

uptr GetDeviceMemoryAddress(){
    NN_TASSERTMSG_(sDeviceMemoryAddress != NULL, "Device Memory is NOT Initialized.");
    return sDeviceMemoryAddress;
}

Result SetDeviceMemorySize(size_t size){
    if ((size % NN_OS_MEMORY_PAGE_SIZE) != 0){
        return ResultMisalignedSize();
    }
    Result res;

    if(size > sDeviceMemorySize){
        if (sDeviceMemorySize != 0){
            if (!(sDeviceMemorySize % (1024 * 1024) == 0 && size % (1024 * 1024) == 0)){
                    return ResultMisalignedSize();
            }
        }
        uptr addr;
        const size_t mapSize = size - sDeviceMemorySize;
        const uptr requestAddress = (sDeviceMemorySize == 0) ? NULL: (sDeviceMemoryAddress + sDeviceMemorySize);

        res = nn::svc::ControlMemory(&addr,requestAddress,NULL,mapSize, (nn::os::MEMORY_OPERATION_COMMIT | nn::os::MEMORY_OPERATION_FLAG_LINEAR),nn::os::MEMORY_PERMISSION_READ_WRITE);

        if(res.IsSuccess() ){
            NN_TASSERT_(sDeviceMemorySize == 0 || addr == requestAddress);

            if(sDeviceMemorySize == 0 ){
                sDeviceMemoryAddress   = addr;
            }

            sDeviceMemorySize  = size;
        }
    }
    else{
        uptr addr;
        const size_t unmapSize = sDeviceMemorySize - size;
        const uptr freeAddress = sDeviceMemoryAddress + size;

        res = nn::svc::ControlMemory(&addr,freeAddress,NULL,unmapSize,nn::os::MEMORY_OPERATION_FREE,nn::os::MEMORY_PERMISSION_NONE);

        if(res.IsSuccess()){
            if(size == 0){
                sDeviceMemoryAddress   = NULL;
            }

            sDeviceMemorySize  = size;
        }
    }

    return res;
}

size_t GetDeviceMemorySize(){
    return sDeviceMemorySize;
}

Result SetHeapSize(size_t size){
    NN_TASSERTMSG_(!nninitIsStartUpDefaultUsing(),"Using default nninitStartUp.\nPlease override nninitStartUp.");

    if ((size % NN_OS_MEMORY_PAGE_SIZE) != 0){
        return ResultMisalignedSize();
    }
    Result res;

    if(size > sHeapSize){
        uptr addr;
        const size_t mapSize = size - sHeapSize;
        const uptr requestAddress = 0x08000000 + sHeapSize;

        res = nn::svc::ControlMemory(&addr,requestAddress,NULL,mapSize,nn::os::MEMORY_OPERATION_COMMIT,nn::os::MEMORY_PERMISSION_READ_WRITE);

        if(res.IsSuccess()){
            NN_TASSERT_(addr == requestAddress);
            sHeapSize  = size;
        }
    }
    else{
        uptr addr;
        const size_t unmapSize = sHeapSize - size;
        const uptr freeAddress = 0x08000000 + size;

        res = nn::svc::ControlMemory(&addr,freeAddress,NULL,unmapSize,nn::os::MEMORY_OPERATION_FREE,nn::os::MEMORY_PERMISSION_NONE);

        if(res.IsSuccess()){
            sHeapSize  = size;
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
    return sHeapSize;
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