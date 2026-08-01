#pragma once

#include <nn/Handle.h>
#include <nn/fnd/fnd_DateTime.h>

namespace nn{
namespace ptm{
namespace CTR{
namespace detail{

class PtmIpc{
public:

    static nn::Handle sSession;

    static Result GetStepHistory(u16* pStepCounts, s32 numHours, fnd::DateTime start);
};
}
}
}
}