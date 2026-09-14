#pragma once

enum PicaReg
{
    PICA_REG_INTERRUPT                 = 0x010, 
    PICA_REG_CULL_FACE                 = 0x040,
    PICA_REG_VIEWPORT_WIDTH1           = 0x041,
    PICA_REG_VIEWPORT_WIDTH2           = 0x042,
    PICA_REG_VIEWPORT_HEIGHT1          = 0x043,
    PICA_REG_VIEWPORT_HEIGHT2          = 0x044,
    PICA_REG_FRAGOP_CLIP               = 0x047,
    PICA_REG_FRAGOP_CLIP_DATA1         = 0x048,
    PICA_REG_FRAGOP_CLIP_DATA2         = 0x049,
    PICA_REG_FRAGOP_CLIP_DATA3         = 0x04a,
    PICA_REG_FRAGOP_CLIP_DATA4         = 0x04b,
    PICA_REG_FRAGOP_WSCALE_DATA1       = 0x04d,
    PICA_REG_FRAGOP_WSCALE_DATA2       = 0x04e,
    PICA_REG_VS_OUT_REG_NUM0           = 0x04f,
    PICA_REG_GS_OUT_REG_NUM0           = 0x04f,
    PICA_REG_VS_OUT_ATTR0              = 0x050,
    PICA_REG_GS_OUT_ATTR0              = 0x050,
    PICA_REG_VS_OUT_ATTR1              = 0x051,
    PICA_REG_GS_OUT_ATTR1              = 0x051,
    //
    PICA_REG_VS_OUT_ATTR2              = 0x052,
    //
    PICA_REG_GS_OUT_ATTR2              = 0x052,
    //
    PICA_REG_VS_OUT_ATTR3              = 0x053,
    //
    PICA_REG_GS_OUT_ATTR3              = 0x053,
    //
    PICA_REG_VS_OUT_ATTR4              = 0x054,
    //
    PICA_REG_GS_OUT_ATTR4              = 0x054,
    //
    PICA_REG_VS_OUT_ATTR5              = 0x055,
    //
    PICA_REG_GS_OUT_ATTR5              = 0x055,
    //
    PICA_REG_VS_OUT_ATTR6              = 0x056,
    //
    PICA_REG_GS_OUT_ATTR6              = 0x056,

    //
    PICA_REG_EARLY_DEPTH_FUNC          = 0x061,
    //
    PICA_REG_EARLY_DEPTH_TEST1         = 0x062,
    //
    PICA_REG_EARLY_DEPTH_CLEAR         = 0x063,
    //
    PICA_REG_VS_OUT_ATTR_MODE          = 0x064,
    //
    PICA_REG_GS_OUT_ATTR_MODE          = 0x064,
 
    //
    PICA_REG_SCISSOR                   = 0x065,
    //
    PICA_REG_SCISSOR_XY                = 0x066,
    //
    PICA_REG_SCISSOR_SIZE              = 0x067,

    //
    PICA_REG_VIEWPORT_XY               = 0x068,

    //
    PICA_REG_EARLY_DEPTH_DATA          = 0x06a,

    //
    PICA_REG_FRAGOP_WSCALE             = 0x06d,

    //
    PICA_REG_RENDER_BUFFER_RESOLUTION1 = 0x06e,

    //
    PICA_REG_VS_OUT_ATTR_CLK           = 0x06f,
    //
    PICA_REG_GS_OUT_ATTR_CLK           = 0x06f,



    //
    PICA_REG_TEXTURE_FUNC              = 0x080,

    //
    PICA_REG_TEXTURE0_BORDER_COLOR     = 0x081,
    //
    PICA_REG_TEXTURE0_SIZE             = 0x082,
    //
    PICA_REG_TEXTURE0_WRAP_FILTER      = 0x083,
    //
    PICA_REG_TEXTURE0_LOD              = 0x084,
    //
    PICA_REG_TEXTURE0_ADDR1            = 0x085,
    //
    PICA_REG_TEXTURE0_ADDR2            = 0x086,
    //
    PICA_REG_TEXTURE0_ADDR3            = 0x087,
    //
    PICA_REG_TEXTURE0_ADDR4            = 0x088,
    //
    PICA_REG_TEXTURE0_ADDR5            = 0x089,
    //
    PICA_REG_TEXTURE0_ADDR6            = 0x08a,
    //
    PICA_REG_TEXTURE0_SHADOW           = 0x08b,

