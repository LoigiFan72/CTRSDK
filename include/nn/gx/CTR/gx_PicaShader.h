#pragma once

#include <nn/gx/CTR/gx_PicaCommon.h>

enum PicaDataVertexAttrType{
    PICA_DATA_SIZE_1_BYTE          = 0x0,
    PICA_DATA_SIZE_1_UNSIGNED_BYTE = 0x1,
    PICA_DATA_SIZE_1_SHORT         = 0x2,
    PICA_DATA_SIZE_1_FLOAT         = 0x3,
    PICA_DATA_SIZE_2_BYTE          = 0x4,
    PICA_DATA_SIZE_2_UNSIGNED_BYTE = 0x5,
    PICA_DATA_SIZE_2_SHORT         = 0x6,
    PICA_DATA_SIZE_2_FLOAT         = 0x7,
    PICA_DATA_SIZE_3_BYTE          = 0x8,
    PICA_DATA_SIZE_3_UNSIGNED_BYTE = 0x9,
    PICA_DATA_SIZE_3_SHORT         = 0xa,
    PICA_DATA_SIZE_3_FLOAT         = 0xb,
    PICA_DATA_SIZE_4_BYTE          = 0xc,
    PICA_DATA_SIZE_4_UNSIGNED_BYTE = 0xd,
    PICA_DATA_SIZE_4_SHORT         = 0xe,
    PICA_DATA_SIZE_4_FLOAT         = 0xf
};

enum PicaDataVSOutAttr{
    PICA_DATA_VS_OUT_ATTR_X        = 0x00,
    PICA_DATA_VS_OUT_ATTR_Y        = 0x01,
    PICA_DATA_VS_OUT_ATTR_Z        = 0x02,
    PICA_DATA_VS_OUT_ATTR_W        = 0x03,
    PICA_DATA_VS_OUT_ATTR_QUART_X  = 0x04,
    PICA_DATA_VS_OUT_ATTR_QUART_Y  = 0x05,
    PICA_DATA_VS_OUT_ATTR_QUART_Z  = 0x06,
    PICA_DATA_VS_OUT_ATTR_QUART_W  = 0x07,
    PICA_DATA_VS_OUT_ATTR_R        = 0x08,
    PICA_DATA_VS_OUT_ATTR_G        = 0x09,
    PICA_DATA_VS_OUT_ATTR_B        = 0x0a,
    PICA_DATA_VS_OUT_ATTR_A        = 0x0b,
    PICA_DATA_VS_OUT_ATTR_TEX0_U   = 0x0c,
    PICA_DATA_VS_OUT_ATTR_TEX0_V   = 0x0d,
    PICA_DATA_VS_OUT_ATTR_TEX1_U   = 0x0e,
    PICA_DATA_VS_OUT_ATTR_TEX1_V   = 0x0f,
    PICA_DATA_VS_OUT_ATTR_TEX0_W   = 0x10,
    PICA_DATA_VS_OUT_ATTR_VIEW_X   = 0x12,
    PICA_DATA_VS_OUT_ATTR_VIEW_Y   = 0x13,
    PICA_DATA_VS_OUT_ATTR_VIEW_Z   = 0x14,
    PICA_DATA_VS_OUT_ATTR_TEX2_U   = 0x16,
    PICA_DATA_VS_OUT_ATTR_TEX2_V   = 0x17,
    PICA_DATA_VS_OUT_ATTR_INVALID  = 0x1f
};

enum PicaDataDrawMode{
    PICA_DATA_DRAW_TRIANGLE_STRIP      = 0x1,
    PICA_DATA_DRAW_TRIANGLE_FAN        = 0x2,
    PICA_DATA_DRAW_TRIANGLES           = 0x3,
    PICA_DATA_DRAW_GEOMETRY_PRIMITIVE  = 0x3
};

enum PicaDataVSFloat{
    PICA_DATA_VS_F24 = 0x0,
    PICA_DATA_VS_F32 = 0x1
};

#define PICA_CMD_DATA_VS_GS_OUT_ATTR_CLK(posZ, col, tex0, tex1, tex2, tex0_w, view_quart) \
    ( ((posZ)       ? 1 : 0)       | \
      ((col)        ? 1 : 0) <<  1 | \
      ((tex0)       ? 1 : 0) <<  8 | \
      ((tex1)       ? 1 : 0) <<  9 | \
      ((tex2)       ? 1 : 0) << 10 | \
      ((tex0_w)     ? 1 : 0) << 16 | \
      ((view_quart) ? 1 : 0) << 24 )

#define PICA_CMD_DATA_VS_OUT_MASK(mask) \
    ( (mask) & 0xffff )

#define PICA_CMD_DATA_VS_INT(x, y, z) ( (x) | (y) << 8 | (z) << 16 )

#define PICA_CMD_DATA_DRAW_VERTEX_NUM(num) (num)

#define PICA_CMD_DATA_INDEX_ARRAY_ADDR_OFFSET(offset, type) \
    ( (offset) | ((type) ? 0x80000000 : 0 ) )

#define PICA_CMD_DATA_VS_FIXED_ATTR(order)  ( (order) & 0xf )

#define PICA_CMD_SET_VS_FIXED_ATTR(order, c0, c1, c2) \
    PICA_CMD_DATA_VS_FIXED_ATTR(order), PICA_CMD_HEADER_BURSTSEQ( PICA_REG_VS_FIXED_ATTR, 4), \
    (u32)(c0),                          (u32)(c1), \
    (u32)(c2),                          PICA_CMD_DATA_ZERO()

