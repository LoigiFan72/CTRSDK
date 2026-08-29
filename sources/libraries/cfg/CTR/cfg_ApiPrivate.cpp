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
    nn::Result res;
    DebugParamCfgData debugParam;
    if (!os::IsRunOnDevelopmentHardWare()){
        return 0U;
    }

    detail::_IPCPortType portType;
    res = detail::InitializeProperPort(&portType);
    if (res.IsFailure()) {
        NN_ERR_THROW_FATAL_ALL(res);
        NN_TLOG_("[cfg] Application is not permitted to use cfg.\n");
    }

    void* pData = &debugParam;
    NN_ERR_THROW_FATAL_ALL(detail::IpcUser::GetConfig(pData, 4, 0x130000));

    detail::FinalizeProperPort(portType);

    return debugParam.fsLatencyParam;
}

bool IsDebugMode(){
    Result res;
    DebugParamCfgData debugParam;

    if (!os::IsRunOnDevelopmentHardWare()){
        return false;
    }

    detail::_IPCPortType portType;
    res = detail::InitializeProperPort(&portType);
    if (res.IsFailure()) {
        NN_ERR_THROW_FATAL_ALL(res);
        NN_TLOG_("[cfg] Application is not permitted to use cfg.\n");
    }

    void* pData = &debugParam;
    NN_ERR_THROW_FATAL_ALL(detail::IpcUser::GetConfig(pData, 4, 0x130000));

    detail::FinalizeProperPort(portType);
    bool isMode;
    if(debugParam.param.flags1 & 1){
        return true;
    } 
    else{
        return false;
    }
}

}
}
}