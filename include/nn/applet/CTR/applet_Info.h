#pragma once

#include "nn/applet/CTR/applet_Paramaters.h"

namespace nn { 
namespace applet {
namespace CTR {
    nn::applet::CTR::AppletAttr GetAttribute();
    nn::applet::CTR::AppletAttr GetAppletType();
    void SetAttribute(nn::applet::CTR::AppletAttr);
    s8 IsSystemApplet();
    s8 IsApplication();
    s8 IsInfoAccess();
    void SetHomeButtonState(nn::applet::CTR::HomeButtonState);
    nn::applet::CTR::HomeButtonState GetHomeButtonState();
    void SetExpectationToJumpToHome(bool flag);
    s8 IsExpectedToJumpToHomeMenu();
    nn::applet::CTR::AppletId GetId();
    void SetId(nn::applet::CTR::AppletId);
    u32 GetMessageCommand();
    void SetMessageCommand(u32 message);
    nn::applet::CTR::SleepNotificationState GetSleepNoticationState();
    void SetSleepNotificationState(nn::applet::CTR::SleepNotificationState);
    nn::applet::CTR::TransitionType GetTransitionType();
    void SetTransitionType(nn::applet::CTR::TransitionType);
    void SetShutdownCallbackFlag();
    s8 IsToCallShutdownCallback();
    void ClearShutdownCallbackFlag();
    void SetOrderToCloseState(CTR::OrderToCloseState state);
    void SetPowerButtonCallbackFlag();
    s8 IsToCallPowerButtonCallback();
    void ClearPowerButtonCallbackFlag();
    void SetReceivedWakeupByCancelFlag();
    s8 IsReceivedWakeupByCancel();
    void SetActive(void);

namespace detail{
    bool IsActive(void);
    CTR::HomeButtonState GetAbsoluteHomeButtonState();
    void SetAbsoluteHomeButtonState(CTR::HomeButtonState);
    void ClearAbsoluteHomeButtonState();
    CTR::OrderToCloseState GetOrderToCloseState();
    void SetPowerButtonState(CTR::PowerButtonState);
    CTR::SleepSysState GetSleepSysState();
    void SetSleepSysState(CTR::SleepSysState);
    void ClearSleepSysState();
    void SetInactive(void);
    CTR::PowerButtonState GetPowerButtonState();
    void ClearPowerButtonState(void);
    CTR::OrderToCloseState GetOrderToCloseState();
    void ClearOrderToCloseState(void);
    void SetShutdownState(CTR::ShutdownState);
    s8 IsAppletMode();
    
}
namespace{

    s8 isAppletMode; // 0x0
    s8 isActive; // 0x1
    u8 sAbsoluteHomeButtonState; // 0x2
    u8 sSleepSysState; // 0x3
    u8 sShutdownState; // 0x4
    u8 sPowerButtonState; // 0x5
    u8 sOrderToCloseState; // 0x6
    s8 sIsToCallPowerButtonCallback; // 0x7
    u8 sIsToCallShutdownCallback; // 0x8
    s8 sIsReceivedWakeupByCancelFlag; // 0x9
    u8 sPrevTransitionType; // 0xa
    u8 sSleepNotificationState; // 0xb
    u8 sHomeButtonState; // 0xc
    u8 sIsExpectedToJumpToHomeMenu; // 0xd
    u32 sMessageCommand; // 0x10
    u32 sId; // 0x14
    u32 sAttribute; // 0x18

}
}
}
}