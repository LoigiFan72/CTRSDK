#pragma once

#include <nn/Result.h>
#include <nn/snd/CTR/Common/snd_Types.h>
#include <nn/os.h>

namespace nn {
namespace snd {
namespace CTR {

class ThreadStack
{
private:
    uptr m_StackBottom;
public:
    ThreadStack(uptr stackBottom) { m_StackBottom = stackBottom; }
    ~ThreadStack() {};
    uptr GetStackBottom() { return m_StackBottom; }
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

    void Lock(){ this->m_CriticalSection.Enter(); }

    void Unlock(){ this->m_CriticalSection.Leave(); }

private:
    os::Tick m_SoundThreadTick;
    bool m_IsTickCounterEnabled;

    s8 __padding__[3];

    bool m_IsSoundThreadCreated;
    bool m_IsSoundThreadEnabled;
    bool m_IsUserSoundThreadCreated;
    bool m_IsUserSoundThreadEnabled;
    os::Thread m_SoundThread;
    os::Thread m_UserSoundThread;
    void (*m_NwSoundThreadCallback)(uptr);
    uptr m_ArgForNw;
    void (*m_UserSoundThreadCallback)(uptr);
    uptr m_ArgForUser;
    s8   m_CoreNo;

    s8 __padding2__[3];

    os::CriticalSection m_CriticalSection;
    os::LightEvent m_EventSystem2User;
    os::LightEvent m_EventUser2System;
};

}
}
}