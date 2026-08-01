#pragma once

#include <nn/gx/CTR/gx_PicaCommon.h>

#define PICA_CMD_DATA_RENDER_BUFFER_COLOR_ADDR( addr ) ( (addr) >> 3 )

#define PICA_CMD_DATA_RENDER_BUFFER_DEPTH_ADDR( addr ) ( (addr) >> 3 )

enum PicaDataColorPixelSize{
    PICA_DATA_COLOR_PIXEL_SIZE16 = 0x0,
    PICA_DATA_COLOR_PIXEL_SIZE32 = 0x2
};

enum PicaDataColor{
    PICA_DATA_COLOR_RGBA8_OES = 0x0,
    PICA_DATA_COLOR_GAS_DMP   = 0x0,
    PICA_DATA_COLOR_RGB5_A1   = 0x2,
    PICA_DATA_COLOR_RGB565    = 0x3,
    PICA_DATA_COLOR_RGBA4     = 0x4
};

enum PicaDataDepth{
    PICA_DATA_DEPTH_COMPONENT16     = 0x0,
    PICA_DATA_DEPTH_COMPONENT24_OES = 0x2,
    PICA_DATA_DEPTH24_STENCIL8_EXT  = 0x3
};

enum PicaDataStencilOp{
    PICA_DATA_STENCIL_OP_KEEP      = 0x0,
    PICA_DATA_STENCIL_OP_ZERO      = 0x1,
    PICA_DATA_STENCIL_OP_REPLACE   = 0x2,
    PICA_DATA_STENCIL_OP_INCR      = 0x3,
    PICA_DATA_STENCIL_OP_DECR      = 0x4,
    PICA_DATA_STENCIL_OP_INVERT    = 0x5,
    PICA_DATA_STENCIL_OP_INCR_WRAP = 0x6,
    PICA_DATA_STENCIL_OP_DECR_WRAP = 0x7
};

enum PicaDataStencilTest{
    PICA_DATA_STENCIL_TEST_NEVER    = 0x0,
    PICA_DATA_STENCIL_TEST_ALWAYS   = 0x1,
    PICA_DATA_STENCIL_TEST_EQUAL    = 0x2,
    PICA_DATA_STENCIL_TEST_NOTEQUAL = 0x3,
    PICA_DATA_STENCIL_TEST_LESS     = 0x4,
    PICA_DATA_STENCIL_TEST_LEQUAL   = 0x5,
    PICA_DATA_STENCIL_TEST_GREATER  = 0x6,
    PICA_DATA_STENCIL_TEST_GEQUAL   = 0x7
};

#define PICA_CMD_DATA_RENDER_BUFFER_DEPTH_MODE( mode ) (mode)

#define PICA_CMD_DATA_RENDER_BUFFER_RESOLUTION( width, height ) \
    ( (width)            | \
      (height - 1) << 12 | \
      0x01000000 )

#define PICA_CMD_DATA_GAS_LIGHT_XY(lightXY1, lightXY2, lightXY3) \
    ( (lightXY1) | (lightXY2) << 8 | (lightXY3) << 16 )

#define PICA_CMD_DATA_GAS_LIGHT_Z(lightZ1, lightZ2, lightZ3) \
    ( (lightZ1) | (lightZ2) << 8 | (lightZ3) << 16 )

enum PicaDataGasColorLutInput{
    PICA_DATA_GAS_DENSITY_DMP      = 0x0,
    PICA_DATA_GAS_LIGHT_FACTOR_DMP = 0x1
};

#define PICA_CMD_DATA_GAS_LIGHT_Z_COLOR(lightZ4, colorLutInput) \
    ((lightZ4) | ((colorLutInput) ? 1 : 0) << 8)

#define PICA_CMD_DATA_RENDER_BUFFER_COLOR_MODE( pixSize, format ) \
    ((pixSize) | (format) << 16)

