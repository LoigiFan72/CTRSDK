#pragma once

#include <nn/WithInitialize.h>
#include <nn/os/os_SimpleLock.h>
#include <nn/os/os_Types.h>
#include <nn/hardware/hardware_RegAccess.h>

namespace nn { 
namespace os {
class CriticalSection : private nn::util::ADLFireWall::NonCopyable<CriticalSection>{
private:
#if NN_VERSION_MAJOR > 2
    SimpleLock mLock;
#else
    struct ReverseIfPositiveUpdater{
        bool operator()(s32& x){
            if (x > 0){
                x = -x;
                return true;
            }
            return false;
        }
    };
    struct ReverseUpdater{
        s32 afterUpdate;

        bool operator()(s32& x){
            x           = -x;
            afterUpdate = x;
            return true;
        }
    };

    WaitableCounter mCounter;
#endif
    u32 mThreadUniqueValue;
    s32 mLockCount;

public:

    CriticalSection() : mThreadUniqueValue(GetInvalidThreadUniqueValue()), mLockCount(-1) {}
    CriticalSection(const nn::WithInitialize&) { this->Initialize(); }

#if NN_VERSION_MAJOR <= 2 || \
    (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR < 4) || \
    (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO < 1)
    void EnterImpl();
    bool TryEnterImpl(){
        ReverseIfPositiveUpdater updater;
        if (this->mCounter->AtomicUpdateConditional(updater)) {
            NN_ASSERT_(mLockCount == 0);
            mThreadUniqueValue = this->GetThreadUniqueValue();
            return true;
        }
        return false;
    }
#endif

#if NN_VERSION_MAJOR > 2 || \
    (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR > 4) || \
    (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO > 1)
    void Initialize();
    void Enter();
    void Leave();
    void Initialize();
    bool TryEnter();
#else
    void Leave(){
        NN_ASSERT_(IsInitialized());
        NN_ASSERTMSG_(LockedByCurrentThread() && mLockCount > 0, "CriticalSection is not entered on the current thread.");

        if (--mLockCount == 0) {
            NN_ASSERTMSG_(*mCounter < 0, "CriticalSection is not entered.");
            mThreadUniqueValue = GetInvalidThreadUniqueValue();
            ReverseUpdater updater;
            mCounter->AtomicUpdateConditional(updater);

            if (updater.afterUpdate > 1) {
                this->mCounter.Signal(1);
            }
        }
    }

    void Initialize(){
        *mCounter = 1;
        mThreadUniqueValue = this->GetInvalidThreadUniqueValue();
        mLockCount = 0;
    }
#endif

    Result TryInitialize(){
        this->Initialize();
        return ResultSuccess();
    }
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