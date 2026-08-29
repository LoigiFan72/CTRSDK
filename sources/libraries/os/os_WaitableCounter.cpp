// Filename: os_WaitableCounter.cpp
//
// Project: Horizon

#include <nn/os/os_WaitableCounter.h>
#include <nn/svc.h>
#include <nn/Assert.h>

namespace nn{
namespace os{

// me: no sti, roll back to kitchen
//
// sti : aww :(

nnHandle WaitableCounter::s_Handle = {0}; 

void WaitableCounter::Initialize()
{
    if(s_Handle.value == INVALID_HANDLE_VALUE.value)
    {
        Handle h;
        Result ret = nn::svc::CreateAddressArbiter(&h);
        NN_TASSERT_(ret.IsSuccess());
        if(ret.IsSuccess())
        {
            s_Handle = h;
        }
    }
}

}
}