    //
    PICA_REG_TEXTURE0_FORMAT           = 0x08e,
    //
    PICA_REG_FRAG_LIGHT_EN0            = 0x08f,

    //
    PICA_REG_TEXTURE1_BORDER_COLOR     = 0x091,
    //
    PICA_REG_TEXTURE1_SIZE             = 0x092,
    //
    PICA_REG_TEXTURE1_WRAP_FILTER      = 0x093,
    //
    PICA_REG_TEXTURE1_LOD              = 0x094,
    //
    PICA_REG_TEXTURE1_ADDR             = 0x095,
    //
    PICA_REG_TEXTURE1_FORMAT           = 0x096,

    //
    PICA_REG_TEXTURE2_BORDER_COLOR     = 0x099,
    //
    PICA_REG_TEXTURE2_SIZE             = 0x09a,
    //
    PICA_REG_TEXTURE2_WRAP_FILTER      = 0x09b,
    //
    PICA_REG_TEXTURE2_LOD              = 0x09c,
    //
    PICA_REG_TEXTURE2_ADDR             = 0x09d,
    //
    PICA_REG_TEXTURE2_FORMAT           = 0x09e,

    //
    PICA_REG_TEXTURE3_PROCTEX0         = 0x0a8,
    //
    PICA_REG_TEXTURE3_PROCTEX1         = 0x0a9,
    //
    PICA_REG_TEXTURE3_PROCTEX2         = 0x0aa,
    //
    PICA_REG_TEXTURE3_PROCTEX3         = 0x0ab,
    //
    PICA_REG_TEXTURE3_PROCTEX4         = 0x0ac,
    //
    PICA_REG_TEXTURE3_PROCTEX5         = 0x0ad,

    //
    PICA_REG_PROCTEX_LUT               = 0x0af,
    //
    PICA_REG_PROCTEX_LUT_DATA0         = 0x0b0,
    //
    PICA_REG_PROCTEX_LUT_DATA1         = 0x0b1,
    //
    PICA_REG_PROCTEX_LUT_DATA2         = 0x0b2,
    //
    PICA_REG_PROCTEX_LUT_DATA3         = 0x0b3,
    //
    PICA_REG_PROCTEX_LUT_DATA4         = 0x0b4,
    //
    PICA_REG_PROCTEX_LUT_DATA5         = 0x0b5,
    //
    PICA_REG_PROCTEX_LUT_DATA6         = 0x0b6,
    //
    PICA_REG_PROCTEX_LUT_DATA7         = 0x0b7,

    //
    PICA_REG_TEX_ENV0                  = 0x0c0,
    //
    PICA_REG_TEX_ENV0_OPERAND          = 0x0c1,
    //
    PICA_REG_TEX_ENV0_COMBINE          = 0x0c2,
    //
    PICA_REG_TEX_ENV0_COLOR            = 0x0c3,
    //
    PICA_REG_TEX_ENV0_SCALE            = 0x0c4,

    //
    PICA_REG_TEX_ENV1                  = 0x0c8,
    //
    PICA_REG_TEX_ENV1_OPERAND          = 0x0c9,
    //
    PICA_REG_TEX_ENV1_COMBINE          = 0x0ca,
    //
    PICA_REG_TEX_ENV1_COLOR            = 0x0cb,
    //
    PICA_REG_TEX_ENV1_SCALE            = 0x0cc,

    //
    PICA_REG_TEX_ENV2 = 0x0d0,
    //
    PICA_REG_TEX_ENV2_OPERAND          = 0x0d1,
    //
    PICA_REG_TEX_ENV2_COMBINE          = 0x0d2,
    //
    PICA_REG_TEX_ENV2_COLOR            = 0x0d3,
    //
    PICA_REG_TEX_ENV2_SCALE            = 0x0d4,

