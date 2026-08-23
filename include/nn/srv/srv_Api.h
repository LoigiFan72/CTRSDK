#pragma once

#include <nn/os/os_LightEvent.h>
#include <nn/os/os_CriticalSection.h>
#include <nn/fnd/fnd_Intrusive.h>

namespace nn {
namespace os{
class Semaphore;

}
namespace srv {
class NotificationHandler;

struct NotificationHandler : public fnd::IntrusiveLinkedList<NotificationHandler>::Item{
    bit32 mAttachedMessage;

    NotificationHandler(): 
        mAttachedMessage(0)
    { }
    virtual ~NotificationHandler(){ }
    virtual void HandleNotification(u32 message) = 0;
};

template <typename T>
class EventNotificationHandlerBase : public NotificationHandler{
public:
    EventNotificationHandlerBase(): 
        mpEvent(NULL) 
    { }
    EventNotificationHandlerBase(T* p): 
        mpEvent(p) 
    { }

    void Initialize(T* p){
        NN_POINTER_TASSERT_(p);
        mpEvent = p;
    }
    T* mpEvent;

    virtual void HandleNotification(bit32 mMessage){
        NN_POINTER_TASSERT_(this->mpEvent);
        this->mpEvent->Signal();
    }
};

typedef EventNotificationHandlerBase<os::LightEvent> LightEventNotificationHandler;

Result Initialize();
Result StartNotification();
Result EnableNotification(os::Semaphore*);
Result DispatchNotification();
Result RegisterNotificationHandler(NotificationHandler* pHandler, bit32 message);
NotificationHandler* UnregisterNotificationHandler(bit32 message);
Result GetServiceHandle(Handle* pOut, const char* pName, s32 nameLen, bit32 flags);

inline Result Subscribe(bit32 message){ return detail::Service::Subscribe(message); }
inline Result GetServiceHandle(Handle* pOut, const char* pName){ return GetServiceHandle(pOut, pName, strlen(pName), 0); }

namespace detail {

Result Connect(const char* pName);

} // namespace detail

namespace {
    extern const char PORT_NAME_SERVICE;
} // namespace

} // namespace srv
} // namespace nn
