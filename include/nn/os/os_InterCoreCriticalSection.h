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

    WaitableCounter m_Counter;
    uptr m_ThreadUniqueValue;
    s32 m_LockCount;
public:
    class ScopedLock;

    InterCoreCriticalSection(): 
        m_ThreadUniqueValue(GetInvalidThreadUniqueValue()), 
        m_LockCount(-1) 
    {}

    InterCoreCriticalSection(const nn::WithInitialize&) { this->Initialize(); }

    void EnterImpl();
    bool TryEnterImpl(){
        ReverseIfPositiveUpdater updater;
        bool ret = m_Counter->AtomicUpdateConditional(updater);
        ARM::DataSynchronizationBarrier();
        if (ret) {
            NN_ASSERT_(mLockCount == 0);
            m_ThreadUniqueValue = this->GetThreadUniqueValue();
            return true;
        }
        return false;
    }

    void Enter(){
        NN_TASSERT_(IsInitialized());

        if (!LockedByCurrentThread() && !TryEnterImpl()){
            this->EnterImpl();
        }
        ++this->m_LockCount;
    }

    void Leave(){
        NN_ASSERT_(IsInitialized());
        NN_ASSERTMSG_(LockedByCurrentThread() && m_LockCount > 0, "CriticalSection is not entered on the current thread.");

        if (--m_LockCount == 0) {
            NN_ASSERTMSG_(*m_Counter < 0, "CriticalSection is not entered.");
            m_ThreadUniqueValue = GetInvalidThreadUniqueValue();
            ReverseUpdater updater;
            m_Counter->AtomicUpdateConditional(updater);
            ARM::DataSynchronizationBarrier();

            if (updater.afterUpdate > 1) {
                this->m_Counter.Signal(1);
            }
        }
    }

    void Initialize(){
        *m_Counter = 1;
        m_ThreadUniqueValue = this->GetInvalidThreadUniqueValue();
        m_LockCount = 0;
    }

    void Finalize(){ m_LockCount = -1;}

    bool IsLocked() const{
        return (*m_Counter < 0);
    }

    void OnLocked(){
        this->m_ThreadUniqueValue = GetThreadUniqueValue();
    }
    bool LockedByCurrentThread() const{
        return GetThreadUniqueValue() == m_ThreadUniqueValue;
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
        return this->m_LockCount >= 0;
    }
};

NN_UTIL_DETAIL_DEFINE_SCOPED_LOCK(InterCoreCriticalSection, Enter(), Leave());

#endif

}
}