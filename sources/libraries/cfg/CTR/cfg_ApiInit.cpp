// Filename: cfg_ApiInit.cpp
//
// Project: Horizon

#include <nn/cfg.h>
#include <nn/cfg/CTR/cfg_ApiInit.h>
#include <nn/cfg/CTR/cfg_DetailApi.h>
#include <nn/cfg/CTR/cfg_IpcInit.h>
#include <nn/cfg/CTR/cfg_IpcUser.h>
#include <nn/cfg/CTR/cfg_IpcSys.h>
#include <nn/srv/srv_API.h>

#include <string.h>

namespace nn {
namespace cfg {
namespace CTR {
namespace detail {
namespace{
    int s_InitializeInitCount = 0;
}

Result InitializeInit(){
    if(s_InitializeInitCount == 0){
        Result res = detail::InitializeBase(&IpcInit::s_Session,CTR::PORT_NAME_INIT);
        if(res.IsSuccess()){
            IpcSys::s_Session = IpcInit::s_Session;
            IpcUser::s_Session = IpcInit::s_Session;
        } 

        else if(res == ResultCancelRequested()){
            return res;
        }
    }
    s_InitializeInitCount++;
    return ResultSuccess();
}

void FinalizeInit(){
    if(s_InitializeInitCount > 0){
        s_InitializeInitCount--;
    }

    if(s_InitializeInitCount == 0){
        nn::Result result = detail::FinalizeBase(&detail::IpcInit::s_Session);
        if(result.IsSuccess()){
            IpcUser::s_Session = INVALID_HANDLE_VALUE;
            IpcSys::s_Session  = INVALID_HANDLE_VALUE;
        }
    }
}

}
}
}
}