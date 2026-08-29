// Filename: applet_InitialParameters.cpp
//
// Project: Horizon

#include <nn/applet/CTR/applet_InitialParamaters.h>

namespace nn{
namespace applet{ 
namespace CTR { 
namespace detail {
namespace {
    bool              s_IsInitialParamValid = false;
    AppletId          s_InitialSenderId;
    u8                s_InitializeParamBuffer[4096];
    s32               s_InitialParamBufferSize;
    AppletWakeupState s_InitialWakeupState;
}

u8* GetInitialParamBuffer(){
    return s_InitializeParamBuffer;
}

void SetInitialParamSenderId(AppletId id){
    s_InitialSenderId = id;
}

void SetInitialParamSenderSize(s32 size){
    s_InitialParamBufferSize = size;
}

void SetInitialParamValid(){
    s_IsInitialParamValid = true;
}

void SetInitialWakeupState(WakeupState state){
    s_InitialWakeupState = state;
}

}
}
}
}