// Filename: os_Thread.cpp
//
// Project: Horizon

#include <nn/os/os_Thread.h>
#include <nn/os/os_ThreadLocalStorage.h>
#include <nn/os/CTR/os_ThreadLocalRegion.h>
#include <nn/os/CTR/os_CppException.h>
#include <nn/os/CTR/os_ErrorHandler.h>
#include <nn/os/ARM/os_SpinWait.h>
#include <nn/svc.h>
#include <nn/util/util_Result.h>
#include <rt_fp.h>

extern "C" void _fp_init();

extern "C" nn::os::AutoStackManager* s_pAutoStackManager;

namespace nn{
namespace os{

Thread Thread::s_MainThread = Thread::InitializeAsCurrentTag();
Thread::AutoStackManager* Thread::s_pAutoStackManager = NULL;

/* Inlines */

inline void Thread::OnThreadStart(){
    nn::os::detail::InitializeThreadEnvrionment();
}

inline void Thread::OnThreadExit(){
    // nop{0}
}

Thread::Thread(const Thread::InitializeAsCurrentTag&){
    Handle handle;
    NN_OS_ERROR_IF_FAILED(nn::svc::DuplicateHandle(&handle, PSEUDO_HANDLE_CURRENT_THREAD));
    this->SetHandle(handle);
    this->m_CanFinalize = false;
    this->m_UsingAutoStack = false;
}

/* ThreadStart */
void Thread::ThreadStart(uptr p){
    FunctionInfo& info = *reinterpret_cast<FunctionInfo*>(p);

    OnThreadStart();
    info.Invoke();
    info.Destroy();
    OnThreadExit();

    if(info.pAutoStackBuffer != NULL){
        CallDestructorAndExit(info.pAutoStackBuffer);
    }

    nn::svc::ExitThread();
}

/* FinalizeImpl */
void Thread::FinalizeImpl(){
    if (!m_CanFinalize){
        NN_TASSERTMSG_(m_CanFinalize, "Thread should be Joined or Detached before being Finalized.");
        this->WaitOne();
        this->m_CanFinalize = true;
    }
}

void Thread::NoParameterFunc(void (*f)()){
    f();
}

/* AutoStackManagers */

/* SetAutoStackManager */
void Thread::SetAutoStackManager(nn::os::AutoStackManager* pManager){
    nn::os::Thread::s_pAutoStackManager = pManager;
}

/* PreStartUsingAutoStack */
uptr Thread::PreStartUsingAutoStack(size_t stackSize){
    void* pStackBottom = s_pAutoStackManager->Construct(stackSize);

    return reinterpret_cast<uptr>(pStackBottom);
}

/* PostStartUsingAutoStack */
Result Thread::PostStartUsingAutoStack(Result result, uptr stackBottom){
    if (result.IsFailure()){
        spAutoStackManager->Destruct(reinterpret_cast<void*>(stackBottom), true);
        return result;
    }

    this->mUsingAutoStack = true;
    return ResultSuccess();
}

/* TryInitializeImplUsingAutoStack, use the inline StartUsingAutoStack() */
Result Thread::TryInitializeAndStartImplUsingAutoStack(const TypeInfo& typeInfo, ThreadFunc f, const void* p, size_t stackSize, s32 priority, s32 coreNo){
    const uptr stackBottom = PreStartUsingAutoStack(stackSize);
    Result result = TryInitializeAndStartImpl(typeInfo, f, p, stackBottom, priority, coreNo, true);
    return PostStartUsingAutoStack(result, stackBottom);
}

/* SleepImpl */
void Thread::SleepImpl(fnd::TimeSpan span){
    if(span.GetNanoSeconds() >= 0){
        svc::SleepThread(span.GetNanoSeconds());
    }
    else{
        os::ARM::SpinWaitCpuCycles();
    }
}

/* TryInitializeAndStartImpl, use this entry. */
Result Thread::TryInitializeAndStartImpl(const TypeInfo& typeInfo,nn::os::ThreadFunc f,const void *p,uptr stackBottom,s32 priority, s32 coreNo,bool isAutoStack){
    return TryInitializeAndStartImpl(typeInfo,f,p,stackBottom,priority,coreNo,(isAutoStack ? stackBottom: NULL));
}


Result Thread::TryInitializeAndStartImpl(const TypeInfo& typeInfo,nn::os::ThreadFunc f,const void *p,uptr stackBottom,s32 priority, s32 coreNo,uptr autoStackBuffer){
    uptr stack = stackBottom;
    
    stack -= typeInfo.size;
    stack &= 0xfffffff8;
    void* obj = reinterpret_cast<void*>(stack);
    typeInfo.copy(p, obj);

    stack -= sizeof(FunctionInfo);
    stack &= 0xfffffff8;
    FunctionInfo& info = *reinterpret_cast<FunctionInfo*>(stack);
    info.destroy = typeInfo.destroy;
    info.invoke = typeInfo.invoke;
    info.f = f;
    info.p = obj;
    info.pAutoStackBuffer = reinterpret_cast<void*>(autoStackBuffer);

    Handle handle;
    NN_UTIL_RETURN_IF_FAILED(nn::svc::CreateThread(&handle,ThreadStart,stack,stack,os::detail::ConvertLibraryToSvcPriority(priority),coreNo));

    this->SetHandle(handle);
    this->m_CanFinalize = false;
    this->m_UsingAutoStack = false;
    return ResultSuccess();
}

// Ori SDK Asms it.
__asm void Thread::CallDestructorAndExit(void* pStackBottom){    
    MOV             R2, #0 
    MOV             R1, R0 
    LDR             R0, =__cpp(&spAutoStackManager) // load AutoStackManager
    LDR             R0, [R0] 
    LDR             R3, [R0]
    LDR             R3, [R3,#0xC] // load AutoStackManager's 0xC vtable slot
    LDR             LR, =__cpp(nn::svc::ExitThread) // goto -> nn::svc::ExitThread and proceed
    BX              R3 // Branch eXchange AutoStackManager's vtable.
}


os::CTR::ThreadLocalRegion* spTlr = NULL;


namespace detail{


const s32 SVC_USER_THREAD_PRIORITY_HIGHEST    = 0x20; // 32
const s32 SVC_LIBRARY_THREAD_PRIORITY_HIGHEST = 0x18; // 24
const s32 LIBRARY_THREAD_PRIORITY_BASE    = 0x5109D500;
const s32 PRIVILEGED_THREAD_PRIORITY_BASE = 0x6C8DA500;

s32 ConvertSvcToLibraryPriority(s32 svc){
    if (svc >= SVC_USER_THREAD_PRIORITY_HIGHEST){
        const s32 offset = svc - SVC_USER_THREAD_PRIORITY_HIGHEST;
        return offset;
    }
    else if (svc >= SVC_LIBRARY_THREAD_PRIORITY_HIGHEST){
        const s32 offset = svc - SVC_LIBRARY_THREAD_PRIORITY_HIGHEST;
        return LIBRARY_THREAD_PRIORITY_BASE + offset;
    }
    else{
        return PRIVILEGED_THREAD_PRIORITY_BASE + svc;
    }
}

s32 ConvertLibraryToSvcPriority(s32 lib){
  if (lib >= 0 && lib <= SVC_USER_THREAD_PRIORITY_HIGHEST)
    return lib + SVC_USER_THREAD_PRIORITY_HIGHEST;

  if (lib >= LIBRARY_THREAD_PRIORITY_BASE && lib <= 0x5109D527) {
      const s32 offset = lib - LIBRARY_THREAD_PRIORITY_BASE;
    return SVC_LIBRARY_THREAD_PRIORITY_HIGHEST + offset;
  }
  if (lib >= PRIVILEGED_THREAD_PRIORITY_BASE && lib <= 0x6C8DA540) {
      const s32 offset = lib - PRIVILEGED_THREAD_PRIORITY_BASE;
    return offset;
  }
  return -1;
}


void SaveThreadLocalRegionAddress(){
    NN_TASSERT_(spTlr == NULL);
    s_pTlr = CTR::GetThreadLocalRegion();
}

void InitializeThreadEnvrionment(){
    os::ThreadLocalStorage::ClearAllSlots();
    os::CTR::SetupThreadCppExceptionEnvironment();
    _fp_init();
}

}
}
}