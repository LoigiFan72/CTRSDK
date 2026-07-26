#pragma once

#include <nn/hid/CTR/hid_Gyroscope.h>
#include <nn/hidlow/hidlow_LifoRing.h>

namespace nn{
namespace hidlow{
namespace CTR{
    
struct IGyroscopeLowStatus{
    fnd::InterlockedVariable<s16> x;
    fnd::InterlockedVariable<s16> y;
    fnd::InterlockedVariable<s16> z;
};

class GyroscopeLowLifoRing : public LifoRing{
public:
    IGyroscopeLowStatus mRaw;
    s16 rev;
    IGyroscopeLowStatus mBuffers[32];
};
}
}
}