#define PICA_CMD_DATA_VS_GS_OUT_ATTR(attr_x, attr_y, attr_z, attr_w) \
    ( (attr_x) | (attr_y) << 8 | (attr_z) << 16 | (attr_w) << 24 )

#define PICA_CMD_DATA_VS_GS_OUT_ATTR_MODE( mode ) (mode)

#define PICA_CMD_DATA_VS_FLOAT_ADDR(mode, index) \
    ( (index) & 0xff | ( (mode) ? 0x80000000 : 0) )

#define PICA_CMD_DATA_VS_GS_OUT_REG_NUM3(num, mode) \
    ( (num - 1) | (mode) << 8 )

#define PICA_CMD_SET_DRAW_MODE2(mode)                        \
    ((mode) << 8),                                           \
    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_VS_OUT_REG_NUM3, 0x2)

#define PICA_CMD_DATA_VS_START_ADDR(data) \
    ( (data) | 0x7fff0000 )

#define PICA_CMD_SET_VERTEX_ATTR_ARRAYS_BASE_ADDR_DUMMY()                          \
    0x0, PICA_CMD_HEADER_BURST_BE(PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR, 30, 0x0), \
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                                        \
    0x0, 0x0,                                                                      \
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                                        \
    0x0, 0x0,                                                                      \
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                                        \
    0x0, 0x0

#define PICA_CMD_DATA_VS_ATTR_NUM0(num) \
    ( ((num - 1) & 0xf) | ( 0xa0000000 ) )

#define PICA_CMD_DATA_VS_ATTR_NUM1(num) \
    ( (num - 1) & 0xf )
        
#define PICA_CMD_SET_VS_ATTR_NUM(num) \
    PICA_CMD_DATA_VS_ATTR_NUM0(num), PICA_CMD_HEADER_SINGLE_BE(PICA_REG_VS_ATTR_NUM0, 0xB), \
    PICA_CMD_DATA_VS_ATTR_NUM1(num), PICA_CMD_HEADER_SINGLE(PICA_REG_VS_ATTR_NUM1) \

#define PICA_CMD_DATA_GS_MISC_REG0(mode) \
    ( (mode & PICA_DATA_GS_SUBDIVISION_MODE) ? 0x1 : ( (mode == PICA_DATA_GS_PARTICLE_MODE) ? 0x01004302 : 0) )

#define PICA_CMD_DATA_VS_PROG_ADDR(addr) (addr)

#define PICA_CMD_DATA_VS_PROG_END(data) (data)

#define PICA_CMD_DATA_VS_PROG_SWIZZLE_ADDR(addr) (addr)

#define PICA_CMD_DATA_VS_ATTR_IN_REG_MAP0(index1, index2, index3, index4, index5, index6, index7, index8) \
      ((index1)       | (index2) << 4  | (index3) << 8  | (index4) << 12 | \
       (index5) << 16 | (index6) << 20 | (index7) << 24 | (index8) << 28 )

#define PICA_CMD_DATA_VS_ATTR_IN_REG_MAP1(index9, index10, index11, index12) \
      ((index9) | (index10) << 4 | (index11) << 8 | (index12) << 12)

#define PICA_CMD_DATA_GS_ATTR_IN_REG_MAP0(index1, index2, index3, index4, index5, index6, index7, index8) \
      ( (index1)       | (index2) <<  4 | (index3) <<  8 | (index4) << 12 | \
        (index5) << 16 | (index6) << 20 | (index7) << 24 | (index8) << 28 )

#define PICA_CMD_DATA_GS_ATTR_IN_MAP1(index9, index10, index11, index12) \
      ((index9) | (index10) << 4 | (index11) << 8 | (index12) << 12)

#define PICA_CMD_DATA_VS_COM_MODE(mode) ((mode) ? 1 : 0)

#define PICA_CMD_SET_VS_COM_MODE(mode) \
    PICA_CMD_DATA_VS_COM_MODE(mode), PICA_CMD_HEADER_SINGLE_BE(PICA_REG_VS_COM_MODE, 0x1)

#define PICA_CMD_SET_DRAW_MODE0_DUMMY_END() \
  PICA_CMD_SET_VERTEX_ATTR_ARRAYS_BASE_ADDR_DUMMY()

#define PICA_CMD_DATA_DRAW_MODE0(useGeometryShader, drawMode, useGeometryShaderSubdivision) \
    ( (useGeometryShader ? 2 : 0)                   | \
      (drawMode)                              <<  8 | \
       0                                      <<  9 | \
    ( (useGeometryShaderSubdivision) ? 1 : 0) << 31)

#define PICA_CMD_SET_DRAW_MODE0(drawMode)               \
    PICA_CMD_DATA_DRAW_MODE0(0x0, drawMode, 0x0),       \
    PICA_CMD_HEADER_SINGLE_BE(PICA_REG_DRAW_MODE0, 0x2)

#define PICA_CMD_SET_DRAW_MODE0_DUMMY_BEGIN()                         \
    0x0, PICA_CMD_HEADER_BURST_BE(PICA_REG_VS_OUT_REG_NUM2, 10, 0x0), \
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                           \
    0x0, 0x0,                                                         \
    PICA_CMD_SET_VERTEX_ATTR_ARRAYS_BASE_ADDR_DUMMY()