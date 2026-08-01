// Filename: os_LightEvent.cpp
//
// Project: Horizon

#include <nn/os/os_LightEvent.h>
#include <nn/svc.h>

namespace nn{
namespace os{

void LightEvent::Initialize(bool pIsManualReset){
    this->mLock.Initialize();
    *this->mCounter = pIsManualReset ? NOT_RESETED_MANUAL: NOT_RESETED_AUTO;
}

void LightEvent::ClearSignal(){
    if(*this->mCounter == RESETED_MANUAL){
        SimpleLock::ScopedLock lock(this->mLock);
        *this->mCounter = NOT_RESETED_MANUAL;
    }
    else if(*this->mCounter == RESETED_AUTO){
        *this->mCounter = NOT_RESETED_AUTO;
    }
}

void LightEvent::Wait(){
    for(;;){
        switch (*this->mCounter){
            case NOT_RESETED_MANUAL:
                    this->mCounter.WaitIfLessThan(0);
                    return;
            case RESETED_MANUAL:
                    return;
            case NOT_RESETED_AUTO:
                    break;
            case RESETED_AUTO:
                if (this->mCounter->CompareAndSwap (RESETED_AUTO, NOT_RESETED_AUTO) == RESETED_AUTO) {
                    return;
                }
                break;
            }
        this->mCounter.WaitIfLessThan (0);
    }
}

void LightEvent::Signal() {
    if(*this->mCounter == NOT_RESETED_AUTO){
        *this->mCounter = RESETED_AUTO;
        this->mCounter.Signal(1);
    }
    else if(*this->mCounter == NOT_RESETED_MANUAL){
        SimpleLock::ScopedLock lock(this->mLock);
        *this->mCounter = RESETED_MANUAL;
        this->mCounter.SignalAll();
    }
}

bool LightEvent::TryWait(){
    if(*this->mCounter == RESETED_MANUAL ){
        return true;
    }
    else{
        return this->mCounter->CompareAndSwap(RESETED_AUTO, NOT_RESETED_AUTO) == RESETED_AUTO;
    }
}

} // os
} // nn