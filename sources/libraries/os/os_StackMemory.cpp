// Filename: os_StackMemory.cpp
//
// Project: Horizon

#include <nn/os/os_StackMemory.h>
#include <nn/os/os_AddressSpaceManager.h>
#include <nn/os/os_MemoryBlock.h>
#include <nn/os/os_Memory.h>
#include <nn/os/CTR/os_ErrorHandler.h>

namespace nn{
namespace os{

nnosAddressSpaceManager s_SpaceManager;

namespace detail{

void InitializeStackMemory(){
    nnosAddressSpaceManagerInitialize(&s_SpaceManager,0xe000000, 0x2000000);
}

void Switch(nnosMemoryBlockBase* pTo, nnosMemoryBlockBase* pFrom){
    nnosAddressSpaceManagerSwitch(&s_SpaceManager,pTo,pFrom);
}

}
}
}

using namespace nn::os;

void nnosStackMemoryBlockAllocate(nnosStackMemoryBlock* p, size_t size){
//    NN_TASSERT_(os::detail::IsMemoryBlockEnabled());
//    new (p) StackMemoryBlock(size);
}

void nnosStackMemoryBlockFree(nnosStackMemoryBlock* p){
    StackMemoryBlock* pStackMemoryBlock = reinterpret_cast<StackMemoryBlock*>(p);
    pStackMemoryBlock->~StackMemoryBlock();
}

uptr nnosStackMemoryBlockGetStackBottom(nnosStackMemoryBlock* p){
    StackMemoryBlock* pStackMemoryBlock = reinterpret_cast<StackMemoryBlock*>(p);
    return pStackMemoryBlock->GetStackBottom();
}

void nnosStackMemoryBlockInitialize(nnosStackMemoryBlock* p){
    new (p) StackMemoryBlock();
}