#pragma once

#include <nn/gx/CTR/gx_PicaCommon.h>


#define PICA_CMD_DATA_FRAG_LIGHT_EN(flag) ( (flag) ? 1 : 0 )
#define PICA_CMD_DATA_FRAG_LIGHT_EN_INV(flag) ( (flag) ? 0 : 1 )

#define PICA_CMD_SET_FRAG_LIGHT_ENABLE(flag) \
    PICA_CMD_DATA_FRAG_LIGHT_EN(flag), PICA_CMD_HEADER_SINGLE(PICA_REG_FRAG_LIGHT_EN0), \
    PICA_CMD_DATA_FRAG_LIGHT_EN_INV(flag), PICA_CMD_HEADER_SINGLE(PICA_REG_FRAG_LIGHT_EN1)
    
// PICA_REG_FRAG_LIGHT_SRC_NUM     0x1c2
#define PICA_CMD_DATA_FRAG_LIGHT_NUM(num) ( (num > 0) ? (num - 1) : 0 )

#define PICA_CMD_SET_FRAG_LIGHT_NUM(num) \
    PICA_CMD_DATA_FRAG_LIGHT_NUM(num), PICA_CMD_HEADER_SINGLE( PICA_REG_FRAG_LIGHT_SRC_NUM )


#define PICA_CMD_DATA_FRAG_LIGHT_EN_ID(Id1, Id2, Id3, Id4, Id5, Id6, Id7, Id8) \
    ( (Id1)       | (Id2) <<  4 | (Id3) <<  8 | (Id4) << 12 | \
      (Id5) << 16 | (Id6) << 20 | (Id7) << 24 | (Id8) << 28 )

#define PICA_CMD_DATA_FRAG_LIGHT_AMBIENT(color) \
    ( (color.b) | (color.g) << 10 | (color.r) << 20 )


#define PICA_CMD_DATA_FRAG_LIGHT_SRC(color) \
    ( (color.b) | (color.g) << 10 | (color.r) << 20 )


#define PICA_CMD_DATA_FRAG_LIGHT_POSITION_XY(x, y) \
    ( (x) | (y) << 16 )

// PICA_REG_FRAG_LIGHT0_SPOT_XY    0x146 to 0x1b6
#define PICA_CMD_DATA_FRAG_LIGHT_SPOT_XY(x, y) \
    ( (x) | (y) << 16 )


#define PICA_CMD_DATA_FRAG_LIGHT_POSITION_Z(z) ( z )

#define PICA_CMD_DATA_FRAG_LIGHT_SPOT_Z(z) ( z )

#define PICA_CMD_DATA_FRAG_LIGHT_DIST_ATTN_BIAS(bias) ( bias )

#define PICA_CMD_DATA_FRAG_LIGHT_DIST_ATTN_SCALE(scale) ( scale )


#define PICA_CMD_DATA_FRAG_LIGHT_TYPE(w, twoSideDiffuse, geomFactor0, geomFactor1) \
    ( ((w) == 0) ? 1 : 0              | \
      ((twoSideDiffuse) ? 1 : 0) << 1 | \
      ((geomFactor0) ? 1 : 0)    << 2 | \
      ((geomFactor1) ? 1 : 0)    << 3)
 

#define PICA_CMD_DATA_FRAG_LIGHT_FUNC_MODE1(shadowed, spotEnabled, lutEnabledD0,  \
    lutEnabledD1, fresnelSelector, lutEnabledRefl, distanceAttenuationEnabled, i) \
    ( ((shadowed) ? 0 : 1)        << i   | \
      ((spotEnabled) ? 0 : 1)     << 8+i | \
      ((lutEnabledD0) ? 0 : 1)    << 16  | \
      ((lutEnabledD1) ? 0 : 1)    << 17  | \
      1                           << 18  | \
      ((fresnelSelector) ? 0 : 1) << 19  | \
      ((lutEnabledRefl) ? 0 : 7)  << 20  | \
      ((distanceAttenuationEnabled) ? 0 : 1) << 24+i )

#define PICA_CMD_DATA_FRAG_LIGHT_FUNC_MODE1_LIGHT_SOURCE(i, \
    shadowed, spotEnabled, distanceAttenuationEnabled)      \
    ( ((shadowed) ? 0 : 1)        << i   |                  \
      ((spotEnabled) ? 0 : 1)     << 8+i |                  \
      1                           << 18  |                  \
      ((distanceAttenuationEnabled) ? 0 : 1) << 24+i )

