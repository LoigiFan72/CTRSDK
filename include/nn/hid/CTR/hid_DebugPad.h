#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/hid/CTR/hid_HidBase.h>
#include <nn/hid/CTR/hid_DeviceStatus.h>

namespace nn{
namespace hid{
namespace CTR{

class DebugPad : public HidBase
{
private:
    DebugPad() {}
    ~DebugPad() {}

    friend class HidDevices;
};

}
}
}