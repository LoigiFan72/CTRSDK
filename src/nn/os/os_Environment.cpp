// Filename: os_Environment.cpp
//
// Project: Horizon

#include <nn/os/CTR/os_Environment.h>
#include <nn/os/os_Memory.h>
#include <nn/os/os_SharedInfo.h>
#include <nn/assert.h>
#include <nn/svc.h>
#include <nn/util.h>

namespace nn {
namespace os {

size_t GetAppMemorySize(){
    return (GetReadOnlySharedInfo().kParamValues[0]);
}

size_t GetUsingMemorySize(){
    Handle h;
    s64 v[1];
    LimitableResource names[1] = { LIMITABLE_RESOURCE_MAX_COMMIT };

    svc::GetResourceLimit(&h, PSEUDO_HANDLE_CURRENT_PROCESS);
    svc::GetResourceLimitCurrentValues(v, h,  names, sizeof(*util::NumOfElementsT(names)) );
    svc::CloseHandle(h);

    return static_cast<size_t>(v[0]);
}

}
}