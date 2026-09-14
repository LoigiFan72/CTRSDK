#pragma once

#include <nn/Handle.h>
#include <nn/Result.h>
#include <nn/types.h>
#include <nn/hid/CTR/hid_ExtraPadStatus.h>
#include <nn/hid/CTR/hid_HidBase.h>

namespace nn{
namespace hid{
namespace CTR{

class ExtraPad
{
public:
    static bool IsSampling();

    static uptr GetResource();
public:
    uptr mResourcePtr;
};

}
}
}