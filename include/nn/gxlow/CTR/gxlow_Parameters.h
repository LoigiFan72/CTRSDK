#pragma once

#include <nn/types.h>

namespace nn{
namespace gxlow{
namespace CTR{
    enum DisplayBufferMode{
        FORMAT_R8G8B8A8 = 0,
        FORMAT_R8G8B8 = 1,
        FORMAT_R5G6B5 = 2,
        FORMAT_R5G5B5A1 = 3,
        FORMAT_R4G4B4A4 = 4,
        FORMAT_UNIMPORTABLE = 0xFFFFFFFFF
    };
    struct DisplayCaptureInfo{
        union DisplayInfoParams{
            uptr addr;
            uptr addrB;
            DisplayBufferMode mode;
            u32 stride;
        };
        DisplayInfoParams surface[2];
    };
}
}
}