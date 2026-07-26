#pragma once

#include <nn/types.h>
#include <nn/Result.h>

namespace nn{
namespace ndm{
namespace {
    const char PORT_NAME_USER[] = "ndm:u";
}

Result Initialize();
Result SuspendDaemons(bit32 mask);

}
}