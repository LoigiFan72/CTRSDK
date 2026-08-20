#pragma once

#include <nn/gr/CTR/gr_Utility.h>

namespace nn{
namespace gr{
namespace CTR{

class Scissor{
public:

    Scissor():
        isEnable(true),
        x(0),
        y(0),
        width(240),
        height(320),
        bufferWidth(256),
        bufferHeight(320)
    {}

    bit32* MakeCommand(bit32* command) const;

    static bit32* MakeDisableCommand(bit32* command,const s32 colorBufferWidth,const s32 colorBufferHeight){
        s32 temp_width  = colorBufferWidth  - 1;
        s32 temp_height = colorBufferHeight - 1;

        *command++ = PICA_CMD_DATA_SCISSOR( false );
        *command++ = PICA_CMD_HEADER_BURSTSEQ( PICA_REG_SCISSOR, 3 );
        *command++ = 0;
        *command++ = PICA_CMD_DATA_SCISSOR_SIZE( temp_width, temp_height );

        return command;
    }
               

    void Set(s32 x_, s32 y_, u32 width_, u32 height_){
        x = x_; y = y_; width = width_; height = height_;
    }

    void SetBufferSize(s32 bufferWidth_, s32 bufferHeight_){
        bufferWidth = bufferWidth_; bufferHeight = bufferHeight_;
    }

    bool isEnable;
    s8 pad[3];
    s32 x;
    s32 y;
    u32 width;
    u32 height;
    s32 bufferWidth;
    s32 bufferHeight;
};

}
}
}