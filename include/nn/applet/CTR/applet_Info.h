#pragma once

#include "nn/applet/CTR/applet_Paramaters.h"

namespace nn { 
namespace applet {
namespace CTR {
    AppletAttr GetAttribute();
    AppletAttr GetAppletType();
    void SetAttribute(AppletAttr attr);
    bool IsSystemApplet();
    bool IsApplication();
    bool IsInfoAccess();
    void SetHomeButtonState(HomeButtonState state);
    HomeButtonState GetHomeButtonState();
    void SetExpectationToJumpToHome(bool flag);
    bool IsExpectedToJumpToHomeMenu();
    AppletId GetId();
    void SetId(AppletId id);
    u32 GetMessageCommand();
    void SetMessageCommand(u32 message);
    SleepNotificationState GetSleepNoticationState();
    void SetSleepNotificationState(SleepNotificationState noti);
    TransitionType GetTransitionType();
    void SetTransitionType(TransitionType type);
    void SetShutdownCallbackFlag();
    bool IsToCallShutdownCallback();
    void ClearShutdownCallbackFlag();
    void SetOrderToCloseState(OrderToCloseState state);
    void SetPowerButtonCallbackFlag();
    bool IsToCallPowerButtonCallback();
    void ClearPowerButtonCallbackFlag();
    void SetReceivedWakeupByCancelFlag();
    bool IsReceivedWakeupByCancel();
    void SetActive(void);

namespace detail{
    bool IsActive(void);
    CTR::HomeButtonState GetAbsoluteHomeButtonState();
    void SetAbsoluteHomeButtonState(CTR::HomeButtonState hmBtnState);
    void ClearAbsoluteHomeButtonState();
    CTR::OrderToCloseState GetOrderToCloseState();
    void SetPowerButtonState(CTR::PowerButtonState powerState);
    CTR::SleepSysState GetSleepSysState();
    void SetSleepSysState(CTR::SleepSysState state);
    void ClearSleepSysState();
    void SetInactive();
    CTR::PowerButtonState GetPowerButtonState();
    void ClearPowerButtonState();
    CTR::OrderToCloseState GetOrderToCloseState();
    void ClearOrderToCloseState();
    void SetShutdownState(CTR::ShutdownState);
    bool IsAppletMode();
    
}

}
}
}