// Filename: ndm_UserControl.cpp
//
// Project: Horizon

#include <nn/ndm.h>
#include <nn/os/os_CriticalSection.h>
#include <nn/srv.h>
#include <nn/util/util_Result.h>
#include <nn/dbg/dbg_Break.h>
#include <string.h>

namespace{
    static s32 sInitializedCount = 0;
    static nn::os::CriticalSection sCs = nn::WithInitialize();
}

namespace nn{
namespace ndm{

using namespace CTR;
using namespace CTR::detail;

Result Initialize(){
    nn::os::CriticalSection::ScopedLock locker(sCs);
    Result result;

    if (sInitializedCount == 0){
        nn::srv::Initialize();
        result = nn::srv::GetServiceHandle(&CTR::detail::Interface::sSession, PORT_NAME_USER);
        NN_UTIL_RETURN_IF_FAILED(result);
    }
    ++sInitializedCount;
    return ResultSuccess();
}

Result SuspendDaemons(bit32 mask){
    return Interface::SuspendDaemons(mask);
}

Result ResumeDaemons(bit32 mask){
    return Interface::ResumeDaemons(mask);
}

Result Resume(DaemonName name){
    if (name < 0 || name >= NUM_OF_DAEMONS){
        return ResultInvalidEnumValue();
    }
    return ResumeDaemons(1 << name);
}

Result SuspendScheduler(bool bAsync){
    return Interface::SuspendScheduler(bAsync);
}

Result ResumeScheduler(void){
    return Interface::ResumeScheduler();
}

}
}