    //
    PICA_REG_TEX_ENV3                  = 0x0d8,
    //
    PICA_REG_TEX_ENV3_OPERAND          = 0x0d9,
    //
    PICA_REG_TEX_ENV3_COMBINE          = 0x0da,
    //
    PICA_REG_TEX_ENV3_COLOR            = 0x0db,
    //
    PICA_REG_TEX_ENV3_SCALE            = 0x0dc,

    //
    PICA_REG_GAS_FOG_MODE              = 0x0e0,
    //
    PICA_REG_TEX_ENV_BUFFER_INPUT      = 0x0e0,

    //
    PICA_REG_FOG_COLOR                 = 0x0e1,
    //
    PICA_REG_GAS_ATTENUATION           = 0x0e4,
    //
    PICA_REG_GAS_ACCMAX                = 0x0e5,

    //
    PICA_REG_FOG_LUT_INDEX             = 0x0e6,
    //
    PICA_REG_FOG_LUT_DATA0             = 0x0e8,
    //
    PICA_REG_FOG_LUT_DATA1             = 0x0e9,
    //
    PICA_REG_FOG_LUT_DATA2             = 0x0ea,
    //
    PICA_REG_FOG_LUT_DATA3             = 0x0eb,
    //
    PICA_REG_FOG_LUT_DATA4             = 0x0ec,
    //
    PICA_REG_FOG_LUT_DATA5             = 0x0ed,
    //
    PICA_REG_FOG_LUT_DATA6             = 0x0ee,
    //
    PICA_REG_FOG_LUT_DATA7             = 0x0ef,

    //
    PICA_REG_TEX_ENV4                  = 0x0f0,
    //
    PICA_REG_TEX_ENV4_OPERAND          = 0x0f1,
    //
    PICA_REG_TEX_ENV4_COMBINE          = 0x0f2,
    //
    PICA_REG_TEX_ENV4_COLOR            = 0x0f3,
    //
    PICA_REG_TEX_ENV4_SCALE            = 0x0f4,

    //
    PICA_REG_TEX_ENV5                  = 0x0f8,
    //
    PICA_REG_TEX_ENV5_OPERAND          = 0x0f9,
    //
    PICA_REG_TEX_ENV5_COMBINE          = 0x0fa,
    //
    PICA_REG_TEX_ENV5_COLOR            = 0x0fb,
    //
    PICA_REG_TEX_ENV5_SCALE            = 0x0fc,
    //
    PICA_REG_TEX_ENV_BUFFER_COLOR      = 0x0fd,


    //
    PICA_REG_COLOR_OPERATION            = 0x100,
    //
    PICA_REG_BLEND_FUNC                 = 0x101,
    //
    PICA_REG_LOGIC_OP                   = 0x102,
    //
    PICA_REG_BLEND_COLOR                = 0x103,
    //
    PICA_REG_FRAGOP_ALPHA_TEST          = 0x104,

    //
    PICA_REG_STENCIL_TEST               = 0x105,
    //
    PICA_REG_STENCIL_OP                 = 0x106,
    //
    PICA_REG_DEPTH_COLOR_MASK           = 0x107,

    //
    PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0  = 0x110,
    //
    PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1  = 0x111,
    //
    PICA_REG_COLOR_BUFFER_READ          = 0x112,
    //
    PICA_REG_COLOR_BUFFER_WRITE         = 0x113,
    //
    PICA_REG_DEPTH_STENCIL_BUFFER_READ  = 0x114,
    //
    PICA_REG_DEPTH_STENCIL_BUFFER_WRITE = 0x115,

    //
    PICA_REG_RENDER_BUFFER_DEPTH_MODE   = 0x116,
    //
    PICA_REG_RENDER_BUFFER_COLOR_MODE   = 0x117,

    //
    PICA_REG_EARLY_DEPTH_TEST2          = 0x118,

    //
    PICA_REG_RENDER_BLOCK_FORMAT        = 0x11b,

    //
    PICA_REG_RENDER_BUFFER_DEPTH_ADDR   = 0x11c,
    //
    PICA_REG_RENDER_BUFFER_COLOR_ADDR   = 0x11d,
    //
    PICA_REG_RENDER_BUFFER_RESOLUTION0  = 0x11e,
    
