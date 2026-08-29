#pragma once

#include <nn/os/os_Types.h>
#include <nn/os/os_ErrorHandlerSelect.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/svc.h>
#include <nn/Assert.h>

namespace nn{
namespace os{

struct HandleManager{
};

class HandleObject : util::ADLFireWall::NonCopyable<HandleObject>{
public:
    Handle m_Handle;
public:
    HandleObject() {}
    ~HandleObject(){ Close(); }
    Handle GetHandle() const{ return m_Handle; }
    bool IsValid() const{ return m_Handle.IsValid(); }
    void SetHandle(nn::Handle handle);
    void Close(){
        if (IsValid()) {
            nn::svc::CloseHandle(m_Handle);
            m_Handle = Handle();
        }
    }
    void Finalize(){ Close(); }
    void ClearHandle(){ m_Handle = Handle(); }

    Handle DetachHandle();
};
    
inline void HandleObject::SetHandle(nn::Handle handle){
    NN_TASSERTMSG_(!IsValid(), "current handle(=%08X) is active\n", mHandle.GetPrintableBits());
    NN_TASSERT_(handle.IsValid());
    m_Handle = handle;
}

inline Handle HandleObject::DetachHandle(){
    Handle h = this->GetHandle();
    this->ClearHandle();
    return h;
}

class WaitObject : public HandleObject{
public:
    nn::Result WaitOneImpl(s64);
    void WaitOne();
    bool WaitOne(nn::fnd::TimeSpan timeout);
protected:
    WaitObject() {}
    ~WaitObject() {}
};

inline nn::Result WaitObject::WaitOneImpl(s64 nanoSecondsTimeout){
    s32 dummy;
    Handle handle = GetHandle();
    return nn::svc::WaitSynchronizationN(&dummy, &handle, 1, false, nanoSecondsTimeout);
}

inline void WaitObject::WaitOne(){ 
    NN_OS_ERROR_IF_FAILED(WaitOneImpl(WAIT_INFINITE)); 
}

inline bool WaitObject::WaitOne(nn::fnd::TimeSpan timeout){
    nn::Result result = WaitOneImpl(timeout.GetNanoSeconds());
    NN_OS_ERROR_IF_FAILED(result);
    return result.GetDescription() != nn::Result::DESCRIPTION_TIMEOUT;
}

}
}