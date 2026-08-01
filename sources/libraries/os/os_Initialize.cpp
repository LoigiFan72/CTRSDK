// Filename: os_Initialize.cpp
//
// Project: Horizon

#include <nn/os.h>

namespace nn{
namespace os{
    
void Initialize(){
    WaitableCounter::Initialize();
    detail::SaveThreadLocalRegionAddress();
    detail::InitializeSharedMemory();
    detail::InitializeStackMemory();
    detail::InitializeThreadEnvrionment();
}

}
}

extern "C" {

void nnosInitialize(){
    nn::os::Initialize();
}

}