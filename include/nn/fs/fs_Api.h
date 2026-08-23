#pragma once

#include <nn/fs/fs_IpcFileSystem.h>

namespace nn{
namespace fs{
namespace detail{
    const char PORT_NAME_LOADER[] = "fs:LDR";
    const char PORT_NAME_USER[] = "fs:USER";

}

void Initialize();
void InitializeLoader(); // Isn't anywhere, custom bit
bool IsInitialized();
bool IsSdmcInserted();
bool IsSdmcWritable();
Result GetPriortity(int out);

}
}