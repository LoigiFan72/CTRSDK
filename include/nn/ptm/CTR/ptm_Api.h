#pragma once

#include <nn/fnd/fnd_DateTime.h>

namespace nn{
namespace ptm{
namespace CTR{
namespace{
    const char *const PORT_NAME_PTM_USER   = "ptm:u";
    const char *const PORT_NAME_PTM_SYST   = "ptm:s";
    const char *const PORT_NAME_PTM_SYSM   = "ptm:sysm";
    const char *const PORT_NAME_SETSYSTIME = "ptm:sets";
    const char *const PORT_NAME_PTM_PLAY   = "ptm:play";
    const char *const PORT_NAME_GETSYSTIME = "ptm:gets";
}

enum TargetPlatform
{
    TARGET_PLATFORM_CTR,
    TARGET_PLATFORM_SNAKE
};

Result Initialize();
Result Finalize();

}
}
}