#pragma once

#include <nn/os/os_CriticalSection.h>
#include <nn/os/ipc/os_Session.h>
#include <nn/os/os_Synchronization.h>

namespace nn{
namespace ssl{
namespace detail{
    
class LibManager{
protected:
    os::CriticalSection m_CriticalSection;
    s32 m_ReferenceCount;
    os::ipc::Session m_IpcSessionObj;
public:
    virtual ~LibManager(){}
};

}
}
}