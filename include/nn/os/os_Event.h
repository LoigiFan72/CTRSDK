#pragma once

#include <nn/os/os_Synchronization.h>
#include <nn/util/util_Result.h>

namespace nn{
namespace os{

class InterruptEvent : public WaitObject{
};

class EventBase : public InterruptEvent{
public:
    explicit EventBase(ResetType resetType){ this->Initialize(resetType); }
    EventBase() {}
    ~EventBase() {}
    void Initialize(ResetType resetType);
    void Finalize();
    void Signal();
    void ClearSignal();
        
private:
    Result TryInitializeImpl(ResetType resetType);
};

inline Result EventBase::TryInitializeImpl(ResetType resetType){
    Handle handle;
    NN_UTIL_RETURN_IF_FAILED(svc::CreateEvent(&handle, resetType));
    this->SetHandle(handle);
    return ResultSuccess();
}

inline void EventBase::Initialize(ResetType resetType){
    NN_OS_ERROR_IF_FAILED(TryInitializeImpl(resetType));
}

inline void EventBase::Finalize(){
    this->HandleObject::Finalize();
}
    
inline void EventBase::Signal(){
    NN_OS_ERROR_IF_FAILED(svc::SignalEvent(GetHandle()));
}

inline void EventBase::ClearSignal(){
    NN_OS_ERROR_IF_FAILED(nn::svc::ClearEvent(GetHandle()));
}

class Event : public EventBase{
public:
    explicit Event(bool manualReset): 
        EventBase(manualReset ? RESET_TYPE_STICKY: RESET_TYPE_ONESHOT) 
    {}
    Event(){ this->ClearHandle();}
    ~Event(){ this->Close(); }
        

    void Initialize(bool manualReset) { this->EventBase::Initialize(manualReset ? RESET_TYPE_STICKY: RESET_TYPE_ONESHOT); }
    void Finalize() { this->EventBase::Finalize(); }
    void Signal() { this->EventBase::Signal(); }
    void Wait() { this->EventBase::WaitOne(); }
    bool Wait(nn::fnd::TimeSpan timeout) { return this->EventBase::WaitOne(timeout); }
};
}
}