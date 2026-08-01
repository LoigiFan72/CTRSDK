// Filename: gr_RenderState.cpp
//
// Project: Horizon

#include <nn/gr/CTR/gr_RenderState.h>

namespace nn{
namespace gr{
namespace CTR{

/* RenderState::Culling */

bit32* RenderState::Culling::MakeCommand(bit32* command, bool isUpdateFBAccess) const{
    bit32 culling = 0;

    if (isEnable){
        if ((frontFace == FRONT_FACE_CW  && cullFace == CULL_FACE_FRONT) || (frontFace == FRONT_FACE_CCW && cullFace == CULL_FACE_BACK)){
            culling = 2;
        }
        else{
            culling = 1;
        }
    }
  
    *command++ = culling;
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_CULL_FACE);

    return isUpdateFBAccess ? this->mRenderState.fbAccess.MakeCommand(command) : command;
}

RenderState::Culling::Culling(const RenderState& renderState_): 
    isEnable(true),
    frontFace(FRONT_FACE_CCW),
    cullFace(CULL_FACE_BACK),
    mRenderState(renderState_)
{}

/* RenderState::Blend */

bit32* RenderState::Blend::MakeCommand(bit32* command, bool isUpdateFBAccess) const{
    if (isEnable){
        *command++ = PICA_CMD_DATA_COLOR_OPERATION(PICA_DATA_FRAGOP_MODE_DMP, PICA_DATA_ENABLE_BLEND);
        *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_COLOR_OPERATION, 0x3);

        *command++ = PICA_CMD_DATA_BLEND_FUNC_SEPARATE(eqRgb, eqAlpha, srcRgb, dstRgb, srcAlpha, dstAlpha);
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_BLEND_FUNC);

        *command++ = PICA_CMD_DATA_LOGIC_OP(PICA_DATA_LOGIC_NOOP);
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_LOGIC_OP);

        *command++ = colorR | colorG << 8 | colorB << 16 | colorA << 24;
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_BLEND_COLOR);
    }
    else{
        command = this->Blend::MakeDisableCommand(command, false);
    }
                
    return isUpdateFBAccess ? this->mRenderState.fbAccess.MakeCommand(command) : command;
}


bit32* RenderState::Blend::MakeDisableCommand(bit32* command, bool isClearFrameBufferCache){
    *command++ = PICA_CMD_DATA_COLOR_OPERATION(PICA_DATA_FRAGOP_MODE_DMP, PICA_DATA_ENABLE_BLEND);
    *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_COLOR_OPERATION, 0x3);

    *command++ = PICA_CMD_DATA_BLEND_FUNC(PICA_DATA_BLEND_EQUATION_ADD, PICA_DATA_BLEND_FUNC_ONE, PICA_DATA_BLEND_FUNC_ZERO);
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_BLEND_FUNC);

    if (isClearFrameBufferCache){
        command = FBAccess::MakeClearCacheCommand(command);
    }

    return command;
}

RenderState::Blend::Blend(const RenderState& renderState_): 
    isEnable(false),
    eqRgb(PICA_DATA_BLEND_EQUATION_ADD),
    eqAlpha(PICA_DATA_BLEND_EQUATION_ADD),
    srcRgb(PICA_DATA_BLEND_FUNC_SRC_ALPHA),
    srcAlpha(PICA_DATA_BLEND_FUNC_SRC_ALPHA),
    dstRgb(PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA),
    dstAlpha(PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA),
    colorR(0xff),
    colorG(0xff),
    colorB(0xff),
    colorA(0xff),
    mRenderState(renderState_)
{}

/* RenderState::LogicOp */

bit32* RenderState::LogicOp::MakeCommand(bit32* command, bool isUpdateFBAccess) const{
    if (isEnable){
        *command++ = PICA_CMD_DATA_COLOR_OPERATION(PICA_DATA_FRAGOP_MODE_DMP, PICA_DATA_ENABLE_COLOR_LOGIC_OP);
        *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_COLOR_OPERATION, 0x3);

        *command++ = PICA_CMD_DATA_LOGIC_OP_ENABLE();
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_BLEND_FUNC);

        *command++ = PICA_CMD_DATA_LOGIC_OP(opCode);
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_LOGIC_OP);
    }

    return isUpdateFBAccess ? this->mRenderState.fbAccess.MakeCommand(command) : command;
}

RenderState::LogicOp::LogicOp(const RenderState& renderState_): 
    isEnable(false),
    opCode(PICA_DATA_LOGIC_NOOP),
    mRenderState(renderState_)
{}

/* RenderState::ShadowMap */

