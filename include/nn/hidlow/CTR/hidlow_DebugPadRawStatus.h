#pragma once

#include <nn/types.h>

namespace nn{
namespace hidlow{
namespace CTR{

struct DebugPadRawStatus
{
    bit16 hold;
    bit16 trigger;
    bit16 release;
    s8 leftStickX;
    s8 leftStickY;
    s8 rightStickX;
    s8 rightStickY;
};
}
}
}