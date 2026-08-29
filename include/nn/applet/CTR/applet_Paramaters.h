#pragma once

#include <nn/Handle.h>
#include <nn/fnd/fnd_TimeSpan.h>

namespace nn{
namespace applet{
namespace CTR{
    enum AppJumpType{
        JUMP_OTHER = 0,
        JUMP_CALLER = 1,
        JUMP_SELF = 2,
    };

    enum AppletPos{
        POS_NONE = -1,
        POS_APP = 0,
        POS_APPLIB = 1,
        POS_SYS = 2,
        POS_SYSLIB = 3,
        POS_RESIDENT = 4,
        POS_MAX = 5,
    };

    enum AwakeTrigger{
        WAKEUP_TRIGGER_SHELL_OPEN  = (static_cast<bit64>(0x001u<<6) << 32),
        WAKEUP_TRIGGER_SHELL_CLOSE = (static_cast<bit64>(0x001u<<5) << 32),
        WAKEUP_TRIGGER_TP          = (1u <<  30),
        WAKEUP_TRIGGER_KEY         = (1u <<   1),
        WAKEUP_TRIGGER_HOME_CLICK  = (static_cast<bit64>(0x001u<<2) << 32),
        WAKEUP_TRIGGER_WIFI_CLICK  = (static_cast<bit64>(0x001u<<4) << 32),
        WAKEUP_TRIGGER_RTC_ALARM   = (static_cast<bit64>(0x100u<<2) << 32),
        WAKEUP_TRIGGER_NONE        = 0,
        WAKEUP_TRIGGER_ALL         = WAKEUP_TRIGGER_SHELL_OPEN | WAKEUP_TRIGGER_SHELL_CLOSE | WAKEUP_TRIGGER_HOME_CLICK | WAKEUP_TRIGGER_TP | WAKEUP_TRIGGER_KEY
    };

    enum Attribute{
        TYPE_APP = (0x0<<0),
        TYPE_APPLIB = (0x1<<0),
        TYPE_SYS = (0x2<<0),
        TYPE_SYSLIB = (0x3<<0),
        TYPE_RESIDENT = (0x4<<0),
        TYPE_MASK = (0x7<<0),
    };

    enum LockTransition{
        LOCK_TRANSITION_NONE = 0,
        LOCK_TRANSITION_HOME_BUTTON = (1<<0),
        LOCK_TRANSITION_POWER_BUTTON = (1<<1),
        LOCK_TRANSITION_SLEEP = (1<<2),
        LOCK_TRANSITION_APP_JUMP = (1<<3),

        LOCK_TRANSITION_TO_APP = (1<<4),
        LOCK_TRANSITION_TO_APPLIB = (1<<5),
        LOCK_TRANSITION_TO_SYS = (1<<6),
        LOCK_TRANSITION_TO_SYSLIB = (1<<7),
        LOCK_TRANSITION_APPLETS = (LOCK_TRANSITION_TO_SYS|LOCK_TRANSITION_TO_APP|LOCK_TRANSITION_TO_SYSLIB|LOCK_TRANSITION_TO_APPLIB),

        LOCK_TRANSITION_REBOOT = (1<<8)
    };

    enum AppletPreperationState{
        NO_PREP = 0,
        PREP_TO_LAUNCH_APP = 1,
        PREP_TO_CLOSE_APP = 2,
        PREP_TO_FORCE_CLOSE_APP = 3,
        PREP_TO_PRELOAD_APPLIB = 4,
        PREP_TO_LAUNCH_APPLIB = 5,
        PREP_TO_CLOSE_APPLIB = 6,
        PREP_TO_LAUNCH_SYS = 7,
        PREP_TO_CLOSE_SYS = 8,
        PREP_TO_PRELOAD_SYSLIB = 9,
        PREP_TO_LAUNCH_SYSLIB = 10,
        PREP_TO_CLOSE_SYSLIB = 11,
        PREP_TO_LAUNCH_RESIDENT = 12,
        PREP_TO_LEAVE_RESIDENT = 13,
        PREP_TO_DO_HOMEMENU = 14,
        PREP_TO_LEAVE_HOMEMENU = 15,
        PREP_TO_START_RESIDENT = 16,
        PREP_TO_DO_APP_JUMP = 17,
        PREP_TO_FORCE_CLOSE_SYS = 18,
        PREP_TO_LAUNCH_OTHER_SYS = 19,
        PREP_TO_JUMP_TO_APP = 20,
    };

    enum AppletType{
        HOME_BUTTON = 0,
        POWER_BUTTON = 1,
        MII_SELECT = 2,
        KEYBOARD = 3,
        EULA_ERROR = 4,
        APPLET_TYPE_MAX = 5,
    };

