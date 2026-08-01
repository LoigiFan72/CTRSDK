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
    return CTR::detail::Interface::SuspendDaemons(mask);
}

}
}