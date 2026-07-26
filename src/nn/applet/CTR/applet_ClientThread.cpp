// Filename: applet_ClientThread.cpp
//
// Project: Horizon

#include <nn/applet/CTR/applet_ClientThread.h>
#include <nn/applet/CTR/applet_Connect.h>
#include <nn/applet/CTR/applet_Ipc.h>
#include <nn/applet/CTR/applet_API.h>
#include <nn/applet/CTR/applet_Info.h>
#include <nn/err.h>
#include <nn/os/os_LightEvent.h>
#include <nn/os/os_Thread.h>

namespace nn { 
namespace applet {
namespace CTR {
namespace detail{
namespace{
    static bool sIsThreadEnd;
    static AppletReceiveCallback sReceiveCallback;
    static uptr sReceiveCallbackParam;
}
namespace{
    nn::os::StackBuffer<1> sBuffer;
}
    static os::Event sEvent[3];
    static os::Thread sThread;
    static os::LightEvent sControlLight;

void InitializeClientThread(s32 threadPriority, Handle hControl, Handle hMessage){
    sEvent[1].Initialize(false);
    sEvent[1].Finalize();
    sEvent[1].SetHandle(hMessage);
    sEvent[2].Initialize(false);
    sControlLight.Initialize(true);
    sIsThreadEnd = false;
    sThread.Start(ThreadFunc,0,sBuffer,threadPriority);
}

void FinalizeClientThread(){
    sIsThreadEnd = true;
    sEvent[0].Signal();
    sThread.Join();
    sThread.Finalize();
    sControlLight.Finalize();
    for(int i = 0; i < 3; i++){
        sEvent[i].Finalize();
    }
}

void SetReceiveCallback(AppletReceiveCallback callback,uptr parameter){
    sReceiveCallback = callback;
    sReceiveCallbackParam = parameter;
}

void WaitForControlEvent(){
    sControlLight.Wait();
}

bool TryWaitForControlEvent(){
    return sControlLight.TryWait();
}

void ClearControlEvent(){
    sControlLight.ClearSignal();
}

void ThreadFunc(int param){
    NN_UNUSED_VAR(param);

    nn::Handle handles[3];
    for (int i = 0; i < 3; ++i){
        handles[i] = sEvent[i].GetHandle();
    }

    while(!sIsThreadEnd){
        s32 index;
        Result result = nn::svc::WaitSynchronizationN(&index, handles, 3, false, nn::os::WAIT_INFINITE);
        NN_ERR_THROW_FATAL(result);

        if (sIsThreadEnd){
            break;
        }

        if (sControlLight.TryWait()){
            WaitBySleep(10);
            sEvent[index].Signal();
            continue;
        }

        sEvent[index].ClearSignal();

        if (index == 2){
            SetMessageCommand(COMMAND_WAKEUP_BY_CANCEL);

            sControlLight.Signal();
        }
        else if (index == 1){
            bool bSignal = true;
            if (sReceiveCallback){
                bSignal = sReceiveCallback(sReceiveCallbackParam);
            }
            if (bSignal){
                sControlLight.Signal();
            }
        }
        else if (index == 0){
            AppletNotification notification;
            detail::LockAndConnect();
            result = APPLET::InquireNotification( GetId(), &notification );
            detail::DisconnectAndUnlock();

            if (!result.IsSuccess()){
                continue;
            }

            switch(notification){
            case NOTIFICATION_HOME_BUTTON_1:
            case NOTIFICATION_HOME_BUTTON_2:{
                    if (detail::GetAbsoluteHomeButtonState() == HOME_BUTTON_NONE){
                        detail::SetAbsoluteHomeButtonState((notification == NOTIFICATION_HOME_BUTTON_1) ? HOME_BUTTON_SINGLE_PRESSED : HOME_BUTTON_DOUBLE_PRESSED);
                    }

                    bool bSignal = true;
                    if (sReceiveCallback){
                        bSignal = sReceiveCallback(sReceiveCallbackParam);
                    }

                    if (bSignal){
                        SetMessageCommand((notification == NOTIFICATION_HOME_BUTTON_1) ? COMMAND_HOME_BUTTON_SINGLE : COMMAND_HOME_BUTTON_DOUBLE );
                        sControlLight.Signal();
                    }
                }
                break;

            case NOTIFICATION_SLEEP_QUERY:
            case NOTIFICATION_SLEEP_CANCELED_BY_OPEN:
            case NOTIFICATION_SLEEP_ACCEPTED:
            case NOTIFICATION_AWAKE:{
                    switch(notification){
                    case NOTIFICATION_SLEEP_QUERY:
                        detail::SetSleepSysState(SLEEP_SYS_STATE_QUERY);
                        break;
                    case NOTIFICATION_SLEEP_CANCELED_BY_OPEN:
                        detail::SetSleepSysState(SLEEP_SYS_STATE_CANCELED);
                        break;
                    case NOTIFICATION_SLEEP_ACCEPTED:
                        detail::SetSleepSysState(SLEEP_SYS_STATE_ACCEPTED);
                        break;
                    case NOTIFICATION_AWAKE:
                        detail::SetSleepSysState(SLEEP_SYS_STATE_AWAKE);
                        break;
                    }

                    if (sReceiveCallback){
                        (void)sReceiveCallback(sReceiveCallbackParam);
                    }
                }
                break;
            case NOTIFICATION_SHUTDOWN:{
                    
                SetShutdownCallbackFlag();
                detail::SetShutdownState(SHUTDOWN_STATE_RECEIVED);

                SetOrderToCloseState(ORDER_TO_CLOSE_STATE_RECEIVED);

                if (sReceiveCallback){
                        (void)sReceiveCallback(sReceiveCallbackParam);
                    }
                }
                break;

            case NOTIFICATION_POWER_BUTTON_CLICK:{
                    SetPowerButtonCallbackFlag();
                    SetPowerButtonState( POWER_BUTTON_STATE_CLICK );

                    if (sReceiveCallback){
                        (void)sReceiveCallback(sReceiveCallbackParam);
                    }
                }
                break;

            case NOTIFICATION_POWER_BUTTON_CLEAR:{
                    SetPowerButtonState(POWER_BUTTON_STATE_NONE);
                }
                break;

            case NOTIFICATION_TRY_SLEEP:{
                    LockAndConnect();
                    {
                        NN_TLOG_("applet_API: SleepSystem\n");
                        result = detail::APPLET::SleepSystem(WAKEUP_TRIGGER_SHELL_OPEN);
                        NN_ERR_THROW_FATAL(result);
                    }
                    DisconnectAndUnlock();
                }
                break;

            case NOTIFICATION_ORDER_TO_CLOSE:{
                    SetOrderToCloseState( ORDER_TO_CLOSE_STATE_RECEIVED );
                }
                break;

            default:
                NN_PANIC_("applet_API: unknown notification\n");
            }
        }
    }
}
}
}
}
}