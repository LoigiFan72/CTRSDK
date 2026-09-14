#pragma once

#include <nn/fnd/fnd_InterlockedVariable.h>

namespace nn{
namespace hidlow{
namespace CTR{

struct IAnalogStickStatus
{
    fnd::InterlockedVariable<s16> x;
    fnd::InterlockedVariable<s16> y;
};

}
}
}