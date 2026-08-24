#pragma once

#include <nn/os/os_Synchronization.h>
#include <nn/os/os_AutoStackManager.h>
#include <nn/os/os_ErrorHandlerSelect.h>
#include <nn/fnd/fnd_TimeSpan.h>
#include <nn/util/util_TypeTraits.h>

#define NN_OS_CORE_NO_ALL                   (-1)
#define NN_OS_CORE_NO_USE_PROCESS_VALUE     (-2)
#define NN_OS_THREAD_PRIORITY_RANGE_SIZE    32
#define NN_OS_LOWEST_THREAD_PRIORITY        (NN_OS_THREAD_PRIORITY_RANGE_SIZE - 1)
#define NN_OS_HIGHEST_THREAD_PRIORITY       0
#define NN_OS_DEFAULT_THREAD_PRIORITY       16

const s32 LOWEST_THREAD_PRIORITY = NN_OS_LOWEST_THREAD_PRIORITY;
const s32 HIGHEST_THREAD_PRIORITY = NN_OS_HIGHEST_THREAD_PRIORITY;
const s32 DEFAULT_THREAD_PRIORITY = NN_OS_DEFAULT_THREAD_PRIORITY;
const s32 CORE_NO_ALL = NN_OS_CORE_NO_ALL;
const s32 CORE_NO_USE_PROCESS_VALUE = NN_OS_CORE_NO_USE_PROCESS_VALUE;
const s32 THREAD_PRIORITY_RANGE_SIZE = NN_OS_THREAD_PRIORITY_RANGE_SIZE;

namespace nn{
namespace os{
namespace detail{
    void SaveThreadLocalRegionAddress();
    s32 ConvertSvcToLibraryPriority(s32 svc);
    s32 ConvertLibraryToSvcPriority(s32 lib);
    void InitializeThreadEnvrionment();
} // detail

template <size_t Size>
class StackBuffer{
private:
    typename nn::util::aligned_storage<Size, 8>::type mBuffer;
public:
    uptr GetStackBottom() const { return reinterpret_cast<uptr>(this + 1); }
    size_t GetStackSize() const { return Size; }

    void MarkCanary(bit32 value = 0xDEADBEEF) { reinterpret_cast<bit32*>(this)[0] = value; }
    bool CheckCanary(bit32 value = 0xDEADBEEF) const { return reinterpret_cast<const bit32*>(this)[0] == value; }
};
typedef void (*ThreadFunc)(uptr);

class Thread : public WaitObject{
private:
    struct FunctionInfo;
    struct TypeInfo;
    struct InitializeAsCurrentTag {};

private:

    // Use the top to call this!
    
    Result TryInitializeAndStartImpl(const TypeInfo& typeInfo,nn::os::ThreadFunc f,const void *p,uptr stackBottom,s32 priority, s32 coreNo,bool isAutoStack = false);
    Result TryInitializeAndStartImpl(const TypeInfo& typeInfo,nn::os::ThreadFunc f,const void *p,uptr stackBottom,s32 priority, s32 coreNo,uptr autoStackBuffer);
    Result TryInitializeAndStartImplUsingAutoStack(const TypeInfo& typeInfo, ThreadFunc f, const void* p, size_t stackSize, s32 priority, s32 coreNo);
    uptr PreStartUsingAutoStack(size_t stackSize);
    Result PostStartUsingAutoStack(Result result, uptr stackBottom);

public:
    Thread() : mCanFinalize(true) {}
    ~Thread();
    void FinalizeImpl();
    void Finalize();
    void Join();

    s32 GetPriority() const;
    static s32 GetCurrentPriority();

    void ChangePriority(s32 priority);
    static void ChangeCurrentPriority(s32 priority);

    bit32 GetId() const;
    static bit32 GetCurrentId();

    static Thread& GetMainThread() { return sMainThread; }
    
    /* Paramaters */
    typedef AutoStackManager AutoStackManager;

    bool mCanFinalize;
    bool mUsingAutoStack;

    static Thread sMainThread;
    static AutoStackManager* spAutoStackManager;
private:
    Thread(const InitializeAsCurrentTag&);
    static void ThreadStart(uptr p);
    static void ThreadStartUsingAutoStack(uptr);
    static void CallDestructorAndExit(void*);

    static void OnThreadStart();
    static void OnThreadExit();
    static void NoParameterFunc(void (*)());

public:
    class ProtectedAccessor{
    private:
        inline void InitializeAsMainThread(void* p){
            new(p) Thread(Thread::InitializeAsCurrentTag());
        }
    };

    void StartUsingAutoStack(void (*f)(), size_t stackSize, s32 priority = DEFAULT_THREAD_PRIORITY, s32 coreNo = CORE_NO_USE_PROCESS_VALUE);
    
