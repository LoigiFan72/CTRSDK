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
    this->mLock.Initialize();
    mThreadUniqueValue = this->GetInvalidThreadUniqueValue();
    mLockCount = 0;
}

void CriticalSection::Enter() {
    NN_TASSERT_(this->IsInitialized());
    if(!this->LockedByCurrentThread()){
        this->mLock.Lock();
        this->OnLocked();
    }
    mLockCount++;
}

void CriticalSection::Leave() {
    NN_TASSERT_(this->IsInitialized());
    NN_TASSERTMSG_(this->LockedByCurrentThread() && this->mLockCount > 0, "CriticalSection is not entered on the current thread.");
    if (--mLockCount == 0) {
        NN_TASSERTMSG_(this->mLock.IsLocked(), "CriticalSection is not entered.");
        mThreadUniqueValue = 0;
        this->mLock.nn::os::SimpleLock::Unlock();
    }
}

bool CriticalSection::TryEnter() {
    NN_TASSERT_(this->IsInitialized());
    if(!this->LockedByCurrentThread() ){
        if(!this->mLock.TryLock() ){
            return false;
        }
        OnLocked();
    }
    mLockCount++;
    return true;
}

#endif

#if NN_VERSION_MAJOR < 2 || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR < 4) || (NN_VERSION_MAJOR == 2 && NN_VERSION_MINOR == 4 && NN_VERSION_MICRO <= 1)
void CriticalSection::EnterImpl(){
    for(;;){
        if(*mCounter > 0){
            if(TryEnterImpl()){
                break;
            }
        }

        this->mCounter.DecrementAndWaitIfLessThan(0);
    }
}
#endif

}
}