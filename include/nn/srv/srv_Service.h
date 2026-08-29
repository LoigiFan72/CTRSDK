#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>

namespace nn{
namespace srv{
namespace detail{
    
class Service{
public:
    static Handle s_Session;

    static Result EnableNotication(Handle* pSemaphore); 
    static Result GetServiceHandle(Handle* pOut, const char* name, s32 nameLen, u32 flags);
    static Result ReceiveNotification(bit32 *pOut);
    static Result RegisterClient();
    static Result Subscribe(bit32 message);
    static Result Unsubscribe(bit32 message);
};

}
}
}