    //
    PICA_REG_GAS_LIGHT_XY               = 0x120,
    //
    PICA_REG_GAS_LIGHT_Z                = 0x121,
    //
    PICA_REG_GAS_LIGHT_Z_COLOR          = 0x122,
    //
    PICA_REG_GAS_LUT_INDEX              = 0x123,
    //
    PICA_REG_GAS_LUT_DATA               = 0x124,
    //
    PICA_REG_GAS_DELTAZ_DEPTH           = 0x126,

    //
    PICA_REG_FRAGOP_SHADOW              = 0x130,


    //
    PICA_FRAG_LIGHT_STRIDE               = 16,
    //
    PICA_REG_FRAG_LIGHT_START            = 0x140,
    //
    PICA_REG_FRAG_LIGHT0_SPECULAR0       = PICA_REG_FRAG_LIGHT_START,
    //
    PICA_REG_FRAG_LIGHT0_SPECULAR1       = 0x141, 
    //
    PICA_REG_FRAG_LIGHT0_DIFFUSE         = 0x142, 
    //
    PICA_REG_FRAG_LIGHT0_AMBIENT         = 0x143, 
    //
    PICA_REG_FRAG_LIGHT0_POSITION_XY     = 0x144, 
    //
    PICA_REG_FRAG_LIGHT0_POSITION_Z      = 0x145, 
    //
    PICA_REG_FRAG_LIGHT0_SPOT_XY         = 0x146, 
    //
    PICA_REG_FRAG_LIGHT0_SPOT_Z          = 0x147, 
    //
    PICA_REG_FRAG_LIGHT0_TYPE            = 0x149, 
    //
    PICA_REG_FRAG_LIGHT0_DIST_ATTN_BIAS  = 0x14a, 
    //
    PICA_REG_FRAG_LIGHT0_DIST_ATTN_SCALE = 0x14b, 

    //
    PICA_REG_FRAG_LIGHT1_SPECULAR0       = 0x150, 
    //
    PICA_REG_FRAG_LIGHT1_SPECULAR1       = 0x151, 
    //
    PICA_REG_FRAG_LIGHT1_DIFFUSE         = 0x152, 
    //
    PICA_REG_FRAG_LIGHT1_AMBIENT         = 0x153, 
    //
    PICA_REG_FRAG_LIGHT1_POSITION_XY     = 0x154, 
    //
    PICA_REG_FRAG_LIGHT1_POSITION_Z      = 0x155, 
    //
    PICA_REG_FRAG_LIGHT1_SPOT_XY         = 0x156, 
    //
    PICA_REG_FRAG_LIGHT1_SPOT_Z          = 0x157, 
    //
    PICA_REG_FRAG_LIGHT1_TYPE            = 0x159, 
    //
    PICA_REG_FRAG_LIGHT1_DIST_ATTN_BIAS  = 0x15a, 
    //
    PICA_REG_FRAG_LIGHT1_DIST_ATTN_SCALE = 0x15b, 

    //
    PICA_REG_FRAG_LIGHT2_SPECULAR0       = 0x160, 
    //
    PICA_REG_FRAG_LIGHT2_SPECULAR1       = 0x161, 
    //
    PICA_REG_FRAG_LIGHT2_DIFFUSE         = 0x162, 
    //
    PICA_REG_FRAG_LIGHT2_AMBIENT         = 0x163, 
    //
    PICA_REG_FRAG_LIGHT2_POSITION_XY     = 0x164, 
    //
    PICA_REG_FRAG_LIGHT2_POSITION_Z      = 0x165, 
    //
    PICA_REG_FRAG_LIGHT2_SPOT_XY         = 0x166, 
    //
    PICA_REG_FRAG_LIGHT2_SPOT_Z          = 0x167,
    //
    PICA_REG_FRAG_LIGHT2_TYPE            = 0x169, 
    //
    PICA_REG_FRAG_LIGHT2_DIST_ATTN_BIAS  = 0x16a, 
    //
    PICA_REG_FRAG_LIGHT2_DIST_ATTN_SCALE = 0x16b, 

