#pragma once

#include <nn/types.h>
#include <nn/Result.h>
#include <nn/os/os_Synchronization.h>
#include <nn/os/os_MemoryBlock.h>

namespace nn{
namespace os{
namespace detail{
    
uptr AllocateFromSharedMemorySpace(os::MemoryBlockBase* p, size_t s);
void InitializeSharedMemory();
void FreeToSharedMemorySpace(os::MemoryBlockBase* p);
    
}

class SharedMemoryBlock : public MemoryBlockBase, public HandleObject
{
public:
    SharedMemoryBlock():
        MemoryBlockBase(),
        HandleObject(),
        m_SpaceAllocated(false)
    {
    }

    ~SharedMemoryBlock()
    {
        Finalize();
    }

    Result AttachAndMap(Handle handle, size_t size, bool readOnly);
    Result Map(size_t size, bool readOnly);
    void Unmap();
    void Finalize();
private:
    bool m_SpaceAllocated;
    s8 reversed[3];
    int reversed2;
};

}
}