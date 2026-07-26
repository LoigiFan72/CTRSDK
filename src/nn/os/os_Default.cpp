// Filename: os_Default.cpp
//
// Project: Horizon

#include <nn/Result.h>
#include <nn/os/os_Default.h>
#include <nn/os.h>
#include <nn/math.h>

namespace nn{
namespace os{

void SetDefaultAutoStackManager(){
    Thread* thread;
    thread->nn::os::Thread::SetAutoStackManager(spAutoStackManager);
}

namespace{

DefaultAutoStackManager::~DefaultAutoStackManager(){
if (this->mInitialized){
        mDestructorMutex.Finalize();
        mInitialized = false;
    }
}

void* DefaultAutoStackManager::Construct(size_t stackSize){
    if (!mInitialized){
        Initialize();
    }
                
    nnosStackMemoryBlock stackBlock;
    nnosStackMemoryBlockAllocate(&stackBlock, stackSize);

    uptr stackBottom = nnosStackMemoryBlockGetStackBottom(&stackBlock);
    stackBottom -= math::RoundUp(sizeof(nnosStackMemoryBlock), 8);

    nnosStackMemoryBlock* pBlockOnStack = reinterpret_cast<nnosStackMemoryBlock*>(stackBottom);

    nnosStackMemoryBlockInitialize(pBlockOnStack);
    nn::os::detail::Switch(reinterpret_cast<nnosMemoryBlockBase*>(pBlockOnStack),reinterpret_cast<nnosMemoryBlockBase*>(&stackBlock));

    return reinterpret_cast<void*>(stackBottom);
}

void DefaultAutoStackManager::Destruct(void* pStackBottom, bool isError){
    if(isError){
        this->FreeStack(reinterpret_cast<nnosStackMemoryBlock*>(pStackBottom));
    }
    else{
        this->mDestructorMutex.Lock();
        InvokeOnOtherStack(this->mDestructorStack.GetStackBottom(), &FreeStack, pStackBottom, __return_address());
    }
}

void DefaultAutoStackManager::FreeStack(void* pStackBottom){
    nnosStackMemoryBlockFree((nnosStackMemoryBlock*)pStackBottom);
}

__asm void InvokeOnOtherStack(uptr stackBottom,void (*f)(void*),void* param ,uptr returnAddr){
    mov         sp, r0
    mov         r0, r2
    mov         lr, r3
    bx          r1
}

}
}
}