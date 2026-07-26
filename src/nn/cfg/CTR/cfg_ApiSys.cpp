// Filename: cfg_ApiSys.cpp
//
// Project: Horizon

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
    static bool sIsInitializedSys;
    static int sInitializeSysCount;
}

Result InitializeSys(){
    Result res;
    if(sInitializeSysCount == 0){
        res = InitializeBase(&IpcSys::sSession,CTR::PORT_NAME_SYSTEM);
        if(res.IsSuccess()){
            sIsInitializedSys = true;
            IpcUser::sSession = IpcSys::sSession;
        } 
        else {
            if(res == (Result)0xd92103fb){
                return res;
            }
        }
    }
    sInitializeSysCount++;
    return ResultSuccess();
}

void FinalizeSys(){
    if(sInitializeSysCount > 0){
        sInitializeSysCount--;
    }
    if(sInitializeSysCount != 0){
        return;
    }
    if(!sIsInitializedSys){
        return;
    }
    sIsInitializedSys = false;
    Result res = FinalizeBase(&IpcInit::sSession);
    if(res.IsSuccess()){
        nn::Handle h = IpcSys::sSession;
        IpcUser::sSession = h;
    }
}

}
}
}
}