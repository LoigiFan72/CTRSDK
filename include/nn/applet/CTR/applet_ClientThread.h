#pragma once

#include <nn/applet/CTR/applet_Paramaters.h>
#include <nn/os/os_Event.h>

namespace nn { 
namespace applet {
namespace CTR {
namespace detail{
    void InitializeClientThread(s32 threadPriority, Handle hControl, Handle hMessage);
    void ThreadFunc(int param);

    void SetReceiveCallback(AppletReceiveCallback callback,uptr parameter);

    void FinalizeClientThread();
    
    void WaitForControlEvent();
    bool TryWaitForControlEvent();
    void ClearControlEvent();

}
}
}
}