#define PICA_CMD_DATA_STENCIL_OP( fail, zfail, zpass) \
    ( (fail)       | \
      (zfail) << 4 | \
      (zpass) << 8 )

#define PICA_CMD_DATA_STENCIL_TEST( enableStencilTest, stencilFunc, stencilMask, ref, mask) \
    ( ((enableStencilTest) ? 1 : 0 ) | \
      (stencilFunc) << 4             | \
      (stencilMask) << 8             | \
      (ref) << 16                    | \
      (mask) << 24 )

enum PicaDataDepthTest2{ 
    PICA_DATA_DEPTH_TEST2_NEVER   = 0x0,
    PICA_DATA_DEPTH_TEST2_ALWAYS  = 0x1,
    PICA_DATA_DEPTH_TEST2_GREATER = 0x2,
    PICA_DATA_DEPTH_TEST2_GEQUAL  = 0x2,
    PICA_DATA_DEPTH_TEST2_OTHER   = 0x3
};

#define PICA_CMD_DATA_GAS_DELTAZ_DEPTH(deltaZ, depthTest2Func) \
    ( (deltaZ) | (depthTest2Func) << 24 )

enum PicaDataFogMode{
    PICA_DATA_FOG_FALSE  = 0x0,
    PICA_DATA_FOG        = 0x5,
    PICA_DATA_GAS        = 0x7
};

enum PicaDataGasShadingDensitySrc{
    PICA_DATA_GAS_PLAIN_DENSITY_DMP = 0x0,
    PICA_DATA_GAS_DEPTH_DENSITY_DMP = 0x1
};

#define PICA_CMD_DATA_GAS_FOG_MODE(fogMode, shadingDensitySrc, zFlip) \
    ( (fogMode) | \
      ((shadingDensitySrc) ? 1 : 0) << 3 | \
      ((zFlip) ? 1 : 0) << 16)

#define PICA_CMD_SET_GAS_FOG_MODE(fogMode, shadingDensitySrc, zFlip) \
    PICA_CMD_DATA_GAS_FOG_MODE(fogMode, shadingDensitySrc, zFlip), \
    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_GAS_FOG_MODE, 0x5)

#define PICA_CMD_DATA_GAS_ATTENUATION(attenuation) (attenuation)

#define PICA_CMD_DATA_GAS_ACCMAX(accMax) (accMax)

#define PICA_CMD_DATA_GAS_LUT_INDEX(index) (index)

#define PICA_CMD_DATA_GAS_LUT_DATA(color8) \
    ( (color8.r) | (color8.g) << 8 | (color8.b) << 16 )

#define PICA_CMD_DATA_FOG_COLOR(color8) \
    ( (color8.r) | (color8.g) << 8 | (color8.b) << 16 )

#define PICA_CMD_DATA_FOG_LUT_INDEX(index) (index)

#define PICA_CMD_DATA_FOG_LUT_DATA(data) (data)

enum PicaDataFragOpMode{
    PICA_DATA_FRAGOP_MODE_DMP         = 0x0,
    PICA_DATA_FRAGOP_MODE_GAS_ACC_DMP = 0x1,
    PICA_DATA_FRAGOP_MODE_SHADOW_DMP  = 0x3
};

enum PicaDataColorLogicOp{
    PICA_DATA_ENABLE_COLOR_LOGIC_OP = 0x0,
    PICA_DATA_ENABLE_BLEND          = 0x1
};

#define PICA_CMD_DATA_COLOR_OPERATION(fragOpMode, blendMode) \
    ( (fragOpMode) | (blendMode) << 8 | 0x0e4 << 16 )

#define PICA_CMD_DATA_FRAGOP_SHADOW(penumbraScale, penumbraBias) \
    (( (penumbraScale) << 16) | (penumbraBias) )

#define PICA_CMD_DATA_FRAGOP_WSCALE(value) ((value == 0) ? 1 : 0)