    enum SleepCheckOnEnableSleep{
        SLEEP_IF_SHELL_CLOSED = true,
        NO_SHELL_CHECK = false
    };

    enum ReplyRejectOnDisableSleep{
        REPLY_REJECT_IF_LATER = true,
        NO_REPLY_REJECT = false
    };

    enum Command{
        COMMAND_NONE = 0,
        COMMAND_WAKEUP = 1,
        COMMAND_REQUEST = 2,
        COMMAND_RESPONSE = 3,
        COMMAND_EXIT = 4,
        COMMAND_MESSAGE = 5,
        COMMAND_HOME_BUTTON_SINGLE = 6,
        COMMAND_HOME_BUTTON_DOUBLE = 7,
        COMMAND_DSP_SLEEP = 8,
        COMMAND_DSP_WAKEUP = 9,
        COMMAND_WAKEUP_BY_EXIT = 10,
        COMMAND_WAKEUP_BY_PAUSE = 11,
        COMMAND_WAKEUP_BY_CANCEL = 12,
        COMMAND_WAKEUP_BY_CANCELALL = 13,
        COMMAND_WAKEUP_BY_POWER_BUTTON_CLICK  = 14,
        COMMAND_WAKEUP_TO_JUMP_HOME = 15,
        COMMAND_REQUEST_FOR_SYS_APPLET = 16,
        COMMAND_WAKEUP_TO_LAUNCH_APPLICATION  = 17,

        COMMAND_WAKEUP_BY_TERMINATION_APPLICATION   = 64,
        COMMAND_WAKEUP_BY_TERMINATION_SYSTEM_APPLET = 65,

        COMMAND_ACTION_MASK             = 0xffff,
        COMMAND_FINALIZE                = 0x10000
    };

    enum DataManagementScene{
        DATA_TOP = 0,
        DATA_STREETPASS = 1,
    };

    enum DisplayBufferMode{
        FORMAT_R8G8B8A8 = 0,
        FORMAT_R8G8B8 = 1,
        FORMAT_R5G6B5 = 2,
        FORMAT_R5G5B5A1 = 3,
        FORMAT_R4G4B4A4 = 4,
        FORMAT_UNIMPORTABLE = 0xFFFFFFFFF
    };

    enum HomeButtonState{
        HOME_BUTTON_NONE = 0,
        HOME_BUTTON_SINGLE_PRESSED = 1,
        HOME_BUTTON_DOUBLE_PRESSED = 2,
    };

    enum PowerButtonState {
        POWER_BUTTON_STATE_NONE=0,
        POWER_BUTTON_STATE_CLICK=1
    };

    enum OrderToCloseState {
        ORDER_TO_CLOSE_STATE_NONE=0,
        ORDER_TO_CLOSE_STATE_RECEIVED=1
    };

    enum Notification {
        NOTIFICATION_NONE=0,
        NOTIFICATION_HOME_BUTTON_1=1,
        NOTIFICATION_HOME_BUTTON_2=2,
        NOTIFICATION_SLEEP_QUERY=3,
        NOTIFICATION_SLEEP_CANCELED_BY_OPEN=4,
        NOTIFICATION_SLEEP_ACCEPTED=5,
        NOTIFICATION_AWAKE=6,
        NOTIFICATION_SHUTDOWN=7,
        NOTIFICATION_POWER_BUTTON_CLICK=8,
        NOTIFICATION_POWER_BUTTON_CLEAR=9,
        NOTIFICATION_TRY_SLEEP=10,
        NOTIFICATION_ORDER_TO_CLOSE=11
    };

    enum WakeupState {
        WAKEUP_BY_TIMEOUT=-1,
        WAKEUP_SKIP=0,
        WAKEUP_TO_START=1,
        WAKEUP_BY_EXIT=2,
        WAKEUP_BY_PAUSE=3,
        WAKEUP_BY_CANCEL=4,
        WAKEUP_BY_CANCELALL=5,
        WAKEUP_BY_POWER_BUTTON_CLICK=6,
        WAKEUP_TO_JUMP_HOME=7,
        WAKEUP_TO_JUMP_APPLICATION=8,
        WAKEUP_TO_LAUNCH_APPLICATION=9,
        WAKEUP_STATE_MAX=63
    };

    enum WakeupStateEx{
        WAKEUP_BY_TERMINATION_APPLICATION   = WAKEUP_STATE_MAX + 1,
        WAKEUP_BY_TERMINATION_SYSTEM_APPLET = WAKEUP_STATE_MAX + 2
    };

    enum QueryReply {
        REPLY_REJECT=0,
        REPLY_ACCEPT=1,
        REPLY_LATER=2
    };

