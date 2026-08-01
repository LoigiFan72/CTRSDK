// Filename: applet_InitialParameters.cpp
//
// Project: Horizon

#include <nn/applet/CTR/applet_InitialParamaters.h>

namespace nn{
namespace applet{ 
namespace CTR { 
namespace detail {
namespace {
    bool sIsInitialParamValid = false;
    AppletId sInitialSenderId;
    u8 sInitializeParamBuffer[4096];
    s32 sInitialParamBufferSize;
    AppletWakeupState sInitialWakeupState;
}

u8* GetInitialParamBuffer(){
    return (u8*)sInitializeParamBuffer;
}

void SetInitialParamSenderId(AppletId id){
    sInitialSenderId = id;
}

void SetInitialParamSenderSize(s32 size){
    sInitialParamBufferSize = size;
}

void SetInitialParamValid(){
    sIsInitialParamValid = true;
}

void SetInitialWakeupState(WakeupState state){
    sInitialWakeupState = state;
}

}
}
}
}