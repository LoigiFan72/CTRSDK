#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/hid/CTR/hid_DeviceStatus.h>
#include <nn/hid/CTR/hid_HidBase.h>

namespace nn{
namespace hid{
namespace CTR{

class TouchPanel : public HidBase
{
private:
    TouchPanel() {}
    ~TouchPanel() {}

    friend class HidDevices;
};


}
}
}