#define PICA_CMD_DATA_FRAG_LIGHT_FUNC_MODE1_LUT(lutEnabledD0, \
    lutEnabledD1, fresnelSelector, lutEnabledRefl)            \
    ( ((lutEnabledD0) ? 0 : 1)    << 16 |                     \
      ((lutEnabledD1) ? 0 : 1)    << 17 |                     \
      1 << 18                           |                     \
      ((fresnelSelector) ? 0 : 1) << 19 |                     \
      ((lutEnabledRefl) ? 0 : 7)  << 20 )

enum PicaDataFragLightSampler
{
    PICA_DATA_SAMPLER_D0 = 0x0,
    PICA_DATA_SAMPLER_D1 = 0x1,
    PICA_DATA_SAMPLER_FR = 0x3,
    PICA_DATA_SAMPLER_RB = 0x4,
    PICA_DATA_SAMPLER_RG = 0x5,
    PICA_DATA_SAMPLER_RR = 0x6,
    
    PICA_DATA_SAMPLER_SP = 0x8,
    PICA_DATA_FRAG_LIGHT0_SAMPLER_SP = 0x8 + 0x0,
    PICA_DATA_FRAG_LIGHT1_SAMPLER_SP = 0x8 + 0x1,
    PICA_DATA_FRAG_LIGHT2_SAMPLER_SP = 0x8 + 0x2,
    PICA_DATA_FRAG_LIGHT3_SAMPLER_SP = 0x8 + 0x3,
    PICA_DATA_FRAG_LIGHT4_SAMPLER_SP = 0x8 + 0x4,
    PICA_DATA_FRAG_LIGHT5_SAMPLER_SP = 0x8 + 0x5,
    PICA_DATA_FRAG_LIGHT6_SAMPLER_SP = 0x8 + 0x6,
    PICA_DATA_FRAG_LIGHT7_SAMPLER_SP = 0x8 + 0x7,

    PICA_DATA_SAMPLER_DA  = 0x10,
    PICA_DATA_FRAG_LIGHT0_SAMPLER_DA = 0x10 + 0x0,
    PICA_DATA_FRAG_LIGHT1_SAMPLER_DA = 0x10 + 0x1,
    PICA_DATA_FRAG_LIGHT2_SAMPLER_DA = 0x10 + 0x2,
    PICA_DATA_FRAG_LIGHT3_SAMPLER_DA = 0x10 + 0x3,
    PICA_DATA_FRAG_LIGHT4_SAMPLER_DA = 0x10 + 0x4,
    PICA_DATA_FRAG_LIGHT5_SAMPLER_DA = 0x10 + 0x5,
    PICA_DATA_FRAG_LIGHT6_SAMPLER_DA = 0x10 + 0x6,
    PICA_DATA_FRAG_LIGHT7_SAMPLER_DA = 0x10 + 0x7
};

#define PICA_CMD_DATA_FRAG_LIGHT_LUT(index, type) \
    ( (index) | (type) << 8)
    

#define PICA_CMD_DATA_FRAG_LIGHT_LUT_DATA(data1, data2) \
    ( (data1) | (data2) << 12 )


#define PICA_CMD_DATA_FRAG_LIGHT_ABSLUTINPUT(D0, D1, SP, FR, RB, RG, RR) \
    ( ((D0) ? 0 : 1) <<  1 | \
      ((D1) ? 0 : 1) <<  5 | \
      ((SP) ? 0 : 1) <<  9 | \
      ((FR) ? 0 : 1) << 13 | \
      ((RB) ? 0 : 1) << 17 | \
      ((RG) ? 0 : 1) << 21 | \
      ((RR) ? 0 : 1) << 25 )


enum PicaDataFragLightEnvLutInput
{
    PICA_DATA_FRAG_LIGHT_ENV_NH_DMP = 0x0,
    PICA_DATA_FRAG_LIGHT_ENV_VH_DMP = 0x1,
    PICA_DATA_FRAG_LIGHT_ENV_NV_DMP = 0x2,
    PICA_DATA_FRAG_LIGHT_ENV_LN_DMP = 0x3,
    PICA_DATA_FRAG_LIGHT_ENV_SP_DMP = 0x4,
    PICA_DATA_FRAG_LIGHT_ENV_CP_DMP = 0x5
};    

#define PICA_CMD_DATA_FRAG_LIGHT_ENV_LUTINPUT(D0, D1, SP, FR, RB, RG, RR) \
    ( (D0) | (D1) << 4 | (SP) << 8 | \
      (FR) << 12 | (RB) << 16 | (RG) << 20 | (RR) << 24 )
    

