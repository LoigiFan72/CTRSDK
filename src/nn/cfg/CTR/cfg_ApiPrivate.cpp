// Filename: cfg_ApiPrivate.cpp
//
// Project: Horizon

#include <nn/cfg/CTR/cfg_DebugParam.h>
#include <nn/cfg/CTR/cfg_Api.h>
#include <nn/cfg/CTR/cfg_DetailApi.h>
#include <nn/cfg/CTR/cfg_IpcUser.h>
#include <nn/os/CTR/os_Environment.h>
#include <nn/Result.h>
#include <nn/err/CTR/err_Api.h>

namespace nn{
namespace cfg{
namespace CTR{
u8 GetFsLatencyEmulationParam(){
    Result res;
    DebugParamCfgData debugParam;
    detail::_IPCPortType portType;

    if (!os::IsRunOnDevelopmentHardWare()){
        return 0U;
    }

    res = detail::InitializeProperPort(&portType);
    if (res.IsFailure()) {
        NN_ERR_THROW_FATAL_ALL(res);
        NN_TLOG_("[cfg] Application is not permitted to use cfg.\n");
    }

    void* pData = &debugParam;
    NN_ERR_THROW_FATAL_ALL(detail::IpcUser::GetConfig(pData, 4, 0x130000));

    detail::_IPCPortType pt = portType;
    detail::FinalizeProperPort(pt);

    return debugParam.fsLatencyParam;
}

bool IsDebugMode(){
    Result res;
    DebugParamCfgData debugParam;
    detail::_IPCPortType portType;

    if (!os::IsRunOnDevelopmentHardWare()){
        return false;
    }

    res = detail::InitializeProperPort(&portType);
    if (res.IsFailure()) {
        NN_ERR_THROW_FATAL_ALL(res);
        NN_TLOG_("[cfg] Application is not permitted to use cfg.\n");
    }

    void* pData = &debugParam;
    NN_ERR_THROW_FATAL_ALL(detail::IpcUser::GetConfig(pData, 4, 0x130000));

    detail::_IPCPortType pt = portType;
    detail::FinalizeProperPort(pt);
    bool isMode;
    if((debugParam.param.flags1 & 1) == 0){
        return isMode = false;
    } 
    else{
        return isMode = true;
    }
}

}
}
}