#define PICA_CMD_DATA_FRAGOP_WSCALE_DATA(data) (data)

#define PICA_CMD_DATA_FRAGOP_CLIP(mode) ((mode) ? 1 : 0)

#define PICA_CMD_SET_FRAGOP_CLIP(mode) \
    PICA_CMD_DATA_FRAGOP_CLIP(mode),   \
    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_FRAGOP_CLIP, 0x1)

#define PICA_CMD_DATA_FRAGOP_CLIP_DATA(data) (data)

enum PicaDataAlphaTest{
    PICA_DATA_ALPHA_TEST_NEVER    = 0x0,
    PICA_DATA_ALPHA_TEST_ALWAYS   = 0x1,
    PICA_DATA_ALPHA_TEST_EQUAL    = 0x2,
    PICA_DATA_ALPHA_TEST_NOTEQUAL = 0x3,
    PICA_DATA_ALPHA_TEST_LESS     = 0x4,
    PICA_DATA_ALPHA_TEST_LEQUAL   = 0x5,
    PICA_DATA_ALPHA_TEST_GREATER  = 0x6,
    PICA_DATA_ALPHA_TEST_GEQUAL   = 0x7
};

#define PICA_CMD_DATA_FRAGOP_ALPHA_TEST_DISABLE() 0x0

#define PICA_CMD_DATA_FRAGOP_ALPHA_TEST( enable, func, value ) \
    ( ((enable) ? 1 : 0) | \
       (func)  << 4      | \
       (value) << 8 )

#define PICA_CMD_SET_DISABLE_ALPHA_TEST()      \
    PICA_CMD_DATA_FRAGOP_ALPHA_TEST_DISABLE(), \
    PICA_CMD_HEADER_SINGLE_BE( PICA_REG_FRAGOP_ALPHA_TEST, 0x1 )

#define PICA_CMD_SET_ENABLE_ALPHA_TEST(func, value)    \
    PICA_CMD_DATA_FRAGOP_ALPHA_TEST(0x1, func, value), \
    PICA_CMD_HEADER_SINGLE_BE( PICA_REG_FRAGOP_ALPHA_TEST, 0x3 )

#define PICA_CMD_DATA_FRAME_BUFFER_MODE(mode) (mode)

#define PICA_CMD_DATA_VIEWPORT_WIDTH(width)   (width)

#define PICA_CMD_DATA_VIEWPORT_HEIGHT(height) (height)

#define PICA_CMD_DATA_VIEWPORT_XY(x, y)  ( (x) | (y) << 16 )

enum PicaDataDepthTest{
    PICA_DATA_DEPTH_TEST_NEVER    = 0x0,
    PICA_DATA_DEPTH_TEST_ALWAYS   = 0x1,
    PICA_DATA_DEPTH_TEST_EQUAL    = 0x2,
    PICA_DATA_DEPTH_TEST_NOTEQUAL = 0x3,
    PICA_DATA_DEPTH_TEST_LESS     = 0x4,
    PICA_DATA_DEPTH_TEST_LEQUAL   = 0x5,
    PICA_DATA_DEPTH_TEST_GREATER  = 0x6,
    PICA_DATA_DEPTH_TEST_GEQUAL   = 0x7
};

#define PICA_CMD_DATA_DEPTH_TEST_DISABLE() 0x0

#define PICA_CMD_DATA_DEPTH_COLOR_MASK( enableDepthTest, depthFunc,          \
                                        red, green, blue, alpha, depthMask ) \
    ( ((enableDepthTest) ? 1 : 0) | \
      (depthFunc << 4)            | \
      ((red)   ? 0x100 : 0)       | \
      ((green) ? 0x200 : 0)       | \
      ((blue)  ? 0x400 : 0)       | \
      ((alpha) ? 0x800 : 0)       | \
      ((depthMask) ? 0x1000 : 0) )

