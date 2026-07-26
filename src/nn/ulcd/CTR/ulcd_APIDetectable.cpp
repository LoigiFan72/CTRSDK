// Filename: ucld_APIDetectable.cpp
//
// Project: Horizon

#include <nn/ulcd/CTR/ulcd_APIDetectable.h>
#include <nn/util/detail/util_Symbol.h>
#include <nn/module.h>
#include <nn/os/os_Types.h>

namespace nn{
namespace ulcd{
namespace CTR{
namespace{
    NN_MAKE_MODULE(sDetectableString, "NINTENDO", "3DVolume");
}

float GetSliderVolume(){
    NN_REFER_MODULE(sDetectableString);
    if (os::GetWritableSharedInfo().displayModeLockFlag) 
        return 0.0f;

    return os::GetWritableSharedInfo().svr2Volume;
}
}
}
}