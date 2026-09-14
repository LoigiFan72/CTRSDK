// Filename: snd_ThreadManager.cpp
//
// Project: Horizon

#include <nn/os.h>
#include <nn/snd.h>
#include <nn/snd/CTR/snd_Result.h>
#include <nn/snd/CTR/MPCore/snd_Class.h>
#include <nn/applet/CTR/applet_Info.h>
#include <nn/os/ARM/os_MemoryBarrier.h>

#include "snd_MasterManager.h"
#include "snd_ThreadManager.h"

namespace nn {
namespace snd {
namespace CTR {

ThreadManager& ThreadManager::GetInstance()
{
    static ThreadManager instance;
    return instance;
}


void SoundThreadFunc(uptr arg)
{
    ThreadManager::GetInstance().SoundThreadFuncImpl(arg);
}

void UserSoundThreadFunc(uptr arg)
{
    ThreadManager::GetInstance().UserSoundThreadFuncImpl(arg);
}

ThreadManager::ThreadManager()
{
    m_IsSoundThreadCreated = false;
    m_IsUserSoundThreadCreated = false;
    m_IsTickCounterEnabled = false;
    m_CoreNo = 0;
}

ThreadManager::~ThreadManager(){ }

void ThreadManager::SoundThreadFuncImpl(uptr)
{
    m_IsSoundThreadEnabled = true;
    while (m_IsSoundThreadEnabled)
    {
        nn::os::Tick tick0, tick1;

        if (m_IsTickCounterEnabled)
        {
            WaitForDspSync(&tick0);
            tick1 = nn::os::Tick::GetSystemCurrent();
        }
        else
        {
            WaitForDspSync();
        }

        bool isUserSoundThreadRunning = *((volatile bool*)&m_IsUserSoundThreadCreated);
        bool isUserSoundCallbackRequired = (m_UserSoundThreadCallback != NULL);
        bool isAuxCallbackRequired = false;

        if (isUserSoundThreadRunning)
        {
            AuxCallback callbackA, callbackB;
            uptr argA, argB;
            MasterManager::GetInstance().GetAuxCallback(AUX_BUS_A, &callbackA, &argA);
            MasterManager::GetInstance().GetAuxCallback(AUX_BUS_B, &callbackB, &argB);
            isAuxCallbackRequired = (callbackA != NULL || callbackB != NULL);
        }

        if (isUserSoundThreadRunning && (isUserSoundCallbackRequired || isAuxCallbackRequired))
        {
            os::ARM::DataSynchronizationBarrier();
            this->m_EventSystem2User.Signal();
        }

        if (m_CoreNo == 0 && m_UserSoundThreadCallback)
        {
            this->m_UserSoundThreadCallback(m_ArgForUser);
        }

        if (m_NwSoundThreadCallback)
        {
            this->Lock();
            this->m_NwSoundThreadCallback(this->m_ArgForNw);
            this->Unlock();
        }

        if (isUserSoundThreadRunning && (isUserSoundCallbackRequired || isAuxCallbackRequired)){
            this->m_EventUser2System.Wait();
        }

        this->Lock();
        SendParameterToDsp();
        this->Unlock();

        if (m_IsTickCounterEnabled)
        {
            tick1 = nn::os::Tick::GetSystemCurrent() - tick1;
            m_SoundThreadTick = tick0 + tick1;
        }
    }
}

void ThreadManager::UserSoundThreadFuncImpl(uptr)
{
    m_IsUserSoundThreadEnabled = true;
    while (m_IsUserSoundThreadEnabled)
    {
        m_EventSystem2User.Wait();

        if (m_UserSoundThreadCallback)
        {
            this->m_UserSoundThreadCallback(this->m_ArgForUser);
        }
        {
            MasterManager::GetInstance().AuxUserCallback(AUX_BUS_A, reinterpret_cast<uptr>(Dspsnd::GetInstance().GetAuxBusAddr(AUX_BUS_A)));
            MasterManager::GetInstance().AuxUserCallback(AUX_BUS_B, reinterpret_cast<uptr>(Dspsnd::GetInstance().GetAuxBusAddr(AUX_BUS_B)));
        }

        os::ARM::DataSynchronizationBarrier();
        this->m_EventUser2System.Signal();
    }
}

Result ThreadManager::StartSoundThread(void (*callback)(uptr), uptr arg, uptr stackBuffer, size_t stackSize, s32 prio, s32 coreNo)
{
    if (m_IsSoundThreadCreated){
        return ResultAlreadyInitialized();
    }

    ThreadStack stack(stackBuffer + stackSize);
#if NN_VERSION_MAJOR > 2
    if (nn::applet::IsSystemApplet() && coreNo == 1){
        prio = 0x5109d500;
    }
#else
    if (coreNo == 1){
        prio += 0x5109d500;
    }
#endif

    this->m_CriticalSection.Initialize();
    Result result = this->m_SoundThread.TryStart(SoundThreadFunc,NULL,stack,prio,coreNo);
    if (result.IsSuccess())
    {
        m_NwSoundThreadCallback = NULL;
        m_ArgForNw = NULL;
        m_UserSoundThreadCallback = callback;
        m_ArgForUser = arg;
        m_IsSoundThreadCreated = true;

        Dspsnd::GetInstance().EnableAuxCallbackInSendParameter(coreNo == 0);

        m_SoundThreadTick = nn::os::Tick(0);

        m_CoreNo = coreNo;
    }
    else
    {
        this->m_CriticalSection.Finalize();
    }
    return result;
}

Result ThreadManager::StartSoundThread(const ThreadParameter* mainThreadParam,void (*mainThreadCallback)(uptr),uptr mainThreadArg,const ThreadParameter* userThreadParam,void (*userThreadCallback)(uptr),uptr userThreadArg,s32 coreNo)
{
    Result result;
    result = StartSoundThread(userThreadCallback,userThreadArg,mainThreadParam->stackBuffer,mainThreadParam->stackSize,mainThreadParam->priority,coreNo);
    NN_UTIL_RETURN_IF_FAILED(result);
    m_NwSoundThreadCallback = mainThreadCallback;
    m_ArgForNw = mainThreadArg;
    if (userThreadParam)
    {
        result = StartUserSoundThread(userThreadParam->stackBuffer,userThreadParam->stackSize,userThreadParam->priority);
        if (result.IsFailure())
        {
            this->FinalizeSoundThread();
            return result;
        }
    }
    return ResultSuccess();
}

nn::Result ThreadManager::StartUserSoundThread(uptr stackBuffer, size_t stackSize, s32 prio)
{
    if (!m_IsSoundThreadCreated)
    {
        return ResultInvalidUsage();
    }

    if (m_IsUserSoundThreadCreated)
    {
        return ResultAlreadyInitialized();
    }

    if (m_CoreNo != 1)
    {
        return ResultInvalidUsage();
    }

    this->m_EventUser2System.Initialize(false);
    this->m_EventSystem2User.Initialize(false);

    ThreadStack stack(stackBuffer + stackSize);
    nn::Result result = this->m_UserSoundThread.TryStart(UserSoundThreadFunc,NULL,stack,prio,0);
    m_IsUserSoundThreadCreated = result.IsSuccess();
    if (result.IsFailure())
    {
        this->m_EventUser2System.Finalize();
        this->m_EventSystem2User.Finalize();
    }
    return result;
}

void ThreadManager::FinalizeUserSoundThread()
{
    if (!m_IsUserSoundThreadCreated)
    {
        return;
    }

    m_IsUserSoundThreadEnabled = false;
    this->m_UserSoundThread.Join();
    this->m_UserSoundThread.Finalize();
    m_IsUserSoundThreadCreated = false;

    os::ARM::DataSynchronizationBarrier();
    this->m_EventUser2System.Signal();
    this->m_EventUser2System.Finalize();
    this->m_EventSystem2User.Finalize();
}

void ThreadManager::FinalizeSoundThread()
{
    this->FinalizeUserSoundThread();

    if (!m_IsSoundThreadCreated)
    {
        return;
    }

    m_IsSoundThreadEnabled = false;
    this->m_SoundThread.Join();
    this->m_SoundThread.Finalize();
    m_NwSoundThreadCallback = NULL;
    m_UserSoundThreadCallback = NULL;

    m_CoreNo = 0;

    this->m_CriticalSection.Finalize();

    Dspsnd::GetInstance().EnableAuxCallbackInSendParameter(true);

    m_IsSoundThreadCreated = false;
}

void ThreadManager::EnableSoundThreadTickCounter(bool enable)
{
    if (m_CoreNo == 0){
        m_IsTickCounterEnabled = enable;
    }
}

os::Tick ThreadManager::GetSoundThreadTick()
{
    return m_SoundThreadTick;
}

}
}
}