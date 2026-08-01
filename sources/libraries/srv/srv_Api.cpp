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
    nn::fnd::IntrusiveLinkedList<NotificationHandler> mHandler;

    Result Register(NotificationHandler* pHandler, u32 message) {
        NN_POINTER_TASSERT_(pHandler);
        NN_TASSERT_(message != 0);
        NN_TASSERT_(!pHandler->mAttachedMessage == 0);

        pHandler->mAttachedMessage = message;
        this->mHandler.PushBack(pHandler);
        return ResultSuccess();
    }
    NotificationHandler* Find(bit32 message){
        NN_TASSERT_(message != 0);

        NotificationHandler *i;
        for(i = this->mHandler.GetFront(); i != NULL; i = this->mHandler.GetNext(i)){
            if(i->mAttachedMessage == message){
                break;
            }
        }
        return i;
    }

};

} // detail

namespace{
    bool sInitialized = false;
    s32 sInitializeCount = 0;
    static os::CriticalSection sInitializeLock;
    static os::Semaphore sNotificationSemaphore;
    static os::Thread sNotificationDispatcher;
    static detail::HandlerManager sHandlerManager;
    os::StackBuffer<DISPATCHER_STACK_SIZE> sStack;

    void DispatcherThread() {
        Result result;
        for(;;){
            sNotificationSemaphore.Acquire();
            result = DispatchNotification();
            NN_PANIC_IF_FAILED(result);
        }
    }
} // namespace

namespace detail{
    bool IsInitialized() { return sInitializeCount > 0; }

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
            sInitializeCount++;
        }
        return res;
    }
} // detail

Result Initialize() {
    os::CriticalSection::ScopedLock lock(sInitializeLock);
    NN_MIN_TASSERT_(sInitializeCount, 0);
    if (srv::sInitializeCount > 0) {
        sInitializeCount++;
        return MakeInfoResult(Result::SUMMARY_NOTHING_HAPPENED, Result::MODULE_NN_SRV, Result::DESCRIPTION_ALREADY_INITIALIZED);
    }
    else{ 
        return detail::Connect(srv::PORT_NAME); 
    }
}

Result StartNotification() {
    Result res = EnableNotification(&sNotificationSemaphore);
    if (res.IsSuccess()) {
        sNotificationDispatcher.Start(&DispatcherThread, sStack, NN_NOTIFICATION_PRIORITY);
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

    NotificationHandler* pHandler = sHandlerManager.Find(message);
    if (pHandler != NULL) {
        pHandler->HandleNotification(message);
        return ResultSuccess();
    }
    else{
        return ResultSuccess();
    }
}

Result RegisterNotificationHandler(NotificationHandler* pHandler, u32 message) {
    return sHandlerManager.Register(pHandler, message);
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