// Filename: cfg_ApiSys.cpp
//
// Project: Horizon

#include <nn/cfg.h>
#include <nn/cfg/CTR/cfg_ApiSys.h>
#include <nn/cfg/CTR/cfg_DetailApi.h>
#include <nn/cfg/CTR/cfg_IpcSys.h>
#include <nn/cfg/CTR/cfg_IpcInit.h>
#include <nn/cfg/CTR/cfg_IpcUser.h>
#include <nn/srv/srv_Api.h>

#include <string.h>

namespace nn {
namespace cfg {
namespace CTR {
namespace detail {
namespace{
    bool s_IsInitializedSys;
    int  s_InitializeSysCount;
}

Result InitializeSys(){
    Result res;
    if(s_InitializeSysCount == 0){
        Result res = InitializeBase(&IpcSys::s_Session,CTR::PORT_NAME_SYSTEM);
        if(res.IsSuccess()){
            s_IsInitializedSys = true;
            IpcUser::s_Session = IpcSys::s_Session;
        } 
        else if(res == ResultCancelRequested()){
            return res;
        }
    }
    s_InitializeSysCount++;
    return ResultSuccess();
}

void FinalizeSys(){
    if(s_InitializeSysCount > 0){
        --s_InitializeSysCount;
    }

    if(s_InitializeSysCount == 0){
        if(s_IsInitializedSys){
            s_IsInitializedSys = false;
            nn::Result result = detail::FinalizeBase(&detail::IpcSys::s_Session);
            if(result.IsSuccess()){
                IpcUser::s_Session = INVALID_HANDLE_VALUE;
            }
        }
    }
}

}
}
}
}