    //
    PICA_REG_FRAG_LIGHT3_SPECULAR0       = 0x170, 
    //
    PICA_REG_FRAG_LIGHT3_SPECULAR1       = 0x171, 
    //
    PICA_REG_FRAG_LIGHT3_DIFFUSE         = 0x172, 
    //
    PICA_REG_FRAG_LIGHT3_AMBIENT         = 0x173, 
    //
    PICA_REG_FRAG_LIGHT3_POSITION_XY     = 0x174, 
    //
    PICA_REG_FRAG_LIGHT3_POSITION_Z      = 0x175, 
    //
    PICA_REG_FRAG_LIGHT3_SPOT_XY         = 0x176, 
    //
    PICA_REG_FRAG_LIGHT3_SPOT_Z          = 0x177, 
    //
    PICA_REG_FRAG_LIGHT3_TYPE            = 0x179, 
    //
    PICA_REG_FRAG_LIGHT3_DIST_ATTN_BIAS  = 0x17a, 
    //
    PICA_REG_FRAG_LIGHT3_DIST_ATTN_SCALE = 0x17b, 

    //
    PICA_REG_FRAG_LIGHT4_SPECULAR0       = 0x180, 
    //
    PICA_REG_FRAG_LIGHT4_SPECULAR1       = 0x181, 
    //
    PICA_REG_FRAG_LIGHT4_DIFFUSE         = 0x182, 
    //
    PICA_REG_FRAG_LIGHT4_AMBIENT         = 0x183, 
    //
    PICA_REG_FRAG_LIGHT4_POSITION_XY     = 0x184, 
    //
    PICA_REG_FRAG_LIGHT4_POSITION_Z      = 0x185, 
    //
    PICA_REG_FRAG_LIGHT4_SPOT_XY         = 0x186, 
    //
    PICA_REG_FRAG_LIGHT4_SPOT_Z          = 0x187, 
    //
    PICA_REG_FRAG_LIGHT4_TYPE            = 0x189, 
    //
    PICA_REG_FRAG_LIGHT4_DIST_ATTN_BIAS  = 0x18a, 
    //
    PICA_REG_FRAG_LIGHT4_DIST_ATTN_SCALE = 0x18b, 

    //
    PICA_REG_FRAG_LIGHT5_SPECULAR0       = 0x190, 
    //
    PICA_REG_FRAG_LIGHT5_SPECULAR1       = 0x191, 
    //
    PICA_REG_FRAG_LIGHT5_DIFFUSE         = 0x192, 
    //
    PICA_REG_FRAG_LIGHT5_AMBIENT         = 0x193, 
    //
    PICA_REG_FRAG_LIGHT5_POSITION_XY     = 0x194, 
    //
    PICA_REG_FRAG_LIGHT5_POSITION_Z      = 0x195, 
    //
    PICA_REG_FRAG_LIGHT5_SPOT_XY         = 0x196, 
    //
    PICA_REG_FRAG_LIGHT5_SPOT_Z          = 0x197, 
    //
    PICA_REG_FRAG_LIGHT5_TYPE            = 0x199, 
    //
    PICA_REG_FRAG_LIGHT5_DIST_ATTN_BIAS  = 0x19a, 
    //
    PICA_REG_FRAG_LIGHT5_DIST_ATTN_SCALE = 0x19b, 

    //
    PICA_REG_FRAG_LIGHT6_SPECULAR0       = 0x1a0, 
    //
    PICA_REG_FRAG_LIGHT6_SPECULAR1       = 0x1a1, 
    //
    PICA_REG_FRAG_LIGHT6_DIFFUSE         = 0x1a2, 
    //
    PICA_REG_FRAG_LIGHT6_AMBIENT         = 0x1a3, 
    //
    PICA_REG_FRAG_LIGHT6_POSITION_XY     = 0x1a4, 
    //
    PICA_REG_FRAG_LIGHT6_POSITION_Z      = 0x1a5, 
    //
    PICA_REG_FRAG_LIGHT6_SPOT_XY         = 0x1a6, 
    //
    PICA_REG_FRAG_LIGHT6_SPOT_Z          = 0x1a7, 
    //
    PICA_REG_FRAG_LIGHT6_TYPE            = 0x1a9, 
    //
    PICA_REG_FRAG_LIGHT6_DIST_ATTN_BIAS  = 0x1aa, 
    //
    PICA_REG_FRAG_LIGHT6_DIST_ATTN_SCALE = 0x1ab, 

