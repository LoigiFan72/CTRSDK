#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/hid/CTR/hid_HidBase.h>

namespace nn{
namespace hid{
namespace CTR{

class Gyroscope : public HidBase
{
private:
    Gyroscope() {};
    virtual ~Gyroscope() {};

    friend class HidDevices;
};
}
}
}