bit32* RenderState::ShadowMap::MakeCommand(bit32* command, bool isUpdateFBAccess, bool isAddDummyCommand) const{
    if (isEnable){
        *command++ = PICA_CMD_DATA_COLOR_OPERATION(PICA_DATA_FRAGOP_MODE_SHADOW_DMP, PICA_DATA_ENABLE_BLEND);                        
        *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_COLOR_OPERATION, 0x1);

        command = MakeTextureCommand(command, isAddDummyCommand);

        command = MakeAttenuationCommand(command);
    }

    return isUpdateFBAccess ? this->mRenderState.fbAccess.MakeCommand(command) : command;
}

bit32* RenderState::ShadowMap::MakeTextureCommand(bit32* command, bool isAddDummyCommand) const{
    if (isAddDummyCommand){
        *command++ = 0x0;
        *command++ = PICA_CMD_HEADER_BURST_BE(PICA_REG_TEXTURE_FUNC, 0x3, 0x0);

        *command++ = 0x0;
        *command++ = 0x0;
    }
           
    u32 zBiasFix24 = Float32ToUnsignedFix24(zBias);
    zBiasFix24 = zBiasFix24 >> 1;

    u8 zScale8 = 0;

    *command++ = PICA_CMD_DATA_TEXTURE_SHADOW(isPerspective, zBiasFix24, zScale8);
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_TEXTURE0_SHADOW);

    return command;
}

bit32* RenderState::ShadowMap::MakeAttenuationCommand(bit32* command) const{
    *command++ = (Float32ToFloat16(- penumbraScale)) << 16 | Float32ToFloat16(penumbraScale + penumbraBias);
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_FRAGOP_SHADOW);

    return command;
}

RenderState::ShadowMap::ShadowMap(const RenderState& renderState_): 
    isEnable(false),
    isPerspective(true),
    zBias(0.0f),
    zScale(1.0f),
    penumbraScale(0.0f),
    penumbraBias(1.0f),
    mRenderState(renderState_)
{}

/* RenderState::AlphaTest */

bit32* RenderState::AlphaTest::MakeCommand(bit32* command, bool isUpdateFBAccess) const{
    *command++ = PICA_CMD_DATA_FRAGOP_ALPHA_TEST(isEnable, func, refValue);
    *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_FRAGOP_ALPHA_TEST, 0x3);
                
    return isUpdateFBAccess ? this->mRenderState.fbAccess.MakeCommand(command) : command;
}

RenderState::AlphaTest::AlphaTest(const RenderState& renderState_): 
    isEnable(false),
    refValue(0),
    func (PICA_DATA_ALPHA_TEST_NEVER),
    mRenderState(renderState_)
{}

/* RenderState::StencilTest */

bit32* RenderState::StencilTest::MakeCommand(bit32* command, bool isUpdateFBAccess) const{            
    *command++ = PICA_CMD_DATA_STENCIL_TEST(isEnable, func, maskOp, ref, mask);
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_STENCIL_TEST);

    *command++ = PICA_CMD_DATA_STENCIL_OP(opFail, opZFail, opZPass);
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_STENCIL_OP);

    return isUpdateFBAccess ? this->mRenderState.fbAccess.MakeCommand(command) : command;
}


RenderState::StencilTest::StencilTest( const RenderState& renderState_ ): 
    isEnable(false),
    maskOp(0xff),
    func(PICA_DATA_STENCIL_TEST_ALWAYS),
    ref(0),
    mask(0xff),
    opFail(PICA_DATA_STENCIL_OP_KEEP),
    opZFail(PICA_DATA_STENCIL_OP_KEEP),
    opZPass(PICA_DATA_STENCIL_OP_KEEP),
    mRenderState(renderState_)
{}

/* RenderState::DepthTest */

bit32* RenderState::DepthTest::MakeCommand(bit32* command, bool isUpdateFBAccess) const{
    *command++ = PICA_CMD_DATA_DEPTH_COLOR_MASK(isEnable,func,mRenderState.colorMask & COLOR_MASK_R,mRenderState.colorMask & 
        COLOR_MASK_G,mRenderState.colorMask & COLOR_MASK_B,mRenderState.colorMask & COLOR_MASK_A,isEnableWrite);    
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_DEPTH_COLOR_MASK);
                
    return isUpdateFBAccess ? this->mRenderState.fbAccess.MakeCommand( command ) : command;
}

RenderState::DepthTest::DepthTest(const RenderState& renderState_): 
    isEnable(true),
    isEnableWrite(true),
    func(PICA_DATA_DEPTH_TEST_LESS),
    mRenderState(renderState_)
{}

/* RenderState::WBuffer */

