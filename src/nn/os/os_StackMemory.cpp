// Filename: os_StackMemory.cpp
//
// Project: Horizon Decompilation

#include <nn/os/os_StackMemory.h>
#include <nn/os/os_AddressSpaceManager.h>
#include <nn/os/os_MemoryBlock.h>
#include <nn/os/os_Memory.h>
#include <nn/os/CTR/os_ErrorHandler.h>

namespace nn{
namespace os{
extern nnosAddressSpaceManager sSpaceManager;

namespace detail{

void InitializeStackMemory(){
    nnosAddressSpaceManagerInitialize(&os::sSpaceManager,0xe000000, 0x2000000);
}

void Switch(nnosMemoryBlockBase* pTo, nnosMemoryBlockBase* pFrom){
    nnosAddressSpaceManagerSwitch(&os::sSpaceManager,pTo,pFrom);
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