#pragma once

#include <nn/types.h>
#include <nn/Handle.h>
#include <nn/os/os_MemoryBlockBase.h>
#include <nn/os/os_Types.h>

#define NN_OS_MEMORY_PAGE_SIZE 0x1000

namespace nn{
namespace os{
namespace detail{

bool IsMemoryBlockEnabled();
uptr AllocateFromMemoryBlockSpace(MemoryBlockBase* p, size_t size);
void FreeToMemorySpace(MemoryBlockBase* p);
void Switch(nnosMemoryBlockBase* pTo, nnosMemoryBlockBase* pFrom);

} // detail

class MemoryBlock : public MemoryBlockBase{
    void Initialize(size_t pSize);
    void Finalize();
    ~MemoryBlock();
};

static size_t GetPageAlignedSize(size_t size) {
    return (size + NN_OS_MEMORY_PAGE_SIZE - 1) & ~(NN_OS_MEMORY_PAGE_SIZE - 1); 
}

void InitializeMemoryBlock(uptr begin, size_t size);

namespace{
bool s_IsMemoryBlockEnabled;

} // namespace
}
}

typedef union nnosMemoryBlock{
    char buf[20];
    uint alignment_holder;
} nnosMemoryBlock;

extern "C"{
    void nnosMemoryBlockAllocate(nnosMemoryBlock* p, size_t size);
    uptr nnosMemoryBlockGetAddress(nnosMemoryBlock* p);
}