bit32* RenderState::WBuffer::MakeCommand(bit32* command) const{
    if(wScale == 0.0f){
        *command++ = 1;
        *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAGOP_WSCALE);

        *command++ = Float32ToFloat24(depthRangeNear - depthRangeFar);
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_FRAGOP_WSCALE_DATA1);

        f32 zNear = isEnablePolygonOffset ? depthRangeNear - (depthRangeNear - depthRangeFar) * polygonOffsetUnit * 128.0f / f32((1 << depthRangeBit) - 1) : depthRangeNear;
        *command++ = Float32ToFloat24(zNear);
        *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAGOP_WSCALE_DATA2 );
    }

    else{
        *command++ = 0;
        *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FRAGOP_WSCALE);

        *command++ = Float32ToFloat24(-wScale);
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_FRAGOP_WSCALE_DATA1);

        *command++ = isEnablePolygonOffset ? ((depthRangeBit == 24) ? Float32ToFloat24((polygonOffsetUnit * 128.0f / f32((1 << depthRangeBit) - 1))): Float32ToFloat24((polygonOffsetUnit / f32((1 << depthRangeBit) - 1)))): 0.0f;
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_FRAGOP_WSCALE_DATA2);
    }

    return command;
}

RenderState::WBuffer::WBuffer(): 
    wScale(0.f),
    isEnablePolygonOffset(false),
    polygonOffsetUnit(0.f),
    depthRangeNear(0.0f),
    depthRangeFar(1.0f),
    depthRangeBit(24)
{}

/* RenderState::FBAccess */

bit32* RenderState::FBAccess::MakeCommand(bit32* command, bool isClearFrameBufferCache) const{
    if(isClearFrameBufferCache){
        *command++ = 0x1;
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1 );

        *command++ = 0x1;
        *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0 );
    }

    if (mRenderState.shadowMap.isEnable){ 
        *command++ = 0xf; 
        *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_COLOR_BUFFER_READ, 0x1); 

        *command++ = 0xf; 
        *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_COLOR_BUFFER_WRITE, 0x1); 

        *command++ = 0x0; 
        *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_DEPTH_STENCIL_BUFFER_READ, 0x1); 

        *command++ = 0x0; 
        *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_DEPTH_STENCIL_BUFFER_WRITE, 0x1); 
    }
    else{
        *command++ = ((mRenderState.colorMask && mRenderState.colorMask != 0xf) || (mRenderState.colorMask && mRenderState.blend.isEnable) || (mRenderState.colorMask && mRenderState.logicOp.isEnable)) ? 0xf : 0;
        *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_COLOR_BUFFER_READ, 0x1);

        *command++ = mRenderState.colorMask ? 0xf : 0;
        *command++ = PICA_CMD_HEADER_SINGLE_BE(PICA_REG_COLOR_BUFFER_WRITE, 0x1);

        bit32 depth_stencil_read  = 0;
        bit32 depth_stencil_write = 0;

        if (mRenderState.depthTest.isEnable){
            if (mRenderState.depthTest.isEnableWrite){
                depth_stencil_read  |= 2;
                depth_stencil_write |= 2;
            }
            else if (mRenderState.colorMask){
                depth_stencil_read  |= 2;
            }
        }

        if (mRenderState.stencilTest.isEnable){                    
            if (mRenderState.stencilTest.maskOp != 0){
                depth_stencil_read  |= 1;
                depth_stencil_write |= 1;
            }
            else if (mRenderState.colorMask){
                depth_stencil_read  |= 1;
            }
        }

        *command++ = depth_stencil_read;
        *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DEPTH_STENCIL_BUFFER_READ,  0x1);

        *command++ = depth_stencil_write;
        *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DEPTH_STENCIL_BUFFER_WRITE, 0x1);
    }

    return command;
}

bit32* RenderState::FBAccess::MakeDisableCommand(bit32* command, bool isClearFrameBufferCache){
    if (isClearFrameBufferCache){
        command = MakeClearCacheCommand(command);
    }

    *command++ = 0xf;
    *command++ = PICA_CMD_HEADER_BURSTSEQ_BE(PICA_REG_COLOR_BUFFER_READ, 0x4, 0x1);

    *command++ = 0xf;
    *command++ = 0x0;
    *command++ = 0x0;
    *command++ = 0x0;

    return command;
}

bit32* RenderState::FBAccess::MakeClearCacheCommand(bit32* command){
    *command++ = 0x1;
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1);

    *command++ = 0x1;
    *command++ = PICA_CMD_HEADER_SINGLE(PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0);

    return command;
}

RenderState::FBAccess::FBAccess(const RenderState& renderState_): 
    mRenderState(renderState_)
{}

/* RenderState::RenderState */

bit32* RenderState::RenderState::MakeCommand( bit32* buffer, bool isClearFrameBufferCache ) const{
    bit32* command = buffer;

    command = this->cullingTest.MakeCommand(command, false);
    command = this->blend.MakeCommand(command, false);
    command = this->logicOp.MakeCommand(command, false);
    command = this->shadowMap.MakeCommand(command, false);
    command = this->alphaTest.MakeCommand(command, false);
    command = this->stencilTest.MakeCommand(command, false);
    command = this->depthTest.MakeCommand(command, false);
    command = this->wBuffer.MakeCommand(command);
    command = this->fbAccess.MakeCommand(command, isClearFrameBufferCache);

    return command;
}



}
}
}