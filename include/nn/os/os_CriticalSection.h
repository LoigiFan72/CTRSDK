#pragma once

#include <nn/WithInitialize.h>
#include <nn/os/os_SimpleLock.h>
#include <nn/os/os_Types.h>
#include <nn/hardware/hardware_RegAccess.h>

namespace nn { 
namespace os {
class CriticalSection : private nn::util::ADLFireWall::NonCopyable<CriticalSection>{
public:
    SimpleLock mLock;
    u32 mThreadUniqueValue;
    s32 mLockCount;

    CriticalSection() : mThreadUniqueValue(GetInvalidThreadUniqueValue()), mLockCount(-1) {}
    CriticalSection(const nn::WithInitialize&) { Initialize(); }
    void Initialize();

    Result TryInitialize(){
        this->Initialize();
        return ResultSuccess();
    }

    void Enter();
    void Leave();
    bool TryEnter();
    void Finalize(){this->mLockCount = -1;}
    ~CriticalSection() { }
    class ScopedLock;

    void OnLocked(){
        this->mThreadUniqueValue = GetThreadUniqueValue();
    }
    bool LockedByCurrentThread() const{
        return GetThreadUniqueValue() == mThreadUniqueValue;
    }
private:
    static uptr GetThreadUniqueValue(){
        uptr v;
        HW_GET_CP15_THREAD_ID_USER_READ_ONLY(v);
        return v;
    }
    static uptr GetInvalidThreadUniqueValue(){
        return static_cast<uptr>(-1);
    }
    bool IsInitialized() const{
        return this->mLockCount >= 0;
    }
};
    
NN_UTIL_DETAIL_DEFINE_SCOPED_LOCK(CriticalSection, Enter(), Leave());

typedef CriticalSection InterCoreCriticalSection;
}
};