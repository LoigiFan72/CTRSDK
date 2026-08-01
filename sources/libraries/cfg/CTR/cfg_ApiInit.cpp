// Filename: cfg_ApiInit.cpp
//
// Project: Horizon

#include <nn/cfg/CTR/cfg_ApiInit.h>
#include <nn/cfg/CTR/cfg_DetailApi.h>
#include <nn/cfg/CTR/cfg_IpcInit.h>
#include <nn/cfg/CTR/cfg_IpcUser.h>
#include <nn/cfg/CTR/cfg_IpcSys.h>
#include <nn/srv/srv_Api.h>

#include <string.h>

namespace nn {
namespace cfg {
namespace CTR {
namespace detail {
namespace{
    static int sInitializeInitCount;
}

Result InitializeInit(){
    Result res;
    if(sInitializeInitCount == 0){
        res = detail::InitializeBase(&IpcInit::sSession,CTR::PORT_NAME_INIT);
        if(res.IsSuccess()){
            IpcSys::sSession = IpcInit::sSession;
            IpcUser::sSession = IpcInit::sSession;
        } 
        else {
            if(res == (Result)0xd92103fb){
                return res;
            }
        }
    }
    sInitializeInitCount++;
    return ResultSuccess();
}

void FinalizeInit(){
    if(sInitializeInitCount > 0){
        sInitializeInitCount--;
    }
    if(sInitializeInitCount != 0){
        return;
    }
    Result res = FinalizeBase(&IpcInit::sSession);
    if(res.IsSuccess()){
        IpcSys::sSession = INVALID_HANDLE_VALUE;
        IpcUser::sSession = INVALID_HANDLE_VALUE;
    }
}

}
}
}
}