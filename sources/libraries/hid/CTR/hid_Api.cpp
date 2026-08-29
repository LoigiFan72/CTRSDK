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

Result MakeResultAlreadyInitialized(){
    return nn::MakeUsageResult(Result::SUMMARY_INVALID_STATE, Result::Module::MODULE_NN_HID, Result::Description::DESCRIPTION_ALREADY_INITIALIZED);
}

HidDevices s_Devices;
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
        return MakeResultAlreadyInitialized();
    }

    res = srv::Initialize();
    if(res.GetDescription() != Result::DESCRIPTION_ALREADY_INITIALIZED)
        NN_UTIL_PANIC_IF_FAILED(res);
    res = srv::GetServiceHandle(&detail::Ipc::s_Session, portName);
    NN_UTIL_PANIC_IF_FAILED(res);

    res = detail::Ipc::GetIPCHandles(&hSharedMemory, &padEventHandle, &touchEventHandle, &accelerometerEventHandle, &gyroscopeEventHandle, &debugPadEventHandle);
    NN_UTIL_PANIC_IF_FAILED(res);
        
    this->m_SharedMemoryBlock.AttachAndMap(hSharedMemory,0x2b0,true);
    uptr instanceAddress = this->m_SharedMemoryBlock.GetAddress();
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

        res = svc::CloseHandle(this->m_SharedMemoryBlock.DetachHandle());
        NN_UTIL_PANIC_IF_FAILED(res);

        this->m_SharedMemoryBlock.Finalize();

        res = svc::CloseHandle(detail::Ipc::s_Session);
        NN_UTIL_PANIC_IF_FAILED(res);
        
        isInitialized = false;
    }
}

void Finalize(){
    s_Devices.Finalize();
}

Result Initialize(){
    return s_Devices.Initialize(PORT_NAME_USER);
}

Pad& GetPad(){
    NN_TASSERT_(isInitialized);

    return s_Devices.pad;
}

DebugPad& GetDebugPad(){
    NN_TASSERT_(isInitialized);

    return s_Devices.debugPad;
}

TouchPanel& GetTouchPanel(){
    NN_TASSERT_(isInitialized);

    return s_Devices.touchPanel;
}

Accelerometer& GetAccelerometer(){
    NN_TASSERT_(isInitialized);
    
    return s_Devices.accelerometer;
}

Gyroscope& GetGyroscope(){
    NN_TASSERT_(isInitialized);

    return s_Devices.gyroscope;
}

}
}
}