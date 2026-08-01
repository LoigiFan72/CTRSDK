#pragma once

#include <nn/os/os_CriticalSection.h>
#include <nn/os/ipc/os_Session.h>
#include <nn/os/os_Synchronization.h>

namespace nn{
namespace ssl{
namespace detail{
    
class LibManager{
protected:
    os::CriticalSection mCriticalSection;
    s32 mReferenceCount;
    os::ipc::Session mIpcSessionObj;
public:
    virtual ~LibManager(){}
};

}
}
}