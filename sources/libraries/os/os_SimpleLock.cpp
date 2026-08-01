// Filename: os_SimpleLock.cpp
//
// Project: Horizon

#include <nn/os/os_SimpleLock.h>
#include <nn/svc.h>

namespace nn{
namespace os{

namespace{
        struct ReverseIfPositiveUpdater{
            bool operator()(s32& x){
                if(x > 0){
                    x = -x;
                    return true;
                }
                else{
                    return false;
                }
            }
        };

        struct ReverseUpdater{
            s32 afterUpdate;
            bool operator()(s32& x){
                x = -x;
                afterUpdate = x;
                return true;
            }
        };

        struct DecrementIfNegativeUpdater{
            bool operator()(s32& x){
            if( x < 0 ){
                --x;
                return true;
            }
            else{
                return false;
            }
        }
    };

        struct ReverseAndIncrementIfPositiveUpdater{
            bool operator()(s32& x){
            if( x > 0 ){
                x = -x + 1;
                return true;
                }
            else{
                return false;
            }
        }
    };
}

void SimpleLock::Initialize(void) {
    *this->mCounter = 1; // ultimate ASM this creates lmao
}

void SimpleLock::LockImpl(){
    for(;;){
        DecrementIfNegativeUpdater incrementNumWaiterIfLocked;

        if(this->mCounter->AtomicUpdateConditional(incrementNumWaiterIfLocked)){
            break;
        }

        if(TryLock()){
            return;
        }
    }

    for(;;){
        this->mCounter.WaitIfLessThan(0);

        ReverseAndIncrementIfPositiveUpdater decrementNumWaiterAndLockIfUnlocked;

        if(this->mCounter->AtomicUpdateConditional(decrementNumWaiterAndLockIfUnlocked)){
            break;
        }
    }
}

void SimpleLock::Lock(){
    if(!TryLock()){
        LockImpl();
    }
}

bool SimpleLock::TryLock(){
    ReverseIfPositiveUpdater updater;
    return this->mCounter->AtomicUpdateConditional(updater);
}

void SimpleLock::Unlock(){
    ReverseUpdater updater;
    this->mCounter->AtomicUpdateConditional(updater);

    if(updater.afterUpdate > 1){
        this->mCounter.Signal(1);
    }
}

}
}