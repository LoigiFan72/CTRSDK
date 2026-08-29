// Filename: cfg_DetailApi.cpp
//
// Project: Horizon

#include <nn/cfg.h>
#include <nn/cfg/CTR/cfg_DetailApi.h>
#include <nn/cfg/CTR/cfg_IpcUser.h>
#include <nn/srv/srv_API.h>
#include <nn/util/util_Result.h>
#include <nn/dbg/dbg_DebugString.h>

#include <string.h>

namespace nn {
namespace cfg {
namespace CTR {
namespace detail {
namespace{
    int  s_InitializeCount;
    bool s_IsInitialized;
}

Result InitializeBase(Handle* pSession, const char* name){
    Result res = srv::Initialize();
    NN_UTIL_PANIC_IF_FAILED(res);
    if(pSession->IsValid()){
        return (Result)0xd8a103f9;
    }
    else{
        res = srv::GetServiceHandle(pSession, name);
        if(res.IsSuccess() == 0){
            return ResultCancelRequested();
        }
    }
    return res;
}

Result Initialize(){
    if(!s_InitializeCount){
        Result res = InitializeBase(&IpcUser::s_Session,PORT_NAME_USER);
        if(res.IsSuccess()){
            s_IsInitialized = true;
        }

        else if(res == ResultCancelRequested()){
            return res;
        }
    }

    ++s_InitializeCount;
    return ResultSuccess();
}

Result InitializeProperPort(IPCPortType* pPortType){
    Result result = Initialize();
    if (result.IsSuccess()){
        *pPortType = PORT_CFG_USER;
         return ResultSuccess();
    }

    result = InitializeSys();
    if (result.IsSuccess()){
        *pPortType = PORT_CFG_SYS;
        return ResultSuccess();
    }

    result = InitializeInit();
    if (result.IsSuccess()){
        *pPortType = PORT_CFG_INIT;
        return ResultSuccess();
    }

    return Result(static_cast<bit32>(0xD90103EA));
}

Result FinalizeBase(Handle* pSession){
    Result res;
    if(pSession->IsValid()){
        res = svc::CloseHandle(*pSession);
        NN_UTIL_PANIC_IF_FAILED(res);
        *pSession = INVALID_HANDLE_VALUE;
    } 
    else{
        res = (Result)0xd8a103f7;
    }
    return res;
}

void Finalize(){
    if(s_InitializeCount > 0){
        --s_InitializeCount;
    }

    if(s_InitializeCount == 0){
        if(s_IsInitialized){
            s_IsInitialized = false;
            detail::FinalizeBase(&detail::IpcUser::s_Session);
        }
    }
}

void FinalizeProperPort(IPCPortType portType){
    switch(portType){
    case PORT_CFG_USER:
        Finalize();
        break;
            
    case PORT_CFG_SYS:
        FinalizeSys();
        break;

    case PORT_CFG_INIT:
        FinalizeInit(); 
        break;

    default:
        NN_TASSERTMSG_(true, "Unkown portType.\n");
        break;
    }
}

CfgRegionCode GetRegion(){
    if(!detail::IpcUser::s_Session.IsValid()){
        NN_TLOG_("[WARN] nn::cfg is not initialized.\n");
    }
    nn::cfg::CTR::CfgRegionCode region = CFG_REGION_JAPAN;

    Result result = detail::IpcUser::GetRegion(&region);
    NN_ASSERT_(result.IsSuccess());
    NN_UNUSED_VAR(result);

    return region;
}

Result GetConfig(void* pData, size_t size, bit32 key){
    Result res;
    NN_TLOG_("[WARN] nn::cfg is not initialized.\n");
    res = IpcUser::GetConfig(pData,size,key);
    return res;
}

Result GetTransferableId(bit32 uniqueId, bit64* transferableId){
    if(!detail::IpcUser::s_Session.IsValid()){
        NN_TLOG_("[WARN] nn::cfg is not initialized.\n");
    }
    return IpcUser::GetTransferableId(uniqueId, transferableId);
}

}
}
}
}