    enum SleepNotificationState {
        NOTIFY_NONE=0,
        NOTIFY_SLEEP_QUERY=1,
        NOTIFY_SLEEP_ACCEPT=2,
        NOTIFY_SLEEP_REJECT=3,
        NOTIFY_SLEEP_ACCEPTED=4,
        NOTIFY_AWAKE=5
    };

    enum SleepSysState {
        SLEEP_SYS_STATE_NONE=0,
        SLEEP_SYS_STATE_QUERY=1,
        SLEEP_SYS_STATE_ACCEPTED=2,
        SLEEP_SYS_STATE_AWAKE=3,
        SLEEP_SYS_STATE_CANCELED=4
    };

    enum ShutdownState {
        SHUTDOWN_STATE_NONE=0,
        SHUTDOWN_STATE_RECEIVED=1
    };

    enum MiscState{
        MISC_STATE_POWER_BUTTON         = (1 << 0),
        MISC_STATE_SHUTDOWN_PROCESSING  = (1 << 1)
    };

    enum TransitionType {
        TRANSITION_NONE = 0,
        TRANSITION_START_APP = 1,
        TRANSITION_PRELOAD_APPLIB = 2,
        TRANSITION_CANCEL_APPLIB = 3,
        TRANSITION_START_APPLIB = 4,
        TRANSITION_START_SYS = 5,
        TRANSITION_START_SYSLIB = 6,
        TRANSITION_PRELOAD_RES = 7,
        TRANSITION_START_RES = 8,
        TRANSITION_CLOSE_APP = 9,
        TRANSITION_ORDER_CLOSE_APP = 10,
        TRANSITION_CLOSE_APPLIB = 11,
        TRANSITION_CLOSE_SYSLIB = 12,
        TRANSITION_PAUSE_APPLIB = 13,
        TRANSITION_JUMP_HOME = 14,
        TRANSITION_LEAVE_HOME = 15,
        TRANSITION_LEAVE_RES = 16,
        TRANSITION_ORDER_CLOSE_SYS = 17,
        TRANSITION_JUMP_APP = 18,
        TRANSITION_ENABLE_APPLET = 98,
        TRANSITION_SKIP = 99
    };



    struct AppletDisplayInfoParams{
        uptr addr;
        uptr addrB;
        DisplayBufferMode mode;
        u32 stride;
    };

    struct AppletDisplayInfo{
        AppletDisplayInfoParams d[2];
    };

    struct CaptureBufferInfo{
        size_t size;
        bool is3DCapture;
        u8 rev[3];
        union{
            uptr offset;
            uptr offsetB;
            DisplayBufferMode mode;
        }d[2];
    };

    typedef enum WakeupState AppletWakeupState;
    typedef void (*AppletAwakeCallback)(uptr);
    typedef QueryReply (*AppletSleepQueryCallbackFunc)(uptr);
    typedef void (*AppletAwakeCallback)(uptr);
    typedef void (*AppletSysSleepAcceptedCallback)(uptr);
    typedef bool (*AppletReceiveCallback)(uptr);
    typedef bool (*AppletHomeButtonCallback)(uptr arg, bool isActive, nn::applet::CTR::HomeButtonState state);
    typedef void (*AppletMessageCallback)(uptr arg, AppletId senderId, u8 pParam[], size_t paramSize, nn::Handle handle);
    typedef void (*AppletReleaseMemoryCallback)(uptr arg);
    typedef bool (*AppletCommandCallback)(uptr parameter, AppletId id, AppletCommand command, u8 pParam[], size_t paramSize, s32 readLen, nn::Handle handle);
    typedef void (*AppletRequestMemoryCallback)(uptr arg, size_t size, nn::Handle* pHandle);
    typedef void (*AppletReleaseMemoryCallback)(uptr arg);
    typedef void (*AppletDspSleepCallback)(uptr arg);
    typedef void (*AppletDspWakeUpCallback)(uptr arg);
    typedef Notification AppletNotification;
    typedef QueryReply AppletQueryReply;
    typedef DisplayBufferMode AppletDisplayBufferMode;
    typedef SleepSysState AppletSleepSysState;
    typedef HomeButtonState AppletHomeButtonState;
    typedef AppletQueryReply (*AppletSleepQueryCallback)(uptr);
    typedef bit32 AppletAttr;
    typedef bit32 AppletId;
    typedef bit32 AppletCommand;

    extern Handle HANDLE_NONE;

    const nn::fnd::TimeSpan WAIT_INFINITE = nn::fnd::TimeSpan::FromNanoSeconds((s64)-1);
    const nn::fnd::TimeSpan NO_WAIT = nn::fnd::TimeSpan::FromNanoSeconds(0);
}
}
}

#define NN_APPLET_HANDLE_NONE (nn::applet::CTR::HANDLE_NONE)