#pragma once

#include <nn/types.h>
#include <nn/ndm/CTR/ndm_Types.h>
#include <nn/Result.h>

namespace nn{
namespace ndm{
namespace {
    const char PORT_NAME_USER[] = "ndm:u";
}

using namespace CTR;

Result Initialize();
Result SuspendScheduler(bool bAsync);
Result ResumeScheduler();
Result SuspendDaemons(bit32 mask);
Result ResumeDaemons(bit32 mask);
Result Resume(DaemonName name);

}
}