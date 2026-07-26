#pragma once

#include <nn/types.h>
#include <nn/Result.h>
#include <nn/ro/ro_Module.h>

namespace nn {
namespace ro {
namespace detail{

Result RegisterEit(Module* pModule);
Result UnregisterEit(Module* pModule);

}
}
}