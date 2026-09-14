#pragma once

#include <nn/os/os_Types.h>

namespace nn{
namespace os{

inline bool IsRunOnDevelopmentHardWare(){ return GetReadOnlySharedInfo().deviceEnv & 1 ^ 1; }
bool IsRunOnSnake();

}
}