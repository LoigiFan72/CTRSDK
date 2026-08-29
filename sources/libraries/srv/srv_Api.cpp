// Filename: srv_Api.cpp
//
// Project: Horizon

#include <nn/srv.h>
#include <nn/svc.h>
#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/os.h>

#include <nn/util/util_Result.h>

#include <nn/dbg/dbg_DebugString.h>
#include <nn/dbg/dbg_Break.h>

#define NN_NOTIFICATION_PRIORITY 1359598848
#ifdef NN_BUILD_DEVELOPMENT
    const size_t DISPATCHER_STACK_SIZE  = 1024;
#else
    const size_t DISPATCHER_STACK_SIZE  =  512;
#endif

namespace nn {
namespace srv {
namespace detail {
    
class HandlerManager{
public:
    nn::fnd::IntrusiveLinkedList<NotificationHandler> m_Handlers;

    Result Register(NotificationHandler* pHandler, u32 message) {
        NN_POINTER_TASSERT_(pHandler);
        NN_TASSERT_(message != 0);
        NN_TASSERT_(!pHandler->mAttachedMessage == 0);

        pHandler->m_AttachedMessage = message;
        this->m_Handlers.PushBack(pHandler);
        return ResultSuccess();
    }
    NotificationHandler* Find(bit32 message){
        NN_TASSERT_(message != 0);

        NotificationHandler *i;
        for(i = this->m_Handlers.GetFront(); i != NULL; i = this->m_Handlers.GetNext(i)){
            if(i->m_AttachedMessage == message){
                break;
            }
        }
        return i;
    }

    NotificationHandler* Unregister(bit32 message){
        NN_ASSERT_(message != 0);

        NotificationHandler* p = Find(message);
        if(p != NULL){
            m_Handlers.Erase(p);
            p->m_AttachedMessage = NULL;
        }

        return p;
    }
};

} // detail

namespace{
    bool s_Initialized = false;
    s32 s_InitializeCount = 0;
    os::CriticalSection s_InitializeLock;
    os::Semaphore s_NotificationSemaphore;
    os::Thread s_NotificationDispatcher;
    detail::HandlerManager s_HandlerManager;
    os::CriticalSection s_ManagerLock;

    os::StackBuffer<DISPATCHER_STACK_SIZE> sStack;


    void DispatcherThread() {
        Result result;
        for(;;){
            s_NotificationSemaphore.Acquire();
            result = DispatchNotification();
            NN_UTIL_PANIC_IF_FAILED(result);
        }
    }
} // namespace

namespace detail{
    bool IsInitialized() { return s_InitializeCount > 0; }

    NN_NOINLINE Result Connect(const char* name) {
        Result res;
        while (true) {
            res = svc::ConnectToPort(&Service::sSession, name);
            if (res.GetLevel()       != Result::LEVEL_PERMANENT   ||
                res.GetSummary()     != Result::SUMMARY_NOT_FOUND ||
                res.GetDescription() != 1018) 
                break;
            os::Thread::Sleep(fnd::TimeSpan::FromNanoSeconds(1000 * 500));
        }
        if (res.IsSuccess()) {
            res = Service::RegisterClient();
            s_InitializeCount++;
        }
        return res;
    }
} // namespace detail

Result Initialize() {
    os::CriticalSection::ScopedLock lock(s_InitializeLock);
    NN_MIN_TASSERT_(sInitializeCount, 0);
    if (srv::s_InitializeCount > 0) {
        s_InitializeCount++;
        return MakeInfoResult(Result::SUMMARY_NOTHING_HAPPENED, Result::MODULE_NN_SRV, Result::DESCRIPTION_ALREADY_INITIALIZED);
    }
    else{ 
        return detail::Connect(srv::PORT_NAME); 
    }
}

Result StartNotification() {
    Result res = EnableNotification(&s_NotificationSemaphore);
    if (res.IsSuccess()) {
        s_NotificationDispatcher.Start(&DispatcherThread, sStack, NN_NOTIFICATION_PRIORITY);
    }
}

Result EnableNotification(os::Semaphore* pOut) {
    Result res;
    Handle h;
    res = detail::Service::EnableNotication(&h);
    if (res.IsSuccess()){
        pOut->SetHandle(h);
    }
    return res;
}

Result DispatchNotification() {
    bit32 message;
    Result res = detail::Service::ReceiveNotification(&message);
    if(res.IsFailure()){
        return res;
    }

    NotificationHandler* pHandler = s_HandlerManager.Find(message);
    if (pHandler != NULL) {
        pHandler->HandleNotification(message);
        return ResultSuccess();
    }
    else{
        return ResultSuccess();
    }
}

Result RegisterNotificationHandler(NotificationHandler* pHandler, u32 message) {
    return s_HandlerManager.Register(pHandler, message);
}

NotificationHandler* UnregisterNotificationHandler(bit32 message){
    nn::os::CriticalSection::ScopedLock lock(s_ManagerLock);
    return s_HandlerManager.Unregister(message);
}

Result GetServiceHandle(nn::Handle* pOut, const char* pName, s32 nameLen, bit32 flags) {
    Result res;
    if (!detail::IsInitialized()) {
        return ResultNotInitialized();
    }
    if (nameLen > MAX_SERVICE_NAME_LEN) {
        return ResultTooLongServiceName();
    }
    NN_TWARNING_(res.IsSuccess(), "Failed to open service \"%s\"\n", pName);
    return nn::srv::detail::Service::GetServiceHandle(pOut, pName, nameLen, flags);
}

}
}