    template <typename T, typename U>
    void StartUsingAutoStack(void (*f)(T), U param, size_t stackSize, s32 priority = DEFAULT_THREAD_PRIORITY, s32 coreNo = CORE_NO_USE_PROCESS_VALUE);

    Result TryStartUsingAutoStack(void (*f)(), size_t stackSize, s32 priority = DEFAULT_THREAD_PRIORITY, s32 coreNo = CORE_NO_USE_PROCESS_VALUE);
    Result TryStartUsingAutoStack(ThreadFunc f, size_t stackSize, const void* pParam, size_t paramSize, s32 priority = DEFAULT_THREAD_PRIORITY, s32 coreNo = CORE_NO_USE_PROCESS_VALUE);
    template <typename T, typename U>
    Result TryStartUsingAutoStack(void (*f)(T), U param, size_t stackSize, s32 priority = DEFAULT_THREAD_PRIORITY, s32 coreNo = CORE_NO_USE_PROCESS_VALUE);

    static void SetAutoStackManager(os::AutoStackManager*);

    static void Yield();
    static void Sleep(fnd::TimeSpan span){ SleepImpl(span); }
    static void SleepImpl(fnd::TimeSpan span); // nn::fnd::TimeSpan* span

    template <typename T, typename Stack>
    void Start(void (*f)(const T&), const T& param, Stack& stack, s32 priority = DEFAULT_THREAD_PRIORITY, s32 coreNo = CORE_NO_USE_PROCESS_VALUE);

    template <typename T, typename U, typename Stack>
    void Start(void (*f)(T), U param, Stack& stack, s32 priority = DEFAULT_THREAD_PRIORITY, s32 coreNo = CORE_NO_USE_PROCESS_VALUE);

    template <typename T, typename Stack>
    void Start(void (*f)(T*), T& param, Stack& stack, s32 priority = DEFAULT_THREAD_PRIORITY, s32 coreNo = CORE_NO_USE_PROCESS_VALUE);

    template <typename Stack>
    void Start(void (*f)(), Stack& stack, s32 priority = DEFAULT_THREAD_PRIORITY, s32 coreNo = CORE_NO_USE_PROCESS_VALUE);

    template <typename Stack>
    nn::Result TryStart(void (*f)(), Stack& stack, s32 priority = DEFAULT_THREAD_PRIORITY, s32 coreNo = CORE_NO_USE_PROCESS_VALUE);

    template <typename T, typename U, typename Stack>
    nn::Result TryStart(void (*f)(T), U param, Stack& stack, s32 priority = DEFAULT_THREAD_PRIORITY, s32 coreNo = CORE_NO_USE_PROCESS_VALUE);
};

/* Thread::FunctionInfo */

struct Thread::FunctionInfo{
    void (*destroy)(void* p);
    void (*invoke)(ThreadFunc f, const void* p);
    void (*f)(uptr);
    void* p;
    void* pAutoStackBuffer;

    void Invoke(){
        invoke(f, p);
    };
    void Destroy(){
        destroy(p);
    }
};

/* Thread::TypeInfo */

struct Thread::TypeInfo{
private:

    template <typename T, typename U>
    static void Copy(const void* src, void* dst){
        new (dst) T(*reinterpret_cast<const U*>(src));
    }
    template <typename T>
    static void Copy(const void* src, void* dst){
        new (dst) T(*reinterpret_cast<const T*>(src));
    }

    template <typename T>
    static void Destroy(void* p){
        reinterpret_cast<T*>(p)->~T();
    }

    template <typename T>
    static void Invoke(ThreadFunc f, const void* p){
        (*reinterpret_cast<void (*)(T)>(f))(*reinterpret_cast<const T*>(p));
    }
    template <typename T>
    static void Invoke2(ThreadFunc f, const void* p){
        (*reinterpret_cast<void (*)(const T*)>(f))(reinterpret_cast<const T*>(p));
    }

public:
    size_t size;
    void (*copy)(const void* src, void* dst);
    void (*destroy)(void* p);
    void (*invoke)(ThreadFunc f, const void* p);

