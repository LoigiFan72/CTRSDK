#pragma once

#include <nn/applet/CTR/applet_Paramaters.h>

namespace nn{
namespace applet{ 
namespace CTR { 
namespace detail {
    //--- APT Params
    u8* GetInitialParamBuffer();

    void SetInitialParamSenderId(AppletId id);
    void SetInitialParamSenderSize(s32 size);
    void SetInitialParamValid();
    void SetInitialWakeupState(WakeupState state);

}
}
}
}