// Filename: os_Default.cpp
//
// Project: Horizon

#include <nn/Result.h>
#include <nn/os/os_Default.h>
#include <nn/os.h>
#include <nn/math.h>

namespace nn{
namespace os{

static AutoStackManager* spAutoStackManager;

void SetDefaultAutoStackManager(){
    Thread::SetAutoStackManager(spAutoStackManager);
}

namespace{

class DefaultAutoStackManager : public os::AutoStackManager{
protected:
    static const size_t DESTRUCTOR_STACK_SIZE = 512;

    os::Mutex m_DestructorMutex;
    bool m_Initialized;
    s8 rev[3];
    s32 rev2;
    os::StackBuffer<DESTRUCTOR_STACK_SIZE>  m_DestructorStack;
public:
    DefaultAutoStackManager():
        mInitialized(false)
    {}

    virtual ~DefaultAutoStackManager(){
        if (m_Initialized){
            this->m_DestructorMutex.Finalize();
            m_Initialized = false;
        }
    }

    virtual void* Construct(size_t stackSize){
        if (!m_Initialized){
            this->Initialize();
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

    virtual void Destruct(void* pStackBottom, bool isError){
        if(isError){
            this->FreeStack(reinterpret_cast<nnosStackMemoryBlock*>(pStackBottom));
        }
        else{
            this->m_DestructorMutex.Lock();
            this->InvokeOnOtherStack(m_DestructorStack.GetStackBottom(), &FreeStack, pStackBottom, __return_address());
        }
    }
private:
    void Initialize(void){
        this->m_DestructorMutex.Initialize(false);
        m_Initialized = true;
    }
    static void FreeStack(void* pStackBottom){
        nnosStackMemoryBlock* pBlockOnStack = reinterpret_cast<nnosStackMemoryBlock*>(pStackBottom);
        nnosStackMemoryBlockFree(pBlockOnStack);
    }

    static asm void InvokeOnOtherStack(uptr stackBottom,void (*f)(void*),void* param ,uptr returnAddr){
        mov         sp, r0
        mov         r0, r2
        mov         lr, r3
        bx          r1 
    }

};

DefaultAutoStackManager sAutoStackManager;

}
}
}