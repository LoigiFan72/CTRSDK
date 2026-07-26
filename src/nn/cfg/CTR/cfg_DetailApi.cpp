// Filename: cfg_DetailApi.cpp
//
// Project: Horizon

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
    static int sInitializeCount;
    static bool sIsInitialized;
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
            return (Result)0xd92103fb;
        }
    }
    return res;
}

Result Initialize(){
    Result res;
    if(!sInitializeCount){
        res = InitializeBase(&IpcUser::sSession,PORT_NAME_USER);
        if(res.IsSuccess()){
            sIsInitialized = true;
        } 
        else if(res == Result(0xd92103fb)){
            return res;
        }
    }
    ++sInitializeCount;
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
    if(sInitializeCount > 0)
        sInitializeCount--;
    if((sInitializeCount == 0)){
        if(sIsInitialized){
            sIsInitialized = false;
            FinalizeBase(&IpcUser::sSession);
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

Result GetConfig(void* pData, size_t size, bit32 key){
    Result res;
    NN_TLOG_("[WARN] nn::cfg is not initialized.\n");
    res = IpcUser::GetConfig(pData,size,key);
    return res;
}
}
}
}
}