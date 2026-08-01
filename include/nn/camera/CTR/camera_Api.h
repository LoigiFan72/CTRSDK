#pragma once

#include <nn/types.h>
#include <nn/Handle.h>
#include <nn/Result.h>

namespace nn{
namespace camera{
namespace CTR{
namespace detail{
    bool IsInitialized();
    Result ArriveApplication();
    Result LeaveApplication();

}
}
}
}