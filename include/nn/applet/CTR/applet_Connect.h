#pragma once

#include <nn/applet/CTR/applet_Paramaters.h>
#include <nn/os/os_Mutex.h>

namespace nn{
namespace applet{
namespace CTR{
namespace detail{
namespace{

    const char PORT_NAME_SYSTEM[] = "APT:S"; // APPLET SYSTEM
    const char PORT_NAME_USER[] = "APT:U"; // APPLET USER
}
    //--- Initialize / Connect
    void InitializeMutex(Handle handle);
    Result Connect();
    void LockAndConnect();

    //--- Finalize / Disconnect
    Result Disconnect();
    void DisconnectAndUnlock();

}
}
}
}