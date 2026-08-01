#pragma once

#include <nn/applet/CTR/applet_Paramaters.h>
#include <nn/os/os_Event.h>

namespace nn { 
namespace applet {
namespace CTR {
namespace detail{
    //--- Initialize / Finalize
    void InitializeClientThread(s32 threadPriority, Handle hControl, Handle hMessage);
    void FinalizeClientThread();
    void SetReceiveCallback(AppletReceiveCallback callback,uptr parameter);
    
    //--- Control Event
    void WaitForControlEvent();
    bool TryWaitForControlEvent();
    void ClearControlEvent();
}
}
}
}