enum PicaDataFragLightEnvLutScale
{
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_1_0  = 0x0,
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_2_0  = 0x1,
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_4_0  = 0x2,
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_8_0  = 0x3,
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_0_25 = 0x6,
    PICA_DATA_FRAG_LIGHT_ENV_LUTSCALE_0_5  = 0x7
}; 

#define PICA_CMD_DATA_FRAG_LIGHT_ENV_LUTSCALE(D0, D1, SP, FR, RB, RG, RR) \
    ( (D0) | (D1) << 4 | (SP) << 8 | \
      (FR) << 12 | (RB) << 16 | (RG) << 20 | (RR) << 24 )

#define PICA_CMD_DATA_FRAG_LIGHT_ENV_SHADOW_ATTN(shadowPrimary, shadowSecondary, shadowAlpha) \
    ( ( (shadowPrimary) | (shadowSecondary) | (shadowAlpha) ) ? 1 : 0 )

enum PicaDataFragLightEnvTexture
{
    PICA_DATA_FRAG_LIGHT_ENV_TEXTURE0 = 0x0,
    PICA_DATA_FRAG_LIGHT_ENV_TEXTURE1 = 0x1,
    PICA_DATA_FRAG_LIGHT_ENV_TEXTURE2 = 0x2,
    PICA_DATA_FRAG_LIGHT_ENV_TEXTURE3 = 0x3
};

enum PicaDataFragLightEnvLayerConfig
{
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG0 = 0x0,
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG1 = 0x1,
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG2 = 0x2,
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG3 = 0x3,
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG4 = 0x4,
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG5 = 0x5,
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG6 = 0x6,
    PICA_DATA_FRAG_LIGHT_ENV_LAYER_CONFIG7 = 0x8
};

enum PicaDataFragLightEnvFresnel
{
    PICA_DATA_FRAG_LIGHT_ENV_NO_FRESNEL            = 0x0,
    PICA_DATA_FRAG_LIGHT_ENV_PRI_ALPHA_FRESNEL     = 0x1,
    PICA_DATA_FRAG_LIGHT_ENV_SEC_ALPHA_FRESNEL     = 0x2,
    PICA_DATA_FRAG_LIGHT_ENV_PRI_SEC_ALPHA_FRESNEL = 0x3
};

enum PicaDataFragLightEnvBump
{
    PICA_DATA_FRAG_LIGHT_ENV_BUMP_NOT_USED_DMP = 0x0,
    PICA_DATA_FRAG_LIGHT_ENV_BUMP_AS_BUMP_DMP  = 0x1,
    PICA_DATA_FRAG_LIGHT_ENV_BUMP_AS_TANG_DMP  = 0x2
};

#define PICA_CMD_DATA_FRAG_LIGHT_ENV_BUMP(bumpMode, bumpRenorm) \
    ( ((bumpRenorm) | (bumpMode ==  PICA_DATA_FRAG_LIGHT_ENV_BUMP_NOT_USED_DMP) ) ? 0 : 1 )

// PICA_REG_FRAG_LIGHT_FUNC_MODE0      0x1c3
#define PICA_CMD_DATA_FRAG_LIGHT_FUNC_MODE0(fresnelSelector, config,          \
     shadowPrimary, shadowSecondary, invertShadow, shadowAlpha, bumpSelector, \
     shadowSelector,  clampHighlights, bumpMode, bumpRenorm)                  \
     ( PICA_CMD_DATA_FRAG_LIGHT_ENV_SHADOW_ATTN(shadowPrimary, shadowSecondary, shadowAlpha) | \
       ( (fresnelSelector)           <<  2) | \
       ( (config)                    <<  4) | \
       ( 4                           <<  8) | \
       ( ((shadowPrimary) ? 1 : 0)   << 16) | \
       ( ((shadowSecondary) ? 1 : 0) << 17) | \
       ( ((invertShadow) ? 1 : 0)    << 18) | \
       ( ((shadowAlpha) ? 1 : 0)     << 19) | \
       ( (bumpSelector)              << 22) | \
       ( (shadowSelector)            << 24) | \
       ( ((clampHighlights) ? 1 : 0) << 27) | \
       ( (bumpMode)                  << 28) | \
       ( PICA_CMD_DATA_FRAG_LIGHT_ENV_BUMP(bumpMode, bumpRenorm) << 30) | \
         0x80000000 )