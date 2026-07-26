// Filename: applet_Connect.cpp
//
// Project: Horizon

#include <nn/applet/CTR/applet_Connect.h>
#include <nn/applet/CTR/applet_Ipc.h>
#include <nn/srv/srv_API.h>
#include <nn/err/CTR/err_Api.h>
#include <nn/gxlow/CTR/gxlow_SystemUse.h>

#include <string.h>

namespace nn{
namespace applet{
namespace CTR{
namespace detail{
namespace{
    static os::Mutex sMutex;
}
namespace{
    const char* sPortName = PORT_NAME_USER;
}

void SetPortName(const char* name){
    if(sPortName == 0){
        sPortName = name;
    }
}

Result InitializePort(Handle* pSession){
    SetPortName(PORT_NAME_USER);
    Result res;
    if(pSession->IsValid()){
        return (Result)0xE0A0CFF9;
    } 
    return srv::GetServiceHandle(pSession,sPortName);
}

Result FinalizePort(Handle* pSession){
    Result res;
    if(pSession->IsValid()){
        res.mResult = svc::CloseHandle(*pSession).mResult;
        *pSession = INVALID_HANDLE_VALUE;
    } 
    else{
        return (Result)0xe0a0cff8;
    }
    return res;
}

void Lock(){
    if(sMutex.IsValid()){
        sMutex.Lock();
    }
}

void Unlock(){
    if(sMutex.IsValid()){
        sMutex.Unlock();
    }
}

Result Connect(){
    Result res = InitializePort(&APPLET::sSession);
    NN_ERR_THROW_FATAL(res);
    return res;
}

Result Disconnect(){
    Result res = FinalizePort(&APPLET::sSession);
    NN_ERR_THROW_FATAL(res);
    return res;
}

void LockAndConnect(){
    Lock();
    Connect();
}

void DisconnectAndUnlock(){
    Disconnect();
    Unlock();
}

void InitializeMutex(nn::Handle handle){
    sMutex.mHandle = handle.mHandle;
}

}
}
}
}