    //
    PICA_REG_FRAG_LIGHT7_SPECULAR0       = 0x1b0, 
    //
    PICA_REG_FRAG_LIGHT7_SPECULAR1       = 0x1b1, 
    //
    PICA_REG_FRAG_LIGHT7_DIFFUSE         = 0x1b2, 
    //
    PICA_REG_FRAG_LIGHT7_AMBIENT         = 0x1b3, 
    //
    PICA_REG_FRAG_LIGHT7_POSITION_XY     = 0x1b4, 
    //
    PICA_REG_FRAG_LIGHT7_POSITION_Z      = 0x1b5, 
    //
    PICA_REG_FRAG_LIGHT7_SPOT_XY         = 0x1b6, 
    //
    PICA_REG_FRAG_LIGHT7_SPOT_Z          = 0x1b7, 
    //
    PICA_REG_FRAG_LIGHT7_TYPE            = 0x1b9, 
    //
    PICA_REG_FRAG_LIGHT7_DIST_ATTN_BIAS  = 0x1ba, 
    //
    PICA_REG_FRAG_LIGHT7_DIST_ATTN_SCALE = 0x1bb,
 
    //
    PICA_REG_FRAG_LIGHT_AMBIENT          = 0x1c0,
    //
    PICA_REG_FRAG_LIGHT_SRC_NUM          = 0x1c2,
    //
    PICA_REG_FRAG_LIGHT_FUNC_MODE0       = 0x1c3,
    //
    PICA_REG_FRAG_LIGHT_FUNC_MODE1       = 0x1c4,
    //
    PICA_REG_FRAG_LIGHT_LUT              = 0x1c5,
    //
    PICA_REG_FRAG_LIGHT_EN1              = 0x1c6,
    //
    PICA_REG_FRAG_LIGHT_LUT_DATA0        = 0x1c8,
    //
    PICA_REG_FRAG_LIGHT_LUT_DATA1        = 0x1c9,
    //
    PICA_REG_FRAG_LIGHT_LUT_DATA2        = 0x1ca,
    //
    PICA_REG_FRAG_LIGHT_LUT_DATA3        = 0x1cb,
    //
    PICA_REG_FRAG_LIGHT_LUT_DATA4        = 0x1cc,
    //
    PICA_REG_FRAG_LIGHT_LUT_DATA5        = 0x1cd,
    //
    PICA_REG_FRAG_LIGHT_LUT_DATA6        = 0x1ce,
    //
    PICA_REG_FRAG_LIGHT_LUT_DATA7        = 0x1cf,
    //
    PICA_REG_FRAG_LIGHT_ABSLUTINPUT      = 0x1d0,
    //
    PICA_REG_FRAG_LIGHT_LUTINPUT         = 0x1d1,
    //
    PICA_REG_FRAG_LIGHT_LUTSCALE         = 0x1d2,
    //
    PICA_REG_FRAG_LIGHT_SRC_EN_ID        = 0x1d9,

    //
    PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR = 0x200,
    //
    PICA_REG_VERTEX_ATTR_ARRAYS0          = 0x201,
    //
    PICA_REG_VERTEX_ATTR_ARRAYS1          = 0x202,
    
    //
    PICA_LOAD_ARRAY_STRIDE                = 3,
    //
    PICA_REG_LOAD_ARRAY0_ATTR_OFFSET      = 0x203,
    //
    PICA_REG_LOAD_ARRAY0_ELEMENT0         = 0x204,
    //
    PICA_REG_LOAD_ARRAY0_ELEMENT1         = 0x205,
    
    //
    PICA_REG_INDEX_ARRAY_ADDR_OFFSET      = 0x227,
    //
    PICA_REG_DRAW_VERTEX_NUM              = 0x228,
    //
    PICA_REG_DRAW_MODE0                   = 0x229,

    //
    PICA_REG_DRAW_VERTEX_OFFSET           = 0x22a,
    //
    PICA_REG_POST_VERTEX_CACHE_NUM        = 0x22d,
    //
    PICA_REG_START_DRAW_ARRAY             = 0x22e,
    //
    PICA_REG_START_DRAW_ELEMENT           = 0x22f,

