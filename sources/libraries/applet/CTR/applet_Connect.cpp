// Filename: applet_Connect.cpp
//
// Project: Horizon

#include <nn/applet.h>
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
    const char* s_PortName = PORT_NAME_USER;
    nn::os::Mutex            s_Mutex;
}

void SetPortName(const char* name)
{
    if(s_PortName == NULL)
    {
        s_PortName = name;
    }
}

Result InitializePort(Handle* pSession)
{
    SetPortName(PORT_NAME_USER);
    if(pSession->IsValid())
    {
        return ResultAlreadyInitialized();
    } 
    return srv::GetServiceHandle(pSession,s_PortName);
}

Result FinalizePort(Handle* pSession)
{
    if (!pSession->IsValid())
    {
        return ResultNotInitialized();
    }

    Result result = nn::svc::CloseHandle(*pSession);
    *pSession = INVALID_HANDLE_VALUE;
    return result;
}

void Lock()
{
    if(s_Mutex.IsValid())
    {
        s_Mutex.Lock();
    }
}

void Unlock()
{
    if(s_Mutex.IsValid())
    {
        s_Mutex.Unlock();
    }
}

Result Connect()
{
    Result res = InitializePort(&APPLET::s_Session);
    NN_ERR_THROW_FATAL(res);
    return res;
}

Result Disconnect()
{
    Result res = FinalizePort(&APPLET::s_Session);
    NN_ERR_THROW_FATAL(res);
    return res;
}

void LockAndConnect()
{
    Lock();
    Connect();
}

void DisconnectAndUnlock()
{
    Disconnect();
    Unlock();
}

void InitializeMutex(nn::Handle handle)
{
    s_Mutex.SetHandle(handle);
}

}
}
}
}