    template <typename T, typename U>
    void SetData(typename nn::util::enable_if<nn::util::is_convertible<U, T>::value>::type* = 0){
        this->size = sizeof(T);
        this->copy = &(Copy<T, U>);
        this->destroy = &(Destroy<T>);
        this->invoke = &(Invoke<T>);
    }
    template <typename T>
    void SetData(){
        this->size = sizeof(T);
        this->copy = &(Copy<T>);
        this->destroy = &(Destroy<T>);
        this->invoke = &(Invoke2<T>);
    }
};


/* Inlines */
inline Thread::~Thread(){ 
    this->FinalizeImpl();
}

inline void Thread::Finalize(){
    FinalizeImpl();
    this->WaitObject::Finalize();
}

inline void Thread::Join(){
    this->WaitOne();
    this->mCanFinalize = true;
}

inline s32 Thread::GetPriority() const{
    s32 ret;
    NN_OS_ERROR_IF_FAILED(nn::svc::GetThreadPriority(&ret, GetHandle()));
    return os::detail::ConvertSvcToLibraryPriority(ret);
}

inline void Thread::ChangePriority(s32 priority){
    NN_OS_ERROR_IF_FAILED(nn::svc::SetThreadPriority(GetHandle(), os::detail::ConvertLibraryToSvcPriority(priority)));
}

inline void Thread::ChangeCurrentPriority(s32 priority){
    NN_OS_ERROR_IF_FAILED(nn::svc::SetThreadPriority(PSEUDO_HANDLE_CURRENT_THREAD, os::detail::ConvertLibraryToSvcPriority(priority)));
}

inline void Thread::Yield(){
    nn::svc::SleepThread(0);
}

inline bit32 Thread::GetId() const{
    bit32 ret;
    NN_OS_ERROR_IF_FAILED(nn::svc::GetThreadId(&ret, GetHandle()));
    return ret;
}

inline bit32 Thread::GetCurrentId(){
    bit32 ret;
    NN_OS_ERROR_IF_FAILED(nn::svc::GetThreadId(&ret, PSEUDO_HANDLE_CURRENT_THREAD));
    return ret;
}

inline s32 Thread::GetCurrentPriority(){
    s32 ret;
    NN_OS_ERROR_IF_FAILED(nn::svc::GetThreadPriority(&ret, PSEUDO_HANDLE_CURRENT_THREAD));
    return os::detail::ConvertSvcToLibraryPriority(ret);
}

/* AutoStacks */

template <typename T, typename U>
inline void Thread::StartUsingAutoStack(void (*f)(T), U param, size_t stackSize, s32 priority, s32 coreNo){
    TypeInfo info;
    info.SetData<T, U>();
    NN_OS_ERROR_IF_FAILED(TryInitializeAndStartImplUsingAutoStack(info, reinterpret_cast<ThreadFunc>(f), &param, stackSize, priority, coreNo));
}

template <typename T, typename U, typename Stack>
inline void Thread::Start(void (*f)(T), U param, Stack& stack, s32 priority, s32 coreNo){
    TypeInfo info;
    info.SetData<T, U>();
    NN_OS_ERROR_IF_FAILED(TryInitializeAndStartImpl(info, reinterpret_cast<ThreadFunc>(f), &param, stack.GetStackBottom(), priority, coreNo, false));
}

template <typename T, typename Stack>
inline void Thread::Start(void (*f)(T*), T& param, Stack& stack, s32 priority, s32 coreNo){
    TypeInfo info;
    info.SetData<T>();
    NN_OS_ERROR_IF_FAILED(TryInitializeAndStartImpl(info, reinterpret_cast<ThreadFunc>(f), &param, stack.GetStackBottom(), priority, coreNo, false));
}

template <typename Stack>
inline void Thread::Start(void (*f)(), Stack& stack, s32 priority, s32 coreNo){
    Start(NoParameterFunc, f, stack, priority, coreNo);
}

inline void Thread::StartUsingAutoStack(void (*f)(), size_t stackSize, s32 priority, s32 coreNo){
    StartUsingAutoStack(NoParameterFunc, f, stackSize, priority, coreNo);
}

template <typename T, typename U>
inline Result Thread::TryStartUsingAutoStack(void (*f)(T), U param, size_t stackSize, s32 priority, s32 coreNo){
    TypeInfo info;
    info.SetData<T, U>();
    Result result = TryInitializeAndStartImplUsingAutoStack(info, reinterpret_cast<ThreadFunc>(f), &param, stackSize, priority, coreNo);
    if (result.GetSummary() == Result::SUMMARY_OUT_OF_RESOURCE){
        return result;
    }
    NN_OS_ERROR_IF_FAILED(result);
    return result;
}

inline nn::Result Thread::TryStartUsingAutoStack(void (*f)(), size_t stackSize, s32 priority, s32 coreNo){
    return TryStartUsingAutoStack(NoParameterFunc, f, stackSize, priority, coreNo);
}

template <typename T, typename U, typename Stack>
inline nn::Result Thread::TryStart(void (*f)(T), U param, Stack& stack, s32 priority, s32 coreNo){
    TypeInfo info;
    info.SetData<T, U>();
    Result result = TryInitializeAndStartImpl(info, reinterpret_cast<ThreadFunc>(f), &param, stack.GetStackBottom(), priority, coreNo, false);
    if (result.GetSummary() == Result::SUMMARY_OUT_OF_RESOURCE){
        return result;
    }
    NN_OS_ERROR_IF_FAILED(result);
    return result;
}

template <typename Stack>
inline nn::Result Thread::TryStart(void (*f)(), Stack& stack, s32 priority, s32 coreNo){
    return TryStart(NoParameterFunc, f, stack, priority, coreNo);
}

}
}