    //
    PICA_REG_VERTEX_FUNC                  = 0x231,

    //
    PICA_REG_VS_FIXED_ATTR                = 0x232,
    //
    PICA_REG_VS_FIXED_ATTR_DATA0          = 0x233,
    //
    PICA_REG_VS_FIXED_ATTR_DATA1          = 0x234,
    //
    PICA_REG_VS_FIXED_ATTR_DATA2          = 0x235,
    
    //
    PICA_REG_COMMAND_BUF_SIZE_CH0         = 0x238,
    //
    PICA_REG_COMMAND_BUF_SIZE_CH1         = 0x239,
    //
    PICA_REG_COMMAND_BUF_ADDR_CH0         = 0x23a,
    //
    PICA_REG_COMMAND_BUF_ADDR_CH1         = 0x23b,
    //
    PICA_REG_COMMAND_BUF_KICK_CH0         = 0x23c,
    //
    PICA_REG_COMMAND_BUF_KICK_CH1         = 0x23d,
    
    //
    PICA_REG_VS_ATTR_NUM1                 = 0x242,
    //
    PICA_REG_VS_COM_MODE                  = 0x244,
    
    //
    PICA_REG_START_DRAW_FUNC0             = 0x245,

    //
    PICA_REG_VS_OUT_REG_NUM1              = 0x24a,
    //
    PICA_REG_VS_OUT_REG_NUM2              = 0x251,
    //
    PICA_REG_GS_MISC_REG0                 = 0x252,
    //
    PICA_REG_DRAW_MODE1                   = 0x253,
    //
    PICA_REG_GS_MISC_REG1                 = 0x254,
    //
    PICA_REG_VS_OUT_REG_NUM3              = 0x25e,
    //
    PICA_REG_DRAW_MODE2                   = 0x25e,
    //
    PICA_REG_GS_OUT_REG_NUM3              = 0x25e,
    
    //
    PICA_REG_START_DRAW_FUNC1             = 0x25f,
             
    //
    PICA_REG_GS_BOOL                      = 0x280,
    //
    PICA_REG_GS_INT0                      = 0x281,
    //
    PICA_REG_GS_INT1                      = 0x282,
    //
    PICA_REG_GS_INT2                      = 0x283,
    //
    PICA_REG_GS_INT3                      = 0x284,
    //
    PICA_REG_GS_ATTR_NUM                  = 0x289,
    //
    PICA_REG_GS_START_ADDR                = 0x28a,
    //
    PICA_REG_GS_ATTR_IN_REG_MAP0          = 0x28b,
    //
    PICA_REG_GS_ATTR_IN_REG_MAP1          = 0x28c,
    //
    PICA_REG_GS_OUT_REG_MASK              = 0x28d,
    //
    PICA_REG_GS_PROG_UPDATE_END           = 0x28f,
    //
    PICA_REG_GS_FLOAT_ADDR                = 0x290,
    //
    PICA_REG_GS_FLOAT0                    = 0x291,
    //
    PICA_REG_GS_FLOAT1                    = 0x292,
    //
    PICA_REG_GS_FLOAT2                    = 0x293,
    //
    PICA_REG_GS_FLOAT3                    = 0x294,
    //
    PICA_REG_GS_FLOAT4                    = 0x295,
    //
    PICA_REG_GS_FLOAT5                    = 0x296,
    //
    PICA_REG_GS_FLOAT6                    = 0x297,
    //
    PICA_REG_GS_FLOAT7                    = 0x298,
    //
    PICA_REG_GS_PROG_ADDR                 = 0x29b,
    //
    PICA_REG_GS_PROG_DATA0                = 0x29c,
    //
    PICA_REG_GS_PROG_DATA1                = 0x29d,
    //
    PICA_REG_GS_PROG_DATA2                = 0x29e,
    //
    PICA_REG_GS_PROG_DATA3                = 0x29f,
    //
    PICA_REG_GS_PROG_DATA4                = 0x2a0,
    //
    PICA_REG_GS_PROG_DATA5                = 0x2a1,
    //
    PICA_REG_GS_PROG_DATA6                = 0x2a2,
    //
    PICA_REG_GS_PROG_DATA7                = 0x2a3,
    //
    PICA_REG_GS_PROG_SWIZZLE_ADDR         = 0x2a5,
    //
    PICA_REG_GS_PROG_SWIZZLE_DATA0        = 0x2a6,
    //
    PICA_REG_GS_PROG_SWIZZLE_DATA1        = 0x2a7,
    //
    PICA_REG_GS_PROG_SWIZZLE_DATA2        = 0x2a8,
    //
    PICA_REG_GS_PROG_SWIZZLE_DATA3        = 0x2a9,
    //
    PICA_REG_GS_PROG_SWIZZLE_DATA4        = 0x2aa,
    //
    PICA_REG_GS_PROG_SWIZZLE_DATA5        = 0x2ab,
    //
    PICA_REG_GS_PROG_SWIZZLE_DATA6        = 0x2ac,
    //
    PICA_REG_GS_PROG_SWIZZLE_DATA7        = 0x2ad,

