#pragma once

#include <nn/types.h>
#include <nn/Handle.h>
#include <nn/Result.h>

namespace nn{
namespace ndm{
namespace CTR{
namespace detail{

class Interface{
public:
    static nn::Handle sSession;

    static Result OverrideDefaultDaemons(bit32 mask);
    static Result SuspendDaemons(bit32 mask);
    static Result ResumeDaemons(bit32 mask);
    static Result SuspendScheduler(bool bAsync);
    static Result ResumeScheduler();
};

}
}
}
}