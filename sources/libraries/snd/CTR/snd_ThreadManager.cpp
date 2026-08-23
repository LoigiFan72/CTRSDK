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

ThreadManager& ThreadManager::GetInstance(){
    static ThreadManager instance;
    return instance;
}


void SoundThreadFunc(uptr arg){
    ThreadManager::GetInstance().SoundThreadFuncImpl(arg);
}

void UserSoundThreadFunc(uptr arg){
    ThreadManager::GetInstance().UserSoundThreadFuncImpl(arg);
}

ThreadManager::ThreadManager(){
    mIsSoundThreadCreated = false;
    mIsUserSoundThreadCreated = false;
    mIsTickCounterEnabled = false;
    mCoreNo = 0;
}

ThreadManager::~ThreadManager(){ }

void ThreadManager::SoundThreadFuncImpl(uptr){
    mIsSoundThreadEnabled = true;
    while (mIsSoundThreadEnabled){
        nn::os::Tick tick0, tick1;

        if (mIsTickCounterEnabled){
            WaitForDspSync(&tick0);
            tick1 = nn::os::Tick::GetSystemCurrent();
        }
        else{
            WaitForDspSync();
        }

        bool isUserSoundThreadRunning = *((volatile bool*)&mIsUserSoundThreadCreated);
        bool isUserSoundCallbackRequired = (mUserSoundThreadCallback != NULL);
        bool isAuxCallbackRequired = false;

        if (isUserSoundThreadRunning){
            AuxCallback callbackA, callbackB;
            uptr argA, argB;
            MasterManager::GetInstance().GetAuxCallback(AUX_BUS_A, &callbackA, &argA);
            MasterManager::GetInstance().GetAuxCallback(AUX_BUS_B, &callbackB, &argB);
            isAuxCallbackRequired = (callbackA != NULL || callbackB != NULL);
        }

        if (isUserSoundThreadRunning && (isUserSoundCallbackRequired || isAuxCallbackRequired)){
            os::ARM::DataSynchronizationBarrier();
            this->mEventSystem2User.Signal();
        }

        if (mCoreNo == 0 && mUserSoundThreadCallback){
            this->mUserSoundThreadCallback(mArgForUser);
        }

        if (mNwSoundThreadCallback){
            this->Lock();
            this->mNwSoundThreadCallback(this->mArgForNw);
            this->Unlock();
        }

        if (isUserSoundThreadRunning && (isUserSoundCallbackRequired || isAuxCallbackRequired)){
            this->mEventUser2System.Wait();
        }

        this->Lock();
        SendParameterToDsp();
        this->Unlock();

        if (mIsTickCounterEnabled){
            tick1 = nn::os::Tick::GetSystemCurrent() - tick1;
            mSoundThreadTick = tick0 + tick1;
        }
    }
}

void ThreadManager::UserSoundThreadFuncImpl(uptr){
    mIsUserSoundThreadEnabled = true;
    while (mIsUserSoundThreadEnabled){
        mEventSystem2User.Wait();

        if (mUserSoundThreadCallback){
            this->mUserSoundThreadCallback(this->mArgForUser);
        }
        {
            MasterManager::GetInstance().AuxUserCallback(AUX_BUS_A, reinterpret_cast<uptr>(Dspsnd::GetInstance().GetAuxBusAddr(AUX_BUS_A)));
            MasterManager::GetInstance().AuxUserCallback(AUX_BUS_B, reinterpret_cast<uptr>(Dspsnd::GetInstance().GetAuxBusAddr(AUX_BUS_B)));
        }

        os::ARM::DataSynchronizationBarrier();
        this->mEventUser2System.Signal();
    }
}

// FINISH FROM HERE

Result ThreadManager::StartSoundThread(void (*callback)(uptr), uptr arg, uptr stackBuffer, size_t stackSize, s32 prio, s32 coreNo){
    if (mIsSoundThreadCreated){
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

    this->mCriticalSection.Initialize();
    Result result = this->mSoundThread.TryStart(SoundThreadFunc,NULL,stack,prio,coreNo);
    if (result.IsSuccess()){
        mNwSoundThreadCallback = NULL;
        mArgForNw = NULL;
        mUserSoundThreadCallback = callback;
        mArgForUser = arg;
        mIsSoundThreadCreated = true;

        Dspsnd::GetInstance().EnableAuxCallbackInSendParameter(coreNo == 0);

        mSoundThreadTick = nn::os::Tick(0);

        mCoreNo = coreNo;
    }
    else{
        this->mCriticalSection.Finalize();
    }
    return result;
}

Result ThreadManager::StartSoundThread(const ThreadParameter* mainThreadParam,void (*mainThreadCallback)(uptr),uptr mainThreadArg,const ThreadParameter* userThreadParam,void (*userThreadCallback)(uptr),uptr userThreadArg,s32 coreNo){
    Result result;
    result = StartSoundThread(userThreadCallback,userThreadArg,mainThreadParam->stackBuffer,mainThreadParam->stackSize,mainThreadParam->priority,coreNo);
    NN_UTIL_RETURN_IF_FAILED(result);
    mNwSoundThreadCallback = mainThreadCallback;
    mArgForNw = mainThreadArg;
    if (userThreadParam){
        result = StartUserSoundThread(userThreadParam->stackBuffer,userThreadParam->stackSize,userThreadParam->priority);
        if (result.IsFailure()){
            this->FinalizeSoundThread();
            return result;
        }
    }
    return ResultSuccess();
}

nn::Result ThreadManager::StartUserSoundThread(uptr stackBuffer, size_t stackSize, s32 prio){
    if (!mIsSoundThreadCreated){
        return ResultInvalidUsage();
    }

    if (mIsUserSoundThreadCreated){
        return ResultAlreadyInitialized();
    }

    if (mCoreNo != 1){
        return ResultInvalidUsage();
    }

    this->mEventUser2System.Initialize(false);
    this->mEventSystem2User.Initialize(false);

    ThreadStack stack(stackBuffer + stackSize);
    nn::Result result = this->mUserSoundThread.TryStart(UserSoundThreadFunc,NULL,stack,prio,0);
    mIsUserSoundThreadCreated = result.IsSuccess();
    if (result.IsFailure()){
        this->mEventUser2System.Finalize();
        this->mEventSystem2User.Finalize();
    }
    return result;
}

void ThreadManager::FinalizeUserSoundThread(){
    if (!mIsUserSoundThreadCreated){
        return;
    }

    mIsUserSoundThreadEnabled = false;
    this->mUserSoundThread.Join();
    this->mUserSoundThread.Finalize();
    mIsUserSoundThreadCreated = false;

    os::ARM::DataSynchronizationBarrier();
    this->mEventUser2System.Signal();
    this->mEventUser2System.Finalize();
    this->mEventSystem2User.Finalize();
}

void ThreadManager::FinalizeSoundThread(){
    this->FinalizeUserSoundThread();

    if (!mIsSoundThreadCreated){
        return;
    }

    mIsSoundThreadEnabled = false;
    this->mSoundThread.Join();
    this->mSoundThread.Finalize();
    mNwSoundThreadCallback = NULL;
    mUserSoundThreadCallback = NULL;

    mCoreNo = 0;

    this->mCriticalSection.Finalize();

    Dspsnd::GetInstance().EnableAuxCallbackInSendParameter(true);

    mIsSoundThreadCreated = false;
}

void ThreadManager::EnableSoundThreadTickCounter(bool enable){
    if (mCoreNo == 0){
        mIsTickCounterEnabled = enable;
    }
}

os::Tick ThreadManager::GetSoundThreadTick(){
    return mSoundThreadTick;
}

}
}
}