    //
    PICA_REG_VS_BOOL                      = 0x2b0,
    //
    PICA_REG_VS_INT0                      = 0x2b1,
    //
    PICA_REG_VS_INT1                      = 0x2b2,
    //
    PICA_REG_VS_INT2                      = 0x2b3,
    //
    PICA_REG_VS_INT3                      = 0x2b4,
    //
    PICA_REG_VS_ATTR_NUM0                 = 0x2b9,
    //
    PICA_REG_VS_START_ADDR                = 0x2ba,
    //
    PICA_REG_VS_ATTR_IN_REG_MAP0          = 0x2bb,
    //
    PICA_REG_VS_ATTR_IN_REG_MAP1          = 0x2bc,
    //
    PICA_REG_VS_OUT_REG_MASK              = 0x2bd,
    //
    PICA_REG_VS_PROG_UPDATE_END           = 0x2bf,
    //
    PICA_REG_VS_FLOAT_ADDR                = 0x2c0,
    //
    PICA_REG_VS_FLOAT0                    = 0x2c1,
    //
    PICA_REG_VS_FLOAT1                    = 0x2c2,
    //
    PICA_REG_VS_FLOAT2                    = 0x2c3,
    //
    PICA_REG_VS_FLOAT3                    = 0x2c4,
    //
    PICA_REG_VS_FLOAT4                    = 0x2c5,
    //
    PICA_REG_VS_FLOAT5                    = 0x2c6,
    //
    PICA_REG_VS_FLOAT6                    = 0x2c7,
    //
    PICA_REG_VS_FLOAT7                    = 0x2c8,
    //
    PICA_REG_VS_PROG_ADDR                 = 0x2cb,
    //
    PICA_REG_VS_PROG_DATA0                = 0x2cc,
    //
    PICA_REG_VS_PROG_DATA1                = 0x2cd,
    //
    PICA_REG_VS_PROG_DATA2                = 0x2ce,
    //
    PICA_REG_VS_PROG_DATA3                = 0x2cf,
    //
    PICA_REG_VS_PROG_DATA4                = 0x2d0,
    //
    PICA_REG_VS_PROG_DATA5                = 0x2d1,
    //
    PICA_REG_VS_PROG_DATA6                = 0x2d2,
    //
    PICA_REG_VS_PROG_DATA7                = 0x2d3,
    //
    PICA_REG_VS_PROG_SWIZZLE_ADDR         = 0x2d5,
    //
    PICA_REG_VS_PROG_SWIZZLE_DATA0        = 0x2d6,
    //
    PICA_REG_VS_PROG_SWIZZLE_DATA1        = 0x2d7,
    //
    PICA_REG_VS_PROG_SWIZZLE_DATA2        = 0x2d8,
    //
    PICA_REG_VS_PROG_SWIZZLE_DATA3        = 0x2d9,
    //
    PICA_REG_VS_PROG_SWIZZLE_DATA4        = 0x2da,
    //
    PICA_REG_VS_PROG_SWIZZLE_DATA5        = 0x2db,
    //
    PICA_REG_VS_PROG_SWIZZLE_DATA6        = 0x2dc,
    //
    PICA_REG_VS_PROG_SWIZZLE_DATA7        = 0x2dd
};