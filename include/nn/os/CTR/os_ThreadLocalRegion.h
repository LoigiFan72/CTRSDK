#pragma once

#include <nn/types.h>
#include <nn/Assert.h>
#include <nn/hardware/hardware_RegAccess.h>

namespace nn {
namespace os {
namespace CTR {


struct ThreadLocalRegion
{
    uptr  tls[16];
    uptr  handlerAddress;
    uptr  handlerStackBottomAddress;
    uptr  exceptionBufferAddress;
    bit8  reserved[16];
    void* ehGlobalsAddr;
    bit32 ehGlobals[8];
    bit32 messageBuffer[64];
    bit32 receiveBuffer[32];
};

inline ThreadLocalRegion* GetThreadLocalRegion()
{
    ThreadLocalRegion* p;
    HW_GET_CP15_THREAD_ID_USER_READ_ONLY(p);
    return p;
}



}
}
}