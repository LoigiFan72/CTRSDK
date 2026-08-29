#pragma once

#include <nn/types.h>
#include <nn/os/os_Synchronization.h>
#include <nn/os/os_MemoryBlock.h>

namespace nn{
namespace os{

class TransferMemoryBlock : public MemoryBlockBase, public HandleObject{
public:
    bool m_SpaceAllocated;
    s8 reseversed1[3];
    int reseversed2;

public:
    TransferMemoryBlock(): 
        m_SpaceAllocated(false) 
    {}
    ~TransferMemoryBlock(){ this->Finalize(); }
    void Initialize(void* p, size_t size, bit32 myPermission, bit32 otherPermission);
    Result TryInitialize(void* p,size_t size,bit32 myPermission,bit32 otherPermission);
    void Finalize();
        
private:
    void Unmap(void);
};

}
}