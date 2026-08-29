// Filename: os_CriticalSection.cpp
//
// Project: Horizon

#include <nn/os/os_CriticalSection.h>
#include <nn/dbg/dbg_Break.h>
#include <nn/Assert.h>

namespace nn{
namespace os{

#if NN_VERSION_MAJOR > 2 || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR > 4) || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO > 1)

void CriticalSection::Initialize() {
    this->m_Lock.Initialize();
    m_ThreadUniqueValue = this->GetInvalidThreadUniqueValue();
    m_LockCount = 0;
}

void CriticalSection::Enter() {
    NN_TASSERT_(IsInitialized());
    if(!this->LockedByCurrentThread()){
        m_Lock.Lock();
        this->OnLocked();
    }
    m_LockCount++;
}

void CriticalSection::Leave() {
    NN_TASSERT_(IsInitialized());
    NN_TASSERTMSG_(LockedByCurrentThread() && m_LockCount > 0, "CriticalSection is not entered on the current thread.");
    if (--m_LockCount == 0) {
        NN_TASSERTMSG_(m_Lock.IsLocked(), "CriticalSection is not entered.");
        m_ThreadUniqueValue = 0;
        m_Lock.Unlock();
    }
}

bool CriticalSection::TryEnter() {
    NN_TASSERT_(IsInitialized());
    if(!this->LockedByCurrentThread() ){
        if(!this->m_Lock.TryLock() ){
            return false;
        }
        OnLocked();
    }
    m_LockCount++;
    return true;
}

#endif

#if NN_VERSION_MAJOR < 2 || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR < 4) || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO <= 1)
void CriticalSection::EnterImpl(){
    for(;;){
        if(*m_Counter > 0){
            if(TryEnterImpl()){
                break;
            }
        }

        this->m_Counter.DecrementAndWaitIfLessThan(0);
    }
}
#endif

}
}