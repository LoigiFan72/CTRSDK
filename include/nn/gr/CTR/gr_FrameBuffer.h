#pragma once

#include <nn/gr/CTR/gr_Utility.h>
#include <nn/gx/CTR/gx_CTRRaw.h>

namespace nn{
namespace gr{
namespace CTR{

class FrameBuffer{
public:

    enum BlockSize{
        BLOCK_SIZE8,
        BLOCK_SIZE32
    };

    static const u32 COLOR_BUFFER_BIT         = 0x1;
    static const u32 DEPTH_BUFFER_BIT         = 0x2;
    static const u32 STENCIL_BUFFER_BIT       = 0x4;
    static const u32 DEPTH_STENCIL_BUFFER_BIT = DEPTH_BUFFER_BIT | STENCIL_BUFFER_BIT;

    class ColorBuffer{
    public:
        uptr virtualAddr;
        PicaDataColor format;
        BlockSize blockSize;
        s16 rev1;
        s32 width;
        s32 height;
        f32 clearColor[4];

        explicit ColorBuffer(const FrameBuffer& frameBuffer_);
        bit32* MakeRenderBlockModeCommand(bit32* command) const;
        bit32* MakeCommand(bit32* command, bool isAddRenderBlockMode = false) const;
    protected:
        const FrameBuffer& mFrameBuffer;
    };

    class DepthStencilBuffer{
    public:
        uptr  virtualAddr;
        PicaDataDepth format;
        s8 rev[3];
        s32 width;
        s32 height;
        f32 clearDepth;
        u8 clearStencil;
        s8 rev2[3];

        bit32* MakeCommand(bit32* command) const;
        explicit DepthStencilBuffer(const FrameBuffer& frameBuffer_);
    protected:
        const FrameBuffer& mFrameBuffer;
    };

    ColorBuffer colorBuffer;
    DepthStencilBuffer depthStencilBuffer;
    s32 width;
    s32 height;

    explicit FrameBuffer();
    static bit32* MakeClearCacheCommand(bit32* command);
    bit32* MakeCommand(bit32* command, const u32 bufferBit, bool isClearCache = true) const;
};

}
}
}