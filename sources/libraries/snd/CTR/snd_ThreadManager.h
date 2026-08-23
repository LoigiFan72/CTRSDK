#pragma once

#include <nn/Result.h>
#include <nn/snd/CTR/Common/snd_Types.h>
#include <nn/os.h>

namespace nn {
namespace snd {
namespace CTR {

class ThreadStack{
private:
    uptr mStackBottom;
public:
    ThreadStack(uptr stackBottom) { mStackBottom = stackBottom; }
    ~ThreadStack() {};
    uptr GetStackBottom() { return mStackBottom; }
};


class ThreadManager{
public:
    static ThreadManager& GetInstance();

    ThreadManager();
    ~ThreadManager();

    void SoundThreadFuncImpl(uptr arg);
    void UserSoundThreadFuncImpl(uptr arg);

    Result StartSoundThread(void (*callback)(uptr), uptr arg, uptr stackBuffer, size_t stackSize, s32 prio, s32 coreNo);
    Result StartUserSoundThread(uptr stackBuffer, size_t stackSize, s32 prio);
    Result StartSoundThread(const ThreadParameter* mainThreadParam,void (*mainThreadCallback)(uptr),uptr mainThreadArg,const ThreadParameter* userThreadParam,void (*userThreadCallback)(uptr),
        uptr userThreadArg, s32 coreNo);

    void FinalizeSoundThread();
    void FinalizeUserSoundThread();

    void EnableSoundThreadTickCounter(bool enable);
    os::Tick GetSoundThreadTick();

    void EnableVoiceDropCallbackOnCore1(bool enable);

    void Lock(){ this->mCriticalSection.Enter(); }

    void Unlock(){ this->mCriticalSection.Leave(); }

private:
    os::Tick mSoundThreadTick;
    bool mIsTickCounterEnabled;

    s8 __padding__[3];

    bool mIsSoundThreadCreated;
    bool mIsSoundThreadEnabled;
    bool mIsUserSoundThreadCreated;
    bool mIsUserSoundThreadEnabled;
    os::Thread mSoundThread;
    os::Thread mUserSoundThread;
    void (*mNwSoundThreadCallback)(uptr);
    uptr mArgForNw;
    void (*mUserSoundThreadCallback)(uptr);
    uptr mArgForUser;
    s8   mCoreNo;

    s8 __padding2__[3];

    os::CriticalSection mCriticalSection;
    os::LightEvent mEventSystem2User;
    os::LightEvent mEventUser2System;
};

}
}
}