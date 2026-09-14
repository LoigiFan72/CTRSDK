#pragma once

#include <nn/fnd/fnd_InterlockedVariable.h>

namespace nn{
namespace hidlow{

class LifoRing{
public:
    fnd::InterlockedVariable<s64> m_tickWriteZero;
    fnd::InterlockedVariable<s64> m_oldTickWriteZero;
    fnd::InterlockedVariable<s32> m_writePointer;
    s32 rev;
};

}
}