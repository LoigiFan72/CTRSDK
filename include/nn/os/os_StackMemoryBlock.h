#pragma once

#include <nn/os/os_MemoryBlockBase.h>

namespace nn{
namespace os{
namespace detail{
    void InitializeStackMemory();
    void Switch(nnosMemoryBlockBase* pTo, nnosMemoryBlockBase* pFrom);
}

class StackMemoryBlock : public MemoryBlockBase
{
public:
    StackMemoryBlock():
        MemoryBlockBase()
    {
    }
    
    StackMemoryBlock(size_t size): 
        MemoryBlockBase()
    {
        Initialize(size)
    }

    void Initialize(size_t size);

    uptr GetStackBottom() const
    {
        return GetAddress() + GetSize();
    }
private:
    uptr m_MemoryAddress;
};

}
}

typedef union nnosStackMemoryBlock{
    char buf[20];
    uint alignment_holder;
} nnosStackMemoryBlock;

extern "C"{
    
void nnosStackMemoryBlockAllocate(nnosStackMemoryBlock* p, size_t size);
void nnosStackMemoryBlockFree(nnosStackMemoryBlock* p);
uptr nnosStackMemoryBlockGetStackBottom(nnosStackMemoryBlock* p);
void nnosStackMemoryBlockInitialize(nnosStackMemoryBlock* p);

}