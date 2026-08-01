// Filename: gr_FrameBuffer.cpp
//
// Project: Horizon

#include <nn/gr/CTR/gr_FrameBuffer.h>
#include <nn/gx/CTR/gx_CommandAccess.h>

namespace nn{
namespace gr{
namespace CTR{

bit32* FrameBuffer::ColorBuffer::MakeRenderBlockModeCommand( bit32* command ) const{
    *command++ = blockSize;
    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_RENDER_BLOCK_FORMAT );
                
    return command;
}

bit32* FrameBuffer::ColorBuffer::MakeCommand(bit32* command, bool isAddRenderBlockModeCommand) const{
    *command++ =  PICA_CMD_DATA_RENDER_BUFFER_COLOR_MODE(((format == PICA_DATA_COLOR_RGBA8_OES) || (format == PICA_DATA_COLOR_GAS_DMP)) ? PICA_DATA_COLOR_PIXEL_SIZE32 : PICA_DATA_COLOR_PIXEL_SIZE16, format);
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_RENDER_BUFFER_COLOR_MODE);

    *command++ = PICA_CMD_DATA_RENDER_BUFFER_COLOR_ADDR(nngxGetPhysicalAddr(virtualAddr));
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_RENDER_BUFFER_COLOR_ADDR);

    if (isAddRenderBlockModeCommand){
        command = this->MakeRenderBlockModeCommand(command);
    }

    return command;
}

FrameBuffer::ColorBuffer::ColorBuffer(const FrameBuffer& frameBuffer_): 
    virtualAddr(0),
    format(PICA_DATA_COLOR_RGBA8_OES),
    blockSize(BLOCK_SIZE8),
    width(240), 
    height(400),  
    mFrameBuffer(frameBuffer_){
    for (u32 index = 0; index < 4; index++){
        clearColor[index] = 0.0f;
    }
}

bit32* FrameBuffer::DepthStencilBuffer::MakeCommand(bit32* command) const{
    *command++ = PICA_CMD_DATA_RENDER_BUFFER_DEPTH_MODE(format);
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_RENDER_BUFFER_DEPTH_MODE);

    *command++ = PICA_CMD_DATA_RENDER_BUFFER_DEPTH_ADDR(nngxGetPhysicalAddr(virtualAddr));
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_RENDER_BUFFER_DEPTH_ADDR);
                
    return command;
}

FrameBuffer::DepthStencilBuffer::DepthStencilBuffer(const FrameBuffer& frameBuffer_): 
    virtualAddr(0),
    format(PICA_DATA_DEPTH24_STENCIL8_EXT),
    width(240), 
    height(400),
    clearDepth(1.0f),
    clearStencil(0),
    mFrameBuffer(frameBuffer_)
{}

bit32* FrameBuffer::FrameBuffer::MakeCommand(bit32* command, const u32 bufferBit, bool isClearCache) const{   
    if (isClearCache){
        command = MakeClearCacheCommand(command);
    }

    if (bufferBit & COLOR_BUFFER_BIT){
        command = this->colorBuffer.MakeCommand(command);
    }

    if ((bufferBit & DEPTH_BUFFER_BIT) || (bufferBit & STENCIL_BUFFER_BIT)){
        command = this->depthStencilBuffer.MakeCommand(command);
    }

    *command++ = PICA_CMD_DATA_RENDER_BUFFER_RESOLUTION(width, height);
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_RENDER_BUFFER_RESOLUTION0);

    *command++ = PICA_CMD_DATA_RENDER_BUFFER_RESOLUTION(width, height);
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_RENDER_BUFFER_RESOLUTION1);

    command = this->MakeClearCacheCommand(command);

    return command;
}

bit32* FrameBuffer::MakeClearCacheCommand(bit32* command){
    *command++ = 0x1;
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1);

    *command++ = 0x1;
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0);

    return command;
}

FrameBuffer::FrameBuffer():
    colorBuffer(*this),
    depthStencilBuffer(*this),
    width(240), 
    height(400)
{}

}
}
}