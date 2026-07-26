#pragma once

#include <nn/util/util_NonCopyable.h>
#include <nn/util/detail/util_ScopedLockImpl.h>
#include <nn/os/os_WaitableCounter.h>

namespace nn{ 
namespace os{
    
class SimpleLock : private nn::util::ADLFireWall::NonCopyable<SimpleLock>{
private:
    WaitableCounter mCounter;
public:
    class ScopedLock;

    SimpleLock()  {}
    ~SimpleLock() {}
        
    void Initialize(void);
    void Lock(void);
    void LockImpl(void);
    bool TryLock(void);
    void Unlock(void);
    bool IsLocked() const{ return (*this->mCounter < 0);}
    
};
}
}

NN_UTIL_DETAIL_DEFINE_SCOPED_LOCK(nn::os::SimpleLock, Lock(), Unlock());