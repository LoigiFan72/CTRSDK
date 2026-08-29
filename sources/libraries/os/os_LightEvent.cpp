// Filename: os_LightEvent.cpp
//
// Project: Horizon

#include <nn/os/os_LightEvent.h>
#include <nn/svc.h>

namespace nn{
namespace os{

#if NN_VERSION_MAJOR > 2 || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR > 4) ||  (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO > 1)
void LightEvent::Initialize(bool pIsManualReset){
    this->m_Lock.Initialize();
    *this->m_Counter = pIsManualReset ? NOT_RESETED_MANUAL: NOT_RESETED_AUTO;
}
#endif

void LightEvent::ClearSignal(){
    if(*this->m_Counter == RESETED_MANUAL){

    #if NN_VERSION_MAJOR > 2 || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR > 4) || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO > 1)
        SimpleLock::ScopedLock lock(this->m_Lock);
    #endif
        *this->m_Counter = NOT_RESETED_MANUAL;
    }
    else if(*this->m_Counter == RESETED_AUTO){
        *this->m_Counter = NOT_RESETED_AUTO;
    }
}

void LightEvent::Wait(){
    for(;;){
    switch (*this->m_Counter){
        case NOT_RESETED_MANUAL:
            this->m_Counter.WaitIfLessThan(0);
            return;
        case RESETED_MANUAL:
            return;
        case NOT_RESETED_AUTO:
            break;
        case RESETED_AUTO:
            if (this->m_Counter->CompareAndSwap (RESETED_AUTO, NOT_RESETED_AUTO) == RESETED_AUTO) {
                return;
            }
            break;
    }
        this->m_Counter.WaitIfLessThan (0);
    }
}

void LightEvent::Signal(){
    if(*this->m_Counter == NOT_RESETED_AUTO){
        *this->m_Counter = RESETED_AUTO;
        this->m_Counter.Signal(1);
    }
    else if(*this->m_Counter == NOT_RESETED_MANUAL){

    #if NN_VERSION_MAJOR > 2 || (NN_VERSION_MAJOR == 2 && NN_VERSION_MICRO > 1)
        SimpleLock::ScopedLock lock(this->m_Lock);
    #endif
        *this->m_Counter = RESETED_MANUAL;
        this->m_Counter.SignalAll();
    }
}

bool LightEvent::TryWait(){
    if(*this->m_Counter == RESETED_MANUAL ){
        return true;
    }
    else{
        return this->m_Counter->CompareAndSwap(RESETED_AUTO, NOT_RESETED_AUTO) == RESETED_AUTO;
    }
}

} // os
} // nn