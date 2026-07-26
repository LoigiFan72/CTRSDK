// Filename: hid_Api.cpp
//
// Project: Horizon

#include <nn/hid/CTR/hid_Api.h>
#include <nn/hid/CTR/hid_IpcClient.h>
#include <nn/hidlow/CTR/hidlow_LifoRingCollector.h>
#include <nn/srv/srv_API.h>
#include <nn/Assert.h>

namespace nn{
namespace hid{
namespace CTR{

HidDevices sDevices;
bool isInitialized;


HidDevices::~HidDevices(){ }

Result HidDevices::Initialize(const char* portName){
    Handle hSharedMemory;
    Result res;
    Handle padEventHandle;
    Handle touchEventHandle;
    Handle accelerometerEventHandle;
    Handle gyroscopeEventHandle;
    Handle debugPadEventHandle;
    if(isInitialized){
        return Result(0xe0a04ff9); // MakeResultAlreadyInitialized
    }

    res = srv::Initialize();
    if(res.GetDescription() != Result::DESCRIPTION_ALREADY_INITIALIZED)
            NN_PANIC_IF_FAILED(res);
    res = srv::GetServiceHandle(&detail::Ipc::sSession, portName);
    NN_PANIC_IF_FAILED(res);

    res = detail::Ipc::GetIPCHandles(&hSharedMemory, &padEventHandle, &touchEventHandle, &accelerometerEventHandle, &gyroscopeEventHandle, &debugPadEventHandle);
    NN_PANIC_IF_FAILED(res);
        
    this->mSharedMemoryBlock.AttachAndMap(hSharedMemory,0x2b0,true);
    uptr instanceAddress = this->mSharedMemoryBlock.GetAddress();
    NN_TASSERT_(instanceAddress);

    hidlow::CTR::LifoRingCollector* ring;
    this->pad.SetResource(reinterpret_cast<uptr>(ring->GetPadLifoRingAddress()));
    this->touchPanel.SetResource(reinterpret_cast<uptr>(ring->GetTouchPanelLifoRingAddress()));
    this->accelerometer.SetResource(reinterpret_cast<uptr>(ring->GetAccelerometerLifoRingAddress()));
    this->gyroscope.SetResource(reinterpret_cast<uptr>(ring->GetGyroscopeLowLifoRingAddress()));
    this->debugPad.SetResource(reinterpret_cast<uptr>(ring->GetDebugPadLifoRingAddress()));

    this->pad.SetHandle(padEventHandle);
    this->touchPanel.SetHandle(touchEventHandle);
    this->accelerometer.SetHandle(accelerometerEventHandle);
    this->gyroscope.SetHandle(gyroscopeEventHandle);
    this->debugPad.SetHandle(debugPadEventHandle);

    isInitialized = true;
    return ResultSuccess();
}

void HidDevices::Finalize(){
    Result res;
    if(isInitialized){
        res = svc::CloseHandle(this->pad.DetachHandle());
        NN_UTIL_PANIC_IF_FAILED(res);

        res = svc::CloseHandle(this->touchPanel.DetachHandle());
        NN_UTIL_PANIC_IF_FAILED(res);

        res = svc::CloseHandle(this->accelerometer.DetachHandle());
        NN_UTIL_PANIC_IF_FAILED(res);

        res = svc::CloseHandle(this->gyroscope.DetachHandle());
        NN_UTIL_PANIC_IF_FAILED(res);

        res = svc::CloseHandle(this->debugPad.DetachHandle());
        NN_UTIL_PANIC_IF_FAILED(res);

        res = svc::CloseHandle(this->mSharedMemoryBlock.DetachHandle());
        NN_UTIL_PANIC_IF_FAILED(res);

        this->mSharedMemoryBlock.Finalize();

        res = svc::CloseHandle(detail::Ipc::sSession);
        NN_UTIL_PANIC_IF_FAILED(res);
        
        isInitialized = false;
    }
}

void Finalize(){
    sDevices.Finalize();
}

Result Initialize(){
    return sDevices.Initialize(PORT_NAME_USER);
}

Result InitializeSpecialPrivilage(){
    return sDevices.Initialize(PORT_NAME_SPVR);
}

Pad& GetPad(){
    NN_TASSERT_(isInitialized);

    return sDevices.pad;
}

DebugPad& GetDebugPad(){
    NN_TASSERT_(isInitialized);

    return sDevices.debugPad;
}

TouchPanel& GetTouchPanel(){
    NN_TASSERT_(isInitialized);

    return sDevices.touchPanel;
}

Accelerometer& GetAccelerometer(){
    NN_TASSERT_(isInitialized);
    
    return sDevices.accelerometer;
}

Gyroscope& GetGyroscope(){
    NN_TASSERT_(isInitialized);

    return sDevices.gyroscope;
}

}
}
}