enum PicaDataBlendEquation{
    PICA_DATA_BLEND_EQUATION_ADD                = 0,
    PICA_DATA_BLEND_EQUATION_SUBTRACT           = 1,
    PICA_DATA_BLEND_EQUATION_REVERSE_SUBTRACT   = 2,
    PICA_DATA_BLEND_EQUATION_MIN                = 3,
    PICA_DATA_BLEND_EQUATION_MAX                = 4
};

enum PicaDataBlendFunc{
    PICA_DATA_BLEND_FUNC_ZERO                     =  0,
    PICA_DATA_BLEND_FUNC_ONE                      =  1,
    PICA_DATA_BLEND_FUNC_SRC_COLOR                =  2,
    PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_COLOR      =  3,
    PICA_DATA_BLEND_FUNC_DST_COLOR                =  4,
    PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_COLOR      =  5,
    PICA_DATA_BLEND_FUNC_SRC_ALPHA                =  6,
    PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA      =  7,
    PICA_DATA_BLEND_FUNC_DST_ALPHA                =  8,
    PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA      =  9,
    PICA_DATA_BLEND_FUNC_CONSTANT_COLOR           = 10,
    PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_COLOR = 11,
    PICA_DATA_BLEND_FUNC_CONSTANT_ALPHA           = 12,
    PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_ALPHA = 13,
    PICA_DATA_BLEND_FUNC_SRC_ALPHA_SATURATE       = 14
};

#define PICA_CMD_DATA_BLEND_FUNC_SEPARATE(eqRgb, eqAlpha, srcRgb, dstRgb, srcAlpha, dstAlpha) \
    ((eqRgb) | (eqAlpha) << 8 | (srcRgb) << 16 | (dstRgb) << 20 | (srcAlpha) << 24 | static_cast<u32>(dstAlpha) << 28)

#define PICA_CMD_DATA_BLEND_FUNC( eq, src, dst) \
    PICA_CMD_DATA_BLEND_FUNC_SEPARATE(eq, eq, src, dst, src, dst)

#define PICA_CMD_SET_BLEND_FUNC( eq, src, dst ) \
    PICA_CMD_DATA_COLOR_OPERATION( PICA_DATA_FRAGOP_MODE, PICA_DATA_ENABLE_BLEND ), \
    PICA_CMD_HEADER_SINGLE( PICA_REG_COLOR_OPERATION ), \
    PICA_CMD_DATA_BLEND_FUNC( eq, src, dst ), \
    PICA_CMD_HEADER_SINGLE( PICA_REG_BLEND_FUNC )

#define PICA_CMD_SET_BLEND_FUNC_SEPARATE( eqRgb, eqAlpha, srcRgb, dstRgb, srcAlpha, dstAlpha) \
    PICA_CMD_DATA_COLOR_OPERATION( PICA_DATA_FRAGOP_MODE, PICA_DATA_ENABLE_BLEND ), \
    PICA_CMD_HEADER_SINGLE( PICA_REG_COLOR_OPERATION ), \
    PICA_CMD_DATA_BLEND_FUNC_SEPARATE( eqRgb, eqAlpha, srcRgb, dstRgb, srcAlpha, dstAlpha ),  \
    PICA_CMD_HEADER_SINGLE( PICA_REG_BLEND_FUNC )

#define PICA_CMD_SET_BLEND_DEFAULT() \
    PICA_CMD_SET_BLEND_FUNC( PICA_DATA_BLEND_EQUATION_ADD, PICA_DATA_BLEND_FUNC_SRC_ALPHA, PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA )

#define PICA_CMD_SET_BLEND_NOTHING() \
    PICA_CMD_SET_BLEND_FUNC( PICA_DATA_BLEND_EQUATION_ADD, PICA_DATA_BLEND_FUNC_ONE, PICA_DATA_BLEND_FUNC_ZERO )

