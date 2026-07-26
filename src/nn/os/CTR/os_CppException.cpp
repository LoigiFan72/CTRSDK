// Filename: os_CppException.cpp
//
// Project: Horizon

#include <cstdlib>
#include <exception>

#include <nn/os/CTR/os_CppException.h>
#include <nn/os/CTR/os_ThreadLocalRegion.h>

namespace{

extern "C" __weak void __ARM_exceptions_buffer_required(void);
extern "C" void* __ARM_exceptions_buffer_init(void);

typedef void (*handler)(void);

struct __cxa_eh_globals{
    uint32_t uncaughtExceptions;
    std::unexpected_handler unexpectedHandler;
    std::terminate_handler terminateHandler;
    bool implementation_ever_called_terminate;
    s8 rev[3];
    handler call_hook;
    void* caughtExceptions;
    void* propagatingExceptions;
    void* emergency_buffer;
};

bool IsArmExceptionsBufferNotRequired(){
    return &__ARM_exceptions_buffer_required == NULL;
}

void DefaultTerminateHandler(){
    std::abort();
}

void DefaultUnexpectedHandler(){
    std::terminate();
}

}

namespace nn{
namespace os{
namespace CTR{

__weak void SetupThreadCppExceptionEnvironment(){
    CTR::ThreadLocalRegion* pTlr = os::CTR::GetThreadLocalRegion();
    pTlr->ehGlobalsAddr = pTlr->ehGlobals;

    __cxa_eh_globals& ceg = *reinterpret_cast<__cxa_eh_globals*>(pTlr->ehGlobals);
    ceg.uncaughtExceptions = 0;
    ceg.unexpectedHandler = DefaultUnexpectedHandler;
    ceg.terminateHandler  = DefaultTerminateHandler;
    ceg.implementation_ever_called_terminate = false;
    ceg.call_hook = NULL;
    ceg.caughtExceptions = NULL;
    ceg.propagatingExceptions = NULL;
    ceg.emergency_buffer = IsArmExceptionsBufferNotRequired() ? NULL: __ARM_exceptions_buffer_init();
}


}
}
}