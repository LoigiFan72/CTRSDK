#pragma once

#include <nn/hid/CTR/hid_Accelerometer.h>
#include <nn/hid/CTR/hid_DebugPad.h>
#include <nn/hid/CTR/hid_Gyroscope.h>
#include <nn/hid/CTR/hid_Pad.h>
#include <nn/hid/CTR/hid_TouchPanel.h>

#include <nn/os/os_SharedMemory.h>

namespace nn{
namespace hid{
namespace CTR{
namespace detail{
    typedef enum _IPCPortType
    {
        PORT_SPVR = 0,
        PORT_USER,
        NUM_OF_IPC_PORTS
    } IPCPortType;
}
namespace{
    const char const PORT_NAME_USER[] = "hid:USER";
    const char const PORT_NAME_SPVR[] = "hid:SPVR";
}

class HidDevices{
public:
    Pad pad;
    TouchPanel touchPanel;
    Accelerometer accelerometer;
    Gyroscope gyroscope;
    DebugPad debugPad;
    os::SharedMemoryBlock m_SharedMemoryBlock;
    
    void Finalize();
    Result Initialize(const char* portName);
    ~HidDevices();
};

Result Initialize();
void Finalize();

Pad& GetPad();
DebugPad& GetDebugPad();
TouchPanel& GetTouchPanel();
Accelerometer& GetAccelerometer();
Gyroscope& GetGyroscope();

}
}
}