enum PicaDataLogicOp{
    PICA_DATA_LOGIC_CLEAR                   = 0x0,
    PICA_DATA_LOGIC_AND                     = 0x1,
    PICA_DATA_LOGIC_AND_REVERSE             = 0x2,
    PICA_DATA_LOGIC_COPY                    = 0x3,
    PICA_DATA_LOGIC_SET                     = 0x4,
    PICA_DATA_LOGIC_COPY_INVERTED           = 0x5,
    PICA_DATA_LOGIC_NOOP                    = 0x6,
    PICA_DATA_LOGIC_INVERT                  = 0x7,
    PICA_DATA_LOGIC_NAND                    = 0x8,
    PICA_DATA_LOGIC_OR                      = 0x9,
    PICA_DATA_LOGIC_NOR                     = 0xa,
    PICA_DATA_LOGIC_XOR                     = 0xb,
    PICA_DATA_LOGIC_EQUIV                   = 0xc,
    PICA_DATA_LOGIC_AND_INVERTED            = 0xd,
    PICA_DATA_LOGIC_OR_REVERSE              = 0xe,
    PICA_DATA_LOGIC_OR_INVERTED             = 0xf
};

#define PICA_CMD_DATA_LOGIC_OP( logicOp ) (logicOp)

#define PICA_CMD_DATA_LOGIC_OP_ENABLE() \
    ( (0x1 << 16) | (0x1 << 24) )
    
#define PICA_CMD_SET_LOGIC_OP( logicOp ) \
    PICA_CMD_DATA_COLOR_OPERATION( PICA_DATA_FRAGOP_MODE, PICA_DATA_ENABLE_COLOR_LOGIC_OP ), \
    PICA_CMD_HEADER_SINGLE( PICA_REG_COLOR_OPERATION ), \
    PICA_CMD_DATA_LOGIC_OP(logicOp), PICA_CMD_HEADER_SINGLE( PICA_REG_LOGIC_OP )

#define PICA_CMD_DATA_BLEND_COLOR( color8 ) \
    ( (color8.r) | (color8.g) << 8 | (color8.b) << 16 | (color8.a) << 24 )


#define PICA_CMD_DATA_EARLY_DEPTH_TEST( mode ) ((mode) ? 1 : 0)

#define PICA_CMD_DATA_EARLY_DEPTH_TEST_DISABLE() 0x0

#define PICA_CMD_SET_DISABLE_EARLY_DEPTH_TEST() \
    PICA_CMD_DATA_EARLY_DEPTH_TEST_DISABLE(), PICA_CMD_HEADER_SINGLE_BE( PICA_REG_EARLY_DEPTH_TEST1, 0x1), \
    PICA_CMD_DATA_EARLY_DEPTH_TEST_DISABLE(), PICA_CMD_HEADER_SINGLE( PICA_REG_EARLY_DEPTH_TEST2 )

enum PicaDataEarlyDepth{
    PICA_DATA_EARLY_DEPTH_GEQUAL  = 0x0,
    PICA_DATA_EARLY_DEPTH_GREATER = 0x1,
    PICA_DATA_EARLY_DEPTH_LEQUAL  = 0x2,
    PICA_DATA_EARLY_DEPTH_LESS    = 0x3
};

#define PICA_CMD_DATA_EARLY_DEPTH_FUNC( func ) (func)

#define PICA_CMD_SET_EARLY_DEPTH_FUNC(func) \
    PICA_CMD_DATA_EARLY_DEPTH_FUNC(func), PICA_CMD_HEADER_SINGLE_BE( PICA_REG_EARLY_DEPTH_FUNC, 0x1)    

#define PICA_CMD_DATA_EARLY_DEPTH_DATA( depth ) (depth)

#define PICA_CMD_SET_EARLY_DEPTH_DATA(data) \
    PICA_CMD_DATA_EARLY_DEPTH_DATA(data), PICA_CMD_HEADER_SINGLE_BE( PICA_REG_EARLY_DEPTH_DATA, 0x7)