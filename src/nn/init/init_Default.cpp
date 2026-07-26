// Filename: init_Default.cpp
//
// Project: Horizon

#include <nn/svc.h>
#include <nn/init.h>
#include <nn/os/os_Memory.h>
#include <nn/util/util_Result.h>
#include <nn/srv/srv_API.h>

namespace{
    class ExitHandler : public nn::srv::NotificationHandler{
    public:
        virtual void HandleNotification(bit32 message){
            nn::svc::ExitProcess();
        }
    };
    bool sUsingStartUpDefault = false;
    ExitHandler sExitHandler;
}

extern "C" {
    
void nninitStartUpDefault(){
    const size_t assingment = nn::os::GetAppMemorySize();
    const size_t currentUsing = nn::os::GetUsingMemorySize();

    const size_t DEVICE_MEMORY_SIZE = 32 * 1024 * 1024;
    const size_t available = assingment - currentUsing;
    const size_t heapSize  = available - DEVICE_MEMORY_SIZE;

    nn::os::SetupHeapForMemoryBlock(heapSize);
    NN_UTIL_PANIC_IF_FAILED(nn::os::SetDeviceMemorySize( DEVICE_MEMORY_SIZE));

    nn::init::InitializeAllocator(8 * 1024 * 1024);

    sUsingStartUpDefault = true;
}

bool nninitIsStartUpDefaultUsing(){
    return sUsingStartUpDefault;
}

void nninitSetupDefault(){
    nn::srv::Initialize();
    nn::srv::StartNotification();
}


}