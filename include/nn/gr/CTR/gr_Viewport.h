#pragma once

#include <nn/gr/CTR/gr_Utility.h>

namespace nn{
namespace gr{
namespace CTR{

class Viewport{
public:
    Viewport():
        x(0),
        y(0),
        width(240),
        height(320)
    {}

    bit32* MakeCommand(bit32* command) const;
            
    void Set(s32 x_, s32 y_, u32 width_, u32 height_){
        x = x_; y = y_; width = width_; height = height_;
    }

    s32 x;
    s32 y;
    u32 width;
    u32 height;
};

}
}
}