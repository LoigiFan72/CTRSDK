#pragma once

#include <nn/os/os_CriticalSection.h>
#include <nn/os/ARM/os_MemoryBarrier.h>

namespace nn{
namespace os{

#if NN_VERSION_MAJOR > 2
    typedef CriticalSection InterCoreCriticalSection;
#else
class InterCoreCriticalSection : private nn::util::NonCopyable<InterCoreCriticalSection>{
private:
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

    WaitableCounter mCounter;
    uptr mThreadUniqueValue;
    s32 mLockCount;
public:
    class ScopedLock;

    InterCoreCriticalSection(): 
        mThreadUniqueValue(GetInvalidThreadUniqueValue()), 
        mLockCount(-1) 
    {}

    InterCoreCriticalSection(const nn::WithInitialize&) { this->Initialize(); }

    void EnterImpl();
    bool TryEnterImpl(){
        ReverseIfPositiveUpdater updater;
        bool ret = mCounter->AtomicUpdateConditional(updater);
        ARM::DataSynchronizationBarrier();
        if (ret) {
            NN_ASSERT_(mLockCount == 0);
            mThreadUniqueValue = this->GetThreadUniqueValue();
            return true;
        }
        return false;
    }

    void Enter(){
        NN_TASSERT_(IsInitialized());

        if (!LockedByCurrentThread() && !TryEnterImpl()){
            this->EnterImpl();
        }
        ++this->mLockCount;
    }

    void Leave(){
        NN_ASSERT_(IsInitialized());
        NN_ASSERTMSG_(LockedByCurrentThread() && mLockCount > 0, "CriticalSection is not entered on the current thread.");

        if (--mLockCount == 0) {
            NN_ASSERTMSG_(*mCounter < 0, "CriticalSection is not entered.");
            mThreadUniqueValue = GetInvalidThreadUniqueValue();
            ReverseUpdater updater;
            mCounter->AtomicUpdateConditional(updater);
            ARM::DataSynchronizationBarrier();

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

    bool IsLocked() const{
        return (*mCounter < 0);
    }

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

NN_UTIL_DETAIL_DEFINE_SCOPED_LOCK(InterCoreCriticalSection, Enter(), Leave());

#endif

}
}