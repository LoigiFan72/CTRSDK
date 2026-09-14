#pragma once

#include <nn/types.h>
#include <nn/hid/CTR/hid_DeviceStatus.h>

namespace nn{
namespace hid{
namespace CTR{

struct ExtraPadStatus
{
    AnalogStickStatus stick;
    AnalogStickStatus extraStick;
    bit32 hold;
    bit32 trigger;
    bit32 release;
    u8 batteryLevel;
    bool isConnected;
    short rev;
};

}
}
}