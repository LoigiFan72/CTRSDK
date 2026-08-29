// Filename: os_SharedMemory.cpp
//
// Project: Horizon

#include <nn/os/os_SharedMemory.h>
#include <nn/os/os_AddressSpaceManager.h>
#include <nn/os/os_ErrorHandlerSelect.h>
#include <nn/os/os_Result.h>
#include <nn/os/os_Types.h>
#include <nn/svc.h>
#include <nn/util/util_Result.h>

namespace nn{
namespace os{
namespace{
nnosAddressSpaceManager s_SpaceManager;

}

namespace detail{
void InitializeSharedMemory(){
    nnosAddressSpaceManagerInitialize(&s_SpaceManager,0x10000000,0x4000000);
}

uptr AllocateFromSharedMemorySpace(MemoryBlockBase* p, size_t s){
    AddressSpaceManager* pManager = reinterpret_cast<AddressSpaceManager*>(&s_SpaceManager);
    return pManager->Allocate(p, s, NN_OS_MEMORY_PAGE_SIZE);
}

void FreeToSharedMemorySpace(MemoryBlockBase* p){
    AddressSpaceManager* pManager = reinterpret_cast<AddressSpaceManager*>(&s_SpaceManager);
    pManager->Free(p);
}

}

Result SharedMemoryBlock::AttachAndMap(nn::Handle handle, size_t size, bool readOnly){
    size = GetPageAlignedSize(size);

    this->SetHandle(handle);
    return this->Map(size, readOnly);
}

Result SharedMemoryBlock::Map(size_t size, bool readOnly){
    NN_TASSERTMSG_(GetAddress() == NULL, "This SharedMemoryBlock instance has been already initialized.\n" );
    if (GetAddress()){
        return ResultAlreadyInitialized();
    }

    if ((size % NN_OS_MEMORY_PAGE_SIZE) != 0){
        return ResultMisalignedSize();
    }

    uptr addr = os::detail::AllocateFromSharedMemorySpace(this, size);
    if (addr == NULL){
        return ResultNoAddressSpace();
    }

    this->MemoryBlockBase::SetReadOnly(readOnly);

    Result result;
    const bit32 myPermission = readOnly ? MEMORY_PERMISSION_READ : (MEMORY_PERMISSION_READ | MEMORY_PERMISSION_WRITE);
    result = nn::svc::MapMemoryBlock(GetHandle(), addr, myPermission, os::MEMORY_PERMISSION_DONT_CARE);
    NN_UTIL_RETURN_IF_FAILED(result);
    mSpaceAllocated = true;
    
    return result;
}

void SharedMemoryBlock::Unmap(){
    if (GetAddress() != NULL){
        if(mSpaceAllocated){
            NN_OS_ERROR_IF_FAILED(nn::svc::UnmapMemoryBlock(GetHandle(), GetAddress()));
            os::detail::FreeToSharedMemorySpace(this);
        }
        else{
            SetAddressAndSize(0, 0);
        }
    }
}

void SharedMemoryBlock::Finalize(){
    if (this->IsValid()){
        this->Unmap();
        this->HandleObject::Close();
    }
}

}
}