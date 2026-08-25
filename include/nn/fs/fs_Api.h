#pragma once

#include <nn/fs/fs_IpcFileSystem.h>
#include <nn/os/os_LightEvent.h>

namespace nn{
namespace fs{
namespace detail{
    const char PORT_NAME_LOADER[] = "fs:LDR";
    const char PORT_NAME_USER[] = "fs:USER";

}

void Initialize();
bool IsInitialized();
void RegisterSdmcEjectedEvent(os::LightEvent* p);
void UnregisterSdmcEjectedEvent();
bool IsSdmcInserted();
bool IsSdmcWritable();
Result GetPriortity(int out);

}
}