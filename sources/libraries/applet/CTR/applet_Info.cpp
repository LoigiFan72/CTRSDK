// Filename: applet_Info.cpp
//
// Project: Horizon

#include <nn/applet/CTR/applet_Info.h>

namespace nn{
namespace applet{
namespace CTR{
namespace{
    AppletId                    s_Id;
    AppletAttr                  s_Attribute;
    bool                        s_IsAppletMode = false;
    bool                        s_IsActive = false;
    u32                         s_MessageCommand = COMMAND_NONE;
    HomeButtonState             s_AbsoluteHomeButtonState = HOME_BUTTON_NONE;
    SleepSysState               s_SleepSysState = SLEEP_SYS_STATE_NONE;
    ShutdownState               s_ShutdownState = SHUTDOWN_STATE_NONE;
    PowerButtonState            s_PowerButtonState = POWER_BUTTON_STATE_NONE;
    OrderToCloseState           s_OrderToCloseState = ORDER_TO_CLOSE_STATE_NONE;
    bool                        s_IsToCallPowerButtonCallback = false;
    bool                        s_IsToCallShutdownCallback = false;
    bool                        s_IsReceivedWakeupByCancelFlag = false;
    TransitionType              s_PrevTransition = TRANSITION_NONE;
    SleepNotificationState      s_SleepNotificationState = NOTIFY_NONE;
    HomeButtonState             s_HomeButtonState = HOME_BUTTON_NONE;
    bool                        s_IsExpectedToJumpToHomeMenu = false;
}

CTR::AppletAttr GetAttribute(){
    return s_Attribute;
}

CTR::AppletAttr GetAppletType(){
    return GetAttribute() & 7;
}

void SetAttribute(CTR::AppletAttr attribute){
    s_Attribute = attribute;
}

bool IsSystemApplet(){
    return s_Attribute & 7 == 2;
}

bool IsApplication(){
    return s_Attribute & 7 == 0;
}

bool IsInfoAccess(){
    return s_Attribute & 7 == 6;
}

void SetHomeButtonState(CTR::HomeButtonState state){
    s_HomeButtonState = state;
}

CTR::HomeButtonState GetHomeButtonState(){
    return s_HomeButtonState;
}

void SetExpectationToJumpToHome(bool flag){
    s_IsExpectedToJumpToHomeMenu = flag;
}

bool IsExpectedToJumpToHomeMenu(){
    return s_IsExpectedToJumpToHomeMenu;
}

CTR::AppletId GetId(){
    return s_Id;
}

void SetId(CTR::AppletId id){
    s_Id = id;
}

u32 GetMessageCommand(){
    return s_MessageCommand;
}

void SetMessageCommand(u32 message){
    s_MessageCommand = message;
}

SleepNotificationState GetSleepNoticationState(){
    return s_SleepNotificationState;
}

void SetSleepNotificationState(SleepNotificationState state){
    s_SleepNotificationState = state;
}

TransitionType GetTransitionType(){
    return s_PrevTransition;
}

void SetTransitionType(TransitionType type){
    s_PrevTransition = type;
}

void SetShutdownCallbackFlag(){
    s_IsToCallShutdownCallback = true;
}

void ClearShutdownCallbackFlag(){
    s_IsToCallShutdownCallback = false;
}

bool IsToShutdownCallback(){
    return s_IsToCallShutdownCallback;
}

void SetPowerButtonCallbackFlag(){
    s_IsToCallPowerButtonCallback = 1;
}

bool IsToCallPowerButtonCallback(){
    return s_IsToCallPowerButtonCallback;
}

void ClearPowerButtonCallbackFlag(){
    s_IsToCallPowerButtonCallback = 0;
}

void SetReceivedWakeupByCancelFlag(){
    s_IsReceivedWakeupByCancelFlag = true;
}

bool IsReceivedWakeupByCancel(){
    return s_IsReceivedWakeupByCancelFlag;
}

void SetOrderToCloseState(OrderToCloseState state){
    s_OrderToCloseState = state;
}

namespace detail{

CTR::HomeButtonState GetAbsoluteHomeButtonState(){
    CTR::s_AbsoluteHomeButtonState;
}

void SetAbsoluteHomeButtonState(CTR::HomeButtonState state){
    CTR::s_AbsoluteHomeButtonState = state;
}

void ClearAbsoluteHomeButtonState(){
    CTR::s_AbsoluteHomeButtonState = HOME_BUTTON_NONE;
}

CTR::SleepSysState GetSleepSysState(){
    return CTR::s_SleepSysState;
}

void SetSleepSysState(CTR::SleepSysState state){
    CTR::s_SleepSysState = state;
}

bool IsActive(){
    return CTR::s_IsActive;
}

void SetActive(){
    CTR::s_IsActive = true;
}

void SetInactive(){
    CTR::s_IsActive = false;
}

CTR::PowerButtonState GetPowerButtonState(){
    return CTR::s_PowerButtonState;
}

void SetPowerButtonState(CTR::PowerButtonState state){
    CTR::s_PowerButtonState = state;
}

CTR::OrderToCloseState GetOrderToCloseState(){
    return CTR::s_OrderToCloseState;
}

void ClearSleepSysState(){
    CTR::s_SleepSysState = SLEEP_SYS_STATE_NONE;
}

void SetShutdownState(CTR::ShutdownState state){
    CTR::s_ShutdownState = state;
}

bool IsAppletMode(){
    return CTR::s_IsAppletMode;
}

}
}
}
}