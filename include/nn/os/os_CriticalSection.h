#pragma once

#include <nn/WithInitialize.h>
#include <nn/os/os_SimpleLock.h>
#include <nn/os/os_Types.h>
#include <nn/hardware/hardware_RegAccess.h>

namespace nn { 
namespace os {
class CriticalSection : private nn::util::ADLFireWall::NonCopyable<CriticalSection>{
private:
#if NN_VERSION_MAJOR > 2 || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR > 4) || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO > 1)
    SimpleLock m_Lock;
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

    WaitableCounter m_Counter;
#endif
    u32 m_ThreadUniqueValue;
    s32 m_LockCount;

public:

    CriticalSection() : m_ThreadUniqueValue(GetInvalidThreadUniqueValue()), m_LockCount(-1) {}
    CriticalSection(const nn::WithInitialize&) { this->Initialize(); }

#if NN_VERSION_MAJOR <= 2 || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR < 4) || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO < 1)
    void EnterImpl();
    bool TryEnterImpl(){
        ReverseIfPositiveUpdater updater;
        if (this->m_Counter->AtomicUpdateConditional(updater)) {
            NN_ASSERT_(m_LockCount == 0);
            m_ThreadUniqueValue = this->GetThreadUniqueValue();
            return true;
        }
        return false;
    }
#endif

#if NN_VERSION_MAJOR > 2 || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR > 4) || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO > 1)
    void Initialize();
    void Enter();
    void Leave();
    void Initialize();
    bool TryEnter();
#else
    void Leave(){
        NN_ASSERT_(IsInitialized());
        NN_ASSERTMSG_(LockedByCurrentThread() && m_LockCount > 0, "CriticalSection is not entered on the current thread.");

        if (--m_LockCount == 0) {
            NN_ASSERTMSG_(*m_Counter < 0, "CriticalSection is not entered.");
            m_ThreadUniqueValue = GetInvalidThreadUniqueValue();
            ReverseUpdater updater;
            m_Counter->AtomicUpdateConditional(updater);

            if (updater.afterUpdate > 1) {
                this->m_Counter.Signal(1);
            }
        }
    }

    void Enter(){
        NN_TASSERT_(IsInitialized());

        if (!LockedByCurrentThread() && !TryEnterImpl()){
            EnterImpl();
        }
        ++this->m_LockCount;
    }

    void Initialize(){
        *m_Counter = 1;
        m_ThreadUniqueValue = this->GetInvalidThreadUniqueValue();
        m_LockCount = 0;
    }
#endif

    Result TryInitialize(){
        this->Initialize();
        return ResultSuccess();
    }
    void Finalize(){ this->m_LockCount = -1;}
    ~CriticalSection() { }
    class ScopedLock;

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
    
NN_UTIL_DETAIL_DEFINE_SCOPED_LOCK(CriticalSection, Enter(), Leave());

}
}