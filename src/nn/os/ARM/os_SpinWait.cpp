// Filename: os_SpinWait.cpp
//
// Project: Horizon

#include <nn/os/ARM/os_SpinWait.h>

namespace nn{
namespace os{
namespace ARM{

__asm void SpinWaitCpuCycles(){
    SUBS            R0, R0, #2
    NOP
    BGT             __cpp(nn::os::ARM::SpinWaitCpuCycles)
    BX              LR
}

}
}
}