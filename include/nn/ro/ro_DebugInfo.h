#pragma once

#include <nn/ro/ro_Module.h>

namespace nn { 
namespace ro { 
namespace detail {

void NotifyDllLoadedToDebugger(const Module* pModule);
void NotifyDllUnloadingToDebugger(const Module* pModule);
void EnableDebugNotification(bool enable);

}
}
}