// Filename: os_WaitableCounter.cpp
//
// Project: Horizon

#include <nn/os/os_WaitableCounter.h>
#include <nn/svc.h>
#include <nn/Assert.h>

namespace nn{
namespace os{

nnHandle WaitableCounter::sHandle = {0};


void WaitableCounter::Initialize(){
    if(sHandle.value == INVALID_HANDLE_VALUE.value){
        Handle h;
        Result ret = nn::svc::CreateAddressArbiter(&h);
        NN_TASSERT_(ret.IsSuccess());
        if(ret.IsSuccess()){
            sHandle = h;
        }
    }
}

}
}