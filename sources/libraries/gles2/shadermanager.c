// Filename: shadermanager.c
//
// Project: Horizon

#include "common.h"
#include "georegmap.h"
#include "state.h"
#include "error.h"
#include "util.h"
#include "shadermanager.h"
#include "texmanager.h"
#include "commandbuffer.h"
#include "vsasm_objcode_defs.h"
#include "vslink_objcode_common.h"
#include "systemmanager.h"
#include "progstatecache.h"
#include "picaregmap.h"

enum{
	SHADOW_PERSP = 0,
	SHADOW_BIAS,
	SHADOW_SCALE,
	TEXTURE_SAMPLER_MODE0,
	TEXTURE_SAMPLER_MODE1,
	TEXTURE_SAMPLER_MODE2,
	TEXTURE_SAMPLER_MODE3,
	TEXTURE_COORD2,
	TEXTURE_COORD3,
	PROCTEX_1DMAPPING_RGB,
	PROCTEX_1DMAPPING_A,
	PROCTEX_ALPHA_SEPARATE,
	PROCTEX_CLAMP_U,
	PROCTEX_CLAMP_V,
	PROCTEX_SHIFTU_ENABLE,
	PROCTEX_SHIFTV_ENABLE,
	TEXTURE_MIN_FILTER,
	CR_TEXTURE_WIDTH,
	CR_TEXTURE_OFFSET,
	PROCTEX_BIAS,
	PROCTEX_ENABLE_NOISE,
	PROCTEX_NOISE_U,
	PROCTEX_NOISE_V,
	PROCTEX_SAMPLER_RGB,
	PROCTEX_SAMPLER_ALPHA,
	PROCTEX_SAMPLER_NOISE,
	PROCTEX_SAMPLER_R,
	PROCTEX_SAMPLER_G,
	PROCTEX_SAMPLER_B,
	PROCTEX_SAMPLER_A,
	FRAGOP_MODE,
	FRAGOP_PENUMBRA_SCALE,
	FRAGOP_PENUMBRA_BIAS,
	FRAGOP_WSCALE,
	FRAGOP_UCLIP_ENABLE,
	FRAGOP_UCLIP_EQN,
	FRAGOP_ATEST_ENABLE,
	FRAGOP_ATEST_FUNC,
	FRAGOP_ATEST_REF,
	GAS_LIGHTXY,
	GAS_LIGHTZ,	
	GAS_DELTA_Z,
	GAS_ACCMAX,
	GAS_AUTO_ACCMAX,
	GAS_ATTENUATION,
	GAS_COLOR_LUT_INPUT,
	GAS_SHADING_DENSITY_SRC,
	GAS_SAMPLER_RR,
	GAS_SAMPLER_RG,
	GAS_SAMPLER_RB,
	FRAGMENTLIGHTING_ENABLED,
	FRAGMENTLIGHTING_AMBIENT,
	FRAGMENTMATERIAL_EMISSION,
	FRAGMENTMATERIAL_AMBIENT,
	FRAGMENTMATERIAL_DIFFUSE,
	FRAGMENTMATERIAL_SPECULAR,
	FRAGMENTMATERIAL_SPECULAR2,
	FRAGMENTLIGHTSOURCE_0_ENABLED,
	FRAGMENTLIGHTSOURCE_1_ENABLED,
	FRAGMENTLIGHTSOURCE_2_ENABLED,
	FRAGMENTLIGHTSOURCE_3_ENABLED,
	FRAGMENTLIGHTSOURCE_4_ENABLED,
	FRAGMENTLIGHTSOURCE_5_ENABLED,
	FRAGMENTLIGHTSOURCE_6_ENABLED,
	FRAGMENTLIGHTSOURCE_7_ENABLED,
	FRAGMENTLIGHTSOURCE_0_AMBIENT,
	FRAGMENTLIGHTSOURCE_1_AMBIENT,
	FRAGMENTLIGHTSOURCE_2_AMBIENT,
	FRAGMENTLIGHTSOURCE_3_AMBIENT,
	FRAGMENTLIGHTSOURCE_4_AMBIENT,
	FRAGMENTLIGHTSOURCE_5_AMBIENT,
	FRAGMENTLIGHTSOURCE_6_AMBIENT,
	FRAGMENTLIGHTSOURCE_7_AMBIENT,
	FRAGMENTLIGHTSOURCE_0_DIFFUSE,
	FRAGMENTLIGHTSOURCE_1_DIFFUSE,
	FRAGMENTLIGHTSOURCE_2_DIFFUSE,
	FRAGMENTLIGHTSOURCE_3_DIFFUSE,
	FRAGMENTLIGHTSOURCE_4_DIFFUSE,
	FRAGMENTLIGHTSOURCE_5_DIFFUSE,
	FRAGMENTLIGHTSOURCE_6_DIFFUSE,
	FRAGMENTLIGHTSOURCE_7_DIFFUSE,
	FRAGMENTLIGHTSOURCE_0_SPECULAR,
	FRAGMENTLIGHTSOURCE_1_SPECULAR,
	FRAGMENTLIGHTSOURCE_2_SPECULAR,
	FRAGMENTLIGHTSOURCE_3_SPECULAR,
	FRAGMENTLIGHTSOURCE_4_SPECULAR,
	FRAGMENTLIGHTSOURCE_5_SPECULAR,
	FRAGMENTLIGHTSOURCE_6_SPECULAR,
	FRAGMENTLIGHTSOURCE_7_SPECULAR,
	FRAGMENTLIGHTSOURCE_0_SPECULAR2,
	FRAGMENTLIGHTSOURCE_1_SPECULAR2,
	FRAGMENTLIGHTSOURCE_2_SPECULAR2,
	FRAGMENTLIGHTSOURCE_3_SPECULAR2,
	FRAGMENTLIGHTSOURCE_4_SPECULAR2,
	FRAGMENTLIGHTSOURCE_5_SPECULAR2,
	FRAGMENTLIGHTSOURCE_6_SPECULAR2,
	FRAGMENTLIGHTSOURCE_7_SPECULAR2,
	FRAGMENTLIGHTSOURCE_0_POSITION,
	FRAGMENTLIGHTSOURCE_1_POSITION,
	FRAGMENTLIGHTSOURCE_2_POSITION,
	FRAGMENTLIGHTSOURCE_3_POSITION,
	FRAGMENTLIGHTSOURCE_4_POSITION,
	FRAGMENTLIGHTSOURCE_5_POSITION,
	FRAGMENTLIGHTSOURCE_6_POSITION,
	FRAGMENTLIGHTSOURCE_7_POSITION,
	FRAGMENTLIGHTSOURCE_0_SPOTDIRECTION,
	FRAGMENTLIGHTSOURCE_1_SPOTDIRECTION,
	FRAGMENTLIGHTSOURCE_2_SPOTDIRECTION,
	FRAGMENTLIGHTSOURCE_3_SPOTDIRECTION,
	FRAGMENTLIGHTSOURCE_4_SPOTDIRECTION,
	FRAGMENTLIGHTSOURCE_5_SPOTDIRECTION,
	FRAGMENTLIGHTSOURCE_6_SPOTDIRECTION,
	FRAGMENTLIGHTSOURCE_7_SPOTDIRECTION,
	FRAGMENTLIGHTSOURCE_0_SHADOWED,
	FRAGMENTLIGHTSOURCE_1_SHADOWED,
	FRAGMENTLIGHTSOURCE_2_SHADOWED,
	FRAGMENTLIGHTSOURCE_3_SHADOWED,
	FRAGMENTLIGHTSOURCE_4_SHADOWED,
	FRAGMENTLIGHTSOURCE_5_SHADOWED,
	FRAGMENTLIGHTSOURCE_6_SHADOWED,
	FRAGMENTLIGHTSOURCE_7_SHADOWED,
	FRAGMENTLIGHTSOURCE_0_GEOMFACTOR0,
	FRAGMENTLIGHTSOURCE_1_GEOMFACTOR0,
	FRAGMENTLIGHTSOURCE_2_GEOMFACTOR0,
	FRAGMENTLIGHTSOURCE_3_GEOMFACTOR0,
	FRAGMENTLIGHTSOURCE_4_GEOMFACTOR0,
	FRAGMENTLIGHTSOURCE_5_GEOMFACTOR0,
	FRAGMENTLIGHTSOURCE_6_GEOMFACTOR0,
	FRAGMENTLIGHTSOURCE_7_GEOMFACTOR0,
	FRAGMENTLIGHTSOURCE_0_GEOMFACTOR1,
	FRAGMENTLIGHTSOURCE_1_GEOMFACTOR1,
	FRAGMENTLIGHTSOURCE_2_GEOMFACTOR1,
	FRAGMENTLIGHTSOURCE_3_GEOMFACTOR1,
	FRAGMENTLIGHTSOURCE_4_GEOMFACTOR1,
	FRAGMENTLIGHTSOURCE_5_GEOMFACTOR1,
	FRAGMENTLIGHTSOURCE_6_GEOMFACTOR1,
	FRAGMENTLIGHTSOURCE_7_GEOMFACTOR1,
	FRAGMENTLIGHTSOURCE_0_TWOSIDEDIFFUSE,
	FRAGMENTLIGHTSOURCE_1_TWOSIDEDIFFUSE,
	FRAGMENTLIGHTSOURCE_2_TWOSIDEDIFFUSE,
	FRAGMENTLIGHTSOURCE_3_TWOSIDEDIFFUSE,
	FRAGMENTLIGHTSOURCE_4_TWOSIDEDIFFUSE,
	FRAGMENTLIGHTSOURCE_5_TWOSIDEDIFFUSE,
	FRAGMENTLIGHTSOURCE_6_TWOSIDEDIFFUSE,
	FRAGMENTLIGHTSOURCE_7_TWOSIDEDIFFUSE,
	FRAGMENTLIGHTSOURCE_0_SAMPLER_SP,
	FRAGMENTLIGHTSOURCE_1_SAMPLER_SP,
	FRAGMENTLIGHTSOURCE_2_SAMPLER_SP,
	FRAGMENTLIGHTSOURCE_3_SAMPLER_SP,
	FRAGMENTLIGHTSOURCE_4_SAMPLER_SP,
	FRAGMENTLIGHTSOURCE_5_SAMPLER_SP,
	FRAGMENTLIGHTSOURCE_6_SAMPLER_SP,
	FRAGMENTLIGHTSOURCE_7_SAMPLER_SP,
	FRAGMENTLIGHTSOURCE_0_SPOT_ENABLED,
	FRAGMENTLIGHTSOURCE_1_SPOT_ENABLED,
	FRAGMENTLIGHTSOURCE_2_SPOT_ENABLED,
	FRAGMENTLIGHTSOURCE_3_SPOT_ENABLED,
	FRAGMENTLIGHTSOURCE_4_SPOT_ENABLED,
	FRAGMENTLIGHTSOURCE_5_SPOT_ENABLED,
	FRAGMENTLIGHTSOURCE_6_SPOT_ENABLED,
	FRAGMENTLIGHTSOURCE_7_SPOT_ENABLED,
	FRAGMENTLIGHTSOURCE_0_DA_BIAS,
	FRAGMENTLIGHTSOURCE_1_DA_BIAS,
	FRAGMENTLIGHTSOURCE_2_DA_BIAS,
	FRAGMENTLIGHTSOURCE_3_DA_BIAS,
	FRAGMENTLIGHTSOURCE_4_DA_BIAS,
	FRAGMENTLIGHTSOURCE_5_DA_BIAS,
	FRAGMENTLIGHTSOURCE_6_DA_BIAS,
	FRAGMENTLIGHTSOURCE_7_DA_BIAS,
	FRAGMENTLIGHTSOURCE_0_DA_SCALE,
	FRAGMENTLIGHTSOURCE_1_DA_SCALE,
	FRAGMENTLIGHTSOURCE_2_DA_SCALE,
	FRAGMENTLIGHTSOURCE_3_DA_SCALE,
	FRAGMENTLIGHTSOURCE_4_DA_SCALE,
	FRAGMENTLIGHTSOURCE_5_DA_SCALE,
	FRAGMENTLIGHTSOURCE_6_DA_SCALE,
	FRAGMENTLIGHTSOURCE_7_DA_SCALE,
	FRAGMENTLIGHTSOURCE_0_DA_ENABLED,
	FRAGMENTLIGHTSOURCE_1_DA_ENABLED,
	FRAGMENTLIGHTSOURCE_2_DA_ENABLED,
	FRAGMENTLIGHTSOURCE_3_DA_ENABLED,
	FRAGMENTLIGHTSOURCE_4_DA_ENABLED,
	FRAGMENTLIGHTSOURCE_5_DA_ENABLED,
	FRAGMENTLIGHTSOURCE_6_DA_ENABLED,
	FRAGMENTLIGHTSOURCE_7_DA_ENABLED,
	FRAGMENTLIGHTSOURCE_0_SAMPLER_DA,
	FRAGMENTLIGHTSOURCE_1_SAMPLER_DA,
	FRAGMENTLIGHTSOURCE_2_SAMPLER_DA,
	FRAGMENTLIGHTSOURCE_3_SAMPLER_DA,
	FRAGMENTLIGHTSOURCE_4_SAMPLER_DA,
	FRAGMENTLIGHTSOURCE_5_SAMPLER_DA,
	FRAGMENTLIGHTSOURCE_6_SAMPLER_DA,
	FRAGMENTLIGHTSOURCE_7_SAMPLER_DA,
	LIGHTENV_ABSLUTINPUT_D0,
	LIGHTENV_ABSLUTINPUT_D1,
	LIGHTENV_ABSLUTINPUT_SP,
	LIGHTENV_ABSLUTINPUT_FR,
	LIGHTENV_ABSLUTINPUT_RB,
	LIGHTENV_ABSLUTINPUT_RG,
	LIGHTENV_ABSLUTINPUT_RR,
	LIGHTENV_LUTINPUT_D0,
	LIGHTENV_LUTINPUT_D1,
	LIGHTENV_LUTINPUT_SP,
	LIGHTENV_LUTINPUT_FR,
	LIGHTENV_LUTINPUT_RB,
	LIGHTENV_LUTINPUT_RG,
	LIGHTENV_LUTINPUT_RR,
	LIGHTENV_LUTSCALE_D0,
	LIGHTENV_LUTSCALE_D1,
	LIGHTENV_LUTSCALE_SP,
	LIGHTENV_LUTSCALE_FR,
	LIGHTENV_LUTSCALE_RB,
	LIGHTENV_LUTSCALE_RG,
	LIGHTENV_LUTSCALE_RR,
	LIGHTING_SAMPLER_D0,
	LIGHTING_SAMPLER_D1,
	LIGHTING_SAMPLER_FR,
	LIGHTING_SAMPLER_RB,
	LIGHTING_SAMPLER_RG,
	LIGHTING_SAMPLER_RR,
	LIGHTENV_SHADOWSELECTOR,
	LIGHTENV_BUMPSELECTOR,
	LIGHTENV_BUMPMODE,
	LIGHTENV_BUMPRENORM,
	LIGHTENV_CONFIG,
	LIGHTENV_INVERTSHADOW,
	LIGHTENV_SHADOWPRIMARY,
	LIGHTENV_SHADOWSECONDARY,
	LIGHTENV_SHADOWALPHA,
	LIGHTENV_FRESNELSELECTOR,
	LIGHTENV_CLAMPHIGHLIGHTS,
	LIGHTENV_LUTENABLEDD0,
	LIGHTENV_LUTENABLEDD1,
	LIGHTENV_LUTENABLEDREFL,
	TEXENV0_COMBINE_RGB,
	TEXENV1_COMBINE_RGB,
	TEXENV2_COMBINE_RGB,
	TEXENV3_COMBINE_RGB,
	TEXENV4_COMBINE_RGB,
	TEXENV5_COMBINE_RGB,
	TEXENV0_COMBINE_ALPHA,
	TEXENV1_COMBINE_ALPHA,
	TEXENV2_COMBINE_ALPHA,
	TEXENV3_COMBINE_ALPHA,
	TEXENV4_COMBINE_ALPHA,
	TEXENV5_COMBINE_ALPHA,
	TEXENV0_SRC_RGB,
	TEXENV1_SRC_RGB,
	TEXENV2_SRC_RGB,
	TEXENV3_SRC_RGB,
	TEXENV4_SRC_RGB,
	TEXENV5_SRC_RGB,
	TEXENV0_SRC_ALPHA,
	TEXENV1_SRC_ALPHA,
	TEXENV2_SRC_ALPHA,
	TEXENV3_SRC_ALPHA,
	TEXENV4_SRC_ALPHA,
	TEXENV5_SRC_ALPHA,
	TEXENV0_OPERAND_RGB,
	TEXENV1_OPERAND_RGB,
	TEXENV2_OPERAND_RGB,
	TEXENV3_OPERAND_RGB,
	TEXENV4_OPERAND_RGB,
	TEXENV5_OPERAND_RGB,
	TEXENV0_OPERAND_ALPHA,
	TEXENV1_OPERAND_ALPHA,
	TEXENV2_OPERAND_ALPHA,
	TEXENV3_OPERAND_ALPHA,
	TEXENV4_OPERAND_ALPHA,
	TEXENV5_OPERAND_ALPHA,
	TEXENV0_SCALE_RGB,
	TEXENV1_SCALE_RGB,
	TEXENV2_SCALE_RGB,
	TEXENV3_SCALE_RGB,
	TEXENV4_SCALE_RGB,
	TEXENV5_SCALE_RGB,
	TEXENV0_SCALE_ALPHA,
	TEXENV1_SCALE_ALPHA,
	TEXENV2_SCALE_ALPHA,
	TEXENV3_SCALE_ALPHA,
	TEXENV4_SCALE_ALPHA,
	TEXENV5_SCALE_ALPHA,
	TEXENV0_CONST_RGBA,
	TEXENV1_CONST_RGBA,
	TEXENV2_CONST_RGBA,
	TEXENV3_CONST_RGBA,
	TEXENV4_CONST_RGBA,
	TEXENV5_CONST_RGBA,
	TEXENV0_BUFFER_COLOR,
	TEXENV1_BUFFER_INPUT,
	TEXENV2_BUFFER_INPUT,
	TEXENV3_BUFFER_INPUT,
	TEXENV4_BUFFER_INPUT,
	FOG_MODE,
	FOG_COLOR,
	FOG_ZFLIP,
	FOG_SAMPLER,
	NUM_FRAGMENT_UNIFORM,
	
	UNIFORM_MAX_BIT = (1u << 30)
};

/* parameter for triangle setup */
enum
{
	PK_X = 0x00,
	PK_Y = 0x01,
	PK_Z = 0x02,
	PK_W = 0x03,
	PK_QC0 = 0x04,
	PK_QC1 = 0x05,
	PK_QC2 = 0x06,
	PK_QC3 = 0x07,
	PK_R = 0x08,
	PK_G = 0x09,
	PK_B = 0x0a,
	PK_A = 0x0b,
	PK_U0 = 0x0c, PK_LCX = 0x0c, PK_RX = 0x0c, PK_S = 0x0c,
	PK_V0 = 0x0d, PK_LCY = 0x0d, PK_RY = 0x0d, PK_T = 0x0d,
	PK_U1 = 0x0e,
	PK_V1 = 0x0f,
	PK_LCZ = 0x10, PK_TW = 0x10,
	PK_LCW = 0x11, PK_RZ = 0x11, PK_Q = 0x11,
	PK_VX = 0x12,
	PK_VY = 0x13,
	PK_VZ = 0x14,
	PK_DA = 0x15,
	PK_U2 = 0x16,
	PK_V2 = 0x17,
	PK_U3 = 0x18,
	PK_V3 = 0x19,
	PK_LAST = PK_V3,
	PK_INVALID = 0x1f,
	
	PK_MAX_BIT = (1u << 30)
};

/* proctex table select address */
enum
{
	PT_LUT_HRDN = 0x0,
	PT_LUT_RESERVED_1,
	PT_LUT_IN_RGB,
	PT_LUT_IN_A,
	PT_LUT_RGBA_LOD_0,
	PT_LUT_RGBA_LOD_1,
	
	PT_LUT_MAX_BIT = (1u << 30)
};

/* material sampler index */
enum
{
	MS_ID_D0 = LAP_LR_LUT_LR_D0 - LAP_LR_LUT_START,
	MS_ID_D1 = LAP_LR_LUT_LR_D1 - LAP_LR_LUT_START,
	MS_ID_FR = LAP_LR_LUT_LR_FR - LAP_LR_LUT_START,
	MS_ID_RB = LAP_LR_LUT_LR_RB - LAP_LR_LUT_START,
	MS_ID_RG = LAP_LR_LUT_LR_RG - LAP_LR_LUT_START,
	MS_ID_RR = LAP_LR_LUT_LR_RR - LAP_LR_LUT_START,
	MS_ID_SP = LAP_LR_LUT_SP0   - LAP_LR_LUT_START
};

/******************************************
* static symbol
*******************************************/
static GLuint __shminpgID = 0;
static GLuint __shminshID = 0;
static shader_manager_t*	__shman = 0;
static fixed_uniform_info_t s_fixed_uniforms[NUM_FRAGMENT_UNIFORM];
static unsigned s_ras_addr_map[RA_LAST];
static int s_vp_mode = -1;

static const struct fixed_uniform_info_table_t{
    int index;
    GLsizei	size;
    GLenum	type;
	char* name;
} s_fixed_uniform_table[] ={
	{SHADOW_PERSP,									1, GL_BOOL,				"dmp_Texture[0].perspectiveShadow"						},
	{SHADOW_BIAS,									1, GL_FLOAT,			"dmp_Texture[0].shadowZBias"							},
	{SHADOW_SCALE,									1, GL_FLOAT,			"dmp_Texture[0].shadowZScale"							},
	{TEXTURE_SAMPLER_MODE0,							1, GL_INT,				"dmp_Texture[0].samplerType"							},
	{TEXTURE_SAMPLER_MODE1,							1, GL_INT,				"dmp_Texture[1].samplerType"							},
	{TEXTURE_SAMPLER_MODE2,							1, GL_INT,				"dmp_Texture[2].samplerType"							},
	{TEXTURE_SAMPLER_MODE3,							1, GL_INT,				"dmp_Texture[3].samplerType"							},
	{TEXTURE_COORD2,								1, GL_INT,				"dmp_Texture[2].texcoord"								},
	{TEXTURE_COORD3,								1, GL_INT,				"dmp_Texture[3].texcoord"								},
	{PROCTEX_1DMAPPING_RGB,							1, GL_INT,				"dmp_Texture[3].ptRgbMap"								},
	{PROCTEX_1DMAPPING_A,							1, GL_INT,				"dmp_Texture[3].ptAlphaMap"								},
	{PROCTEX_ALPHA_SEPARATE,						1, GL_BOOL,				"dmp_Texture[3].ptAlphaSeparate"						},
	{PROCTEX_CLAMP_U,								1, GL_INT,				"dmp_Texture[3].ptClampU"								},
	{PROCTEX_CLAMP_V,								1, GL_INT,				"dmp_Texture[3].ptClampV"								},
	{PROCTEX_SHIFTU_ENABLE,							1, GL_INT,				"dmp_Texture[3].ptShiftU"								},
	{PROCTEX_SHIFTV_ENABLE,							1, GL_INT,				"dmp_Texture[3].ptShiftV"								},
	{TEXTURE_MIN_FILTER,							1, GL_INT,				"dmp_Texture[3].ptMinFilter"							},
	{CR_TEXTURE_WIDTH,								1, GL_INT,				"dmp_Texture[3].ptTexWidth"								},
	{CR_TEXTURE_OFFSET,								1, GL_INT,				"dmp_Texture[3].ptTexOffset"							},
	{PROCTEX_BIAS,									1, GL_FLOAT,			"dmp_Texture[3].ptTexBias"								},
	{PROCTEX_ENABLE_NOISE,							1, GL_BOOL,				"dmp_Texture[3].ptNoiseEnable"							},
	{PROCTEX_NOISE_U,								1, GL_FLOAT_VEC3,		"dmp_Texture[3].ptNoiseU"								},
	{PROCTEX_NOISE_V,								1, GL_FLOAT_VEC3,		"dmp_Texture[3].ptNoiseV"								},
	{PROCTEX_SAMPLER_RGB,							1, GL_SAMPLER_1D,		"dmp_Texture[3].ptSamplerRgbMap"						},
	{PROCTEX_SAMPLER_ALPHA,							1, GL_SAMPLER_1D,		"dmp_Texture[3].ptSamplerAlphaMap"						},
	{PROCTEX_SAMPLER_NOISE,							1, GL_SAMPLER_1D,		"dmp_Texture[3].ptSamplerNoiseMap"						},
	{PROCTEX_SAMPLER_R,								1, GL_SAMPLER_1D,		"dmp_Texture[3].ptSamplerR"								},
	{PROCTEX_SAMPLER_G,								1, GL_SAMPLER_1D,		"dmp_Texture[3].ptSamplerG"								},
	{PROCTEX_SAMPLER_B,								1, GL_SAMPLER_1D,		"dmp_Texture[3].ptSamplerB"								},
	{PROCTEX_SAMPLER_A,								1, GL_SAMPLER_1D,		"dmp_Texture[3].ptSamplerA"								},
	{FRAGOP_MODE,									1, GL_INT,				"dmp_FragOperation.mode"								},
	{FRAGOP_PENUMBRA_SCALE,							1, GL_FLOAT,			"dmp_FragOperation.penumbraScale"						},
	{FRAGOP_PENUMBRA_BIAS,							1, GL_FLOAT,			"dmp_FragOperation.penumbraBias"						},
	{FRAGOP_WSCALE,									1, GL_FLOAT,			"dmp_FragOperation.wScale"								},
	{FRAGOP_UCLIP_ENABLE,							1, GL_BOOL,				"dmp_FragOperation.enableClippingPlane"					},
	{FRAGOP_UCLIP_EQN,								1, GL_FLOAT_VEC4,		"dmp_FragOperation.clippingPlane"						},
	{FRAGOP_ATEST_ENABLE,							1, GL_BOOL,				"dmp_FragOperation.enableAlphaTest"						},
	{FRAGOP_ATEST_FUNC,								1, GL_INT,				"dmp_FragOperation.alphaTestFunc"						},
	{FRAGOP_ATEST_REF,								1, GL_FLOAT,			"dmp_FragOperation.alphaRefValue"						},
	{GAS_LIGHTXY,									1, GL_FLOAT_VEC3, 		"dmp_Gas.lightXY"										},
	{GAS_LIGHTZ,									1, GL_FLOAT_VEC4, 		"dmp_Gas.lightZ"										},
	{GAS_DELTA_Z,									1, GL_FLOAT,			"dmp_Gas.deltaZ"										},
	{GAS_ACCMAX,									1, GL_FLOAT,			"dmp_Gas.accMax"										},
	{GAS_AUTO_ACCMAX,								1, GL_BOOL,				"dmp_Gas.autoAcc"										},
	{GAS_ATTENUATION,								1, GL_FLOAT,			"dmp_Gas.attenuation"									},
	{GAS_COLOR_LUT_INPUT,							1, GL_INT,				"dmp_Gas.colorLutInput"									},
	{GAS_SHADING_DENSITY_SRC,						1, GL_INT,				"dmp_Gas.shadingDensitySrc"								},
	{GAS_SAMPLER_RR,								1, GL_SAMPLER_1D,		"dmp_Gas.samplerTR"										},
	{GAS_SAMPLER_RG,								1, GL_SAMPLER_1D,		"dmp_Gas.samplerTG"										},
	{GAS_SAMPLER_RB,								1, GL_SAMPLER_1D,		"dmp_Gas.samplerTB"										},
	{FRAGMENTLIGHTING_ENABLED,						1, GL_BOOL,				"dmp_FragmentLighting.enabled"							},
	{FRAGMENTLIGHTING_AMBIENT,						1, GL_FLOAT_VEC4,		"dmp_FragmentLighting.ambient"							},
	{FRAGMENTMATERIAL_EMISSION,						1, GL_FLOAT_VEC4,		"dmp_FragmentMaterial.emission"							},
	{FRAGMENTMATERIAL_AMBIENT,						1, GL_FLOAT_VEC4,		"dmp_FragmentMaterial.ambient"							},
	{FRAGMENTMATERIAL_DIFFUSE,						1, GL_FLOAT_VEC4,		"dmp_FragmentMaterial.diffuse"							},
	{FRAGMENTMATERIAL_SPECULAR,						1, GL_FLOAT_VEC4,		"dmp_FragmentMaterial.specular0"						},
	{FRAGMENTMATERIAL_SPECULAR2,					1, GL_FLOAT_VEC4,		"dmp_FragmentMaterial.specular1"						},
	{FRAGMENTLIGHTSOURCE_0_ENABLED,					1, GL_BOOL,				"dmp_FragmentLightSource[0].enabled"					},
	{FRAGMENTLIGHTSOURCE_1_ENABLED,					1, GL_BOOL,				"dmp_FragmentLightSource[1].enabled"					},
	{FRAGMENTLIGHTSOURCE_2_ENABLED,					1, GL_BOOL,				"dmp_FragmentLightSource[2].enabled"					},
	{FRAGMENTLIGHTSOURCE_3_ENABLED,					1, GL_BOOL,				"dmp_FragmentLightSource[3].enabled"					},
	{FRAGMENTLIGHTSOURCE_4_ENABLED,					1, GL_BOOL,				"dmp_FragmentLightSource[4].enabled"					},
	{FRAGMENTLIGHTSOURCE_5_ENABLED,					1, GL_BOOL,				"dmp_FragmentLightSource[5].enabled"					},
	{FRAGMENTLIGHTSOURCE_6_ENABLED,					1, GL_BOOL,				"dmp_FragmentLightSource[6].enabled"					},
	{FRAGMENTLIGHTSOURCE_7_ENABLED,					1, GL_BOOL,				"dmp_FragmentLightSource[7].enabled"					},
	{FRAGMENTLIGHTSOURCE_0_AMBIENT,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[0].ambient"					},
	{FRAGMENTLIGHTSOURCE_1_AMBIENT,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[1].ambient"					},
	{FRAGMENTLIGHTSOURCE_2_AMBIENT,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[2].ambient"					},
	{FRAGMENTLIGHTSOURCE_3_AMBIENT,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[3].ambient"					},
	{FRAGMENTLIGHTSOURCE_4_AMBIENT,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[4].ambient"					},
	{FRAGMENTLIGHTSOURCE_5_AMBIENT,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[5].ambient"					},
	{FRAGMENTLIGHTSOURCE_6_AMBIENT,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[6].ambient"					},
	{FRAGMENTLIGHTSOURCE_7_AMBIENT,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[7].ambient"					},
	{FRAGMENTLIGHTSOURCE_0_DIFFUSE,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[0].diffuse"					},
	{FRAGMENTLIGHTSOURCE_1_DIFFUSE,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[1].diffuse"					},
	{FRAGMENTLIGHTSOURCE_2_DIFFUSE,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[2].diffuse"					},
	{FRAGMENTLIGHTSOURCE_3_DIFFUSE,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[3].diffuse"					},
	{FRAGMENTLIGHTSOURCE_4_DIFFUSE,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[4].diffuse"					},
	{FRAGMENTLIGHTSOURCE_5_DIFFUSE,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[5].diffuse"					},
	{FRAGMENTLIGHTSOURCE_6_DIFFUSE,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[6].diffuse"					},
	{FRAGMENTLIGHTSOURCE_7_DIFFUSE,					1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[7].diffuse"					},
	{FRAGMENTLIGHTSOURCE_0_SPECULAR,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[0].specular0"					},
	{FRAGMENTLIGHTSOURCE_1_SPECULAR,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[1].specular0"					},
	{FRAGMENTLIGHTSOURCE_2_SPECULAR,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[2].specular0"					},
	{FRAGMENTLIGHTSOURCE_3_SPECULAR,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[3].specular0"					},
	{FRAGMENTLIGHTSOURCE_4_SPECULAR,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[4].specular0"					},
	{FRAGMENTLIGHTSOURCE_5_SPECULAR,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[5].specular0"					},
	{FRAGMENTLIGHTSOURCE_6_SPECULAR,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[6].specular0"					},
	{FRAGMENTLIGHTSOURCE_7_SPECULAR,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[7].specular0"					},
	{FRAGMENTLIGHTSOURCE_0_SPECULAR2,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[0].specular1"					},
	{FRAGMENTLIGHTSOURCE_1_SPECULAR2,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[1].specular1"					},
	{FRAGMENTLIGHTSOURCE_2_SPECULAR2,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[2].specular1"					},
	{FRAGMENTLIGHTSOURCE_3_SPECULAR2,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[3].specular1"					},
	{FRAGMENTLIGHTSOURCE_4_SPECULAR2,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[4].specular1"					},
	{FRAGMENTLIGHTSOURCE_5_SPECULAR2,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[5].specular1"					},
	{FRAGMENTLIGHTSOURCE_6_SPECULAR2,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[6].specular1"					},
	{FRAGMENTLIGHTSOURCE_7_SPECULAR2,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[7].specular1"					},
	{FRAGMENTLIGHTSOURCE_0_POSITION,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[0].position"					},
	{FRAGMENTLIGHTSOURCE_1_POSITION,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[1].position"					},
	{FRAGMENTLIGHTSOURCE_2_POSITION,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[2].position"					},
	{FRAGMENTLIGHTSOURCE_3_POSITION,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[3].position"					},
	{FRAGMENTLIGHTSOURCE_4_POSITION,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[4].position"					},
	{FRAGMENTLIGHTSOURCE_5_POSITION,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[5].position"					},
	{FRAGMENTLIGHTSOURCE_6_POSITION,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[6].position"					},
	{FRAGMENTLIGHTSOURCE_7_POSITION,				1, GL_FLOAT_VEC4,		"dmp_FragmentLightSource[7].position"					},
	{FRAGMENTLIGHTSOURCE_0_SPOTDIRECTION,			1, GL_FLOAT_VEC3,		"dmp_FragmentLightSource[0].spotDirection"				},
	{FRAGMENTLIGHTSOURCE_1_SPOTDIRECTION,			1, GL_FLOAT_VEC3,		"dmp_FragmentLightSource[1].spotDirection"				},
	{FRAGMENTLIGHTSOURCE_2_SPOTDIRECTION,			1, GL_FLOAT_VEC3,		"dmp_FragmentLightSource[2].spotDirection"				},
	{FRAGMENTLIGHTSOURCE_3_SPOTDIRECTION,			1, GL_FLOAT_VEC3,		"dmp_FragmentLightSource[3].spotDirection"				},
	{FRAGMENTLIGHTSOURCE_4_SPOTDIRECTION,			1, GL_FLOAT_VEC3,		"dmp_FragmentLightSource[4].spotDirection"				},
	{FRAGMENTLIGHTSOURCE_5_SPOTDIRECTION,			1, GL_FLOAT_VEC3,		"dmp_FragmentLightSource[5].spotDirection"				},
	{FRAGMENTLIGHTSOURCE_6_SPOTDIRECTION,			1, GL_FLOAT_VEC3,		"dmp_FragmentLightSource[6].spotDirection"				},
	{FRAGMENTLIGHTSOURCE_7_SPOTDIRECTION,			1, GL_FLOAT_VEC3,		"dmp_FragmentLightSource[7].spotDirection"				},
	{FRAGMENTLIGHTSOURCE_0_SHADOWED,				1, GL_BOOL,				"dmp_FragmentLightSource[0].shadowed"					},
	{FRAGMENTLIGHTSOURCE_1_SHADOWED,				1, GL_BOOL,				"dmp_FragmentLightSource[1].shadowed"					},
	{FRAGMENTLIGHTSOURCE_2_SHADOWED,				1, GL_BOOL,				"dmp_FragmentLightSource[2].shadowed"					},
	{FRAGMENTLIGHTSOURCE_3_SHADOWED,				1, GL_BOOL,				"dmp_FragmentLightSource[3].shadowed"					},
	{FRAGMENTLIGHTSOURCE_4_SHADOWED,				1, GL_BOOL,				"dmp_FragmentLightSource[4].shadowed"					},
	{FRAGMENTLIGHTSOURCE_5_SHADOWED,				1, GL_BOOL,				"dmp_FragmentLightSource[5].shadowed"					},
	{FRAGMENTLIGHTSOURCE_6_SHADOWED,				1, GL_BOOL,				"dmp_FragmentLightSource[6].shadowed"					},
	{FRAGMENTLIGHTSOURCE_7_SHADOWED,				1, GL_BOOL,				"dmp_FragmentLightSource[7].shadowed"					},
	{FRAGMENTLIGHTSOURCE_0_GEOMFACTOR0,				1, GL_BOOL,				"dmp_FragmentLightSource[0].geomFactor0"				},
	{FRAGMENTLIGHTSOURCE_1_GEOMFACTOR0,				1, GL_BOOL,				"dmp_FragmentLightSource[1].geomFactor0"				},
	{FRAGMENTLIGHTSOURCE_2_GEOMFACTOR0,				1, GL_BOOL,				"dmp_FragmentLightSource[2].geomFactor0"				},
	{FRAGMENTLIGHTSOURCE_3_GEOMFACTOR0,				1, GL_BOOL,				"dmp_FragmentLightSource[3].geomFactor0"				},
	{FRAGMENTLIGHTSOURCE_4_GEOMFACTOR0,				1, GL_BOOL,				"dmp_FragmentLightSource[4].geomFactor0"				},
	{FRAGMENTLIGHTSOURCE_5_GEOMFACTOR0,				1, GL_BOOL,				"dmp_FragmentLightSource[5].geomFactor0"				},
	{FRAGMENTLIGHTSOURCE_6_GEOMFACTOR0,				1, GL_BOOL,				"dmp_FragmentLightSource[6].geomFactor0"				},
	{FRAGMENTLIGHTSOURCE_7_GEOMFACTOR0,				1, GL_BOOL,				"dmp_FragmentLightSource[7].geomFactor0"				},
	{FRAGMENTLIGHTSOURCE_0_GEOMFACTOR1,				1, GL_BOOL,				"dmp_FragmentLightSource[0].geomFactor1"				},
	{FRAGMENTLIGHTSOURCE_1_GEOMFACTOR1,				1, GL_BOOL,				"dmp_FragmentLightSource[1].geomFactor1"				},
	{FRAGMENTLIGHTSOURCE_2_GEOMFACTOR1,				1, GL_BOOL,				"dmp_FragmentLightSource[2].geomFactor1"				},
	{FRAGMENTLIGHTSOURCE_3_GEOMFACTOR1,				1, GL_BOOL,				"dmp_FragmentLightSource[3].geomFactor1"				},
	{FRAGMENTLIGHTSOURCE_4_GEOMFACTOR1,				1, GL_BOOL,				"dmp_FragmentLightSource[4].geomFactor1"				},
	{FRAGMENTLIGHTSOURCE_5_GEOMFACTOR1,				1, GL_BOOL,				"dmp_FragmentLightSource[5].geomFactor1"				},
	{FRAGMENTLIGHTSOURCE_6_GEOMFACTOR1,				1, GL_BOOL,				"dmp_FragmentLightSource[6].geomFactor1"				},
	{FRAGMENTLIGHTSOURCE_7_GEOMFACTOR1,				1, GL_BOOL,				"dmp_FragmentLightSource[7].geomFactor1"				},
	{FRAGMENTLIGHTSOURCE_0_TWOSIDEDIFFUSE,			1, GL_BOOL,				"dmp_FragmentLightSource[0].twoSideDiffuse"				},
	{FRAGMENTLIGHTSOURCE_1_TWOSIDEDIFFUSE,			1, GL_BOOL,				"dmp_FragmentLightSource[1].twoSideDiffuse"				},
	{FRAGMENTLIGHTSOURCE_2_TWOSIDEDIFFUSE,			1, GL_BOOL,				"dmp_FragmentLightSource[2].twoSideDiffuse"				},
	{FRAGMENTLIGHTSOURCE_3_TWOSIDEDIFFUSE,			1, GL_BOOL,				"dmp_FragmentLightSource[3].twoSideDiffuse"				},
	{FRAGMENTLIGHTSOURCE_4_TWOSIDEDIFFUSE,			1, GL_BOOL,				"dmp_FragmentLightSource[4].twoSideDiffuse"				},
	{FRAGMENTLIGHTSOURCE_5_TWOSIDEDIFFUSE,			1, GL_BOOL,				"dmp_FragmentLightSource[5].twoSideDiffuse"				},
	{FRAGMENTLIGHTSOURCE_6_TWOSIDEDIFFUSE,			1, GL_BOOL,				"dmp_FragmentLightSource[6].twoSideDiffuse"				},
	{FRAGMENTLIGHTSOURCE_7_TWOSIDEDIFFUSE,			1, GL_BOOL,				"dmp_FragmentLightSource[7].twoSideDiffuse"				},
	{FRAGMENTLIGHTSOURCE_0_SAMPLER_SP,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[0].samplerSP"					},
	{FRAGMENTLIGHTSOURCE_1_SAMPLER_SP,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[1].samplerSP"					},
	{FRAGMENTLIGHTSOURCE_2_SAMPLER_SP,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[2].samplerSP"					},
	{FRAGMENTLIGHTSOURCE_3_SAMPLER_SP,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[3].samplerSP"					},
	{FRAGMENTLIGHTSOURCE_4_SAMPLER_SP,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[4].samplerSP"					},
	{FRAGMENTLIGHTSOURCE_5_SAMPLER_SP,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[5].samplerSP"					},
	{FRAGMENTLIGHTSOURCE_6_SAMPLER_SP,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[6].samplerSP"					},
	{FRAGMENTLIGHTSOURCE_7_SAMPLER_SP,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[7].samplerSP"					},
	{FRAGMENTLIGHTSOURCE_0_SPOT_ENABLED,			1, GL_BOOL,				"dmp_FragmentLightSource[0].spotEnabled"				},
	{FRAGMENTLIGHTSOURCE_1_SPOT_ENABLED,			1, GL_BOOL,				"dmp_FragmentLightSource[1].spotEnabled"				},
	{FRAGMENTLIGHTSOURCE_2_SPOT_ENABLED,			1, GL_BOOL,				"dmp_FragmentLightSource[2].spotEnabled"				},
	{FRAGMENTLIGHTSOURCE_3_SPOT_ENABLED,			1, GL_BOOL,				"dmp_FragmentLightSource[3].spotEnabled"				},
	{FRAGMENTLIGHTSOURCE_4_SPOT_ENABLED,			1, GL_BOOL,				"dmp_FragmentLightSource[4].spotEnabled"				},
	{FRAGMENTLIGHTSOURCE_5_SPOT_ENABLED,			1, GL_BOOL,				"dmp_FragmentLightSource[5].spotEnabled"				},
	{FRAGMENTLIGHTSOURCE_6_SPOT_ENABLED,			1, GL_BOOL,				"dmp_FragmentLightSource[6].spotEnabled"				},
	{FRAGMENTLIGHTSOURCE_7_SPOT_ENABLED,			1, GL_BOOL,				"dmp_FragmentLightSource[7].spotEnabled"				},
	{FRAGMENTLIGHTSOURCE_0_DA_BIAS,					1, GL_FLOAT,			"dmp_FragmentLightSource[0].distanceAttenuationBias"	},
	{FRAGMENTLIGHTSOURCE_1_DA_BIAS,					1, GL_FLOAT,			"dmp_FragmentLightSource[1].distanceAttenuationBias"	},
	{FRAGMENTLIGHTSOURCE_2_DA_BIAS,					1, GL_FLOAT,			"dmp_FragmentLightSource[2].distanceAttenuationBias"	},
	{FRAGMENTLIGHTSOURCE_3_DA_BIAS,					1, GL_FLOAT,			"dmp_FragmentLightSource[3].distanceAttenuationBias"	},
	{FRAGMENTLIGHTSOURCE_4_DA_BIAS,					1, GL_FLOAT,			"dmp_FragmentLightSource[4].distanceAttenuationBias"	},
	{FRAGMENTLIGHTSOURCE_5_DA_BIAS,					1, GL_FLOAT,			"dmp_FragmentLightSource[5].distanceAttenuationBias"	},
	{FRAGMENTLIGHTSOURCE_6_DA_BIAS,					1, GL_FLOAT,			"dmp_FragmentLightSource[6].distanceAttenuationBias"	},
	{FRAGMENTLIGHTSOURCE_7_DA_BIAS,					1, GL_FLOAT,			"dmp_FragmentLightSource[7].distanceAttenuationBias"	},
	{FRAGMENTLIGHTSOURCE_0_DA_SCALE,				1, GL_FLOAT,			"dmp_FragmentLightSource[0].distanceAttenuationScale"	},
	{FRAGMENTLIGHTSOURCE_1_DA_SCALE,				1, GL_FLOAT,			"dmp_FragmentLightSource[1].distanceAttenuationScale"	},
	{FRAGMENTLIGHTSOURCE_2_DA_SCALE,				1, GL_FLOAT,			"dmp_FragmentLightSource[2].distanceAttenuationScale"	},
	{FRAGMENTLIGHTSOURCE_3_DA_SCALE,				1, GL_FLOAT,			"dmp_FragmentLightSource[3].distanceAttenuationScale"	},
	{FRAGMENTLIGHTSOURCE_4_DA_SCALE,				1, GL_FLOAT,			"dmp_FragmentLightSource[4].distanceAttenuationScale"	},
	{FRAGMENTLIGHTSOURCE_5_DA_SCALE,				1, GL_FLOAT,			"dmp_FragmentLightSource[5].distanceAttenuationScale"	},
	{FRAGMENTLIGHTSOURCE_6_DA_SCALE,				1, GL_FLOAT,			"dmp_FragmentLightSource[6].distanceAttenuationScale"	},
	{FRAGMENTLIGHTSOURCE_7_DA_SCALE,				1, GL_FLOAT,			"dmp_FragmentLightSource[7].distanceAttenuationScale"	},
	{FRAGMENTLIGHTSOURCE_0_DA_ENABLED,				1, GL_BOOL,				"dmp_FragmentLightSource[0].distanceAttenuationEnabled"	},
	{FRAGMENTLIGHTSOURCE_1_DA_ENABLED,				1, GL_BOOL,				"dmp_FragmentLightSource[1].distanceAttenuationEnabled"	},
	{FRAGMENTLIGHTSOURCE_2_DA_ENABLED,				1, GL_BOOL,				"dmp_FragmentLightSource[2].distanceAttenuationEnabled"	},
	{FRAGMENTLIGHTSOURCE_3_DA_ENABLED,				1, GL_BOOL,				"dmp_FragmentLightSource[3].distanceAttenuationEnabled"	},
	{FRAGMENTLIGHTSOURCE_4_DA_ENABLED,				1, GL_BOOL,				"dmp_FragmentLightSource[4].distanceAttenuationEnabled"	},
	{FRAGMENTLIGHTSOURCE_5_DA_ENABLED,				1, GL_BOOL,				"dmp_FragmentLightSource[5].distanceAttenuationEnabled"	},
	{FRAGMENTLIGHTSOURCE_6_DA_ENABLED,				1, GL_BOOL,				"dmp_FragmentLightSource[6].distanceAttenuationEnabled"	},
	{FRAGMENTLIGHTSOURCE_7_DA_ENABLED,				1, GL_BOOL,				"dmp_FragmentLightSource[7].distanceAttenuationEnabled"	},
	{FRAGMENTLIGHTSOURCE_0_SAMPLER_DA,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[0].samplerDA"					},
	{FRAGMENTLIGHTSOURCE_1_SAMPLER_DA,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[1].samplerDA"					},
	{FRAGMENTLIGHTSOURCE_2_SAMPLER_DA,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[2].samplerDA"					},
	{FRAGMENTLIGHTSOURCE_3_SAMPLER_DA,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[3].samplerDA"					},
	{FRAGMENTLIGHTSOURCE_4_SAMPLER_DA,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[4].samplerDA"					},
	{FRAGMENTLIGHTSOURCE_5_SAMPLER_DA,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[5].samplerDA"					},
	{FRAGMENTLIGHTSOURCE_6_SAMPLER_DA,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[6].samplerDA"					},
	{FRAGMENTLIGHTSOURCE_7_SAMPLER_DA,				1, GL_SAMPLER_1D,		"dmp_FragmentLightSource[7].samplerDA"					},
	{LIGHTENV_ABSLUTINPUT_D0,						1, GL_BOOL,				"dmp_LightEnv.absLutInputD0"							},
	{LIGHTENV_ABSLUTINPUT_D1,						1, GL_BOOL,				"dmp_LightEnv.absLutInputD1"							},
	{LIGHTENV_ABSLUTINPUT_SP,						1, GL_BOOL,				"dmp_LightEnv.absLutInputSP"							},
	{LIGHTENV_ABSLUTINPUT_FR,						1, GL_BOOL,				"dmp_LightEnv.absLutInputFR"							},
	{LIGHTENV_ABSLUTINPUT_RB,						1, GL_BOOL,				"dmp_LightEnv.absLutInputRB"							},
	{LIGHTENV_ABSLUTINPUT_RG,						1, GL_BOOL,				"dmp_LightEnv.absLutInputRG"							},
	{LIGHTENV_ABSLUTINPUT_RR,						1, GL_BOOL,				"dmp_LightEnv.absLutInputRR"							},
	{LIGHTENV_LUTINPUT_D0,							1, GL_INT,				"dmp_LightEnv.lutInputD0"								},
	{LIGHTENV_LUTINPUT_D1,							1, GL_INT,				"dmp_LightEnv.lutInputD1"								},
	{LIGHTENV_LUTINPUT_SP,							1, GL_INT, 				"dmp_LightEnv.lutInputSP"								},
	{LIGHTENV_LUTINPUT_FR,							1, GL_INT,				"dmp_LightEnv.lutInputFR"								},
	{LIGHTENV_LUTINPUT_RB,							1, GL_INT,				"dmp_LightEnv.lutInputRB"								},
	{LIGHTENV_LUTINPUT_RG,							1, GL_INT,				"dmp_LightEnv.lutInputRG"								},
	{LIGHTENV_LUTINPUT_RR,							1, GL_INT,				"dmp_LightEnv.lutInputRR"								},
	{LIGHTENV_LUTSCALE_D0,							1, GL_FLOAT,			"dmp_LightEnv.lutScaleD0"								},
	{LIGHTENV_LUTSCALE_D1,							1, GL_FLOAT,			"dmp_LightEnv.lutScaleD1"								},
	{LIGHTENV_LUTSCALE_SP,							1, GL_FLOAT,			"dmp_LightEnv.lutScaleSP"								},
	{LIGHTENV_LUTSCALE_FR,							1, GL_FLOAT,			"dmp_LightEnv.lutScaleFR"								},
	{LIGHTENV_LUTSCALE_RB,							1, GL_FLOAT,			"dmp_LightEnv.lutScaleRB"								},
	{LIGHTENV_LUTSCALE_RG,							1, GL_FLOAT,			"dmp_LightEnv.lutScaleRG"								},
	{LIGHTENV_LUTSCALE_RR,							1, GL_FLOAT,			"dmp_LightEnv.lutScaleRR"								},
	{LIGHTING_SAMPLER_D0,							1, GL_SAMPLER_1D,		"dmp_FragmentMaterial.samplerD0"						},
	{LIGHTING_SAMPLER_D1,							1, GL_SAMPLER_1D,		"dmp_FragmentMaterial.samplerD1"						},
	{LIGHTING_SAMPLER_FR,							1, GL_SAMPLER_1D,		"dmp_FragmentMaterial.samplerFR"						},
	{LIGHTING_SAMPLER_RB,							1, GL_SAMPLER_1D,		"dmp_FragmentMaterial.samplerRB"						},
	{LIGHTING_SAMPLER_RG,							1, GL_SAMPLER_1D,		"dmp_FragmentMaterial.samplerRG"						},
	{LIGHTING_SAMPLER_RR,							1, GL_SAMPLER_1D,		"dmp_FragmentMaterial.samplerRR"						},
	{LIGHTENV_SHADOWSELECTOR,						1, GL_INT,				"dmp_LightEnv.shadowSelector"							},
	{LIGHTENV_BUMPSELECTOR,							1, GL_INT,				"dmp_LightEnv.bumpSelector"								},
	{LIGHTENV_BUMPMODE,								1, GL_INT,				"dmp_LightEnv.bumpMode"									},
	{LIGHTENV_BUMPRENORM,							1, GL_BOOL,				"dmp_LightEnv.bumpRenorm"								},
	{LIGHTENV_CONFIG,								1, GL_INT,				"dmp_LightEnv.config"									},
	{LIGHTENV_INVERTSHADOW,							1, GL_BOOL,				"dmp_LightEnv.invertShadow"								},
	{LIGHTENV_SHADOWPRIMARY,						1, GL_BOOL,				"dmp_LightEnv.shadowPrimary"							},
	{LIGHTENV_SHADOWSECONDARY,						1, GL_BOOL,				"dmp_LightEnv.shadowSecondary"							},
	{LIGHTENV_SHADOWALPHA,							1, GL_BOOL,				"dmp_LightEnv.shadowAlpha"								},
	{LIGHTENV_FRESNELSELECTOR,						1, GL_INT,				"dmp_LightEnv.fresnelSelector"							},
	{LIGHTENV_CLAMPHIGHLIGHTS,						1, GL_BOOL,				"dmp_LightEnv.clampHighlights"							},
	{LIGHTENV_LUTENABLEDD0,							1, GL_BOOL,				"dmp_LightEnv.lutEnabledD0"								},
	{LIGHTENV_LUTENABLEDD1,							1, GL_BOOL,				"dmp_LightEnv.lutEnabledD1"								},
	{LIGHTENV_LUTENABLEDREFL,						1, GL_BOOL,				"dmp_LightEnv.lutEnabledRefl"							},
	{TEXENV0_COMBINE_RGB,							1, GL_INT,				"dmp_TexEnv[0].combineRgb"								},
	{TEXENV1_COMBINE_RGB,							1, GL_INT,				"dmp_TexEnv[1].combineRgb"								},
	{TEXENV2_COMBINE_RGB,							1, GL_INT,				"dmp_TexEnv[2].combineRgb"								},
	{TEXENV3_COMBINE_RGB,							1, GL_INT,				"dmp_TexEnv[3].combineRgb"								},
	{TEXENV4_COMBINE_RGB,							1, GL_INT,				"dmp_TexEnv[4].combineRgb"								},
	{TEXENV5_COMBINE_RGB,							1, GL_INT,				"dmp_TexEnv[5].combineRgb"								},
	{TEXENV0_COMBINE_ALPHA,							1, GL_INT,				"dmp_TexEnv[0].combineAlpha"							},
	{TEXENV1_COMBINE_ALPHA,							1, GL_INT,				"dmp_TexEnv[1].combineAlpha"							},
	{TEXENV2_COMBINE_ALPHA,							1, GL_INT,				"dmp_TexEnv[2].combineAlpha"							},
	{TEXENV3_COMBINE_ALPHA,							1, GL_INT,				"dmp_TexEnv[3].combineAlpha"							},
	{TEXENV4_COMBINE_ALPHA,							1, GL_INT,				"dmp_TexEnv[4].combineAlpha"							},
	{TEXENV5_COMBINE_ALPHA,							1, GL_INT,				"dmp_TexEnv[5].combineAlpha"							},
	{TEXENV0_SRC_RGB,								1, GL_INT_VEC3,			"dmp_TexEnv[0].srcRgb"									},
	{TEXENV1_SRC_RGB,								1, GL_INT_VEC3,			"dmp_TexEnv[1].srcRgb"									},
	{TEXENV2_SRC_RGB,								1, GL_INT_VEC3,			"dmp_TexEnv[2].srcRgb"									},
	{TEXENV3_SRC_RGB,								1, GL_INT_VEC3,			"dmp_TexEnv[3].srcRgb"									},
	{TEXENV4_SRC_RGB,								1, GL_INT_VEC3,			"dmp_TexEnv[4].srcRgb"									},
	{TEXENV5_SRC_RGB,								1, GL_INT_VEC3,			"dmp_TexEnv[5].srcRgb"									},
	{TEXENV0_SRC_ALPHA,								1, GL_INT_VEC3,			"dmp_TexEnv[0].srcAlpha"								},
	{TEXENV1_SRC_ALPHA,								1, GL_INT_VEC3,			"dmp_TexEnv[1].srcAlpha"								},
	{TEXENV2_SRC_ALPHA,								1, GL_INT_VEC3,			"dmp_TexEnv[2].srcAlpha"								},
	{TEXENV3_SRC_ALPHA,								1, GL_INT_VEC3,			"dmp_TexEnv[3].srcAlpha"								},
	{TEXENV4_SRC_ALPHA,								1, GL_INT_VEC3,			"dmp_TexEnv[4].srcAlpha"								},
	{TEXENV5_SRC_ALPHA,								1, GL_INT_VEC3,			"dmp_TexEnv[5].srcAlpha"								},
	{TEXENV0_OPERAND_RGB,							1, GL_INT_VEC3,			"dmp_TexEnv[0].operandRgb"								},
	{TEXENV1_OPERAND_RGB,							1, GL_INT_VEC3,			"dmp_TexEnv[1].operandRgb"								},
	{TEXENV2_OPERAND_RGB,							1, GL_INT_VEC3,			"dmp_TexEnv[2].operandRgb"								},
	{TEXENV3_OPERAND_RGB,							1, GL_INT_VEC3,			"dmp_TexEnv[3].operandRgb"								},
	{TEXENV4_OPERAND_RGB,							1, GL_INT_VEC3,			"dmp_TexEnv[4].operandRgb"								},
	{TEXENV5_OPERAND_RGB,							1, GL_INT_VEC3,			"dmp_TexEnv[5].operandRgb"								},
	{TEXENV0_OPERAND_ALPHA,							1, GL_INT_VEC3,			"dmp_TexEnv[0].operandAlpha"							},
	{TEXENV1_OPERAND_ALPHA,							1, GL_INT_VEC3,			"dmp_TexEnv[1].operandAlpha"							},
	{TEXENV2_OPERAND_ALPHA,							1, GL_INT_VEC3,			"dmp_TexEnv[2].operandAlpha"							},
	{TEXENV3_OPERAND_ALPHA,							1, GL_INT_VEC3,			"dmp_TexEnv[3].operandAlpha"							},
	{TEXENV4_OPERAND_ALPHA,							1, GL_INT_VEC3,			"dmp_TexEnv[4].operandAlpha"							},
	{TEXENV5_OPERAND_ALPHA,							1, GL_INT_VEC3,			"dmp_TexEnv[5].operandAlpha"							},
	{TEXENV0_SCALE_RGB,								1, GL_FLOAT,			"dmp_TexEnv[0].scaleRgb"								},
	{TEXENV1_SCALE_RGB,								1, GL_FLOAT,			"dmp_TexEnv[1].scaleRgb"								},
	{TEXENV2_SCALE_RGB,								1, GL_FLOAT,			"dmp_TexEnv[2].scaleRgb"								},
	{TEXENV3_SCALE_RGB,								1, GL_FLOAT,			"dmp_TexEnv[3].scaleRgb"								},
	{TEXENV4_SCALE_RGB,								1, GL_FLOAT,			"dmp_TexEnv[4].scaleRgb"								},
	{TEXENV5_SCALE_RGB,								1, GL_FLOAT,			"dmp_TexEnv[5].scaleRgb"								},
	{TEXENV0_SCALE_ALPHA,							1, GL_FLOAT,			"dmp_TexEnv[0].scaleAlpha"								},
	{TEXENV1_SCALE_ALPHA,							1, GL_FLOAT,			"dmp_TexEnv[1].scaleAlpha"								},
	{TEXENV2_SCALE_ALPHA,							1, GL_FLOAT,			"dmp_TexEnv[2].scaleAlpha"								},
	{TEXENV3_SCALE_ALPHA,							1, GL_FLOAT,			"dmp_TexEnv[3].scaleAlpha"								},
	{TEXENV4_SCALE_ALPHA,							1, GL_FLOAT,			"dmp_TexEnv[4].scaleAlpha"								},
	{TEXENV5_SCALE_ALPHA,							1, GL_FLOAT,			"dmp_TexEnv[5].scaleAlpha"								},
	{TEXENV0_CONST_RGBA,							1, GL_FLOAT_VEC4,		"dmp_TexEnv[0].constRgba"								},
	{TEXENV1_CONST_RGBA,							1, GL_FLOAT_VEC4,		"dmp_TexEnv[1].constRgba"								},
	{TEXENV2_CONST_RGBA,							1, GL_FLOAT_VEC4,		"dmp_TexEnv[2].constRgba"								},
	{TEXENV3_CONST_RGBA,							1, GL_FLOAT_VEC4,		"dmp_TexEnv[3].constRgba"								},
	{TEXENV4_CONST_RGBA,							1, GL_FLOAT_VEC4,		"dmp_TexEnv[4].constRgba"								},
	{TEXENV5_CONST_RGBA,							1, GL_FLOAT_VEC4,		"dmp_TexEnv[5].constRgba"								},
	{TEXENV0_BUFFER_COLOR,							1, GL_FLOAT_VEC4,		"dmp_TexEnv[0].bufferColor"								},
	{TEXENV1_BUFFER_INPUT,							1, GL_INT_VEC2,			"dmp_TexEnv[1].bufferInput"								},
	{TEXENV2_BUFFER_INPUT,							1, GL_INT_VEC2,			"dmp_TexEnv[2].bufferInput"								},
	{TEXENV3_BUFFER_INPUT,							1, GL_INT_VEC2,			"dmp_TexEnv[3].bufferInput"								},
	{TEXENV4_BUFFER_INPUT,							1, GL_INT_VEC2,			"dmp_TexEnv[4].bufferInput"								},
	{FOG_MODE,										1, GL_INT,				"dmp_Fog.mode"											},
	{FOG_COLOR,										1, GL_FLOAT_VEC3,		"dmp_Fog.color"											},
	{FOG_ZFLIP,										1, GL_BOOL,				"dmp_Fog.zFlip"											},
	{FOG_SAMPLER,									1, GL_SAMPLER_1D,		"dmp_Fog.sampler"										},
	{-1, 0, 0, 0}
};

static const struct pica_ras_addr_map_table_t{
	int				index;
	unsigned		address;
} s_ras_addr_map_table[] =
{
	{RA_CI_VP_MODE,						PA_RW_CI_VP_MODE,									},
	{RA_VP0_BOOL_REG,					PA_RW_VP_BOOL_REG,									},
	{RA_VP0_INT_REG_TOP_00,				PA_RW_VP_INT_REG_TOP,								},
	{RA_VP0_INT_REG_TOP_01,				PA_RW_VP_INT_REG_TOP + 1,							},
	{RA_VP0_INT_REG_TOP_02,				PA_RW_VP_INT_REG_TOP + 2,							},
	{RA_VP0_INT_REG_TOP_03,				PA_RW_VP_INT_REG_TOP + 3,							},
	{RA_VP0_MODE,						PA_RW_VP_MODE,										},
	{RA_VP0_PROGRAM_ADDRESS,			PA_RW_VP_PROGRAM_ADDRESS,							},
	{RA_VP0_OUTPUT_MASK,				PA_RW_VP_OUTPUT_MASK,								},
	{RA_VP1_BOOL_REG,					PA_RW_VP_BOOL_REG + GEO_VP_LEN,						},
	{RA_VP1_INT_REG_TOP_00,				PA_RW_VP_INT_REG_TOP + GEO_VP_LEN,					},
	{RA_VP1_INT_REG_TOP_01,				PA_RW_VP_INT_REG_TOP + 1 + GEO_VP_LEN,				},
	{RA_VP1_INT_REG_TOP_02,				PA_RW_VP_INT_REG_TOP + 2 + GEO_VP_LEN,				},
	{RA_VP1_INT_REG_TOP_03,				PA_RW_VP_INT_REG_TOP + 3 + GEO_VP_LEN,				},
	{RA_VP1_MODE,						PA_RW_VP_MODE + GEO_VP_LEN,							},
	{RA_VP1_PROGRAM_ADDRESS,			PA_RW_VP_PROGRAM_ADDRESS + GEO_VP_LEN,				},
	{RA_VP1_OUTPUT_MASK,				PA_RW_VP_OUTPUT_MASK + GEO_VP_LEN,					},
	{RA_VC_ATTR_COUNT,					PA_RW_VC_ATTR_COUNT,								},
	{RA_VC_SEND_MODE,					PA_RW_VC_SEND_MODE,									},
	{RA_VC_SUBDIV_CONF,					PA_RW_VC_SUBDIV_CONF,								},
	{RA_LB_ATTR_COUNT,					PA_RW_LB_ATTR_COUNT,								},
	{RA_SYNC_ATTR_COUNT,				PA_RW_SYNC_ATTR_COUNT,								},
	{RA_RASIN_TRI_SETTING,				PA_RW_RASIN_TRI_SETTING,							},
	{RA_TS_ZSCALE,						PA_RW_TS_ZSCALE,									},
	{RA_TS_ZBIAS,						PA_RW_TS_ZBIAS,										},
	{RA_TS_WORD_NUM,					PA_RW_TS_WORD_NUM,									},
	{RA_TS_VTXWORD1,					PA_RW_TS_VTXWORD1,									},
	{RA_TS_VTXWORD2,					PA_RW_TS_VTXWORD2,									},
	{RA_TS_VTXWORD3,					PA_RW_TS_VTXWORD3,									},
	{RA_TS_VTXWORD4,					PA_RW_TS_VTXWORD4,									},
	{RA_TS_VTXWORD5,					PA_RW_TS_VTXWORD5,									},
	{RA_TS_VTXWORD6,					PA_RW_TS_VTXWORD6,									},
	{RA_TS_VTXWORD7,					PA_RW_TS_VTXWORD7,									},
	{RA_TS_USRCLIPENABLE,				PA_RW_TS_USRCLIPENABLE,								},
	{RA_TS_USRCLIPA,					PA_RW_TS_USRCLIPA,									},
	{RA_TS_USRCLIPB,					PA_RW_TS_USRCLIPB,									},
	{RA_TS_USRCLIPC,					PA_RW_TS_USRCLIPC,									},
	{RA_TS_USRCLIPD,					PA_RW_TS_USRCLIPD,									},
	{RA_TI_TEX_DERIVS_ENABLE,			PA_RW_TI_TEX_DERIVS_ENABLE,							},
	{RA_TI_Z_DIV_W,						PA_RW_TI_Z_DIV_W,									},
	{RA_TI_CLOCK_ENABLE,				PA_RW_TI_CLOCK_ENABLE,								},
	{RA_TU0_BEGIN_00,					PA_WO_TU0_BEGIN,									},
	{RA_TU0_BEGIN_0B,					PA_WO_TU0_BEGIN + 0xb,								},
	{RA_TU0_BEGIN_0F,					PA_WO_TU0_BEGIN + 0xf,								},
	{RA_PROC_TEX_M2REG_CONF_START_00,	PA_WO_PROC_TEX_M2REG_CONF_START,					},
	{RA_PROC_TEX_M2REG_CONF_START_01,	PA_WO_PROC_TEX_M2REG_CONF_START + 1,				},
	{RA_PROC_TEX_M2REG_CONF_START_02,	PA_WO_PROC_TEX_M2REG_CONF_START + 2,				},
	{RA_PROC_TEX_M2REG_CONF_START_03,	PA_WO_PROC_TEX_M2REG_CONF_START + 3,				},
	{RA_PROC_TEX_M2REG_CONF_START_04,	PA_WO_PROC_TEX_M2REG_CONF_START + 4,				},
	{RA_PROC_TEX_M2REG_CONF_START_05,	PA_WO_PROC_TEX_M2REG_CONF_START + 5,				},
	{RA_TB0_BU_ARG,						PA_WO_TB_STARTUNIT0 + PA_WO_TB_BU_ARG,				},
	{RA_TB0_BU_FILTER,					PA_WO_TB_STARTUNIT0 + PA_WO_TB_BU_FILTER,			},
	{RA_TB0_BU_OP,						PA_WO_TB_STARTUNIT0 + PA_WO_TB_BU_OP,				},
	{RA_TB0_BU_CONST_RGBA,				PA_WO_TB_STARTUNIT0 + PA_WO_TB_BU_CONST_RGBA,		},
	{RA_TB0_BU_SCALE,					PA_WO_TB_STARTUNIT0 + PA_WO_TB_BU_SCALE,			},
	{RA_TB1_BU_ARG,						PA_WO_TB_STARTUNIT1 + PA_WO_TB_BU_ARG,				},
	{RA_TB1_BU_FILTER,					PA_WO_TB_STARTUNIT1 + PA_WO_TB_BU_FILTER,			},
	{RA_TB1_BU_OP,						PA_WO_TB_STARTUNIT1 + PA_WO_TB_BU_OP,				},
	{RA_TB1_BU_CONST_RGBA,				PA_WO_TB_STARTUNIT1 + PA_WO_TB_BU_CONST_RGBA,		},
	{RA_TB1_BU_SCALE,					PA_WO_TB_STARTUNIT1 + PA_WO_TB_BU_SCALE,			},
	{RA_TB2_BU_ARG,						PA_WO_TB_STARTUNIT2 + PA_WO_TB_BU_ARG,				},
	{RA_TB2_BU_FILTER,					PA_WO_TB_STARTUNIT2 + PA_WO_TB_BU_FILTER,			},
	{RA_TB2_BU_OP,						PA_WO_TB_STARTUNIT2 + PA_WO_TB_BU_OP,				},
	{RA_TB2_BU_CONST_RGBA,				PA_WO_TB_STARTUNIT2 + PA_WO_TB_BU_CONST_RGBA,		},
	{RA_TB2_BU_SCALE,					PA_WO_TB_STARTUNIT2 + PA_WO_TB_BU_SCALE,			},
	{RA_TB3_BU_ARG,						PA_WO_TB_STARTUNIT3 + PA_WO_TB_BU_ARG,				},
	{RA_TB3_BU_FILTER,					PA_WO_TB_STARTUNIT3 + PA_WO_TB_BU_FILTER,			},
	{RA_TB3_BU_OP,						PA_WO_TB_STARTUNIT3 + PA_WO_TB_BU_OP,				},
	{RA_TB3_BU_CONST_RGBA,				PA_WO_TB_STARTUNIT3 + PA_WO_TB_BU_CONST_RGBA,		},
	{RA_TB3_BU_SCALE,					PA_WO_TB_STARTUNIT3 + PA_WO_TB_BU_SCALE,			},
	{RA_TB4_BU_ARG,						PA_WO_TB_STARTUNIT4 + PA_WO_TB_BU_ARG,				},
	{RA_TB4_BU_FILTER,					PA_WO_TB_STARTUNIT4 + PA_WO_TB_BU_FILTER,			},
	{RA_TB4_BU_OP,						PA_WO_TB_STARTUNIT4 + PA_WO_TB_BU_OP,				},
	{RA_TB4_BU_CONST_RGBA,				PA_WO_TB_STARTUNIT4 + PA_WO_TB_BU_CONST_RGBA,		},
	{RA_TB4_BU_SCALE,					PA_WO_TB_STARTUNIT4 + PA_WO_TB_BU_SCALE,			},
	{RA_TB5_BU_ARG,						PA_WO_TB_STARTUNIT5 + PA_WO_TB_BU_ARG,				},
	{RA_TB5_BU_FILTER,					PA_WO_TB_STARTUNIT5 + PA_WO_TB_BU_FILTER,			},
	{RA_TB5_BU_OP,						PA_WO_TB_STARTUNIT5 + PA_WO_TB_BU_OP,				},
	{RA_TB5_BU_CONST_RGBA,				PA_WO_TB_STARTUNIT5 + PA_WO_TB_BU_CONST_RGBA,		},
	{RA_TB5_BU_SCALE,					PA_WO_TB_STARTUNIT5 + PA_WO_TB_BU_SCALE,			},
	{RA_TB_BUFFER_COLOR,				PA_RW_TB_BU_CONST_RGBA2,							},
	{RA_TB_FOG_EN,						PA_WO_TB_FOG_EN,									},
	{RA_TB_FOG_RGB,						PA_WO_TB_FOG_RGB,									},
	{RA_TB_FOG_ATTN,					PA_WO_TB_FOG_ATTN,									},
	{RA_TB_GAS_ACC_MAX,					PA_WO_TB_GAS_ACC_MAX,								},
	{RA_GAS_LIGHT_XY,					PA_W0_GAS_LIGHT_XY,									},
	{RA_GAS_LIGHT_SC,					PA_W0_GAS_LIGHT_SC,									},
	{RA_GAS_LIGHT_DR,					PA_W0_GAS_LIGHT_DR,									},
	{RA_GAS_DELTAZ_DEPTH,				PA_W0_GAS_DELTAZ_DEPTH,								},
	{RA_CU_MODE,						PA_WO_CU_MODE,										},
	{RA_CU_ALPHA_T,						PA_WO_CU_ALPHA_T,									},
	{RA_CU_SDW_SC,						PA_WO_CU_SDW_SC,									},
	{RA_LR_L0_SPC0,						PA_WO_LR_L0_SPC0,									},
	{RA_LR_L0_SPC1,						PA_WO_LR_L0_SPC1,									},
	{RA_LR_L0_DIFF,						PA_WO_LR_L0_DIFF,									},
	{RA_LR_L0_AMB,						PA_WO_LR_L0_AMB,									},
	{RA_LR_L0_POS_XY,					PA_WO_LR_L0_POS_XY,									},
	{RA_LR_L0_POS_Z,					PA_WO_LR_L0_POS_Z,									},
	{RA_LR_L0_SPOT_DIREC_XY,			PA_WO_LR_L0_SPOT_DIREC_XY,							},
	{RA_LR_L0_SPOT_DIREC_Z,				PA_WO_LR_L0_SPOT_DIREC_Z,							},
	{RA_LR_L0_LIGHT_TYPE,				PA_WO_LR_L0_LIGHT_TYPE,								},
	{RA_LR_L0_BIAS_DA,					PA_WO_LR_L0_BIAS_DA,								},
	{RA_LR_L0_SCALE_DA,					PA_WO_LR_L0_SCALE_DA,								},
	{RA_LR_L1_SPC0,						PA_WO_LR_L1_SPC0,									},
	{RA_LR_L1_SPC1,						PA_WO_LR_L1_SPC1,									},
	{RA_LR_L1_DIFF,						PA_WO_LR_L1_DIFF,									},
	{RA_LR_L1_AMB,						PA_WO_LR_L1_AMB,									},
	{RA_LR_L1_POS_XY,					PA_WO_LR_L1_POS_XY,									},
	{RA_LR_L1_POS_Z,					PA_WO_LR_L1_POS_Z,									},
	{RA_LR_L1_SPOT_DIREC_XY,			PA_WO_LR_L1_SPOT_DIREC_XY,							},
	{RA_LR_L1_SPOT_DIREC_Z,				PA_WO_LR_L1_SPOT_DIREC_Z,							},
	{RA_LR_L1_LIGHT_TYPE,				PA_WO_LR_L1_LIGHT_TYPE,								},
	{RA_LR_L1_BIAS_DA,					PA_WO_LR_L1_BIAS_DA,								},
	{RA_LR_L1_SCALE_DA,					PA_WO_LR_L1_SCALE_DA,								},
	{RA_LR_L2_SPC0,						PA_WO_LR_L2_SPC0,									},
	{RA_LR_L2_SPC1,						PA_WO_LR_L2_SPC1,									},
	{RA_LR_L2_DIFF,						PA_WO_LR_L2_DIFF,									},
	{RA_LR_L2_AMB,						PA_WO_LR_L2_AMB,									},
	{RA_LR_L2_POS_XY,					PA_WO_LR_L2_POS_XY,									},
	{RA_LR_L2_POS_Z,					PA_WO_LR_L2_POS_Z,									},
	{RA_LR_L2_SPOT_DIREC_XY,			PA_WO_LR_L2_SPOT_DIREC_XY,							},
	{RA_LR_L2_SPOT_DIREC_Z,				PA_WO_LR_L2_SPOT_DIREC_Z,							},
	{RA_LR_L2_LIGHT_TYPE,				PA_WO_LR_L2_LIGHT_TYPE,								},
	{RA_LR_L2_BIAS_DA,					PA_WO_LR_L2_BIAS_DA,								},
	{RA_LR_L2_SCALE_DA,					PA_WO_LR_L2_SCALE_DA,								},
	{RA_LR_L3_SPC0,						PA_WO_LR_L3_SPC0,									},
	{RA_LR_L3_SPC1,						PA_WO_LR_L3_SPC1,									},
	{RA_LR_L3_DIFF,						PA_WO_LR_L3_DIFF,									},
	{RA_LR_L3_AMB,						PA_WO_LR_L3_AMB,									},
	{RA_LR_L3_POS_XY,					PA_WO_LR_L3_POS_XY,									},
	{RA_LR_L3_POS_Z,					PA_WO_LR_L3_POS_Z,									},
	{RA_LR_L3_SPOT_DIREC_XY,			PA_WO_LR_L3_SPOT_DIREC_XY,							},
	{RA_LR_L3_SPOT_DIREC_Z,				PA_WO_LR_L3_SPOT_DIREC_Z,							},
	{RA_LR_L3_LIGHT_TYPE,				PA_WO_LR_L3_LIGHT_TYPE,								},
	{RA_LR_L3_BIAS_DA,					PA_WO_LR_L3_BIAS_DA,								},
	{RA_LR_L3_SCALE_DA,					PA_WO_LR_L3_SCALE_DA,								},
	{RA_LR_L4_SPC0,						PA_WO_LR_L4_SPC0,									},
	{RA_LR_L4_SPC1,						PA_WO_LR_L4_SPC1,									},
	{RA_LR_L4_DIFF,						PA_WO_LR_L4_DIFF,									},
	{RA_LR_L4_AMB,						PA_WO_LR_L4_AMB,									},
	{RA_LR_L4_POS_XY,					PA_WO_LR_L4_POS_XY,									},
	{RA_LR_L4_POS_Z,					PA_WO_LR_L4_POS_Z,									},
	{RA_LR_L4_SPOT_DIREC_XY,			PA_WO_LR_L4_SPOT_DIREC_XY,							},
	{RA_LR_L4_SPOT_DIREC_Z,				PA_WO_LR_L4_SPOT_DIREC_Z,							},
	{RA_LR_L4_LIGHT_TYPE,				PA_WO_LR_L4_LIGHT_TYPE,								},
	{RA_LR_L4_BIAS_DA,					PA_WO_LR_L4_BIAS_DA,								},
	{RA_LR_L4_SCALE_DA,					PA_WO_LR_L4_SCALE_DA,								},
	{RA_LR_L5_SPC0,						PA_WO_LR_L5_SPC0,									},
	{RA_LR_L5_SPC1,						PA_WO_LR_L5_SPC1,									},
	{RA_LR_L5_DIFF,						PA_WO_LR_L5_DIFF,									},
	{RA_LR_L5_AMB,						PA_WO_LR_L5_AMB,									},
	{RA_LR_L5_POS_XY,					PA_WO_LR_L5_POS_XY,									},
	{RA_LR_L5_POS_Z,					PA_WO_LR_L5_POS_Z,									},
	{RA_LR_L5_SPOT_DIREC_XY,			PA_WO_LR_L5_SPOT_DIREC_XY,							},
	{RA_LR_L5_SPOT_DIREC_Z,				PA_WO_LR_L5_SPOT_DIREC_Z,							},
	{RA_LR_L5_LIGHT_TYPE,				PA_WO_LR_L5_LIGHT_TYPE,								},
	{RA_LR_L5_BIAS_DA,					PA_WO_LR_L5_BIAS_DA,								},
	{RA_LR_L5_SCALE_DA,					PA_WO_LR_L5_SCALE_DA,								},
	{RA_LR_L6_SPC0,						PA_WO_LR_L6_SPC0,									},
	{RA_LR_L6_SPC1,						PA_WO_LR_L6_SPC1,									},
	{RA_LR_L6_DIFF,						PA_WO_LR_L6_DIFF,									},
	{RA_LR_L6_AMB,						PA_WO_LR_L6_AMB,									},
	{RA_LR_L6_POS_XY,					PA_WO_LR_L6_POS_XY,									},
	{RA_LR_L6_POS_Z,					PA_WO_LR_L6_POS_Z,									},
	{RA_LR_L6_SPOT_DIREC_XY,			PA_WO_LR_L6_SPOT_DIREC_XY,							},
	{RA_LR_L6_SPOT_DIREC_Z,				PA_WO_LR_L6_SPOT_DIREC_Z,							},
	{RA_LR_L6_LIGHT_TYPE,				PA_WO_LR_L6_LIGHT_TYPE,								},
	{RA_LR_L6_BIAS_DA,					PA_WO_LR_L6_BIAS_DA,								},
	{RA_LR_L6_SCALE_DA,					PA_WO_LR_L6_SCALE_DA,								},
	{RA_LR_L7_SPC0,						PA_WO_LR_L7_SPC0,									},
	{RA_LR_L7_SPC1,						PA_WO_LR_L7_SPC1,									},
	{RA_LR_L7_DIFF,						PA_WO_LR_L7_DIFF,									},
	{RA_LR_L7_AMB,						PA_WO_LR_L7_AMB,									},
	{RA_LR_L7_POS_XY,					PA_WO_LR_L7_POS_XY,									},
	{RA_LR_L7_POS_Z,					PA_WO_LR_L7_POS_Z,									},
	{RA_LR_L7_SPOT_DIREC_XY,			PA_WO_LR_L7_SPOT_DIREC_XY,							},
	{RA_LR_L7_SPOT_DIREC_Z,				PA_WO_LR_L7_SPOT_DIREC_Z,							},
	{RA_LR_L7_LIGHT_TYPE,				PA_WO_LR_L7_LIGHT_TYPE,								},
	{RA_LR_L7_BIAS_DA,					PA_WO_LR_L7_BIAS_DA,								},
	{RA_LR_L7_SCALE_DA,					PA_WO_LR_L7_SCALE_DA,								},
	{RA_LR_CCONST,						PA_WO_LR_CCONST,									},
	{RA_LR_LIGHT_ENABLE,				PA_WO_LR_LIGHT_ENABLE,								},
	{RA_LR_FUNC_MODE,					PA_WO_LR_FUNC_MODE,									},
	{RA_LR_FUNC_MODE2,					PA_WO_LR_FUNC_MODE2,								},
	{RA_LR_MULTIPIPE,					PA_WO_LR_MULTIPIPE,									},
	{RA_LR_TABLE_SEL,					PA_WO_LR_TABLE_SEL,									},
	{RA_LR_SWT_CONF,					PA_WO_LR_SWT_CONF,									},
	{RA_LR_SHIFT_BIT,					PA_WO_LR_SHIFT_BIT,									},
	{RA_LR_LIGHTID,						PA_WO_LR_LIGHTID,									},
	{-1, 0}
};

static const unsigned s_be_mask[16] ={
	0x00000000, 0x000000ff, 0x0000ff00, 0x0000ffff, 0x00ff0000, 0x00ff00ff, 0x00ffff00, 0x00ffffff,
	0xff000000, 0xff0000ff, 0xff00ff00, 0xff00ffff, 0xffff0000, 0xffff00ff, 0xffffff00, 0xffffffff
};

static const unsigned s_layer_config_mask[8] ={
	(1 << MS_ID_RR)                                     | (1 << MS_ID_D0)                                     | (1 << MS_ID_SP),
	(1 << MS_ID_RR)                                                                         | (1 << MS_ID_FR) | (1 << MS_ID_SP),
	(1 << MS_ID_RR)                                     | (1 << MS_ID_D0) | (1 << MS_ID_D1)                                    ,
	                                                      (1 << MS_ID_D0) | (1 << MS_ID_D1) | (1 << MS_ID_FR)                  ,
	(1 << MS_ID_RR) | (1 << MS_ID_RG) | (1 << MS_ID_RB) | (1 << MS_ID_D0) | (1 << MS_ID_D1)                   | (1 << MS_ID_SP),
	(1 << MS_ID_RR) | (1 << MS_ID_RG) | (1 << MS_ID_RB) | (1 << MS_ID_D0)                   | (1 << MS_ID_FR) | (1 << MS_ID_SP),
	(1 << MS_ID_RR)                                     | (1 << MS_ID_D0) | (1 << MS_ID_D1) | (1 << MS_ID_FR) | (1 << MS_ID_SP),
	(1 << MS_ID_RR) | (1 << MS_ID_RG) | (1 << MS_ID_RB) | (1 << MS_ID_D0) | (1 << MS_ID_D1) | (1 << MS_ID_FR) | (1 << MS_ID_SP)
};

static int s_shaderprog_regs[] ={
	RA_CI_VP_MODE,
	RA_VP0_MODE,
	RA_VP0_PROGRAM_ADDRESS,
	RA_VP0_OUTPUT_MASK,
	RA_VP1_MODE,
	RA_VP1_PROGRAM_ADDRESS,
	RA_VP1_OUTPUT_MASK,
	RA_VC_ATTR_COUNT,
	RA_VC_SEND_MODE,
	RA_VC_SUBDIV_CONF,
	RA_LB_ATTR_COUNT,
	RA_SYNC_ATTR_COUNT,
	RA_RASIN_TRI_SETTING,
	RA_TS_WORD_NUM,
	RA_TS_VTXWORD1,
	RA_TS_VTXWORD2,
	RA_TS_VTXWORD3,
	RA_TS_VTXWORD4,
	RA_TS_VTXWORD5,
	RA_TS_VTXWORD6,
	RA_TS_VTXWORD7,
	RA_TI_TEX_DERIVS_ENABLE,
	RA_TI_CLOCK_ENABLE,
	RA_LAST
};

static int s_vsuniform_regs[] ={
	RA_VP1_BOOL_REG,
	RA_VP1_INT_REG_TOP_00,
	RA_VP1_INT_REG_TOP_01,
	RA_VP1_INT_REG_TOP_02,
	RA_VP1_INT_REG_TOP_03,
	RA_LAST
};

static int s_gsuniform_regs[] ={
	RA_VP0_BOOL_REG,
	RA_VP0_INT_REG_TOP_00,
	RA_VP0_INT_REG_TOP_01,
	RA_VP0_INT_REG_TOP_02,
	RA_VP0_INT_REG_TOP_03,
	RA_LAST
};

static int s_fsuniform_regs[] ={
	RA_TS_ZSCALE,
	RA_TS_ZBIAS,
	RA_TS_USRCLIPENABLE,
	RA_TS_USRCLIPA,
	RA_TS_USRCLIPB,
	RA_TS_USRCLIPC,
	RA_TS_USRCLIPD,
	RA_TI_Z_DIV_W,
	RA_TU0_BEGIN_00,
	RA_TU0_BEGIN_0B,
	RA_TU0_BEGIN_0F,
	RA_PROC_TEX_M2REG_CONF_START_00,
	RA_PROC_TEX_M2REG_CONF_START_01,
	RA_PROC_TEX_M2REG_CONF_START_02,
	RA_PROC_TEX_M2REG_CONF_START_03,
	RA_PROC_TEX_M2REG_CONF_START_04,
	RA_PROC_TEX_M2REG_CONF_START_05,
	RA_TB0_BU_ARG,
	RA_TB0_BU_FILTER,
	RA_TB0_BU_OP,
	RA_TB0_BU_CONST_RGBA,
	RA_TB0_BU_SCALE,
	RA_TB1_BU_ARG,
	RA_TB1_BU_FILTER,
	RA_TB1_BU_OP,
	RA_TB1_BU_CONST_RGBA,
	RA_TB1_BU_SCALE,
	RA_TB2_BU_ARG,
	RA_TB2_BU_FILTER,
	RA_TB2_BU_OP,
	RA_TB2_BU_CONST_RGBA,
	RA_TB2_BU_SCALE,
	RA_TB3_BU_ARG,
	RA_TB3_BU_FILTER,
	RA_TB3_BU_OP,
	RA_TB3_BU_CONST_RGBA,
	RA_TB3_BU_SCALE,
	RA_TB4_BU_ARG,
	RA_TB4_BU_FILTER,
	RA_TB4_BU_OP,
	RA_TB4_BU_CONST_RGBA,
	RA_TB4_BU_SCALE,
	RA_TB5_BU_ARG,
	RA_TB5_BU_FILTER,
	RA_TB5_BU_OP,
	RA_TB5_BU_CONST_RGBA,
	RA_TB5_BU_SCALE,
	RA_TB_BUFFER_COLOR,
	RA_TB_FOG_EN,
	RA_TB_FOG_RGB,
	RA_TB_FOG_ATTN,
	RA_TB_GAS_ACC_MAX,
	RA_GAS_LIGHT_XY,
	RA_GAS_LIGHT_SC,
	RA_GAS_LIGHT_DR,
	RA_GAS_DELTAZ_DEPTH,
	RA_CU_MODE,
	RA_CU_ALPHA_T,
	RA_CU_SDW_SC,
	RA_LR_L0_SPC0,
	RA_LR_L0_SPC1,
	RA_LR_L0_DIFF,
	RA_LR_L0_AMB,
	RA_LR_L0_POS_XY,
	RA_LR_L0_POS_Z,
	RA_LR_L0_SPOT_DIREC_XY,
	RA_LR_L0_SPOT_DIREC_Z,
	RA_LR_L0_LIGHT_TYPE,
	RA_LR_L0_BIAS_DA,
	RA_LR_L0_SCALE_DA,
	RA_LR_L1_SPC0,
	RA_LR_L1_SPC1,
	RA_LR_L1_DIFF,
	RA_LR_L1_AMB,
	RA_LR_L1_POS_XY,
	RA_LR_L1_POS_Z,
	RA_LR_L1_SPOT_DIREC_XY,
	RA_LR_L1_SPOT_DIREC_Z,
	RA_LR_L1_LIGHT_TYPE,
	RA_LR_L1_BIAS_DA,
	RA_LR_L1_SCALE_DA,
	RA_LR_L2_SPC0,
	RA_LR_L2_SPC1,
	RA_LR_L2_DIFF,
	RA_LR_L2_AMB,
	RA_LR_L2_POS_XY,
	RA_LR_L2_POS_Z,
	RA_LR_L2_SPOT_DIREC_XY,
	RA_LR_L2_SPOT_DIREC_Z,
	RA_LR_L2_LIGHT_TYPE,
	RA_LR_L2_BIAS_DA,
	RA_LR_L2_SCALE_DA,
	RA_LR_L3_SPC0,
	RA_LR_L3_SPC1,
	RA_LR_L3_DIFF,
	RA_LR_L3_AMB,
	RA_LR_L3_POS_XY,
	RA_LR_L3_POS_Z,
	RA_LR_L3_SPOT_DIREC_XY,
	RA_LR_L3_SPOT_DIREC_Z,
	RA_LR_L3_LIGHT_TYPE,
	RA_LR_L3_BIAS_DA,
	RA_LR_L3_SCALE_DA,
	RA_LR_L4_SPC0,
	RA_LR_L4_SPC1,
	RA_LR_L4_DIFF,
	RA_LR_L4_AMB,
	RA_LR_L4_POS_XY,
	RA_LR_L4_POS_Z,
	RA_LR_L4_SPOT_DIREC_XY,
	RA_LR_L4_SPOT_DIREC_Z,
	RA_LR_L4_LIGHT_TYPE,
	RA_LR_L4_BIAS_DA,
	RA_LR_L4_SCALE_DA,
	RA_LR_L5_SPC0,
	RA_LR_L5_SPC1,
	RA_LR_L5_DIFF,
	RA_LR_L5_AMB,
	RA_LR_L5_POS_XY,
	RA_LR_L5_POS_Z,
	RA_LR_L5_SPOT_DIREC_XY,
	RA_LR_L5_SPOT_DIREC_Z,
	RA_LR_L5_LIGHT_TYPE,
	RA_LR_L5_BIAS_DA,
	RA_LR_L5_SCALE_DA,
	RA_LR_L6_SPC0,
	RA_LR_L6_SPC1,
	RA_LR_L6_DIFF,
	RA_LR_L6_AMB,
	RA_LR_L6_POS_XY,
	RA_LR_L6_POS_Z,
	RA_LR_L6_SPOT_DIREC_XY,
	RA_LR_L6_SPOT_DIREC_Z,
	RA_LR_L6_LIGHT_TYPE,
	RA_LR_L6_BIAS_DA,
	RA_LR_L6_SCALE_DA,
	RA_LR_L7_SPC0,
	RA_LR_L7_SPC1,
	RA_LR_L7_DIFF,
	RA_LR_L7_AMB,
	RA_LR_L7_POS_XY,
	RA_LR_L7_POS_Z,
	RA_LR_L7_SPOT_DIREC_XY,
	RA_LR_L7_SPOT_DIREC_Z,
	RA_LR_L7_LIGHT_TYPE,
	RA_LR_L7_BIAS_DA,
	RA_LR_L7_SCALE_DA,
	RA_LR_CCONST,
	RA_LR_LIGHT_ENABLE,
	RA_LR_FUNC_MODE,
	RA_LR_FUNC_MODE2,
	RA_LR_MULTIPIPE,
	RA_LR_TABLE_SEL,
	RA_LR_SWT_CONF,
	RA_LR_SHIFT_BIT,
	RA_LR_LIGHTID,
	RA_LAST
};

static ras_reg_mask_t s_shaderprog_regmask;
static ras_reg_mask_t s_vsuniform_regmask;
static ras_reg_mask_t s_fsuniform_regmask;

#define __GET_SH_LIST(_id, shlist)	\
{	\
	for (shlist = __shman->sh_list_table[_id & SH_LIST_TABLE_SIZE]; shlist != 0; shlist = shlist->next)	\
	{	\
		if (shlist->id == _id)	\
			break;	\
	}	\
}

#define __GET_PG_LIST(_id, pglist)	\
{	\
	for (pglist = __shman->pg_list_table[_id & PG_LIST_TABLE_SIZE]; pglist != 0; pglist = pglist->next)	\
	{	\
		if (pglist->id == _id)	\
			break;	\
	}	\
}

static void shm_initializeProgram(pg_list_t* pglist);
static void shm_deleteShaderObject(GLuint shader);
static void shm_deleteProgramObject(GLuint program);
static void shm_deleteBinaryObject(bin_list_t* binlist);
static void shm_uniformfv(GLint location, const GLfloat* v, GLint size, GLint count, GLboolean transpose, GLboolean matrix);
static void shm_uniformiv(GLint location, const GLint* v, GLint size, GLint count);

int __shm_initializeShaderManager(shader_manager_t* shman){
	GLint ret;
	
	__shman = shman;
	{
		memset(__shman, 0, sizeof(shader_manager_t));
		ret = 0;
		__shminpgID = __shminshID = 1;
		s_vp_mode = -1;
		
		{
			int i;
			for (i = 0; s_fixed_uniform_table[i].index != -1; i++){
				s_fixed_uniforms[s_fixed_uniform_table[i].index].size = s_fixed_uniform_table[i].size;
				s_fixed_uniforms[s_fixed_uniform_table[i].index].type = s_fixed_uniform_table[i].type;
				s_fixed_uniforms[s_fixed_uniform_table[i].index].name = s_fixed_uniform_table[i].name;
			}
			for (i = 0; s_ras_addr_map_table[i].index != -1; i++){
				s_ras_addr_map[s_ras_addr_map_table[i].index] = s_ras_addr_map_table[i].address;
			}

			{
				unsigned u, u2, addr;
				
				#define SET_DEFAULT_REG(_addr, _data, _be) \
				{ \
					__shman->default_ras_regs[(_addr)] = (_data); \
					__shman->default_ras_regs_be[(_addr)] = _be; \
				}

				SET_DEFAULT_REG(RA_TU0_BEGIN_00, 0x00001000, 0x2);
				SET_DEFAULT_REG(RA_TU0_BEGIN_0B, 0x00000000, 0xf);
				SET_DEFAULT_REG(RA_TU0_BEGIN_0F, 0x00000000, 0xf);
				
				UTL_F2F_10M5E(0.5f, u2);
				u = ((u2 & 0xff) << 20) | (1 << 3) | 1;
				SET_DEFAULT_REG(RA_PROC_TEX_M2REG_CONF_START_00, u, 0xf);
				SET_DEFAULT_REG(RA_PROC_TEX_M2REG_CONF_START_01, 0, 0xf);
				SET_DEFAULT_REG(RA_PROC_TEX_M2REG_CONF_START_02, 0, 0xf);
				SET_DEFAULT_REG(RA_PROC_TEX_M2REG_CONF_START_03, 0, 0xf);
				u = ((u2 >> 8) << 19) | (6 << 7) | 1;
				SET_DEFAULT_REG(RA_PROC_TEX_M2REG_CONF_START_04, u, 0xf);
				SET_DEFAULT_REG(RA_PROC_TEX_M2REG_CONF_START_05, 0xe0c08000, 0xf);
				
				UTL_F2F_10M5E(1.f, u);
				SET_DEFAULT_REG(RA_CU_SDW_SC, u, 0xf);
				SET_DEFAULT_REG(RA_TS_WORD_NUM, 0, 0xf);
				SET_DEFAULT_REG(RA_TS_VTXWORD1, 0x1f1f1f1f, 0xf);
				SET_DEFAULT_REG(RA_TS_VTXWORD2, 0x1f1f1f1f, 0xf);
				SET_DEFAULT_REG(RA_TS_VTXWORD3, 0x1f1f1f1f, 0xf);
				SET_DEFAULT_REG(RA_TS_VTXWORD4, 0x1f1f1f1f, 0xf);
				SET_DEFAULT_REG(RA_TS_VTXWORD5, 0x1f1f1f1f, 0xf);
				SET_DEFAULT_REG(RA_TS_VTXWORD6, 0x1f1f1f1f, 0xf);
				SET_DEFAULT_REG(RA_TS_VTXWORD7, 0x1f1f1f1f, 0xf);
				SET_DEFAULT_REG(RA_TI_Z_DIV_W, 1, 0xf);
				SET_DEFAULT_REG(RA_TS_ZSCALE, 0xbf0000, 0xf);
				SET_DEFAULT_REG(RA_TS_ZBIAS, 0, 0xf);
				SET_DEFAULT_REG(RA_TS_USRCLIPENABLE, 0, 0xf);
				SET_DEFAULT_REG(RA_TS_USRCLIPA, 0, 0xf);
				SET_DEFAULT_REG(RA_TS_USRCLIPB, 0, 0xf);
				SET_DEFAULT_REG(RA_TS_USRCLIPC, 0, 0xf);
				SET_DEFAULT_REG(RA_TS_USRCLIPD, 0, 0xf);
				
				for (addr = RA_LR_L0_SPC0; addr <= RA_LR_L7_SCALE_DA; addr++){
					switch (addr){
						case RA_LR_L0_SCALE_DA: case RA_LR_L1_SCALE_DA: case RA_LR_L2_SCALE_DA: case RA_LR_L3_SCALE_DA:
						case RA_LR_L4_SCALE_DA: case RA_LR_L5_SCALE_DA: case RA_LR_L6_SCALE_DA: case RA_LR_L7_SCALE_DA:
							UTL_F2F_12M_7E(1.f, u);
							SET_DEFAULT_REG(addr, u, 0xf);
							break;
						case RA_LR_L0_POS_Z: case RA_LR_L1_POS_Z: case RA_LR_L2_POS_Z: case RA_LR_L3_POS_Z:
						case RA_LR_L4_POS_Z: case RA_LR_L5_POS_Z: case RA_LR_L6_POS_Z: case RA_LR_L7_POS_Z:
							UTL_F2F_10M5E(1.f, u);
							SET_DEFAULT_REG(addr, u, 0xf);
							break;
						case RA_LR_L0_SPOT_DIREC_Z: case RA_LR_L1_SPOT_DIREC_Z: case RA_LR_L2_SPOT_DIREC_Z: case RA_LR_L3_SPOT_DIREC_Z:
						case RA_LR_L4_SPOT_DIREC_Z: case RA_LR_L5_SPOT_DIREC_Z: case RA_LR_L6_SPOT_DIREC_Z: case RA_LR_L7_SPOT_DIREC_Z:
							UTL_F2FX_13W_2I_T(1.f, u);
							SET_DEFAULT_REG(addr, u, 0xf);
							break;
						case RA_LR_L0_LIGHT_TYPE: case RA_LR_L1_LIGHT_TYPE: case RA_LR_L2_LIGHT_TYPE: case RA_LR_L3_LIGHT_TYPE:
						case RA_LR_L4_LIGHT_TYPE: case RA_LR_L5_LIGHT_TYPE: case RA_LR_L6_LIGHT_TYPE: case RA_LR_L7_LIGHT_TYPE:
							SET_DEFAULT_REG(addr, 1, 0xf);
							break;
						default:
							SET_DEFAULT_REG(addr, 0, 0xf);
							break;
					}
				}
				
				u = UTL_F2C_8(0.8f) | (UTL_F2C_8(0.8f) << 10) | (UTL_F2C_8(0.8f) << 20);
				SET_DEFAULT_REG(RA_LR_L0_DIFF, u, 0xf);
				u = UTL_F2C_8(0.2f * 0.2f) | (UTL_F2C_8(0.2f * 0.2f) << 10) | (UTL_F2C_8(0.2f * 0.2f) << 20);
				SET_DEFAULT_REG(RA_LR_CCONST, u, 0xf);
				SET_DEFAULT_REG(RA_LR_LIGHT_ENABLE, 0, 0xf);
				SET_DEFAULT_REG(RA_LR_MULTIPIPE, 1, 0xf);
				SET_DEFAULT_REG(RA_LR_FUNC_MODE, 0x88000400, 0xf);
				SET_DEFAULT_REG(RA_LR_FUNC_MODE2, 0xffffffff, 0xf);
				SET_DEFAULT_REG(RA_LR_SWT_CONF, 0, 0xf);
				SET_DEFAULT_REG(RA_LR_SHIFT_BIT, 0, 0xf);
				SET_DEFAULT_REG(RA_LR_TABLE_SEL, 0x2222222, 0xf);
				SET_DEFAULT_REG(RA_LR_LIGHTID, 0, 0xf);
				
				SET_DEFAULT_REG(RA_TB_FOG_EN, 0, 0xf);
				SET_DEFAULT_REG(RA_TB_FOG_RGB, 0, 0xf);
				UTL_F2F_10M5E(1.f, u);
				SET_DEFAULT_REG(RA_TB_FOG_ATTN, u, 0xf);
				
				SET_DEFAULT_REG(RA_GAS_LIGHT_XY, 0, 0xf);
				SET_DEFAULT_REG(RA_GAS_LIGHT_SC, 0, 0xf);
				SET_DEFAULT_REG(RA_GAS_LIGHT_DR, 0x100, 0xf);
				UTL_F2UFX_24W_16I(10.f, u);
				SET_DEFAULT_REG(RA_GAS_DELTAZ_DEPTH, u, 0x7);
				UTL_F2F_10M5E(1.f, u);
				SET_DEFAULT_REG(RA_TB_GAS_ACC_MAX, u, 0xf);
				
				SET_DEFAULT_REG(RA_CU_MODE, 0xE40100, 0xd);
				SET_DEFAULT_REG(RA_CU_ALPHA_T, 0x10, 0xf);
				
				for (addr = RA_TB0_BU_ARG; addr <= RA_TB5_BU_SCALE; addr++){
					if (addr == RA_TB0_BU_ARG)
						SET_DEFAULT_REG(addr, 0x0eee0eee, 0xf)
					else if (addr == RA_TB1_BU_ARG || addr == RA_TB2_BU_ARG || addr == RA_TB3_BU_ARG || addr == RA_TB4_BU_ARG || addr == RA_TB5_BU_ARG)
						SET_DEFAULT_REG(addr, 0x0fff0fff, 0xf)
					else
						SET_DEFAULT_REG(addr, 0x0, 0xf);
				}
				SET_DEFAULT_REG(RA_TB_BUFFER_COLOR, 0, 0xf);
				memcpy(__shman->ras_regs, __shman->default_ras_regs, sizeof(unsigned) * RA_LAST);
				memcpy(__shman->ras_regs_be, __shman->default_ras_regs_be, sizeof(unsigned char) * RA_LAST);
			}

			CLEAR_ALL_RASREGMASK(s_shaderprog_regmask);
			for (i = 0; s_shaderprog_regs[i] != RA_LAST; i++){
				SET_RASREGMASK(s_shaderprog_regmask, s_shaderprog_regs[i]);
			}
			
			CLEAR_ALL_RASREGMASK(s_vsuniform_regmask);
			for (i = 0; s_vsuniform_regs[i] != RA_LAST; i++){
				SET_RASREGMASK(s_vsuniform_regmask, s_vsuniform_regs[i]);
			}
			for (i = 0; s_gsuniform_regs[i] != RA_LAST; i++){
				SET_RASREGMASK(s_vsuniform_regmask, s_gsuniform_regs[i]);
			}
			
			CLEAR_ALL_RASREGMASK(s_fsuniform_regmask);
			for (i = 0; s_fsuniform_regs[i] != RA_LAST; i++){
				SET_RASREGMASK(s_fsuniform_regmask, s_fsuniform_regs[i]);
			}
		}
	}
	
	return ret;
}

void __shm_finalizeShaderManager(){
	if (__shman)
	{
		unsigned i;

		{
			bin_list_t* binlist;
			bin_list_t* next;
			
			for (binlist = __shman->bin_list_top; binlist != 0; binlist = next){
				next = binlist->next;
				if (binlist->program)
					free(binlist->program);
				if (binlist->swizzle)
					free(binlist->swizzle);
				for (i = 0; i < binlist->num_exe_infos; i++){
					if (binlist->exe_infos[i].float_consts)
						free(binlist->exe_infos[i].float_consts);
					if (binlist->exe_infos[i].strings)
						free(binlist->exe_infos[i].strings);
					if (binlist->exe_infos[i].uniforms)
						free(binlist->exe_infos[i].uniforms);
				}
				if (binlist->exe_infos)
					free(binlist->exe_infos);
				free(binlist);
			}
		}

		for (i = 0; i < SH_LIST_TABLE_SIZE + 1; i++){
			sh_list_t* shlist;
			sh_list_t* next;
			for (shlist = __shman->sh_list_table[i]; shlist != 0; shlist = next)
			{
				next = shlist->next;
				free(shlist);
			}
		}

		for (i = 0; i < PG_LIST_TABLE_SIZE + 1; i++){
			pg_list_t* pglist;
			pg_list_t* next;
			for (pglist = __shman->pg_list_table[i]; pglist != 0; pglist = next){
				next = pglist->next;
				if (pglist->vs_floats)
					free(pglist->vs_floats);
				if (pglist->gs_floats)
					free(pglist->gs_floats);
				if (pglist->uni_location_map)
					free(pglist->uni_location_map);
				if (pglist->attrib_location){
					attrib_location_t* attribloc;
					attrib_location_t* attrnext;
					for (attribloc = pglist->attrib_location; attribloc != 0; attribloc = attrnext){
						attrnext = attribloc->next;
						if (attribloc->name)
							free(attribloc->name);
						free(attribloc);
					}
				}
				free(pglist);
			}
		}
		
		__shman = 0;
	}
	
	return;
}

static void shm_initializeProgram(pg_list_t* pglist){
	int i;
	
	memset(&pglist->fragment_state, 0, sizeof(fragment_state_t));

	pglist->fragment_state.lighting.use_const_specular2 = GL_TRUE;
	for (i = 0; i < 6; i++)
		pglist->fragment_state.lighting.samplers[i] = -1;
	pglist->fragment_state.lighting.sampler_mask = s_layer_config_mask[0];
	for (i = 0; i < 3; i++)
		pglist->fragment_state.lighting.ambient[i] = 0.2f;
	pglist->fragment_state.lighting.ambient[3] = 1.0f;
	
	for (i = 0; i < 4; i++){
		pglist->fragment_state.lights[0].diffuse[i] = 1.0f;
		pglist->fragment_state.lights[0].specular[i] = 1.0f;
	}
	for (i = 0; i < 8; i++){
		pglist->fragment_state.lights[i].position[2] = 1.0f;
		pglist->fragment_state.lights[i].spot_dir[2] = -1.0f;
		pglist->fragment_state.lights[i].spot_sampler = -1;
		pglist->fragment_state.lights[i].da_sampler = -1;
		pglist->fragment_state.lights[i].da_scale = 1.f;
	}
	
	for (i = 0; i < 3; i++){
		pglist->fragment_state.material.ambient[i] = 0.2f;
		pglist->fragment_state.material.diffuse[i] = 0.8f;
	}
	pglist->fragment_state.material.ambient[3] = 1.0f;
	pglist->fragment_state.material.diffuse[3] = 1.0f;
	pglist->fragment_state.material.specular[3] = 1.0f;
	pglist->fragment_state.material.specular2[3] = 1.0f;
	pglist->fragment_state.material.emission[3] = 1.f;
	
	for (i = 0; i < 7; i++)
		pglist->fragment_state.proctex.samplers[i] = -1;
	pglist->fragment_state.proctex.bias = 0.5f;
	
	pglist->fragment_state.operation.mode = GL_FRAGOP_MODE_GL_DMP;
	pglist->fragment_state.operation.penumbra_bias = 1.f;
	
	pglist->fragment_state.fog.sampler = -1;
	
	pglist->fragment_state.gas.autoacc = GL_TRUE;
	for (i = 0; i < 3; i++)
		pglist->fragment_state.gas.samplers[i] = -1;
	pglist->fragment_state.gas.accmax = 1.f;
	pglist->fragment_state.gas.attenuation = 1.f;
	pglist->fragment_state.gas.deltaz = 10.f;
	
	return;
}

GLuint GL_APIENTRY glCreateProgram(){
	pg_list_t* pglist;
	pg_list_t* newlist;
	pg_list_t* tmp;
	GLuint id = 0;
	
	BASE_GL_FAIL_IF_RET(__shminpgID > MAX_SHM_IMPL_PROGRAMS, GL_INVALID_OPERATION, 0);
	
	newlist = (pg_list_t*)malloc(sizeof(pg_list_t));
	BASE_GL_FAIL_IF_RET(!newlist, GL_OUT_OF_MEMORY, 0);

	memset(newlist, 0, sizeof(pg_list_t));
	
	id = __shminpgID;
	for (;; id++){
		if (__shman->pg_list_table[id & PG_LIST_TABLE_SIZE] == 0){
			newlist->id = id;
			newlist->next = 0;
			__shman->pg_list_table[id & PG_LIST_TABLE_SIZE] = newlist;
			break;
		}
		else if (__shman->pg_list_table[id & PG_LIST_TABLE_SIZE]->id == id){
			continue;
		}
		else if (__shman->pg_list_table[id & PG_LIST_TABLE_SIZE]->id > id){
			newlist->id = id;
			newlist->next = __shman->pg_list_table[id & PG_LIST_TABLE_SIZE];
			__shman->pg_list_table[id & PG_LIST_TABLE_SIZE] = newlist;
			break;
		}
		else{
			int bCont, bBreak;
			bCont = bBreak = 0;
			for (pglist = __shman->pg_list_table[id & PG_LIST_TABLE_SIZE]->next, tmp = __shman->pg_list_table[id & PG_LIST_TABLE_SIZE]; pglist != 0; pglist = pglist->next){
				if (pglist->id == id){
					bCont = 1;
					break;
				}
				else if (pglist->id > id){
					newlist->id = id;
					tmp->next = newlist;
					newlist->next = pglist;
					bBreak = 1;
					break;
				}
				tmp = pglist;
			}
			if (bBreak)
				break;
			if (bCont)
				continue;
			
			if (!pglist){
				tmp->next = newlist;
				newlist->id = id;
				newlist->next = 0;
				break;
			}
		}
	}
	__shminpgID = id + 1;
	
	return id;
}

GLuint GL_APIENTRY glCreateShader(GLenum type){
	sh_list_t* shlist;
	sh_list_t* newlist;
	sh_list_t* tmp;
	GLuint id = 0;
	
	BASE_GL_FAIL_IF_RET(type != GL_VERTEX_SHADER && type != GL_GEOMETRY_SHADER_DMP, GL_INVALID_ENUM, 0);
	newlist = (sh_list_t*)malloc(sizeof(sh_list_t));
	BASE_GL_FAIL_IF_RET(!newlist, GL_OUT_OF_MEMORY, 0);

	memset(newlist, 0, sizeof(sh_list_t));
	newlist->type = type;
	
	id = __shminshID;
	for (;; id++){
		if (__shman->sh_list_table[id & SH_LIST_TABLE_SIZE] == 0){
			newlist->id = id;
			newlist->next = 0;
			__shman->sh_list_table[id & SH_LIST_TABLE_SIZE] = newlist;
			break;
		}
		else if (__shman->sh_list_table[id & SH_LIST_TABLE_SIZE]->id == id){
			continue;
		}
		else if (__shman->sh_list_table[id & SH_LIST_TABLE_SIZE]->id > id){
			newlist->id = id;
			newlist->next = __shman->sh_list_table[id & SH_LIST_TABLE_SIZE];
			__shman->sh_list_table[id & SH_LIST_TABLE_SIZE] = newlist;
			break;
		}
		else{
			int bCont, bBreak;
			bCont = bBreak = 0;
			for (shlist = __shman->sh_list_table[id & SH_LIST_TABLE_SIZE]->next, tmp = __shman->sh_list_table[id & SH_LIST_TABLE_SIZE]; shlist != 0; shlist = shlist->next){
				if (shlist->id == id){
					bCont = 1;
					break;
				}
				else if (shlist->id > id){
					newlist->id = id;
					tmp->next = newlist;
					newlist->next = shlist;
					bBreak = 1;
					break;
				}
				tmp = shlist;
			}
			if (bBreak)
				break;
			if (bCont)
				continue;
			
			if (!shlist){
				tmp->next = newlist;
				newlist->id = id;
				newlist->next = 0;
				break;
			}
		}
	}
	__shminshID = id + 1;
	
	return id;
}

void GL_APIENTRY glShaderBinary(GLint n, const GLuint *shaders, GLenum binaryformat, const void *binary, GLint length){
	int error;
	bin_header_t* bin_header;
	pkg_header_t* pkg_header;
	bin_list_t* binary_list;
	
	BASE_GL_FAIL_IF(n < 0 || length < 0, GL_INVALID_VALUE);
	BASE_GL_FAIL_IF(binaryformat != GL_PLATFORM_BINARY_DMP, GL_INVALID_ENUM);
	
	bin_header = (bin_header_t*)binary;
	BASE_GL_FAIL_IF(bin_header->signature != 0x424C5644 /* "DVLB" */, GL_INVALID_VALUE);

	pkg_header = (pkg_header_t*)((unsigned*)binary + 2 + bin_header->exe_number);	/* get package information header */
	BASE_GL_FAIL_IF(pkg_header->signature != 0x504C5644 /* "DVLP" */, GL_INVALID_VALUE);

	{
		int filelen;
		exe_header_t* exe_header;
		unsigned* image_offsets;
		BASE_GL_FAIL_IF(!bin_header->exe_number || length < (GLint)(8 + 4 * bin_header->exe_number), GL_INVALID_OPERATION);
		
		image_offsets = (unsigned*)&bin_header->exe_offset_top;
		
		BASE_GL_FAIL_IF(length < (GLint)(image_offsets[bin_header->exe_number - 1] + sizeof(exe_header_t)), GL_INVALID_OPERATION);
		
		exe_header = (exe_header_t*)((unsigned char*)binary + image_offsets[bin_header->exe_number - 1]);
		filelen = exe_header->string_offset + exe_header->string_size + image_offsets[bin_header->exe_number - 1];
		BASE_GL_FAIL_IF(length < filelen, GL_INVALID_OPERATION);
	}
	
	do
	{
		unsigned i;
		
		error = 1;

		binary_list = (bin_list_t*)malloc(sizeof(bin_list_t));
		if (!binary_list)
			break;
		memset(binary_list, 0, sizeof(bin_list_t));
		binary_list->program = (unsigned*)malloc(sizeof(obj_inst_t) * pkg_header->inst_count);
		binary_list->num_program = pkg_header->inst_count;
		binary_list->swizzle = (unsigned*)malloc(sizeof(unsigned) * pkg_header->swizzle_count);
		binary_list->num_swizzle = pkg_header->swizzle_count;
		if (!binary_list->program || !binary_list->swizzle)
			break;
		else{
			obj_inst_t* inst;
			obj_swz_t* swizzle;

			inst = (obj_inst_t*)((unsigned char*)pkg_header + pkg_header->inst_offset);
			for (i = 0; i < pkg_header->inst_count; i++){
				binary_list->program[i] = *inst;
				inst++;
			}

			swizzle = (obj_swz_t*)((unsigned char*)pkg_header + pkg_header->swizzle_offset);
			for (i = 0; i < pkg_header->swizzle_count; i++){
				binary_list->swizzle[i] = swizzle->value;
				swizzle++;
			}
		}

		binary_list->exe_infos = (exe_info_t*)malloc(sizeof(exe_info_t) * bin_header->exe_number);
		binary_list->num_exe_infos = bin_header->exe_number;
		if (!binary_list->exe_infos)
			break;
		else{
			unsigned* image_offsets;
			
			memset(binary_list->exe_infos, 0, sizeof(exe_info_t) * bin_header->exe_number);
			image_offsets = (unsigned*)&bin_header->exe_offset_top;
			for (i = 0; i < bin_header->exe_number; i++){
				unsigned j;
				obj_setup_t* setup;
				obj_outmap_t* outmap;
				obj_bsym_t* symbol;
				exe_header_t* exe_header = (exe_header_t*)((unsigned char*)binary + image_offsets[i]);

				binary_list->exe_infos[i].shader_type = exe_header->shader_type;
				binary_list->exe_infos[i].merge_output_maps = exe_header->merge_output_maps & 1;
				binary_list->exe_infos[i].geometry_data_mode = exe_header->geometry_data_mode;
				binary_list->exe_infos[i].start_index = exe_header->start_index;
				binary_list->exe_infos[i].variable_sized_primitive_size = exe_header->variable_sized_primitive_size;
				binary_list->exe_infos[i].fixed_sized_primitive_size = exe_header->fixed_sized_primitive_size;
				binary_list->exe_infos[i].input_mask = exe_header->input_mask;
				binary_list->exe_infos[i].output_mask = exe_header->output_mask;
				binary_list->exe_infos[i].main_addr = exe_header->main_addr;
				binary_list->exe_infos[i].end_addr = exe_header->end_addr;

				setup = (obj_setup_t*)((unsigned char*)exe_header + exe_header->setup_offset);
				for (j = 0; j < exe_header->setup_count; j++){
					if (setup[j].type == OBJ_SETUP_TYPE_CONST)
						binary_list->exe_infos[i].num_float_const++;
				}
				if (binary_list->exe_infos[i].num_float_const){
					binary_list->exe_infos[i].float_consts = (float_const_t*)malloc(sizeof(float_const_t) * binary_list->exe_infos[i].num_float_const);
					if (!binary_list->exe_infos[i].float_consts)
						break;
					binary_list->exe_infos[i].num_float_const = 0;
				}
				for (j = 0; j < exe_header->setup_count; j++){
					switch (setup[j].type){
						case OBJ_SETUP_TYPE_BOOL:
							if (setup[j].value[0])
								binary_list->exe_infos[i].bool_consts |= 1 << setup[j].index;
							break;
						case OBJ_SETUP_TYPE_INT:
							if (setup[j].index < 4)
								binary_list->exe_infos[i].int_consts[setup[j].index] = setup[j].value[0];
							break;
						case OBJ_SETUP_TYPE_CONST:
							binary_list->exe_infos[i].float_consts[binary_list->exe_infos[i].num_float_const].index = setup[j].index;
							binary_list->exe_infos[i].float_consts[binary_list->exe_infos[i].num_float_const].value[0] = (setup[j].value[3] << 8) | (setup[j].value[2] >> 16);
							binary_list->exe_infos[i].float_consts[binary_list->exe_infos[i].num_float_const].value[1] = (setup[j].value[2] << 16) | (setup[j].value[1] >> 8);
							binary_list->exe_infos[i].float_consts[binary_list->exe_infos[i].num_float_const].value[2] = (setup[j].value[1] << 24) | setup[j].value[0];
							binary_list->exe_infos[i].num_float_const++;
							break;
					}
				}

				binary_list->exe_infos[i].num_input = 0;
				for (j = 0; j < 16; j++){
					if (exe_header->input_mask & (1 << j))
						binary_list->exe_infos[i].num_input++;
				}

				binary_list->exe_infos[i].num_output = 0;
				for (j = 0; j < 16; j++){
					if (exe_header->output_mask & (1 << j))
						binary_list->exe_infos[i].num_output++;
				}

				outmap = (obj_outmap_t*)((unsigned char*)exe_header + exe_header->outmap_offset);{
					unsigned non_gen_output_mask = exe_header->output_mask;	/* non generic output mask */
					
					for (j = 0; j < exe_header->outmap_count; j++){
						if (outmap[j].type == OBJ_OUTMAP_TYPE_GEN)
							non_gen_output_mask &= ~(1 << outmap[j].index);
					}
					for (j = 0; j < 7; j++)
						binary_list->exe_infos[i].output_format[j] = 0x1f1f1f1f;
					for (j = 0; j < exe_header->outmap_count; j++){
						int k;
						int index = 0;
						int component = 0;
						unsigned ts_param = 0;
						int err = 0;
						
						switch (outmap[j].type){
							case OBJ_OUTMAP_TYPE_POS:	ts_param = PK_X;	break;
							case OBJ_OUTMAP_TYPE_QC:	ts_param = PK_QC0;	break;
							case OBJ_OUTMAP_TYPE_COLOR:	ts_param = PK_R;	break;
							case OBJ_OUTMAP_TYPE_TEX0W:	ts_param = PK_TW;	break;
							case OBJ_OUTMAP_TYPE_VIEW:	ts_param = PK_VX;	break;
							case OBJ_OUTMAP_TYPE_GEN:	continue;
							case OBJ_OUTMAP_TYPE_TEX0:	ts_param = PK_U0;	break;
							case OBJ_OUTMAP_TYPE_TEX1:	ts_param = PK_U1;	break;
							case OBJ_OUTMAP_TYPE_TEX2:	ts_param = PK_U2;	break;
							default:
								err = 1;
								break;
						}
						BASE_GL_FAIL_IF_BRK(err, GL_INVALID_OPERATION);
						
						for (k = 0; k != outmap[j].index; k++){
							if ((non_gen_output_mask >> k) & 1)
								index++;
						}
						BASE_GL_FAIL_IF_BRK(index >= 7, GL_INVALID_OPERATION);

						for (k = 0; k < 4; k++){
							if (outmap[j].mask & (1 << k)){
								binary_list->exe_infos[i].output_format[index] &= ~(0xff << (8 * k));
								binary_list->exe_infos[i].output_format[index] |= ts_param << (8 * k);
								component++;
								switch (ts_param){
									case PK_Z:	binary_list->exe_infos[i].clock_enable |= 0x00000001; break;
									case PK_R:	binary_list->exe_infos[i].clock_enable |= 0x00000002; break;
									case PK_U0:	binary_list->exe_infos[i].clock_enable |= 0x00000100; break;
									case PK_U1:	binary_list->exe_infos[i].clock_enable |= 0x00000200; break;
									case PK_U2:	binary_list->exe_infos[i].clock_enable |= 0x00000400; break;
									case PK_TW:	binary_list->exe_infos[i].clock_enable |= 0x00010000; break;
									case PK_VX:	binary_list->exe_infos[i].clock_enable |= 0x01000000; break;
									case PK_QC0:binary_list->exe_infos[i].clock_enable |= 0x01000000; break;
								}
								ts_param++;
							}

							switch (outmap[j].type){
								case OBJ_OUTMAP_TYPE_TEX0W:
									if (component == 1)
										k = 4;
									break;
								case OBJ_OUTMAP_TYPE_TEX0:
								case OBJ_OUTMAP_TYPE_TEX1:
								case OBJ_OUTMAP_TYPE_TEX2:
									if (component == 2)
										k = 4;
									break;
								case OBJ_OUTMAP_TYPE_VIEW:
									if (component == 3)
										k = 4;
									break;
							}
						}
					}
					if (j != exe_header->outmap_count)
						break;
				}
				
				binary_list->exe_infos[i].strings = (char*)malloc(exe_header->string_size);
				if (!binary_list->exe_infos[i].strings)
					break;
				memcpy(binary_list->exe_infos[i].strings, (char*)exe_header + exe_header->string_offset, exe_header->string_size);
				binary_list->exe_infos[i].string_size = exe_header->string_size;
				
				symbol = (obj_bsym_t*)((unsigned char*)exe_header + exe_header->bsym_offset);
				for (j = 0; j < exe_header->bsym_count; j++){
					if (symbol[j].start_index >= FLOAT_UNIFORM_OFFSET)
						binary_list->exe_infos[i].num_uniform++;
				}
				
				if (binary_list->exe_infos[i].num_uniform){
					binary_list->exe_infos[i].uniforms = (uniform_info_t*)malloc(sizeof(uniform_info_t) * binary_list->exe_infos[i].num_uniform);
					if (!binary_list->exe_infos[i].uniforms)
						break;
					binary_list->exe_infos[i].num_uniform = 0;
				}
				for (j = 0; j < exe_header->bsym_count; j++){
					int pos;
					int dotpos = -1;
					int compnum = 0;
					char* symname = (binary_list->exe_infos[i].strings + symbol[j].name_index);
					
					/* check the component size */
					for (pos = 0; symname[pos] != 0; pos++){
						if (symname[pos] == '.'){
							dotpos = pos;
							continue;
						}
						if (dotpos != -1){
							if (symname[pos] == 'x' || symname[pos] == 'y' || symname[pos] == 'z' || symname[pos] == 'w')
								compnum++;
							else{
								dotpos = -1;
								compnum = 0;
							}
						}
					}
					if (compnum == 0)
						compnum = 4;
					
					if (symbol[j].end_index < FLOAT_UNIFORM_OFFSET){
						switch (symbol[j].end_index - symbol[j].start_index){
							case 0:
								switch (compnum){
									case 1: binary_list->exe_infos[i].attribs[symbol[j].start_index].type = GL_FLOAT; break;
									case 2: binary_list->exe_infos[i].attribs[symbol[j].start_index].type = GL_FLOAT_VEC2; break;
									case 3: binary_list->exe_infos[i].attribs[symbol[j].start_index].type = GL_FLOAT_VEC3; break;
									case 4: binary_list->exe_infos[i].attribs[symbol[j].start_index].type = GL_FLOAT_VEC4; break;
								}
								break;
							case 1: binary_list->exe_infos[i].attribs[symbol[j].start_index].type = GL_FLOAT_MAT2; break;
							case 2: binary_list->exe_infos[i].attribs[symbol[j].start_index].type = GL_FLOAT_MAT3; break;
							case 3: binary_list->exe_infos[i].attribs[symbol[j].start_index].type = GL_FLOAT_MAT4; break;
						}
						binary_list->exe_infos[i].attribs[symbol[j].start_index].name_index = symbol[j].name_index;
						if (dotpos != -1)
							symname[dotpos] = 0;
					}
					else if (symbol[j].end_index < LOOP_UNIFORM_OFFSET){
						int bound_reg_num = symbol[j].end_index - symbol[j].start_index + 1;
						
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].size = bound_reg_num;
						switch (compnum){
							case 1:
								binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].type = GL_FLOAT;
								break;
							case 2:
								if ((bound_reg_num % 2) == 0)
									binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].type = GL_FLOAT_MAT2;
								else
									binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].type = GL_FLOAT_VEC2;
								break;
							case 3:
								if ((bound_reg_num % 3) == 0)
									binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].type = GL_FLOAT_MAT3;
								else
									binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].type = GL_FLOAT_VEC3;
								break;
							case 4:
								if ((bound_reg_num % 4) == 0)
									binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].type = GL_FLOAT_MAT4;
								else
									binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].type = GL_FLOAT_VEC4;
								break;
							default:
								break;
						}
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].addr = symbol[j].start_index - FLOAT_UNIFORM_OFFSET;
						if (dotpos != -1){
							switch (symname[dotpos + 1]){
								case 'x': binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].index = 0; break;
								case 'y': binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].index = 1; break;
								case 'z': binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].index = 2; break;
								case 'w': binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].index = 3; break;
								default:  binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].index = 0; break;
							}
							symname[dotpos] = 0;
						}
						else
							binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].index = 0;
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].name_index = symbol[j].name_index;
						binary_list->exe_infos[i].num_uniform++;
					}
					else if (symbol[j].end_index < BOOL_UNIFORM_OFFSET){
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].type = GL_INT_VEC3;
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].addr = symbol[j].start_index - LOOP_UNIFORM_OFFSET;
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].index = 0;
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].name_index = symbol[j].name_index;
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].size = symbol[j].end_index - symbol[j].start_index + 1;
						binary_list->exe_infos[i].num_uniform++;
					}
					else{
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].type = GL_BOOL;
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].addr = symbol[j].start_index - BOOL_UNIFORM_OFFSET;
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].index = 0;
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].name_index = symbol[j].name_index;
						binary_list->exe_infos[i].uniforms[binary_list->exe_infos[i].num_uniform].size = symbol[j].end_index - symbol[j].start_index + 1;
						binary_list->exe_infos[i].num_uniform++;
					}
				}
			}
			if (i != bin_header->exe_number)
				break;
		}
		
		error = 0;
	} while (0);
	
	if (!error){
		int i;
		sh_list_t* shlist;
		
		for (i = 0; i < n; i++){
			__GET_SH_LIST(shaders[i], shlist)

			if (shlist->bin_list){
				shlist->bin_list->ref_count--;
				if (shlist->bin_list->ref_count == 0)
					shm_deleteBinaryObject(shlist->bin_list);
			}
			shlist->bin_list = binary_list;
			shlist->exe_id = i;
		}

		if (!error){
			binary_list->ref_count = n;
			binary_list->prev = 0;
			binary_list->next = __shman->bin_list_top;
			if (__shman->bin_list_top)
				__shman->bin_list_top->prev = binary_list;
			__shman->bin_list_top = binary_list;
		}
	}
	
	return;
}

void GL_APIENTRY glAttachShader(GLuint program, GLuint shader){
	pg_list_t* pglist;
	sh_list_t* shlist;
	
	__GET_PG_LIST(program, pglist)
	BASE_GL_FAIL_IF(!pglist, GL_INVALID_VALUE);
	
	pglist->shader_update_status = GL_TRUE;
	
	if (shader == GL_DMP_FRAGMENT_SHADER_DMP){
		pglist->fs = shader;
	}
	else{
		__GET_SH_LIST(shader, shlist)
		BASE_GL_FAIL_IF(!shlist, GL_INVALID_VALUE);
		
		if (shlist->type == GL_VERTEX_SHADER){
			BASE_GL_FAIL_IF(pglist->vs, GL_INVALID_OPERATION);
			shlist->ref_count++;
			pglist->vs = shlist;
		}
		else if (shlist->type == GL_GEOMETRY_SHADER_DMP){
			BASE_GL_FAIL_IF(pglist->gs, GL_INVALID_OPERATION);
			shlist->ref_count++;
			pglist->gs = shlist;
		}
		else{
			__err_setError(GL_INVALID_ENUM);
		}
	}
	
	return;
}

void GL_APIENTRY glDetachShader(GLuint program, GLuint shader)
{
	pg_list_t* pglist;
	sh_list_t* shlist;
	
	__GET_PG_LIST(program, pglist)
	BASE_GL_FAIL_IF(!pglist, GL_INVALID_VALUE);
	
	pglist->shader_update_status = GL_TRUE;
	
	if (shader == GL_DMP_FRAGMENT_SHADER_DMP){
		pglist->fs = 0;
	}
	else{
		GLuint delete_shader = 0;
		__GET_SH_LIST(shader, shlist)
		BASE_GL_FAIL_IF(!shlist, GL_INVALID_VALUE);
		
		if (shlist->type == GL_VERTEX_SHADER){
			BASE_GL_FAIL_IF(!pglist->vs || pglist->vs->id != shader, GL_INVALID_OPERATION);
			shlist->ref_count--;
			pglist->vs = 0;
			if (shlist->ref_count == 0 && shlist->delete_status)
				delete_shader = shader;
		}
		else if (shlist->type == GL_GEOMETRY_SHADER_DMP){
			BASE_GL_FAIL_IF(!pglist->gs || pglist->gs->id != shader, GL_INVALID_OPERATION);
			shlist->ref_count--;
			pglist->gs = 0;
			if (shlist->ref_count == 0 && shlist->delete_status)
				delete_shader = shader;
		}
		else{
			__err_setError(GL_INVALID_ENUM);
		}

		if (delete_shader)
			shm_deleteShaderObject(delete_shader);
	}
	
	return;
}

void GL_APIENTRY glDeleteShader(GLuint shader){
	sh_list_t* shlist;
	
	BASE_GL_FAIL_IF(shader == GL_DMP_FRAGMENT_SHADER_DMP, GL_INVALID_OPERATION);
	
	__GET_SH_LIST(shader, shlist)
	BASE_GL_FAIL_IF(!shlist, GL_INVALID_VALUE);
	
	if (shlist->ref_count == 0)
		shm_deleteShaderObject(shader);
	else
		shlist->delete_status = GL_TRUE;
	
	return;
}

static void shm_deleteShaderObject(GLuint shader){
	sh_list_t* shlist;
	sh_list_t* tmp = 0;
	
	for (shlist = __shman->sh_list_table[shader & SH_LIST_TABLE_SIZE]; shlist != 0; shlist = shlist->next){
		if (shlist->id == shader)
			break;
		tmp = shlist;
	}
	if (tmp == 0)
		__shman->sh_list_table[shader & SH_LIST_TABLE_SIZE] = __shman->sh_list_table[shader & SH_LIST_TABLE_SIZE]->next;
	else
		tmp->next = shlist->next;
	
	if (shader < __shminshID)
		__shminshID = shader;

	if (shlist->bin_list){
		shlist->bin_list->ref_count--;
		if (shlist->bin_list->ref_count == 0)
			shm_deleteBinaryObject(shlist->bin_list);
	}
	free(shlist);
	
	return;
}

static void shm_deleteProgramObject(GLuint program){
	pg_list_t* pglist;
	pg_list_t* tmp = 0;

	for (pglist = __shman->pg_list_table[program & PG_LIST_TABLE_SIZE]; pglist != 0; pglist = pglist->next){
		if (pglist->id == program)
			break;
		tmp = pglist;
	}
	if (pglist->vs)
		glDetachShader(program, pglist->vs->id);
	if (pglist->gs)
		glDetachShader(program, pglist->gs->id);

	if (tmp == 0)
		__shman->pg_list_table[program & PG_LIST_TABLE_SIZE] = __shman->pg_list_table[program & PG_LIST_TABLE_SIZE]->next;
	else
		tmp->next = pglist->next;
	
	if (program < __shminpgID)
		__shminpgID = program;
	
	if (pglist->vs_floats)
		free(pglist->vs_floats);
	if (pglist->gs_floats)
		free(pglist->gs_floats);
	if (pglist->uni_location_map)
		free(pglist->uni_location_map);
	if (pglist->attrib_location){
		attrib_location_t* tmp0;
		attrib_location_t* tmp1;
		for (tmp0 = pglist->attrib_location; tmp0 != 0;){
			tmp1 = tmp0->next;
			free(tmp0->name);
			free(tmp0);
			tmp0 = tmp1;
		}
	}
	free(pglist);
	
	return;
}

static void shm_deleteBinaryObject(bin_list_t* binlist){
	if (__shman->bin_list_top == binlist){
		__shman->bin_list_top = binlist->next;
		if (__shman->bin_list_top)
			__shman->bin_list_top->prev = 0;
	}
	else{
		binlist->prev->next = binlist->next;
		if (binlist->next)
			binlist->next->prev = binlist->prev;
	}

	if (binlist->program)
		free(binlist->program);
	if (binlist->swizzle)
		free(binlist->swizzle);
	if (binlist->exe_infos){
		unsigned i;
		for (i = 0; i < binlist->num_exe_infos; i++){
			if (binlist->exe_infos[i].float_consts)
				free(binlist->exe_infos[i].float_consts);
			if (binlist->exe_infos[i].uniforms)
				free(binlist->exe_infos[i].uniforms);
			if (binlist->exe_infos[i].strings)
				free(binlist->exe_infos[i].strings);
		}
		free(binlist->exe_infos);
	}
	free(binlist);
	
	return;
}

void GL_APIENTRY glLinkProgram(GLuint program)
{
	unsigned i, j;
	pg_list_t* pglist;
	pg_list_t newprog;
	exe_info_t* vs_exe_info;
	exe_info_t* gs_exe_info = 0;

	__GET_PG_LIST(program, pglist)

	BASE_GL_FAIL_IF(!pglist, GL_INVALID_VALUE);

	pglist->link_status = GL_FALSE;

	if (pglist->delete_status || !pglist->vs)
		return;

	if (!pglist->fs || !pglist->vs->bin_list || (pglist->gs && pglist->gs->bin_list != pglist->vs->bin_list))
		return;
	
	vs_exe_info = &pglist->vs->bin_list->exe_infos[pglist->vs->exe_id];
	if (pglist->gs)
		gs_exe_info = &pglist->gs->bin_list->exe_infos[pglist->gs->exe_id];
	newprog.num_vs_floats = 0;
	newprog.num_gs_floats = 0;
	newprog.vs_floats = 0;
	newprog.gs_floats = 0;
	newprog.gs_uniform_offset = 0;
	newprog.uni_location_map = 0;
	
	do{
		unsigned vs_const_float_reg[3] = {0, 0, 0};
		unsigned gs_const_float_reg[3] = {0, 0, 0};
		
		for (i = 0; i < vs_exe_info->num_uniform; i++){
			if (vs_exe_info->uniforms[i].type != GL_INT_VEC3 && vs_exe_info->uniforms[i].type != GL_BOOL){
				for (j = 0; j < (unsigned)vs_exe_info->uniforms[i].size; j++)
					vs_const_float_reg[(vs_exe_info->uniforms[i].addr + j) >> 5] |= 1 << ((vs_exe_info->uniforms[i].addr + j) & 0x1f);
			}
		}
		for (i = 0; i < 3; i++){
			unsigned reg_mask = vs_const_float_reg[i];
			for (j = 0; j < 32 && reg_mask; j++, reg_mask >>= 1){
				if (reg_mask & 1){
					newprog.vs_floats_addr[newprog.num_vs_floats] = (i << 5) + j;
					newprog.num_vs_floats++;
				}
			}
		}

		newprog.num_uniform = vs_exe_info->num_uniform;
		if (newprog.num_vs_floats){
			newprog.vs_floats = (float_const_reg_t*)malloc(sizeof(float_const_reg_t) * newprog.num_vs_floats);
			BASE_GL_FAIL_IF_BRK(!newprog.vs_floats, GL_OUT_OF_MEMORY);
			memset(newprog.vs_floats, 0, sizeof(float_const_reg_t) * newprog.num_vs_floats);
		}
		
		if (pglist->gs){
			for (i = 0; i < gs_exe_info->num_uniform; i++){
				if (gs_exe_info->uniforms[i].type != GL_INT_VEC3 && gs_exe_info->uniforms[i].type != GL_BOOL){
					for (j = 0; j < (unsigned int)gs_exe_info->uniforms[i].size; j++)
						gs_const_float_reg[(gs_exe_info->uniforms[i].addr + j) >> 5] |= 1 << ((gs_exe_info->uniforms[i].addr + j) & 0x1f);
				}
			}
			for (i = 0; i < 3; i++){
				unsigned reg_mask = gs_const_float_reg[i];
				for (j = 0; j < 32 && reg_mask; j++, reg_mask >>= 1){
					if (reg_mask & 1){
						newprog.gs_floats_addr[newprog.num_gs_floats] = (i << 5) + j;
						newprog.num_gs_floats++;
					}
				}
			}
			newprog.gs_uniform_offset = newprog.num_uniform;
			newprog.num_uniform += gs_exe_info->num_uniform;
			if (newprog.num_gs_floats){
				newprog.gs_floats = (float_const_reg_t*)malloc(sizeof(float_const_reg_t) * newprog.num_gs_floats);
				BASE_GL_FAIL_IF_BRK(!newprog.gs_floats, GL_OUT_OF_MEMORY);
				memset(newprog.gs_floats, 0, sizeof(float_const_reg_t) * newprog.num_gs_floats);
			}
		}
		if (newprog.num_uniform > (MAX_SHM_IMPL_UNIFORMS + 1))
			break;
		newprog.fs_uniform_offset = newprog.num_uniform;
		newprog.num_uniform += NUM_FRAGMENT_UNIFORM;
		
		newprog.uni_location_map = (uni_location_t*)malloc(sizeof(uni_location_t) * newprog.num_uniform);
		if (!newprog.uni_location_map)
			break;
		memset(newprog.uni_location_map, 0, sizeof(uni_location_t) * newprog.num_uniform);
	} while(0);
	
	if (!newprog.uni_location_map){
		if (newprog.gs_floats)
			free(newprog.gs_floats);
		if (newprog.vs_floats)
			free(newprog.vs_floats);
		return;
	}

	if (pglist->uni_location_map){
		free(pglist->uni_location_map);
		pglist->uni_location_map = 0;
	}
	if (pglist->vs_floats){
		free(pglist->vs_floats);
		pglist->vs_floats = 0;
	}
	pglist->num_vs_floats = 0;
	pglist->vs_floats_update_mask[0] = 0;
	pglist->vs_floats_update_mask[1] = 0;
	pglist->vs_floats_update_mask[2] = 0;
	if (pglist->gs_floats){
		free(pglist->gs_floats);
		pglist->gs_floats = 0;
	}
	pglist->num_gs_floats = 0;
	pglist->gs_floats_update_mask[0] = 0;
	pglist->gs_floats_update_mask[1] = 0;
	pglist->gs_floats_update_mask[2] = 0;
	for (i = 0; i < 16; i++)
		pglist->attribs[i].reg_index = -1;

	pglist->num_vs_floats = newprog.num_vs_floats;
	for (i = 0; i < pglist->num_vs_floats; i++)
		pglist->vs_floats_addr[i] = newprog.vs_floats_addr[i];
	pglist->vs_floats = newprog.vs_floats;
	pglist->num_gs_floats = newprog.num_gs_floats;
	for (i = 0; i < pglist->num_gs_floats; i++)
		pglist->gs_floats_addr[i] = newprog.gs_floats_addr[i];
	pglist->gs_floats = newprog.gs_floats;
	pglist->gs_uniform_offset = newprog.gs_uniform_offset;
	pglist->fs_uniform_offset = newprog.fs_uniform_offset;
	pglist->num_uniform = newprog.num_uniform;
	pglist->uni_location_map = newprog.uni_location_map;

	{
		
		for (i = 0; i < vs_exe_info->num_uniform; i++){
			uni_location_t loc;
			
			loc.location.value = 0;
			loc.location.vs.prog = program;
			loc.location.vs.mapindex = i;
			
			*(unsigned*)&loc.vs_ext = 0;
			loc.vs_ext.index = vs_exe_info->uniforms[i].index;
			loc.vs_ext.regsize = vs_exe_info->uniforms[i].size;
			
			switch (vs_exe_info->uniforms[i].type){
				case GL_INT_VEC3:
					loc.vs_ext.size = 2;
					loc.vs_ext.intbool = 1;
					loc.vs_ext.offset = vs_exe_info->uniforms[i].addr;
					break;
				case GL_BOOL:
					loc.vs_ext.intbool = 1;
					loc.vs_ext.offset = vs_exe_info->uniforms[i].addr;
					break;
				case GL_FLOAT:
					break;
				case GL_FLOAT_VEC2:
					loc.vs_ext.size = 1;
					break;
				case GL_FLOAT_VEC3:
					loc.vs_ext.size = 2;
					break;
				case GL_FLOAT_VEC4:
					loc.vs_ext.size = 3;
					break;
				case GL_FLOAT_MAT2:
					loc.vs_ext.size = 1;
					loc.vs_ext.matrix = 1;
					break;
				case GL_FLOAT_MAT3:
					loc.vs_ext.size = 2;
					loc.vs_ext.matrix = 1;
					break;
				case GL_FLOAT_MAT4:
					loc.vs_ext.size = 3;
					loc.vs_ext.matrix = 1;
					break;
			}
			
			if (loc.vs_ext.intbool == 0){
				for (j = 0; j < pglist->num_vs_floats; j++){
					if (pglist->vs_floats_addr[j] == vs_exe_info->uniforms[i].addr){
						loc.vs_ext.offset = j;
						break;
					}
				}
			}
			/* set location information */
			loc.name_index = vs_exe_info->uniforms[i].name_index;
			pglist->uni_location_map[i] = loc;
		}
		
		if (pglist->gs){
			for (i = 0; i < gs_exe_info->num_uniform; i++){
				uni_location_t loc;
				
				loc.location.value = 0;
				loc.location.vs.prog = program;
				loc.location.vs.mapindex = i + pglist->gs_uniform_offset;
				
				*(unsigned*)&loc.vs_ext = 0;
				loc.vs_ext.geometry = 1;
				loc.vs_ext.index = gs_exe_info->uniforms[i].index;
				loc.vs_ext.regsize = gs_exe_info->uniforms[i].size;
				
				switch (gs_exe_info->uniforms[i].type){
					case GL_INT_VEC3:
						loc.vs_ext.size = 2;
						loc.vs_ext.intbool = 1;
						loc.vs_ext.offset = gs_exe_info->uniforms[i].addr;
						break;
					case GL_BOOL:
						loc.vs_ext.intbool = 1;
						loc.vs_ext.offset = gs_exe_info->uniforms[i].addr;
						break;
					case GL_FLOAT:
						break ;
					case GL_FLOAT_VEC2:
						loc.vs_ext.size = 1;
						break ;
					case GL_FLOAT_VEC3:
						loc.vs_ext.size = 2;
						break ;
					case GL_FLOAT_VEC4:
						loc.vs_ext.size = 3;
						break ;
					case GL_FLOAT_MAT2:
						loc.vs_ext.size = 1;
						loc.vs_ext.matrix = 1;
						break ;
					case GL_FLOAT_MAT3:
						loc.vs_ext.size = 2;
						loc.vs_ext.matrix = 1;
						break ;
					case GL_FLOAT_MAT4:
						loc.vs_ext.size = 3;
						loc.vs_ext.matrix = 1;
						break ;
				}
				
				if (loc.vs_ext.intbool == 0){
					for (j = 0; j < pglist->num_gs_floats; j++){
						if (pglist->gs_floats_addr[j] == gs_exe_info->uniforms[i].addr){
							loc.vs_ext.offset = j;
							break;
						}
					}
				}
				loc.name_index = gs_exe_info->uniforms[i].name_index;
				pglist->uni_location_map[i + pglist->gs_uniform_offset] = loc;
			}
		}
	}
	
	for (i = pglist->fs_uniform_offset; i < pglist->num_uniform; i++){
		uni_location_t loc;
		loc.location.value = 0;
		loc.location.fs.offset = i - pglist->fs_uniform_offset;
		loc.location.fs.prog = program;
		loc.location.fs.fragment = 1;
		switch (s_fixed_uniforms[loc.location.fs.offset].type){
			case GL_FLOAT_VEC4: case GL_INT_VEC4:
				loc.location.fs.size = 3;
				break;
			case GL_FLOAT_VEC3: case GL_INT_VEC3:
				loc.location.fs.size = 2;
				break;
			case GL_FLOAT_VEC2: case GL_INT_VEC2:
				loc.location.fs.size = 1;
				break;
			default:
				loc.location.fs.size = 0;
				break;
		}
		loc.name_index = i - pglist->fs_uniform_offset;
		*(unsigned*)&loc.vs_ext = 0;
		pglist->uni_location_map[i] = loc;
	}

	{
		attrib_location_t* attribloc;
		unsigned reg_bound_mask = 0;

		for (attribloc = pglist->attrib_location; attribloc != 0; attribloc = attribloc->next){
			for (i = 0; i < 16; i++){
				if (vs_exe_info->attribs[i].type != 0 && strcmp(&vs_exe_info->strings[vs_exe_info->attribs[i].name_index], attribloc->name) == 0){

					reg_bound_mask |= 1 << i;
					pglist->attribs[attribloc->index].reg_index = i;
					pglist->attribs[attribloc->index].type = vs_exe_info->attribs[i].type;
					pglist->attribs[attribloc->index].name_index = vs_exe_info->attribs[i].name_index;
					break;
				}
			}
		}

		for (i = 0; i < 16; i++){
			if (vs_exe_info->attribs[i].type != 0 && !(reg_bound_mask & (1 << i))){
				for (j = 0; j < 16; j++){
					if (pglist->attribs[j].reg_index == -1){
						reg_bound_mask |= 1 << i;
						pglist->attribs[j].reg_index = i;
						pglist->attribs[j].type = vs_exe_info->attribs[i].type;
						pglist->attribs[j].name_index = vs_exe_info->attribs[i].name_index;
						break;
					}
				}
			}
		}
	}

	{
		unsigned gs_num_output = 0;
		unsigned gs_output_mask = 0;
		unsigned output_format[7];

		{
			unsigned* vs_output_format;
			unsigned* gs_output_format;

			vs_output_format = vs_exe_info->output_format;
			
			if (pglist->gs){
				gs_output_format = gs_exe_info->output_format;
				if (gs_exe_info->merge_output_maps){
					int index = 0;
					unsigned gs_copy_mask = 0;
					unsigned vs_copy_mask = 0;
					

					for (j = 0; j < 7; j++){
						if (gs_output_format[j] == 0x1f1f1f1f)
							break;
						else{
							for (i = 0; i < 7; i++){
								if (gs_output_format[j] == vs_output_format[i]){

									output_format[index++] = gs_output_format[j];
									gs_copy_mask |= 1 << j;
									vs_copy_mask |= 1 << i;
									break;
								}
							}
						}
					}

					for (i = 0; i < 7; i++){
						if (gs_output_format[i] == 0x1f1f1f1f)
							break;
						if (!(gs_copy_mask & (1 << i))){
							output_format[index++] = gs_output_format[i];
							if (index == 7)
								break;
						}
					}

					for (i = 0; i < 7; i++){
						if (vs_output_format[i] == 0x1f1f1f1f)
							break;
						if (!(vs_copy_mask & (1 << i))){
							output_format[index++] = vs_output_format[i];
							if (index == 7)
								break;
						}
					}
					for (i = index; i < 7; i++)
						output_format[i] = 0x1f1f1f1f;
					gs_num_output = index;
					gs_output_mask = gs_exe_info->output_mask;
					for (i = 0; index != 0 && gs_output_mask != 0; i++){
						if (gs_output_mask & 1)
							index--;
						gs_output_mask >>= 1;
					}
					gs_output_mask = gs_exe_info->output_mask & ((1 << i) - 1);
					pglist->clock_enable = gs_exe_info->clock_enable | vs_exe_info->clock_enable;
				}
				else{
					for (i = 0; i < 7; i++)
						output_format[i] = gs_output_format[i];
					gs_output_mask = gs_exe_info->output_mask;
					gs_num_output = gs_exe_info->num_output;
					pglist->clock_enable = gs_exe_info->clock_enable;
				}
			}
			else{
				for (i = 0; i < 7; i++)
					output_format[i] = vs_output_format[i];
				pglist->clock_enable = vs_exe_info->clock_enable;
			}
		}

		memcpy(pglist->ras_regs, __shman->default_ras_regs, sizeof(unsigned) * RA_LAST);
		memcpy(pglist->ras_regs_be, __shman->default_ras_regs_be, sizeof(unsigned char) * RA_LAST);

		shm_initializeProgram(pglist);
		
		#define UPDATE_GEO_REGISTER(_addr, _data) \
		{ \
			if (pglist->ras_regs[_addr] != (_data)) \
			{ \
				pglist->ras_regs[_addr] = (_data); \
				SET_RASREGMASK(pglist->ras_reg_mask, (_addr)); \
			} \
			pglist->ras_regs_be[_addr] = 0xf; \
		}

		#define UPDATE_GEO_REGISTER_BE(_addr, _data, _be) \
		{ \
			if ((pglist->ras_regs[_addr] & s_be_mask[_be]) != ((_data) & s_be_mask[_be])) \
			{ \
				pglist->ras_regs[_addr] = (pglist->ras_regs[_addr] & ~s_be_mask[_be]) | ((_data) & s_be_mask[_be]); \
				SET_RASREGMASK(pglist->ras_reg_mask, (_addr)); \
			} \
			pglist->ras_regs_be[_addr] |= _be; \
		}

		UPDATE_GEO_REGISTER(RA_VP1_PROGRAM_ADDRESS, vs_exe_info->main_addr | 0x7fff0000)
		UPDATE_GEO_REGISTER(RA_VP1_BOOL_REG, vs_exe_info->bool_consts | 0x7fff0000)
		for (i = 0; i < 4; i++)
			UPDATE_GEO_REGISTER(RA_VP1_INT_REG_TOP_00 + i, vs_exe_info->int_consts[i])
		UPDATE_GEO_REGISTER(RA_VP1_OUTPUT_MASK, vs_exe_info->output_mask)
		UPDATE_GEO_REGISTER_BE(RA_VP1_MODE, vs_exe_info->num_input - 1, 1)
		
		if (pglist->gs){
			UPDATE_GEO_REGISTER_BE(RA_VP0_MODE, vs_exe_info->num_output - 1, 1)
			UPDATE_GEO_REGISTER(RA_VP0_PROGRAM_ADDRESS, gs_exe_info->main_addr | 0x7fff0000)
			UPDATE_GEO_REGISTER(RA_VP0_BOOL_REG, gs_exe_info->bool_consts | 0x7fff0000)
			for (i = 0; i < 4; i++)
				UPDATE_GEO_REGISTER(RA_VP0_INT_REG_TOP_00 + i, gs_exe_info->int_consts[i])
			UPDATE_GEO_REGISTER(RA_VP0_OUTPUT_MASK, gs_output_mask)
			UPDATE_GEO_REGISTER(RA_TS_WORD_NUM, gs_num_output)
			for (i = 0; i < 7; i++)
				UPDATE_GEO_REGISTER(RA_TS_VTXWORD1 + i, output_format[i])
			UPDATE_GEO_REGISTER(RA_TI_CLOCK_ENABLE, pglist->clock_enable)
			UPDATE_GEO_REGISTER(RA_TI_TEX_DERIVS_ENABLE, ((pglist->clock_enable & 0x00010700) ? 1 : 0))

			switch (gs_exe_info->geometry_data_mode){
				case 0:
					UPDATE_GEO_REGISTER_BE(RA_CI_VP_MODE, 2, 0x8)
					UPDATE_GEO_REGISTER(RA_VC_SEND_MODE, 0)
					UPDATE_GEO_REGISTER_BE(RA_VP0_MODE, 0x08000000, 0xa)
					break;
				case 1:
					UPDATE_GEO_REGISTER_BE(RA_CI_VP_MODE, 0x80000002, 0x8)
					UPDATE_GEO_REGISTER(RA_VC_SEND_MODE, 1)
					UPDATE_GEO_REGISTER_BE(RA_VP0_MODE, 0x08000100, 0xa)
					UPDATE_GEO_REGISTER(RA_VC_SUBDIV_CONF, gs_exe_info->variable_sized_primitive_size - 1)
					break;
				case 2:
					UPDATE_GEO_REGISTER_BE(RA_CI_VP_MODE, 2, 0x8)
					UPDATE_GEO_REGISTER(RA_VC_SEND_MODE,
						0x01000002 |
						((gs_exe_info->fixed_sized_primitive_size - 1) << 8) |
						((vs_exe_info->num_output - 1) << 12) |
						(gs_exe_info->start_index << 16))
					UPDATE_GEO_REGISTER_BE(RA_VP0_MODE, 0x08000100, 0xa)
					break;
			}
			UPDATE_GEO_REGISTER(RA_SYNC_ATTR_COUNT, vs_exe_info->num_output - 1)
			UPDATE_GEO_REGISTER_BE(RA_RASIN_TRI_SETTING, gs_num_output - 1, 1)
			UPDATE_GEO_REGISTER(RA_VC_ATTR_COUNT, vs_exe_info->num_output - 1)
			UPDATE_GEO_REGISTER(RA_LB_ATTR_COUNT, vs_exe_info->num_input - 1)
			UPDATE_GEO_REGISTER_BE(RA_VP1_MODE, 0xa0000000, 0xa)
		}
		else{
			UPDATE_GEO_REGISTER(RA_TS_WORD_NUM, vs_exe_info->num_output)
			for (i = 0; i < 7; i++)
				UPDATE_GEO_REGISTER(RA_TS_VTXWORD1 + i, output_format[i])
			UPDATE_GEO_REGISTER(RA_TI_CLOCK_ENABLE, pglist->clock_enable)
			UPDATE_GEO_REGISTER(RA_TI_TEX_DERIVS_ENABLE, ((pglist->clock_enable & 0x00010700) ? 1 : 0))

			UPDATE_GEO_REGISTER_BE(RA_CI_VP_MODE, 0, 0x8)
			UPDATE_GEO_REGISTER(RA_VC_SEND_MODE, 0)
			UPDATE_GEO_REGISTER_BE(RA_VP1_MODE, 0xa0000000, 0xa)
			UPDATE_GEO_REGISTER(RA_SYNC_ATTR_COUNT, vs_exe_info->num_output - 1)
			UPDATE_GEO_REGISTER_BE(RA_RASIN_TRI_SETTING, vs_exe_info->num_output - 1, 1)
			UPDATE_GEO_REGISTER(RA_VC_ATTR_COUNT, vs_exe_info->num_output - 1)
			UPDATE_GEO_REGISTER(RA_LB_ATTR_COUNT, vs_exe_info->num_input - 1)
		}
		
		#undef UPDATE_GEO_REGISTER
		#undef UPDATE_GEO_REGISTER_BE
	}
	
	pglist->link_status = GL_TRUE;
	pglist->link_update_status = pglist->shader_update_status;
	pglist->shader_update_status = GL_FALSE;
	if (pglist->gs){
		pglist->geometry_shader_used = GL_TRUE;
		pglist->gs_exe_id = pglist->gs->exe_id;
	}
	else
		pglist->geometry_shader_used = GL_FALSE;
	pglist->vs_exe_id = pglist->vs->exe_id;

	if (pglist == __shman->current_program){
		GET_CURRENT_STATE(state);
		
		if (pglist->bin_list != pglist->vs->bin_list)
			SET_MASK(state->upd_mask, IF_GL_BINARY_PACKAGE);
		SET_MASK(state->upd_mask, IF_GL_BINARY_EXE_VS);
		SET_MASK(state->upd_mask, IF_GL_BINARY_EXE_GS);
		SET_MASK(state->upd_mask, IF_GL_ATTACH_GS);
		SET_MASK(state->upd_mask, IF_GL_DETACH_GS);
		SET_MASK(state->upd_mask, IF_GL_PROGRAM);
		
		/* update everything. */
		pglist->vs_floats_update_mask[0] = 0xffffffff;
		pglist->vs_floats_update_mask[1] = 0xffffffff;
		pglist->vs_floats_update_mask[2] = 0xffffffff;
		if (pglist->gs)
		{
			pglist->gs_floats_update_mask[0] = 0xffffffff;
			pglist->gs_floats_update_mask[1] = 0xffffffff;
			pglist->gs_floats_update_mask[2] = 0xffffffff;
		}
		SET_ALL_RASREGMASK(pglist->ras_reg_mask);
		
		SET_MASK(state->upd_mask, IF_GL_LIGHTING_LUT_SAMPLERS);
		SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
		SET_MASK(state->upd_mask, IF_GL_GAS_LUT_SAMPLERS);
		SET_MASK(state->upd_mask, IF_GL_FOG_LUT_SAMPLER);
		SET_MASK(state->upd_mask, IF_GL_FB_ACCESS);
		SET_MASK(state->upd_mask, IF_GL_TRI_OFFSET);

		if (state->gl_state.textureunits.samplerType[0] != pglist->fragment_state.tex.sampler_type[0]){
			state->gl_state.textureunits.enabled2DTex[0] = GL_FALSE;
			state->gl_state.textureunits.enabledCubeTex[0] = GL_FALSE;
			state->gl_state.textureunits.samplerType[0] = GL_FALSE;
			SET_MASK(state->upd_mask, IF_GL_TEXTURE0);
		}
		if (state->gl_state.textureunits.samplerType[1] != pglist->fragment_state.tex.sampler_type[1]){
			state->gl_state.textureunits.enabled2DTex[1] = GL_FALSE;
			state->gl_state.textureunits.samplerType[1] = GL_FALSE;
			SET_MASK(state->upd_mask, IF_GL_TEXTURE1);
		}
		if (state->gl_state.textureunits.samplerType[2] != pglist->fragment_state.tex.sampler_type[2]){
			state->gl_state.textureunits.enabled2DTex[2] = GL_FALSE;
			state->gl_state.textureunits.samplerType[2] = GL_FALSE;
			SET_MASK(state->upd_mask, IF_GL_TEXTURE2);
		}
		
		pglist->link_update_status = GL_FALSE;
		state->gl_state.rendermode.bGeometryShaderUsed = pglist->geometry_shader_used;
	}
	
	pglist->bin_list = pglist->vs->bin_list;
	
	return;
}

void GL_APIENTRY glUseProgram(GLuint program)
{
	pg_list_t* pglist;
	GET_CURRENT_STATE(state);
	
	if (program == 0){
		if (__shman->current_program && __shman->current_program->delete_status){
			shm_deleteProgramObject(__shman->current_program->id);
			__shman->last_validated_program = 0;
		}
		__shman->current_program = 0;
		state->gl_state.rendermode.currentProgram = 0;
		state->gl_state.rendermode.bGeometryShaderUsed = GL_FALSE;
		return;
	}
	
	__GET_PG_LIST(program, pglist)
	
	BASE_GL_FAIL_IF(pglist == 0, GL_INVALID_VALUE);
	BASE_GL_FAIL_IF(pglist->link_status == GL_FALSE, GL_INVALID_OPERATION);
	
	if (__shman->current_program != pglist)
		SET_MASK(state->upd_mask, IF_GL_PROGRAM);
	
	if (__shman->current_program == 0 || pglist->bin_list != __shman->current_program->bin_list){
		SET_MASK(state->upd_mask, IF_GL_BINARY_PACKAGE);
		SET_MASK(state->upd_mask, IF_GL_BINARY_EXE_VS);
		SET_MASK(state->upd_mask, IF_GL_BINARY_EXE_GS);
		SET_MASK(state->upd_mask, IF_GL_ATTACH_GS);
		SET_MASK(state->upd_mask, IF_GL_DETACH_GS);
	}
	else{

		if (pglist->vs_exe_id != __shman->current_program->vs_exe_id)
			SET_MASK(state->upd_mask, IF_GL_BINARY_EXE_VS);

		if (pglist->geometry_shader_used != 0 && __shman->current_program->geometry_shader_used == 0){
			SET_MASK(state->upd_mask, IF_GL_ATTACH_GS);
			SET_MASK(state->upd_mask, IF_GL_BINARY_EXE_GS);
		}

		else if (pglist->geometry_shader_used == 0 && __shman->current_program->geometry_shader_used != 0){
			SET_MASK(state->upd_mask, IF_GL_DETACH_GS);
			SET_MASK(state->upd_mask, IF_GL_BINARY_EXE_VS);
		}

		else if (pglist->geometry_shader_used && __shman->current_program->geometry_shader_used && pglist->gs_exe_id != __shman->current_program->gs_exe_id){
			SET_MASK(state->upd_mask, IF_GL_BINARY_EXE_GS);
		}
	}
	
	if (__shman->current_program == 0 || (pglist == __shman->current_program && pglist->link_update_status == GL_TRUE)){
		pglist->vs_floats_update_mask[0] = 0xffffffff;
		pglist->vs_floats_update_mask[1] = 0xffffffff;
		pglist->vs_floats_update_mask[2] = 0xffffffff;
		if (pglist->geometry_shader_used){
			pglist->gs_floats_update_mask[0] = 0xffffffff;
			pglist->gs_floats_update_mask[1] = 0xffffffff;
			pglist->gs_floats_update_mask[2] = 0xffffffff;
		}
		SET_ALL_RASREGMASK(pglist->ras_reg_mask);
		
		SET_MASK(state->upd_mask, IF_GL_LIGHTING_LUT_SAMPLERS);
		SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
		SET_MASK(state->upd_mask, IF_GL_GAS_LUT_SAMPLERS);
		SET_MASK(state->upd_mask, IF_GL_FOG_LUT_SAMPLER);
		SET_MASK(state->upd_mask, IF_GL_FB_ACCESS);
		SET_MASK(state->upd_mask, IF_GL_TRI_OFFSET);
	}
	else if (__shman->current_program != pglist){
		pglist->vs_floats_update_mask[0] = 0xffffffff;
		pglist->vs_floats_update_mask[1] = 0xffffffff;
		pglist->vs_floats_update_mask[2] = 0xffffffff;
		if (pglist->geometry_shader_used){
			pglist->gs_floats_update_mask[0] = 0xffffffff;
			pglist->gs_floats_update_mask[1] = 0xffffffff;
			pglist->gs_floats_update_mask[2] = 0xffffffff;
		}
		SET_ALL_RASREGMASK(pglist->ras_reg_mask);

		if (pglist->ras_regs[RA_TU0_BEGIN_0F] & 1){
			SET_MASK(state->upd_mask, IF_GL_LIGHTING_LUT_SAMPLERS);
		}

		if (pglist->fragment_state.proctex.sampler_type){
			SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
		}

		if (pglist->ras_regs[RA_TB_FOG_EN] & 1){
			SET_MASK(state->upd_mask, IF_GL_FOG_LUT_SAMPLER);
		}

		if (pglist->ras_regs[RA_TB_FOG_EN] & 2){
			SET_MASK(state->upd_mask, IF_GL_GAS_LUT_SAMPLERS);
		}

		if (state->gl_state.per_frag_op.depthTestEnabled){
			SET_MASK(state->upd_mask, IF_GL_FB_ACCESS);
		}

		if (pglist->fragment_state.operation.wscale == 0){
			SET_MASK(state->upd_mask, IF_GL_TRI_OFFSET);
		}
	}

	{
		switch (pglist->fragment_state.tex.sampler_type[0]){
			case GL_FALSE:
				CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[0], GL_FALSE, IF_GL_TEXTURE0);
				CHECK_ASSIGN(state->gl_state.textureunits.enabledCubeTex[0], GL_FALSE, IF_GL_TEXTURE0);
				break;
			case GL_TEXTURE_2D:
			case GL_TEXTURE_SHADOW_2D_DMP:
			case GL_TEXTURE_PROJECTION_DMP:
				CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[0], GL_TRUE, IF_GL_TEXTURE0);
				CHECK_ASSIGN(state->gl_state.textureunits.enabledCubeTex[0], GL_FALSE, IF_GL_TEXTURE0);
				break;
			case GL_TEXTURE_CUBE_MAP:
			case GL_TEXTURE_SHADOW_CUBE_DMP:
				CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[0], GL_FALSE, IF_GL_TEXTURE0);
				CHECK_ASSIGN(state->gl_state.textureunits.enabledCubeTex[0], GL_TRUE, IF_GL_TEXTURE0);
				break;
		}
		CHECK_ASSIGN(state->gl_state.textureunits.samplerType[0], pglist->fragment_state.tex.sampler_type[0], IF_GL_TEXTURE0);
		
		switch (pglist->fragment_state.tex.sampler_type[1]){
			case GL_FALSE:
				CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[1], GL_FALSE, IF_GL_TEXTURE1);
				break;
			case GL_TEXTURE_2D:
				CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[1], GL_TRUE, IF_GL_TEXTURE1);
				break;
		}
		CHECK_ASSIGN(state->gl_state.textureunits.samplerType[1], pglist->fragment_state.tex.sampler_type[1], IF_GL_TEXTURE1);
		
		switch (pglist->fragment_state.tex.sampler_type[2]){
			case GL_FALSE:
				CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[2], GL_FALSE, IF_GL_TEXTURE2);
				break;
			case GL_TEXTURE_2D:
				CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[2], GL_TRUE, IF_GL_TEXTURE2);
				break;
		}
		CHECK_ASSIGN(state->gl_state.textureunits.samplerType[2], pglist->fragment_state.tex.sampler_type[2], IF_GL_TEXTURE2);
	}
	
	pglist->link_update_status = GL_FALSE;
	
	if (__shman->current_program && __shman->current_program != pglist &&
		__shman->current_program->fragment_state.operation.mode != pglist->fragment_state.operation.mode)
		SET_MASK(state->upd_mask, IF_GL_FB_ACCESS);
	
	if (__shman->current_program && __shman->current_program->delete_status && __shman->current_program != pglist)
		shm_deleteProgramObject(__shman->current_program->id);
	
	__shman->current_program = pglist;
	state->gl_state.rendermode.currentProgram = program;
	state->gl_state.rendermode.bGeometryShaderUsed = pglist->geometry_shader_used;
	
	return;
}

int GL_APIENTRY glGetUniformLocation(GLuint program, const char* name){
	int location;
	unsigned index;
	pg_list_t* pglist;
	
	__GET_PG_LIST(program, pglist)
	BASE_GL_FAIL_IF_RET(pglist == 0, GL_INVALID_VALUE, -1);
	BASE_GL_FAIL_IF_RET(pglist->link_status == GL_FALSE, GL_INVALID_OPERATION, -1);
	
	for (index = 0; index < pglist->num_uniform; index++){
		char* uniform_name;
		
		if (pglist->geometry_shader_used){
			if (index < pglist->gs_uniform_offset)
				uniform_name = &pglist->bin_list->exe_infos[pglist->vs_exe_id].strings[pglist->uni_location_map[index].name_index];
			else if (index < pglist->fs_uniform_offset)
				uniform_name = &pglist->bin_list->exe_infos[pglist->gs_exe_id].strings[pglist->uni_location_map[index].name_index];
			else
				uniform_name = s_fixed_uniforms[index - pglist->fs_uniform_offset].name;
		}
		else{
			if (index < pglist->fs_uniform_offset)
				uniform_name = &pglist->bin_list->exe_infos[pglist->vs_exe_id].strings[pglist->uni_location_map[index].name_index];
			else
				uniform_name = s_fixed_uniforms[index - pglist->fs_uniform_offset].name;
		}
		
		if (strcmp(name, uniform_name) == 0)
			break;
	}
	
	if (index == pglist->num_uniform)
		location = -1;
	else
		location = pglist->uni_location_map[index].location.value;
	
	return location;
}

void GL_APIENTRY glBindAttribLocation(GLuint program, GLuint index, const char *name)
{
	pg_list_t* pglist;
	attrib_location_t* attribloc;
	int len;
	
	__GET_PG_LIST(program, pglist)
	
	BASE_GL_FAIL_IF(!pglist || index >= 16, GL_INVALID_VALUE);
	
	attribloc = (attrib_location_t*)malloc(sizeof(attrib_location_t));
	BASE_GL_FAIL_IF(!attribloc, GL_OUT_OF_MEMORY);
	
	len = (int)strlen(name);
	attribloc->name = (char*)malloc(len + 1);

	{
		attrib_location_t* tmp;

		for (tmp = pglist->attrib_location; tmp != 0; tmp = tmp->next){
			if (strcmp(tmp->name, name) == 0){
				free(attribloc->name);
				free(attribloc);
				tmp->index = index;
				break;
			}
		}

		if (tmp == 0){
			strcpy(attribloc->name, name);
			attribloc->index = index;
			attribloc->next = pglist->attrib_location;
			pglist->attrib_location = attribloc;
		}
	}
	
	return;
}


void GL_APIENTRY glUniform1i(GLint location, GLint x){
	shm_uniformiv(location, &x, 1, 1);
}


void GL_APIENTRY glUniform3i(GLint location, GLint x, GLint y, GLint z){
	GLint v[3];
	v[0] = x;
	v[1] = y;
	v[2] = z;
	
	shm_uniformiv(location, v, 3, 1);
	
	return;
}

void GL_APIENTRY glUniform3iv(GLint location, GLsizei count, const GLint* v){
	shm_uniformiv(location, v, 3, count);
}

static void shm_uniformiv(GLint location, const GLint* v, GLint size, GLint count){
	pg_list_t* curr_prog;
	static uni_location_t loc;
	
	if (location == -1 || count == 0)
		return;
	
	curr_prog = __shman->current_program;
	
	loc.location.value = (unsigned int)location;

	BASE_GL_FAIL_IF(curr_prog == 0 || (!(loc.location.common.fragment && loc.location.common.prog == MAX_SHM_IMPL_PROGRAMS)
		&& curr_prog->id != loc.location.common.prog), GL_INVALID_OPERATION);
	BASE_GL_FAIL_IF(count < 0, GL_INVALID_VALUE);
	
	if (loc.location.common.fragment == 0){
		int i;
		unsigned useroffset;
		GET_CURRENT_STATE(state);

		BASE_GL_FAIL_IF(curr_prog->fs_uniform_offset <= loc.location.vs.mapindex, GL_INVALID_OPERATION);
		useroffset = loc.location.vs.useroffset;
		loc = curr_prog->uni_location_map[loc.location.vs.mapindex];
		
		BASE_GL_FAIL_IF(loc.vs_ext.intbool == 0 || (loc.vs_ext.size + 1 != size) || (useroffset + count > loc.vs_ext.regsize), GL_INVALID_OPERATION);
		
		if (loc.vs_ext.size == 0){
			int regaddr;
			regaddr = loc.vs_ext.geometry ? RA_VP0_BOOL_REG : RA_VP1_BOOL_REG;
			for (i = 0; i < count; i++){
				if (v[i])
					curr_prog->ras_regs[regaddr] |= 1 << (loc.vs_ext.offset + useroffset + i);
				else
					curr_prog->ras_regs[regaddr] &= ~(1 << (loc.vs_ext.offset + useroffset + i));
			}
			if (state->forceMode)
				__shman->ras_regs[regaddr] = ~curr_prog->ras_regs[regaddr];
			SET_RASREGMASK(curr_prog->ras_reg_mask, regaddr);
		}
		else if (loc.vs_ext.size == 2)
		{
			int regaddr;
			for (i = 0; i < count; i++){
				regaddr = (loc.vs_ext.geometry ? RA_VP0_INT_REG_TOP_00 : RA_VP1_INT_REG_TOP_00) + loc.vs_ext.offset + useroffset + i;
				curr_prog->ras_regs[regaddr] = (v[0 + 3 * i] & 0xff) | ((v[1 + 3 * i] & 0xff) << 8) | ((v[2 + 3 * i] & 0xff) << 16);
				SET_RASREGMASK(curr_prog->ras_reg_mask, regaddr);
				if (state->forceMode)
					__shman->ras_regs[regaddr] = ~curr_prog->ras_regs[regaddr];
			}
		}
		else{
			__err_setError(GL_INVALID_OPERATION);
			return;
		}
		
		SET_MASK(state->upd_mask, IF_GL_SHADER_UNIFORM);
	}
	else{
		unsigned u;
		int i, id;
		GET_CURRENT_STATE(state);

		#define UPDATE_REG(_addr, _data, _be) \
		{ \
			curr_prog->ras_regs_be[_addr] |= ((_be) & 0xff ? 1 : 0) | ((_be) & 0xff00 ? 2 : 0) | ((_be) & 0xff0000 ? 4 : 0) | ((_be) & 0xff000000 ? 8 : 0); \
			if (state->forceMode) \
			{ \
				curr_prog->ras_regs[_addr] = (curr_prog->ras_regs[_addr] & ~(_be)) | ((_data) & (_be)); \
				SET_RASREGMASK(curr_prog->ras_reg_mask, (_addr)); \
				SET_MASK(state->upd_mask, IF_HW_RASTERIZER_REGISTER); \
				__shman->ras_regs[_addr] = ~curr_prog->ras_regs[_addr]; \
			} \
			else if ((curr_prog->ras_regs[_addr] & (_be)) != ((_data) & (_be))) \
			{ \
				curr_prog->ras_regs[_addr] = (curr_prog->ras_regs[_addr] & ~(_be)) | ((_data) & (_be)); \
				SET_RASREGMASK(curr_prog->ras_reg_mask, (_addr)); \
				SET_MASK(state->upd_mask, IF_HW_RASTERIZER_REGISTER); \
			} \
		}
		
		#define UPDATE_REG2(_addr, _data, _be, _flag) \
		{ \
			curr_prog->ras_regs_be[_addr] |= ((_be) & 0xff ? 1 : 0) | ((_be) & 0xff00 ? 2 : 0) | ((_be) & 0xff0000 ? 4 : 0) | ((_be) & 0xff000000 ? 8 : 0); \
			if (state->forceMode) \
			{ \
				curr_prog->ras_regs[_addr] = (curr_prog->ras_regs[_addr] & ~(_be)) | ((_data) & (_be)); \
				SET_RASREGMASK(curr_prog->ras_reg_mask, (_addr)); \
				SET_MASK(state->upd_mask, IF_HW_RASTERIZER_REGISTER); \
				SET_MASK(state->upd_mask, _flag); \
				__shman->ras_regs[_addr] = ~curr_prog->ras_regs[_addr]; \
			} \
			else if ((curr_prog->ras_regs[_addr] & (_be)) != ((_data) & (_be))) \
			{ \
				curr_prog->ras_regs[_addr] = (curr_prog->ras_regs[_addr] & ~(_be)) | ((_data) & (_be)); \
				SET_RASREGMASK(curr_prog->ras_reg_mask, (_addr)); \
				SET_MASK(state->upd_mask, IF_HW_RASTERIZER_REGISTER); \
				SET_MASK(state->upd_mask, _flag); \
			} \
		}
		
		#define UPDATE_REG3(_addr, _data, _be, _flag1, _flag2) \
		{ \
			curr_prog->ras_regs_be[_addr] |= ((_be) & 0xff ? 1 : 0) | ((_be) & 0xff00 ? 2 : 0) | ((_be) & 0xff0000 ? 4 : 0) | ((_be) & 0xff000000 ? 8 : 0); \
			if (state->forceMode) \
			{ \
				curr_prog->ras_regs[_addr] = (curr_prog->ras_regs[_addr] & ~(_be)) | ((_data) & (_be)); \
				SET_RASREGMASK(curr_prog->ras_reg_mask, (_addr)); \
				SET_MASK(state->upd_mask, IF_HW_RASTERIZER_REGISTER); \
				SET_MASK(state->upd_mask, _flag1); \
				SET_MASK(state->upd_mask, _flag2); \
				__shman->ras_regs[_addr] = ~curr_prog->ras_regs[_addr]; \
			} \
			else if ((curr_prog->ras_regs[_addr] & (_be)) != ((_data) & (_be))) \
			{ \
				curr_prog->ras_regs[_addr] = (curr_prog->ras_regs[_addr] & ~(_be)) | ((_data) & (_be)); \
				SET_RASREGMASK(curr_prog->ras_reg_mask, (_addr)); \
				SET_MASK(state->upd_mask, IF_HW_RASTERIZER_REGISTER); \
				SET_MASK(state->upd_mask, _flag1); \
				SET_MASK(state->upd_mask, _flag2); \
			} \
		}
		
		BASE_GL_FAIL_IF(loc.location.fs.size + 1 != size || count != 1, GL_INVALID_OPERATION);

		switch (loc.location.fs.offset){
			case SHADOW_PERSP:
				UPDATE_REG(RA_TU0_BEGIN_0B, (v[0] ? 0 : 1), 1);
				break;
			case TEXTURE_SAMPLER_MODE0:
				switch (v[0]){
					case GL_FALSE:
						CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[0], GL_FALSE, IF_GL_TEXTURE0);
						CHECK_ASSIGN(state->gl_state.textureunits.enabledCubeTex[0], GL_FALSE, IF_GL_TEXTURE0);
						break;
					case GL_TEXTURE_2D:
					case GL_TEXTURE_SHADOW_2D_DMP:
					case GL_TEXTURE_PROJECTION_DMP:
						CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[0], GL_TRUE, IF_GL_TEXTURE0);
						CHECK_ASSIGN(state->gl_state.textureunits.enabledCubeTex[0], GL_FALSE, IF_GL_TEXTURE0);
						break;
					case GL_TEXTURE_CUBE_MAP:
					case GL_TEXTURE_SHADOW_CUBE_DMP:
						CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[0], GL_FALSE, IF_GL_TEXTURE0);
						CHECK_ASSIGN(state->gl_state.textureunits.enabledCubeTex[0], GL_TRUE, IF_GL_TEXTURE0);
						break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				curr_prog->fragment_state.tex.sampler_type[0] = v[0];
				CHECK_ASSIGN(state->gl_state.textureunits.samplerType[0], v[0], IF_GL_TEXTURE0);
				break;
			case TEXTURE_SAMPLER_MODE1:
				switch (v[0]){
					case GL_FALSE:
						CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[1], GL_FALSE, IF_GL_TEXTURE1);
						break;
					case GL_TEXTURE_2D:
						CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[1], GL_TRUE, IF_GL_TEXTURE1);
						break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				curr_prog->fragment_state.tex.sampler_type[1] = v[0];
				CHECK_ASSIGN(state->gl_state.textureunits.samplerType[1], v[0], IF_GL_TEXTURE1);
				break;
			case TEXTURE_SAMPLER_MODE2:
				switch (v[0]){
					case GL_FALSE:
						CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[2], GL_FALSE, IF_GL_TEXTURE2);
						break;
					case GL_TEXTURE_2D:
						CHECK_ASSIGN(state->gl_state.textureunits.enabled2DTex[2], GL_TRUE, IF_GL_TEXTURE2);
						break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				curr_prog->fragment_state.tex.sampler_type[2] = v[0];
				CHECK_ASSIGN(state->gl_state.textureunits.samplerType[2], v[0], IF_GL_TEXTURE2);
				break;
			case TEXTURE_SAMPLER_MODE3:
				switch (v[0]){
					case GL_TEXTURE_PROCEDURAL_DMP:	u = 0x400;		break;
					case GL_FALSE:					u = 0;			break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG2(RA_TU0_BEGIN_00, u, 0x0400, IF_GL_PROCTEX_LUT_SAMPLERS);
				curr_prog->fragment_state.proctex.sampler_type = v[0];
				break;
			case TEXTURE_COORD2:
				switch (v[0]){
					case GL_TEXTURE1:	u = 0x2000;	break;
					case GL_TEXTURE2:	u = 0x0000;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_TU0_BEGIN_00, u, 0x2000);
				break;
			case TEXTURE_COORD3:
				switch (v[0]){
					case GL_TEXTURE0:	u = 0x0000;	break;
					case GL_TEXTURE1:	u = 0x0100;	break;
					case GL_TEXTURE2:	u = 0x0200;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_TU0_BEGIN_00, u, 0x300);
				break;
			case PROCTEX_1DMAPPING_RGB:
				switch (v[0]){
					case GL_PROCTEX_U_DMP:			u = 0;	break;
					case GL_PROCTEX_U2_DMP:			u = 1;	break;
					case GL_PROCTEX_V_DMP:			u = 2;	break;
					case GL_PROCTEX_V2_DMP:			u = 3;	break;
					case GL_PROCTEX_ADD_DMP:		u = 4;	break;
					case GL_PROCTEX_ADD2_DMP:		u = 5;	break;
					case GL_PROCTEX_ADDSQRT2_DMP:	u = 6;	break;
					case GL_PROCTEX_MIN_DMP:		u = 7;	break;
					case GL_PROCTEX_MAX_DMP:		u = 8;	break;
					case GL_PROCTEX_RMAX_DMP:		u = 9;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_PROC_TEX_M2REG_CONF_START_00, (u << 6), 0x3c0);
				break;
			case PROCTEX_1DMAPPING_A:
				switch (v[0]){
					case GL_PROCTEX_U_DMP:			u = 0;	break;
					case GL_PROCTEX_U2_DMP:			u = 1;	break;
					case GL_PROCTEX_V_DMP:			u = 2;	break;
					case GL_PROCTEX_V2_DMP:			u = 3;	break;
					case GL_PROCTEX_ADD_DMP:		u = 4;	break;
					case GL_PROCTEX_ADD2_DMP:		u = 5;	break;
					case GL_PROCTEX_ADDSQRT2_DMP:	u = 6;	break;
					case GL_PROCTEX_MIN_DMP:		u = 7;	break;
					case GL_PROCTEX_MAX_DMP:		u = 8;	break;
					case GL_PROCTEX_RMAX_DMP:		u = 9;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_PROC_TEX_M2REG_CONF_START_00, (u << 10), 0x3c00);
				break;
			case PROCTEX_ALPHA_SEPARATE:
				UPDATE_REG2(RA_PROC_TEX_M2REG_CONF_START_00, (v[0] ? 0x4000 : 0), 0x4000, IF_GL_PROCTEX_LUT_SAMPLERS);
				break;
			case PROCTEX_CLAMP_U:
				switch (v[0]){
					case GL_CLAMP_TO_ZERO_DMP:		u = 0;	break;
					case GL_CLAMP_TO_EDGE:			u = 1;	break;
					case GL_SYMMETRICAL_REPEAT_DMP:	u = 2;	break;
					case GL_MIRRORED_REPEAT:		u = 3;	break;
					case GL_PULSE_DMP:				u = 4;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_PROC_TEX_M2REG_CONF_START_00, u, 0x7);
				break;
			case PROCTEX_CLAMP_V:
				switch (v[0])
				{
					case GL_CLAMP_TO_ZERO_DMP:		u = 0;	break;
					case GL_CLAMP_TO_EDGE:			u = 1;	break;
					case GL_SYMMETRICAL_REPEAT_DMP:	u = 2;	break;
					case GL_MIRRORED_REPEAT:		u = 3;	break;
					case GL_PULSE_DMP:				u = 4;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_PROC_TEX_M2REG_CONF_START_00, (u << 3), 0x38);
				break;
			case PROCTEX_SHIFTU_ENABLE:
				switch (v[0]){
					case GL_NONE_DMP:	u = 0;	break;
					case GL_ODD_DMP:	u = 1;	break;
					case GL_EVEN_DMP:	u = 2;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_PROC_TEX_M2REG_CONF_START_00, (u << 16), 0x30000);
				break;
			case PROCTEX_SHIFTV_ENABLE:
				switch (v[0]){
					case GL_NONE_DMP:	u = 0;	break;
					case GL_ODD_DMP:	u = 1;	break;
					case GL_EVEN_DMP:	u = 2;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_PROC_TEX_M2REG_CONF_START_00, (u << 18), 0xc0000);
				break;
			case TEXTURE_MIN_FILTER:
				switch (v[0]){
					case GL_NEAREST:				u = 0;	break;
					case GL_LINEAR:					u = 1;	break;
					case GL_NEAREST_MIPMAP_NEAREST:	u = 2;	break;
					case GL_LINEAR_MIPMAP_NEAREST:	u = 3;	break;
					case GL_NEAREST_MIPMAP_LINEAR:	u = 4;	break;
					case GL_LINEAR_MIPMAP_LINEAR:	u = 5;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_PROC_TEX_M2REG_CONF_START_04, u, 0x7);
				break;
			case CR_TEXTURE_WIDTH:
				BASE_GL_FAIL_IF(v[0] > 128 || v[0] < 0, GL_INVALID_ENUM);
				UPDATE_REG(RA_PROC_TEX_M2REG_CONF_START_04, (v[0] << 11), 0x7F800);
				break;
			case CR_TEXTURE_OFFSET:
				BASE_GL_FAIL_IF(v[0] > 128 || v[0] < 0, GL_INVALID_ENUM);
				UPDATE_REG(RA_PROC_TEX_M2REG_CONF_START_05, v[0], 0x000000ff);
				break;
			case PROCTEX_ENABLE_NOISE:
				UPDATE_REG2(RA_PROC_TEX_M2REG_CONF_START_00, (v[0] ? 0x8000 : 0), 0x8000, IF_GL_PROCTEX_LUT_SAMPLERS);
				break;
			case PROCTEX_SAMPLER_RGB:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.proctex.samplers[0] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_PT_LUT_RGB_MAP] = 0;
				}
				else if (curr_prog->fragment_state.proctex.samplers[0] != v[0]){
					curr_prog->fragment_state.proctex.samplers[0] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
				}
				break;
			case PROCTEX_SAMPLER_ALPHA:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.proctex.samplers[1] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_PT_LUT_A_MAP] = 0;
				}
				else if (curr_prog->fragment_state.proctex.samplers[1] != v[0]){
					curr_prog->fragment_state.proctex.samplers[1] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
				}
				break;
			case PROCTEX_SAMPLER_NOISE:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.proctex.samplers[2] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_PT_LUT_NOISE_MAP] = 0;
				}
				else if (curr_prog->fragment_state.proctex.samplers[2] != v[0]){
					curr_prog->fragment_state.proctex.samplers[2] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
				}
				break;
			case PROCTEX_SAMPLER_R:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.proctex.samplers[3] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_PT_LUT_R] = 0;
				}
				else if (curr_prog->fragment_state.proctex.samplers[3] != v[0]){
					curr_prog->fragment_state.proctex.samplers[3] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
				}
				break;
			case PROCTEX_SAMPLER_G:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.proctex.samplers[4] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_PT_LUT_G] = 0;
				}
				else if (curr_prog->fragment_state.proctex.samplers[4] != v[0]){
					curr_prog->fragment_state.proctex.samplers[4] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
				}
				break;
			case PROCTEX_SAMPLER_B:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.proctex.samplers[5] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_PT_LUT_B] = 0;
				}
				else if (curr_prog->fragment_state.proctex.samplers[5] != v[0]){
					curr_prog->fragment_state.proctex.samplers[5] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
				}
				break;
			case PROCTEX_SAMPLER_A:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.proctex.samplers[6] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_PT_LUT_A] = 0;
				}
				else if (curr_prog->fragment_state.proctex.samplers[6] != v[0]){
					curr_prog->fragment_state.proctex.samplers[6] = v[0];
					SET_MASK(state->upd_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
				}
				break;
			case FRAGOP_MODE:
				switch (v[0])
				{
					case GL_FRAGOP_MODE_GL_DMP:			u = 0xE40000;	break;
					case GL_FRAGOP_MODE_GAS_ACC_DMP:	u = 0xE40001;	break;
					case GL_FRAGOP_MODE_SHADOW_DMP:		u = 0xE40003;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				curr_prog->fragment_state.operation.mode = v[0];
				UPDATE_REG(RA_CU_MODE, u, 0xffff00ff);
				SET_MASK(state->upd_mask, IF_GL_FB_ACCESS);
				break;
			case GAS_AUTO_ACCMAX:
				curr_prog->fragment_state.gas.autoacc = (v[0] == 0) ? GL_FALSE : GL_TRUE;
				break;
			case GAS_COLOR_LUT_INPUT:
				switch (v[0])
				{
					case GL_GAS_DENSITY_DMP:		u = 0;		break;
					case GL_GAS_LIGHT_FACTOR_DMP:	u = 0x100;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_GAS_LIGHT_DR, u, 0x100);
				break;
			case GAS_SHADING_DENSITY_SRC:
				switch (v[0])
				{
					case GL_GAS_PLAIN_DENSITY_DMP:	u = 0;		break;
					case GL_GAS_DEPTH_DENSITY_DMP:	u = 0x8;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_TB_FOG_EN, u, 0x8);
				break;
			case GAS_SAMPLER_RR:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.gas.samplers[0] = v[0];
					SET_MASK(state->upd_mask, IF_GL_GAS_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_GS_LUT_R] = 0;
				}
				else if (curr_prog->fragment_state.gas.samplers[0] != v[0]){
					curr_prog->fragment_state.gas.samplers[0] = v[0];
					SET_MASK(state->upd_mask, IF_GL_GAS_LUT_SAMPLERS);
				}
				break;
			case GAS_SAMPLER_RG:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.gas.samplers[1] = v[0];
					SET_MASK(state->upd_mask, IF_GL_GAS_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_GS_LUT_G] = 0;
				}
				else if (curr_prog->fragment_state.gas.samplers[1] != v[0]){
					curr_prog->fragment_state.gas.samplers[1] = v[0];
					SET_MASK(state->upd_mask, IF_GL_GAS_LUT_SAMPLERS);
				}
				break;
			case GAS_SAMPLER_RB:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.gas.samplers[2] = v[0];
					SET_MASK(state->upd_mask, IF_GL_GAS_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_GS_LUT_B] = 0;
				}
				else if (curr_prog->fragment_state.gas.samplers[2] != v[0]){
					curr_prog->fragment_state.gas.samplers[2] = v[0];
					SET_MASK(state->upd_mask, IF_GL_GAS_LUT_SAMPLERS);
				}
				break;
			case FRAGMENTLIGHTING_ENABLED:
				if (curr_prog->ras_regs[RA_TU0_BEGIN_0F] & 1){
					if (!v[0]){
						UPDATE_REG(RA_LR_FUNC_MODE, 0 << 4, 0xf << 4);
					}
				}
				else{
					if (v[0]){
						for (i = 0; i < 8; i++){
							if (s_layer_config_mask[i] == curr_prog->fragment_state.lighting.sampler_mask){
								switch (i){
									case 0: u = 0x0; break;
									case 1: u = 0x1; break;
									case 2: u = 0x2; break;
									case 3: u = 0x3; break;
									case 4: u = 0x4; break;
									case 5: u = 0x5; break;
									case 6: u = 0x6; break;
									case 7: u = 0x8; break;
								}
								break;
							}
						}
						UPDATE_REG(RA_LR_FUNC_MODE, u << 4, 0xf << 4);
					}
				}
				UPDATE_REG(RA_LR_MULTIPIPE, (!v[0]), 0xffffffff);
				UPDATE_REG2(RA_TU0_BEGIN_0F, (v[0] ? 1 : 0), 1, IF_GL_LIGHTING_LUT_SAMPLERS);
				break;
			case FRAGMENTLIGHTSOURCE_0_ENABLED: case FRAGMENTLIGHTSOURCE_1_ENABLED:
			case FRAGMENTLIGHTSOURCE_2_ENABLED: case FRAGMENTLIGHTSOURCE_3_ENABLED:
			case FRAGMENTLIGHTSOURCE_4_ENABLED: case FRAGMENTLIGHTSOURCE_5_ENABLED:
			case FRAGMENTLIGHTSOURCE_6_ENABLED: case FRAGMENTLIGHTSOURCE_7_ENABLED:
				id = loc.location.fs.offset - FRAGMENTLIGHTSOURCE_0_ENABLED;
				if (curr_prog->fragment_state.lights[id].enabled != v[0]){
					curr_prog->fragment_state.lights[id].enabled = (v[0] != 0) ? GL_TRUE : GL_FALSE;

					{
						unsigned remap, cnt;
						for (i = remap = cnt = 0; i < 8; i++){
							if (curr_prog->fragment_state.lights[i].enabled)
								remap |= i << (4 * cnt++);
						}
						UPDATE_REG(RA_LR_LIGHT_ENABLE, cnt ? cnt - 1 : 0, 0xffffffff);
						UPDATE_REG(RA_LR_LIGHTID, remap, 0xffffffff);
					}

					if (v[0] && (!(curr_prog->ras_regs[RA_LR_FUNC_MODE2] & (1 << (id + 8))) || !(curr_prog->ras_regs[RA_LR_FUNC_MODE2] & (1 << (id + 24))))){
						SET_MASK(state->upd_mask, IF_GL_LIGHTING_LUT_SAMPLERS);
					}
				}
				break;
			case FRAGMENTLIGHTSOURCE_0_TWOSIDEDIFFUSE: case FRAGMENTLIGHTSOURCE_1_TWOSIDEDIFFUSE:
			case FRAGMENTLIGHTSOURCE_2_TWOSIDEDIFFUSE: case FRAGMENTLIGHTSOURCE_3_TWOSIDEDIFFUSE:
			case FRAGMENTLIGHTSOURCE_4_TWOSIDEDIFFUSE: case FRAGMENTLIGHTSOURCE_5_TWOSIDEDIFFUSE:
			case FRAGMENTLIGHTSOURCE_6_TWOSIDEDIFFUSE: case FRAGMENTLIGHTSOURCE_7_TWOSIDEDIFFUSE:
				id = loc.location.fs.offset - FRAGMENTLIGHTSOURCE_0_TWOSIDEDIFFUSE;
				UPDATE_REG(RA_LR_L0_LIGHT_TYPE + (RA_LR_L1_LIGHT_TYPE - RA_LR_L0_LIGHT_TYPE) * id, (*v ? 2 : 0), 2);
				break;
			case FRAGMENTLIGHTSOURCE_0_GEOMFACTOR0: case FRAGMENTLIGHTSOURCE_1_GEOMFACTOR0:
			case FRAGMENTLIGHTSOURCE_2_GEOMFACTOR0: case FRAGMENTLIGHTSOURCE_3_GEOMFACTOR0:
			case FRAGMENTLIGHTSOURCE_4_GEOMFACTOR0: case FRAGMENTLIGHTSOURCE_5_GEOMFACTOR0:
			case FRAGMENTLIGHTSOURCE_6_GEOMFACTOR0: case FRAGMENTLIGHTSOURCE_7_GEOMFACTOR0:
				id = loc.location.fs.offset - FRAGMENTLIGHTSOURCE_0_GEOMFACTOR0;
				UPDATE_REG(RA_LR_L0_LIGHT_TYPE + (RA_LR_L1_LIGHT_TYPE - RA_LR_L0_LIGHT_TYPE) * id, (*v ? 4 : 0), 4);
				break;
			case FRAGMENTLIGHTSOURCE_0_GEOMFACTOR1: case FRAGMENTLIGHTSOURCE_1_GEOMFACTOR1:
			case FRAGMENTLIGHTSOURCE_2_GEOMFACTOR1: case FRAGMENTLIGHTSOURCE_3_GEOMFACTOR1:
			case FRAGMENTLIGHTSOURCE_4_GEOMFACTOR1: case FRAGMENTLIGHTSOURCE_5_GEOMFACTOR1:
			case FRAGMENTLIGHTSOURCE_6_GEOMFACTOR1: case FRAGMENTLIGHTSOURCE_7_GEOMFACTOR1:
				id = loc.location.fs.offset - FRAGMENTLIGHTSOURCE_0_GEOMFACTOR1;
				UPDATE_REG(RA_LR_L0_LIGHT_TYPE + (RA_LR_L1_LIGHT_TYPE - RA_LR_L0_LIGHT_TYPE) * id, (*v ? 8 : 0), 8);
				break;
			case FRAGMENTLIGHTSOURCE_0_SHADOWED: case FRAGMENTLIGHTSOURCE_1_SHADOWED:
			case FRAGMENTLIGHTSOURCE_2_SHADOWED: case FRAGMENTLIGHTSOURCE_3_SHADOWED:
			case FRAGMENTLIGHTSOURCE_4_SHADOWED: case FRAGMENTLIGHTSOURCE_5_SHADOWED:
			case FRAGMENTLIGHTSOURCE_6_SHADOWED: case FRAGMENTLIGHTSOURCE_7_SHADOWED:
				id = loc.location.fs.offset - FRAGMENTLIGHTSOURCE_0_SHADOWED;
				UPDATE_REG(RA_LR_FUNC_MODE2, (*v ? 0 : 1 << id), 1 << id);
				break;
			case FRAGMENTLIGHTSOURCE_0_SAMPLER_SP: case FRAGMENTLIGHTSOURCE_1_SAMPLER_SP:
			case FRAGMENTLIGHTSOURCE_2_SAMPLER_SP: case FRAGMENTLIGHTSOURCE_3_SAMPLER_SP:
			case FRAGMENTLIGHTSOURCE_4_SAMPLER_SP: case FRAGMENTLIGHTSOURCE_5_SAMPLER_SP:
			case FRAGMENTLIGHTSOURCE_6_SAMPLER_SP: case FRAGMENTLIGHTSOURCE_7_SAMPLER_SP:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				id = loc.location.fs.offset - FRAGMENTLIGHTSOURCE_0_SAMPLER_SP;
				if (state->forceMode){
					curr_prog->fragment_state.lights[id].spot_sampler = v[0];
					SET_MASK(state->upd_mask, IF_GL_LIGHTING_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_LR_LUT_SP0 + id] = 0;
				}
				else if (curr_prog->fragment_state.lights[id].spot_sampler != v[0]){
					curr_prog->fragment_state.lights[id].spot_sampler = v[0];
					SET_MASK(state->upd_mask, IF_GL_LIGHTING_LUT_SAMPLERS);
				}
				break;
			case FRAGMENTLIGHTSOURCE_0_SPOT_ENABLED: case FRAGMENTLIGHTSOURCE_1_SPOT_ENABLED:
			case FRAGMENTLIGHTSOURCE_2_SPOT_ENABLED: case FRAGMENTLIGHTSOURCE_3_SPOT_ENABLED:
			case FRAGMENTLIGHTSOURCE_4_SPOT_ENABLED: case FRAGMENTLIGHTSOURCE_5_SPOT_ENABLED:
			case FRAGMENTLIGHTSOURCE_6_SPOT_ENABLED: case FRAGMENTLIGHTSOURCE_7_SPOT_ENABLED:
				id = loc.location.fs.offset - FRAGMENTLIGHTSOURCE_0_SPOT_ENABLED;
				UPDATE_REG2(RA_LR_FUNC_MODE2, (v[0] ? 0 : 1 << (id + 8)), 1 << (id + 8), IF_GL_LIGHTING_LUT_SAMPLERS);
				break;
			case FRAGMENTLIGHTSOURCE_0_DA_ENABLED: case FRAGMENTLIGHTSOURCE_1_DA_ENABLED:
			case FRAGMENTLIGHTSOURCE_2_DA_ENABLED: case FRAGMENTLIGHTSOURCE_3_DA_ENABLED:
			case FRAGMENTLIGHTSOURCE_4_DA_ENABLED: case FRAGMENTLIGHTSOURCE_5_DA_ENABLED:
			case FRAGMENTLIGHTSOURCE_6_DA_ENABLED: case FRAGMENTLIGHTSOURCE_7_DA_ENABLED:
				id = loc.location.fs.offset - FRAGMENTLIGHTSOURCE_0_DA_ENABLED;
				UPDATE_REG2(RA_LR_FUNC_MODE2, (v[0] ? 0 : 1 << (id + 24)), 1 << (id + 24), IF_GL_LIGHTING_LUT_SAMPLERS);
				break;
			case FRAGMENTLIGHTSOURCE_0_SAMPLER_DA: case FRAGMENTLIGHTSOURCE_1_SAMPLER_DA:
			case FRAGMENTLIGHTSOURCE_2_SAMPLER_DA: case FRAGMENTLIGHTSOURCE_3_SAMPLER_DA:
			case FRAGMENTLIGHTSOURCE_4_SAMPLER_DA: case FRAGMENTLIGHTSOURCE_5_SAMPLER_DA:
			case FRAGMENTLIGHTSOURCE_6_SAMPLER_DA: case FRAGMENTLIGHTSOURCE_7_SAMPLER_DA:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				id = loc.location.fs.offset - FRAGMENTLIGHTSOURCE_0_SAMPLER_DA;
				if (state->forceMode){
					curr_prog->fragment_state.lights[id].da_sampler = v[0];
					SET_MASK(state->upd_mask, IF_GL_LIGHTING_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_LR_LUT_DA0 + id] = 0;
				}
				else if (curr_prog->fragment_state.lights[id].da_sampler != v[0]){
					curr_prog->fragment_state.lights[id].da_sampler = v[0];
					SET_MASK(state->upd_mask, IF_GL_LIGHTING_LUT_SAMPLERS);
				}
				break;
			case LIGHTENV_ABSLUTINPUT_D0:
			case LIGHTENV_ABSLUTINPUT_D1:
			case LIGHTENV_ABSLUTINPUT_SP:
			case LIGHTENV_ABSLUTINPUT_FR:
			case LIGHTENV_ABSLUTINPUT_RR:
			case LIGHTENV_ABSLUTINPUT_RG:
			case LIGHTENV_ABSLUTINPUT_RB:
				i = 4 * (loc.location.fs.offset - LIGHTENV_ABSLUTINPUT_D0) + 1;
				UPDATE_REG(RA_LR_TABLE_SEL, (*v ? 0 : 1 << i), 1 << i);
				break;
			case LIGHTENV_LUTINPUT_D0:
			case LIGHTENV_LUTINPUT_D1:
			case LIGHTENV_LUTINPUT_SP:
			case LIGHTENV_LUTINPUT_FR:
			case LIGHTENV_LUTINPUT_RR:
			case LIGHTENV_LUTINPUT_RG:
			case LIGHTENV_LUTINPUT_RB:
				BASE_GL_FAIL_IF(v[0] > GL_LIGHT_ENV_CP_DMP || v[0] < GL_LIGHT_ENV_NH_DMP, GL_INVALID_ENUM);
				i = 4 * (loc.location.fs.offset - LIGHTENV_LUTINPUT_D0);
				UPDATE_REG(RA_LR_SWT_CONF, ((*v - GL_LIGHT_ENV_NH_DMP) << i), 0x7 << i);
				break;
			case LIGHTING_SAMPLER_D0:
			case LIGHTING_SAMPLER_D1:
			case LIGHTING_SAMPLER_FR:
			case LIGHTING_SAMPLER_RB:
			case LIGHTING_SAMPLER_RG:
			case LIGHTING_SAMPLER_RR:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.lighting.samplers[loc.location.fs.offset - LIGHTING_SAMPLER_D0] = v[0];
					SET_MASK(state->upd_mask, IF_GL_LIGHTING_LUT_SAMPLERS);
					state->gl_state.lutbindings.bindings[LAP_LR_LUT_LR_D0 + loc.location.fs.offset - LIGHTING_SAMPLER_D0] = 0;
				}
				else if (curr_prog->fragment_state.lighting.samplers[loc.location.fs.offset - LIGHTING_SAMPLER_D0] != v[0]){
					curr_prog->fragment_state.lighting.samplers[loc.location.fs.offset - LIGHTING_SAMPLER_D0] = v[0];
					SET_MASK(state->upd_mask, IF_GL_LIGHTING_LUT_SAMPLERS);
				}
				break;
			case LIGHTENV_LUTENABLEDREFL:
				if (curr_prog->fragment_state.lighting.use_const_specular2 != !v[0])
				{
					UPDATE_REG2(RA_LR_FUNC_MODE2, (*v ? 0 : 7 << 20), 7 << 20, IF_GL_LIGHTING_LUT_SAMPLERS);
					curr_prog->fragment_state.lighting.use_const_specular2 = !v[0];
					
					for (i = 0; i < 8; i++){
						if (!*v)
							UPDATE_REG(RA_LR_L0_SPC1 + (RA_LR_L1_SPC1 - RA_LR_L0_SPC1) * i,
								(UTL_F2C_8(curr_prog->fragment_state.lights[i].specular2[2] * curr_prog->fragment_state.material.specular2[2]) |
								(UTL_F2C_8(curr_prog->fragment_state.lights[i].specular2[1] * curr_prog->fragment_state.material.specular2[1]) << 10) |
								(UTL_F2C_8(curr_prog->fragment_state.lights[i].specular2[0] * curr_prog->fragment_state.material.specular2[0]) << 20)), 0xffffffff)
						else
							UPDATE_REG(RA_LR_L0_SPC1 + (RA_LR_L1_SPC1 - RA_LR_L0_SPC1) * i,
								(UTL_F2C_8(curr_prog->fragment_state.lights[i].specular2[2]) |
								(UTL_F2C_8(curr_prog->fragment_state.lights[i].specular2[1]) << 10) |
								(UTL_F2C_8(curr_prog->fragment_state.lights[i].specular2[0]) << 20)), 0xffffffff);
					}
				}
				break;
			case LIGHTENV_BUMPSELECTOR:
				BASE_GL_FAIL_IF(v[0] > GL_TEXTURE3 || v[0] < GL_TEXTURE0, GL_INVALID_ENUM);
				UPDATE_REG(RA_LR_FUNC_MODE, ((*v - GL_TEXTURE0) << 22), 3 << 22);
				break;
			case LIGHTENV_SHADOWSELECTOR:
				BASE_GL_FAIL_IF(v[0] > GL_TEXTURE3 || v[0] < GL_TEXTURE0, GL_INVALID_ENUM);
				UPDATE_REG(RA_LR_FUNC_MODE, ((*v - GL_TEXTURE0) << 24), 3 << 24);
				break;
			case LIGHTENV_INVERTSHADOW:
				UPDATE_REG(RA_LR_FUNC_MODE, (*v ? 1 << 18 : 0), 1 << 18);
				break;
			case LIGHTENV_SHADOWPRIMARY:
				curr_prog->fragment_state.lighting.shadow_primary = *v ? GL_TRUE : GL_FALSE;
				UPDATE_REG(RA_LR_FUNC_MODE, (*v ? 1 << 16 : 0), 1 << 16);
				UPDATE_REG(RA_LR_FUNC_MODE,
					(curr_prog->fragment_state.lighting.shadow_primary ||
					curr_prog->fragment_state.lighting.shadow_secondary ||
					curr_prog->fragment_state.lighting.shadow_alpha ? 1 : 0), 1);
				break;
			case LIGHTENV_SHADOWSECONDARY:
				curr_prog->fragment_state.lighting.shadow_secondary = *v ? GL_TRUE : GL_FALSE;
				UPDATE_REG(RA_LR_FUNC_MODE, (*v ? 1 << 17 : 0), 1 << 17);
				UPDATE_REG(RA_LR_FUNC_MODE,
					(curr_prog->fragment_state.lighting.shadow_primary ||
					curr_prog->fragment_state.lighting.shadow_secondary ||
					curr_prog->fragment_state.lighting.shadow_alpha ? 1 : 0), 1);
				break;
			case LIGHTENV_SHADOWALPHA:
				curr_prog->fragment_state.lighting.shadow_alpha = *v ? GL_TRUE : GL_FALSE;
				UPDATE_REG(RA_LR_FUNC_MODE, (*v ? 1 << 19 : 0), 1 << 19);
				UPDATE_REG(RA_LR_FUNC_MODE,
					(curr_prog->fragment_state.lighting.shadow_primary ||
					curr_prog->fragment_state.lighting.shadow_secondary ||
					curr_prog->fragment_state.lighting.shadow_alpha ? 1 : 0), 1);
				break;
			case LIGHTENV_BUMPMODE:
				BASE_GL_FAIL_IF(v[0] > GL_LIGHT_ENV_BUMP_AS_TANG_DMP || v[0] < GL_LIGHT_ENV_BUMP_NOT_USED_DMP, GL_INVALID_ENUM);
				UPDATE_REG(RA_LR_FUNC_MODE, ((*v == GL_LIGHT_ENV_BUMP_NOT_USED_DMP || curr_prog->fragment_state.lighting.bump_renorm) ? 0 : (1 << 30))
					| ((*v - GL_LIGHT_ENV_BUMP_NOT_USED_DMP) << 28), 7 << 28);
				break;
			case LIGHTENV_CONFIG:
				switch (*v){
					case GL_LIGHT_ENV_LAYER_CONFIG0_DMP: u = 0x0; break;
					case GL_LIGHT_ENV_LAYER_CONFIG1_DMP: u = 0x1; break;
					case GL_LIGHT_ENV_LAYER_CONFIG2_DMP: u = 0x2; break;
					case GL_LIGHT_ENV_LAYER_CONFIG3_DMP: u = 0x3; break;
					case GL_LIGHT_ENV_LAYER_CONFIG4_DMP: u = 0x4; break;
					case GL_LIGHT_ENV_LAYER_CONFIG5_DMP: u = 0x5; break;
					case GL_LIGHT_ENV_LAYER_CONFIG6_DMP: u = 0x6; break;
					case GL_LIGHT_ENV_LAYER_CONFIG7_DMP: u = 0x8; break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				curr_prog->fragment_state.lighting.sampler_mask = s_layer_config_mask[v[0] - GL_LIGHT_ENV_LAYER_CONFIG0_DMP];
				if (curr_prog->ras_regs[RA_TU0_BEGIN_0F] & 1){
					UPDATE_REG(RA_LR_FUNC_MODE, u << 4, 0xf << 4);
				}
				break;
			case LIGHTENV_FRESNELSELECTOR:
				BASE_GL_FAIL_IF(v[0] > GL_LIGHT_ENV_PRI_SEC_ALPHA_FRESNEL_DMP || v[0] < GL_LIGHT_ENV_NO_FRESNEL_DMP, GL_INVALID_ENUM);
				UPDATE_REG(RA_LR_FUNC_MODE, ((*v - GL_LIGHT_ENV_NO_FRESNEL_DMP) << 2), 3 << 2);
				UPDATE_REG2(RA_LR_FUNC_MODE2, ((*v - GL_LIGHT_ENV_NO_FRESNEL_DMP) ? 0 : 1 << 19), 1 << 19, IF_GL_LIGHTING_LUT_SAMPLERS);
				break;
			case LIGHTENV_BUMPRENORM:
				{
					unsigned bumpmode = (curr_prog->ras_regs[RA_LR_FUNC_MODE] >> 28) & 3;
					UPDATE_REG(RA_LR_FUNC_MODE, ((*v || !bumpmode) ? 0 : 1 << 30), 1 << 30);
					curr_prog->fragment_state.lighting.bump_renorm = *v ? GL_TRUE : GL_FALSE;
				}
				break;
			case LIGHTENV_CLAMPHIGHLIGHTS:
				UPDATE_REG(RA_LR_FUNC_MODE, (*v ? 1 << 27 : 0), 1 << 27);
				break;
			case LIGHTENV_LUTENABLEDD0:
				UPDATE_REG2(RA_LR_FUNC_MODE2, (*v ? 0 : 1 << 16), 1 << 16, IF_GL_LIGHTING_LUT_SAMPLERS);
				break;
			case LIGHTENV_LUTENABLEDD1:
				UPDATE_REG2(RA_LR_FUNC_MODE2, (*v ? 0 : 1 << 17), 1 << 17, IF_GL_LIGHTING_LUT_SAMPLERS);
				break;
			case TEXENV0_COMBINE_RGB:
			case TEXENV1_COMBINE_RGB:
			case TEXENV2_COMBINE_RGB:
			case TEXENV3_COMBINE_RGB:
			case TEXENV4_COMBINE_RGB:
			case TEXENV5_COMBINE_RGB:
				id = loc.location.fs.offset - TEXENV0_COMBINE_RGB;
				switch (*v){
					case GL_REPLACE:		u = 0;	break;
					case GL_MODULATE:		u = 1;	break;
					case GL_ADD:			u = 2;	break;
					case GL_ADD_SIGNED:		u = 3;	break;
					case GL_INTERPOLATE:	u = 4;	break;
					case GL_SUBTRACT:		u = 5;	break;
					case GL_DOT3_RGB:		u = 6;	break;
					case GL_DOT3_RGBA:		u = 7;	break;
					case GL_MULT_ADD_DMP:	u = 8;	break;
					case GL_ADD_MULT_DMP:	u = 9;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_TB0_BU_OP + id * (RA_TB1_BU_OP - RA_TB0_BU_OP), u, 0xf);
				break;
			case TEXENV0_COMBINE_ALPHA:
			case TEXENV1_COMBINE_ALPHA:
			case TEXENV2_COMBINE_ALPHA:
			case TEXENV3_COMBINE_ALPHA:
			case TEXENV4_COMBINE_ALPHA:
			case TEXENV5_COMBINE_ALPHA:
				id = loc.location.fs.offset - TEXENV0_COMBINE_ALPHA;
				switch (*v){
					case GL_REPLACE:		u = 0;	break;
					case GL_MODULATE:		u = 1;	break;
					case GL_ADD:			u = 2;	break;
					case GL_ADD_SIGNED:		u = 3;	break;
					case GL_INTERPOLATE:	u = 4;	break;
					case GL_SUBTRACT:		u = 5;	break;
					case GL_DOT3_RGBA:		u = 7;	break;
					case GL_MULT_ADD_DMP:	u = 8;	break;
					case GL_ADD_MULT_DMP:	u = 9;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_TB0_BU_OP + id * (RA_TB1_BU_OP - RA_TB0_BU_OP), (u << 16), 0xf0000);
				break;
			case TEXENV0_SRC_RGB:
			case TEXENV1_SRC_RGB:
			case TEXENV2_SRC_RGB:
			case TEXENV3_SRC_RGB:
			case TEXENV4_SRC_RGB:
			case TEXENV5_SRC_RGB:
				id = loc.location.fs.offset - TEXENV0_SRC_RGB;
				{
					int prevnum = 0;
					int constnum = 0;
					int buffernum = 0;
					u = 0;
					for (i = 0; i < 3; i++){
						switch (v[i]){
							case GL_PRIMARY_COLOR:					u |= 0 << (i * 4);		break;
							case GL_FRAGMENT_PRIMARY_COLOR_DMP:		u |= 1 << (i * 4);		break;
							case GL_FRAGMENT_SECONDARY_COLOR_DMP:	u |= 2 << (i * 4);		break;
							case GL_TEXTURE0:						u |= 3 << (i * 4);		break;
							case GL_TEXTURE1:						u |= 4 << (i * 4);		break;
							case GL_TEXTURE2:						u |= 5 << (i * 4);		break;
							case GL_TEXTURE3:						u |= 6 << (i * 4);		break;
							case GL_PREVIOUS_BUFFER_DMP:
							                    buffernum++;	    u |= 0xd << (i * 4);	break;
							case GL_CONSTANT:	constnum++;			u |= 0xe << (i * 4);	break;
							case GL_PREVIOUS:	prevnum++;			u |= 0xf << (i * 4);	break;
							default:
								__err_setError(GL_INVALID_ENUM);
								return;
						}
					}
					BASE_GL_FAIL_IF(id ? (!prevnum && !constnum && !buffernum) : (prevnum || buffernum), GL_INVALID_ENUM);
					UPDATE_REG(RA_TB0_BU_ARG + id * (RA_TB1_BU_OP - RA_TB0_BU_OP), u, 0xfff);
				}
				break;
			case TEXENV0_SRC_ALPHA:
			case TEXENV1_SRC_ALPHA:
			case TEXENV2_SRC_ALPHA:
			case TEXENV3_SRC_ALPHA:
			case TEXENV4_SRC_ALPHA:
			case TEXENV5_SRC_ALPHA:
				id = loc.location.fs.offset - TEXENV0_SRC_ALPHA;
				{
					int prevnum = 0;
					int constnum = 0;
					int buffernum = 0;
					u = 0;
					for (i = 0; i < 3; i++){
						switch (v[i]){
							case GL_PRIMARY_COLOR:					u |= 0 << (i * 4 + 16);		break;
							case GL_FRAGMENT_PRIMARY_COLOR_DMP:		u |= 1 << (i * 4 + 16);		break;
							case GL_FRAGMENT_SECONDARY_COLOR_DMP:	u |= 2 << (i * 4 + 16);		break;
							case GL_TEXTURE0:						u |= 3 << (i * 4 + 16);		break;
							case GL_TEXTURE1:						u |= 4 << (i * 4 + 16);		break;
							case GL_TEXTURE2:						u |= 5 << (i * 4 + 16);		break;
							case GL_TEXTURE3:						u |= 6 << (i * 4 + 16);		break;
							case GL_PREVIOUS_BUFFER_DMP:
							                    buffernum++;		u |= 0xd << (i * 4 + 16);	break;
							case GL_CONSTANT:	constnum++;			u |= 0xe << (i * 4 + 16);	break;
							case GL_PREVIOUS:	prevnum++;			u |= 0xf << (i * 4 + 16);	break;
							default:
								__err_setError(GL_INVALID_ENUM);
								return;
						}
					}

					BASE_GL_FAIL_IF(id ? (!prevnum && !constnum && !buffernum) : (prevnum || buffernum), GL_INVALID_ENUM);
					UPDATE_REG(RA_TB0_BU_ARG + id * (RA_TB1_BU_OP - RA_TB0_BU_OP), u, 0xfff0000);
				}
				break;
			case TEXENV0_OPERAND_RGB:
			case TEXENV1_OPERAND_RGB:
			case TEXENV2_OPERAND_RGB:
			case TEXENV3_OPERAND_RGB:
			case TEXENV4_OPERAND_RGB:
			case TEXENV5_OPERAND_RGB:
				id = loc.location.fs.offset - TEXENV0_OPERAND_RGB;
				u = 0;
				for (i = 0; i < 3; i++){
					switch (v[i]){
						case GL_SRC_COLOR:				u |= 0x0 << (i * 4);	break;
						case GL_ONE_MINUS_SRC_COLOR:	u |= 0x1 << (i * 4);	break;
						case GL_SRC_ALPHA:				u |= 0x2 << (i * 4);	break;
						case GL_ONE_MINUS_SRC_ALPHA:	u |= 0x3 << (i * 4);	break;
						case GL_SRC_R_DMP:				u |= 0x4 << (i * 4);	break;
						case GL_SRC_G_DMP:				u |= 0x8 << (i * 4);	break;
						case GL_SRC_B_DMP:				u |= 0xc << (i * 4);	break;
						case GL_ONE_MINUS_SRC_R_DMP:	u |= 0x5 << (i * 4);	break;
						case GL_ONE_MINUS_SRC_G_DMP:	u |= 0x9 << (i * 4);	break;
						case GL_ONE_MINUS_SRC_B_DMP:	u |= 0xd << (i * 4);	break;
						default:
							__err_setError(GL_INVALID_ENUM);
							return;
					}
				}
				UPDATE_REG(RA_TB0_BU_FILTER + id * (RA_TB1_BU_OP - RA_TB0_BU_OP), u, 0xfff);
				break;
			case TEXENV0_OPERAND_ALPHA:
			case TEXENV1_OPERAND_ALPHA:
			case TEXENV2_OPERAND_ALPHA:
			case TEXENV3_OPERAND_ALPHA:
			case TEXENV4_OPERAND_ALPHA:
			case TEXENV5_OPERAND_ALPHA:
				id = loc.location.fs.offset - TEXENV0_OPERAND_ALPHA;
				u = 0;
				for (i = 0; i < 3; i++){
					switch (v[i]){
						case GL_SRC_ALPHA:				u |= 0x0 << (i * 4 + 12);	break;
						case GL_ONE_MINUS_SRC_ALPHA:	u |= 0x1 << (i * 4 + 12);	break;
						case GL_SRC_R_DMP:				u |= 0x2 << (i * 4 + 12);	break;
						case GL_SRC_G_DMP:				u |= 0x4 << (i * 4 + 12);	break;
						case GL_SRC_B_DMP:				u |= 0x6 << (i * 4 + 12);	break;
						case GL_ONE_MINUS_SRC_R_DMP:	u |= 0x3 << (i * 4 + 12);	break;
						case GL_ONE_MINUS_SRC_G_DMP:	u |= 0x5 << (i * 4 + 12);	break;
						case GL_ONE_MINUS_SRC_B_DMP:	u |= 0x7 << (i * 4 + 12);	break;
						default:
							__err_setError(GL_INVALID_ENUM);
							return;
					}
				}
				UPDATE_REG(RA_TB0_BU_FILTER + id * (RA_TB1_BU_OP - RA_TB0_BU_OP), u, 0x777000);
				break;
			case TEXENV1_BUFFER_INPUT:
			case TEXENV2_BUFFER_INPUT:
			case TEXENV3_BUFFER_INPUT:
			case TEXENV4_BUFFER_INPUT:
				id = loc.location.fs.offset - TEXENV1_BUFFER_INPUT;
				switch (v[0]){
					case GL_PREVIOUS:					u = 1 << (id + 8);	break;
					case GL_PREVIOUS_BUFFER_DMP:		u = 0 << (id + 8);	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				switch (v[1]){
					case GL_PREVIOUS:					u |= 1 << (id + 12);	break;
					case GL_PREVIOUS_BUFFER_DMP:		u |= 0 << (id + 12);	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_TB_FOG_EN, u, 0x1100 << id);
				break;
			case FOG_MODE:
				switch (v[0]){
					case GL_FOG:
						UPDATE_REG2(RA_TB_FOG_EN, 5, 0x7, IF_GL_FOG_LUT_SAMPLER);
						break;
					case GL_GAS_DMP:
						UPDATE_REG3(RA_TB_FOG_EN, 7, 0x7, IF_GL_GAS_LUT_SAMPLERS, IF_GL_FOG_LUT_SAMPLER);
						break;
					case GL_FALSE:
						UPDATE_REG(RA_TB_FOG_EN, 0, 0x7);
						break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				break;
			case FOG_ZFLIP:
				UPDATE_REG(RA_TB_FOG_EN, ((v[0]) ? 0x10000 : 0), 0x1 << 16);
				break;
			case FOG_SAMPLER:
				BASE_GL_FAIL_IF(v[0] >= 32 || v[0] < 0, GL_INVALID_VALUE);
				if (state->forceMode){
					curr_prog->fragment_state.fog.sampler = v[0];
					SET_MASK(state->upd_mask, IF_GL_FOG_LUT_SAMPLER);
					state->gl_state.lutbindings.bindings[LAP_FG_LUT_FOG] = 0;
				}
				else if (curr_prog->fragment_state.fog.sampler != v[0]){
					curr_prog->fragment_state.fog.sampler = v[0];
					SET_MASK(state->upd_mask, IF_GL_FOG_LUT_SAMPLER);
				}
				break;
			case FRAGOP_UCLIP_ENABLE:
				UPDATE_REG(RA_TS_USRCLIPENABLE, v[0] ? 1 : 0, 1);
				break;
			case FRAGOP_ATEST_ENABLE:
				UPDATE_REG(RA_CU_ALPHA_T, v[0] ? 1 : 0, 1);
				break;
			case FRAGOP_ATEST_FUNC:
				switch (v[0]){
					case GL_NEVER:		u = 0;	break;
					case GL_ALWAYS:		u = 1;	break;
					case GL_EQUAL:		u = 2;	break;
					case GL_NOTEQUAL:	u = 3;	break;
					case GL_LESS:		u = 4;	break;
					case GL_LEQUAL:		u = 5;	break;
					case GL_GREATER:	u = 6;	break;
					case GL_GEQUAL:		u = 7;	break;
					default:
						__err_setError(GL_INVALID_ENUM);
						return;
				}
				UPDATE_REG(RA_CU_ALPHA_T, u << 4, 0x70);
				break;
			default:
				__err_setError(GL_INVALID_OPERATION);
				return;
		}
		#undef UPDATE_REG
		#undef UPDATE_REG2
		#undef UPDATE_REG3
	}
	
	return;
}

void __shv_validateShaderValidator(bit_mask_t* _mask){
	GLboolean offlight = GL_FALSE;
	GLboolean tableupdate = GL_FALSE;
	pg_list_t* curr_prog;
	GET_CURRENT_STATE(state);
	if (GET_MASK(*_mask, IF_GL_FB_SCISSOR) && !(state->suppressStateMask & NN_GX_STATE_SCISSOR)){
		GLint x, y, x2, y2, attr;
		if (!state->gl_state.per_frag_op.scissorTestEnabled){
			x = y = 0;
			x2 = state->gl_state.fb_state.width - 1;
			y2 = state->gl_state.fb_state.height - 1;
			attr = 0;
		}
		else{
			x = state->gl_state.per_frag_op.scissorLeft;
			y = state->gl_state.per_frag_op.scissorBottom;
			x2 = state->gl_state.per_frag_op.scissorWidth + x - 1;
			y2 = state->gl_state.per_frag_op.scissorHeight + y - 1;
			if (x >= state->gl_state.fb_state.width)
				x = state->gl_state.fb_state.width - 1;
			else if (x < 0)
				x = 0;
			if (y >= state->gl_state.fb_state.height)
				y = state->gl_state.fb_state.height - 1;
			else if (y < 0)
				y = 0;
			if (x2 > state->gl_state.fb_state.width)
				x2 = state->gl_state.fb_state.width - 1;
			else if (x2 < 0)
				x2 = 0;
			if (y2 > state->gl_state.fb_state.height)
				y2 = state->gl_state.fb_state.height - 1;
			else if (y2 < 0)
				y2 = 0;
			attr = 3;
		}
		__cb_writeReg(PA_RW_TI_SCISSORING_ATTR, attr, 0xf);
		__cb_writeReg(PA_RW_TI_SCISSORING_POS0, x | (y << 16), 0xf);
		__cb_writeReg(PA_RW_TI_SCISSORING_POS1, x2 | (y2 << 16), 0xf);
	}
	if (!__shman->current_program)
		return;
	curr_prog = __shman->current_program;
	if (!(state->suppressStateMask & NN_GX_STATE_SHADERMODE)){
		int mode = curr_prog->geometry_shader_used ? 1 : 0;
		if (mode != s_vp_mode || (state->forceValidateMask & NN_GX_STATE_SHADERMODE)){
			__cb_addDummyWrite(0x251, 10);
			__cb_addDummyWrite(0x200, 30);
			__cb_writeReg(PA_RW_CI_VP_MODE, mode ? 2 : 0, 0x1);
			__cb_addDummyWrite(0x200, 30);
			s_vp_mode = mode;
			if (mode)
				__cb_writeReg(PA_RW_LB_VP0_WRITE_DISABLE, 1, 1);
			else
				__cb_writeReg(PA_RW_LB_VP0_WRITE_DISABLE, 0, 1);
		}
	}
	if (curr_prog->geometry_shader_used){
		curr_prog->ras_regs[RA_VP0_BOOL_REG] &= ~(1 << 15);
		__shman->ras_regs[RA_VP0_BOOL_REG] |= 1 << 15;
		curr_prog->ras_regs_be[RA_VP0_BOOL_REG] |= 2;
		SET_RASREGMASK(curr_prog->ras_reg_mask, RA_VP0_BOOL_REG);
		BASE_GL_FAIL_IF(state->gl_state.rendermode.drawarray && curr_prog->bin_list->exe_infos[curr_prog->gs_exe_id].geometry_data_mode != 0, GL_INVALID_OPERATION);
		if (GET_MASK(*_mask, IF_GL_ATTACH_GS) && !(state->suppressStateMask & NN_GX_STATE_SHADERMODE))
		{
			unsigned i;
			for (i = RA_VP0_BOOL_REG; i <= RA_VP0_OUTPUT_MASK; i++)
				__shman->ras_regs[i] = ~curr_prog->ras_regs[i];
		}
	}
	if (GET_MASK(*_mask, IF_GL_BINARY_PACKAGE) && !(state->suppressStateMask & NN_GX_STATE_SHADERBINARY)){
		if (curr_prog->geometry_shader_used){
			__cb_writeReg(PA_RW_VP_PRAM_ADDR, 0, 0xf);
			__cb_multiWriteReg(PA_RW_VP_PRAM_TOP, curr_prog->bin_list->num_program, curr_prog->bin_list->program);
			__cb_writeReg(PA_RW_VP_PROGRAM_RENEWAL, 1, 0xf);
			__cb_writeReg(PA_RW_VP_SWIZZLE_ADDR, 0, 0xf);
			__cb_multiWriteReg(PA_RW_VP_SWIZZLE_TOP, curr_prog->bin_list->num_swizzle, curr_prog->bin_list->swizzle);
		}
		else{
			if (curr_prog->bin_list->num_program > 512){
				__cb_writeReg(PA_RW_VP_PRAM_ADDR, 512, 0xf);
				__cb_multiWriteReg(PA_RW_VP_PRAM_TOP, curr_prog->bin_list->num_program - 512, &curr_prog->bin_list->program[512]);
				__cb_writeReg(PA_RW_VP_PROGRAM_RENEWAL, 1, 0xf);
			}
		}
		__cb_writeReg(PA_RW_VP_PRAM_ADDR + GEO_VP_LEN, 0, 0xf);
		__cb_multiWriteReg(PA_RW_VP_PRAM_TOP + GEO_VP_LEN,
			(curr_prog->bin_list->num_program > 512) ? 512 : curr_prog->bin_list->num_program, curr_prog->bin_list->program);
		__cb_writeReg(PA_RW_VP_PROGRAM_RENEWAL + GEO_VP_LEN, 1, 0xf);
		__cb_writeReg(PA_RW_VP_SWIZZLE_ADDR + GEO_VP_LEN, 0, 0xf);
		__cb_multiWriteReg(PA_RW_VP_SWIZZLE_TOP + GEO_VP_LEN, curr_prog->bin_list->num_swizzle, curr_prog->bin_list->swizzle);
	}
	if (GET_MASK(*_mask, IF_GL_DETACH_GS) && !(state->suppressStateMask & NN_GX_STATE_SHADERMODE)){
		unsigned i;
		for (i = RA_VP1_BOOL_REG; i <= RA_VP1_OUTPUT_MASK; i++)
			__shman->ras_regs[i] = ~curr_prog->ras_regs[i];
	}
	if ((GET_MASK(*_mask, IF_GL_BINARY_EXE_VS) || GET_MASK(*_mask, IF_GL_BINARY_EXE_GS)) && !(state->suppressStateMask & NN_GX_STATE_SHADERFLOAT)){
		unsigned i;
		exe_info_t* exe_info;
		if (curr_prog->geometry_shader_used){
			if (GET_MASK(*_mask, IF_GL_BINARY_EXE_GS)){
				exe_info = &curr_prog->bin_list->exe_infos[curr_prog->gs_exe_id];
				for (i = 0; i < exe_info->num_float_const; i++)
					__cb_writeRegs(PA_RW_VP_CONST_REG_ADDR, 4, (unsigned*)&exe_info->float_consts[i]);
			}
		}
		if (GET_MASK(*_mask, IF_GL_BINARY_EXE_VS)){
			exe_info = &curr_prog->bin_list->exe_infos[curr_prog->vs_exe_id];
			for (i = 0; i < exe_info->num_float_const; i++)
				__cb_writeRegs(PA_RW_VP_CONST_REG_ADDR + GEO_VP_LEN, 4, (unsigned*)&exe_info->float_consts[i]);
		}
	}
	if (!(state->suppressStateMask & NN_GX_STATE_VERTEX)){
		if (GET_MASK(*_mask, IF_GL_VERT_ARRAY) || GET_MASK(*_mask, IF_GL_PROGRAM) || GET_MASK(*_mask, IF_GL_VERTBUFFER)){
			int i;
			GLboolean same_prog, map_match;
			int const_list[MAX_VERTEX_ATTRIBS];
			int const_attr_count, arr_attr_count;
			unsigned const_flag;
			unsigned maxaddr, minaddr;
			attrib_addr_map_t addrmap[MAX_VERTEX_ATTRIBS];
			attrib_addr_map_t* addrmap_top = &addrmap[0];
			same_prog = __shman->last_validated_program == curr_prog ? GL_TRUE : GL_FALSE;
			__shman->last_validated_program = curr_prog;
			state->hw_state.hw_shader.useVertexBuffers = GL_TRUE;
			addrmap[0].next = 0;
			const_attr_count = arr_attr_count = 0;
			const_flag = 0;
			maxaddr = minaddr = 0;
			for (i = MAX_VERTEX_ATTRIBS - 1; i >= 0; i--){
				if (curr_prog->attribs[i].reg_index == -1)
					continue;
				if (!state->gl_state.vertarray.array[i].enabled){
					const_list[const_attr_count++] = i;
					const_flag |= 1 << i;
				}
				else{
					if (state->gl_state.vertarray.array[i].buffer == 0)
						state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
					else
						maxaddr = maxaddr < (unsigned)state->hw_state.hw_arrayproc.attrArrOffset[i] ? state->hw_state.hw_arrayproc.attrArrOffset[i] : maxaddr;
					addrmap[arr_attr_count].attr_index = i;
					if (arr_attr_count){
						if (state->hw_state.hw_arrayproc.attrArrOffset[i] <= state->hw_state.hw_arrayproc.attrArrOffset[addrmap_top->attr_index]){
							addrmap[arr_attr_count].next = addrmap_top;
							addrmap_top = &addrmap[arr_attr_count];
						}
						else{
							attrib_addr_map_t* addrmap_tmp = addrmap_top;
							attrib_addr_map_t* addrmap_next = addrmap_top->next;
							for (; addrmap_next != 0;){
								if (state->hw_state.hw_arrayproc.attrArrOffset[i] <= state->hw_state.hw_arrayproc.attrArrOffset[addrmap_next->attr_index]){
									addrmap_tmp->next = &addrmap[arr_attr_count];
									addrmap[arr_attr_count].next = addrmap_next;
									break;
								}
								addrmap_tmp = addrmap_next;
								addrmap_next = addrmap_next->next;
							}
							if (addrmap_next == 0){
								addrmap_tmp->next = &addrmap[arr_attr_count];
								addrmap[arr_attr_count].next = 0;
							}
						}
					}
					arr_attr_count++;
				}
			}
			if (state->hw_state.hw_arrayproc.bUseDeletedBuffer || arr_attr_count + const_attr_count > MAX_BUFFERED_VERTEX_ATTRIBS)
				state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
			if (arr_attr_count){
				minaddr = state->hw_state.hw_arrayproc.baseAddr = state->hw_state.hw_arrayproc.attrArrOffset[addrmap_top->attr_index] & ~0xf;
				if (state->hw_state.hw_shader.useVertexBuffers){
					if (!state->gl_state.rendermode.drawarray){
						minaddr = minaddr > (unsigned)state->hw_state.hw_arrayproc.elmtArrOffset ? state->hw_state.hw_arrayproc.elmtArrOffset : minaddr;
						maxaddr = maxaddr < (unsigned)state->hw_state.hw_arrayproc.elmtArrOffset ? state->hw_state.hw_arrayproc.elmtArrOffset : maxaddr;
					}
					if ((maxaddr - minaddr) >= 0x10000000){
						BASE_GL_FAIL_IF_NORET(state->gl_state.rendermode.drawing, GL_INVALID_OPERATION);
						state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
					}
				}
			}
			else
				state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
			map_match = GL_TRUE;
			if (arr_attr_count != curr_prog->num_array_attr || const_attr_count != curr_prog->num_const_attr ||
				(state->forceValidateMask & NN_GX_STATE_VERTEX))
				map_match = GL_FALSE;
			else{
				attrib_addr_map_t* addrmap_curr = addrmap_top;
				attrib_addr_map_t* addrmap_prev = curr_prog->attr_addr_map_top;
				for (i = 0; i < arr_attr_count; i++){
					if (addrmap_curr->attr_index != addrmap_prev->attr_index ||
						(state->hw_state.hw_arrayproc.attrArrOffset[addrmap_curr->attr_index] - state->hw_state.hw_arrayproc.attrArrOffset[addrmap_top->attr_index]) !=
						(curr_prog->attr_offset[i] - curr_prog->attr_offset[0]) ||
						state->gl_state.vertarray.array[addrmap_curr->attr_index].stride != curr_prog->attr_stride[i])
					{
						map_match = GL_FALSE;
						break;
					}

					{
						unsigned char prev_typesize, curr_typesize;
						switch (state->gl_state.vertarray.array[addrmap_curr->attr_index].type){
							case GL_FLOAT:			curr_typesize = 3; break;
							case GL_SHORT:			curr_typesize = 2; break;
							case GL_UNSIGNED_BYTE:	curr_typesize = 1; break;
							default:				curr_typesize = 0; break;
						}
						curr_typesize |= (state->gl_state.vertarray.array[addrmap_curr->attr_index].size - 1) << 2;
						if (i < 8)
							prev_typesize = (curr_prog->ci_config[1] >> (i * 4)) & 0xf;
						else
							prev_typesize = (curr_prog->ci_config[2] >> ((i - 8) * 4)) & 0xf;
						if (curr_typesize != prev_typesize){
							map_match = GL_FALSE;
							break;
						}
					}
					addrmap_curr = addrmap_curr->next;
					addrmap_prev = addrmap_prev->next;
				}
				for (i = 0; i < const_attr_count; i++){
					if (const_list[i] != curr_prog->ci_const_list[i]){
						map_match = GL_FALSE;
						break;
					}
				}
			}
			if (map_match && state->hw_state.hw_shader.useVertexBuffers){
				GLboolean base_only_changed = same_prog;
				unsigned base;
				if (state->gl_state.rendermode.drawarray)
					base = state->hw_state.hw_arrayproc.baseAddr;
				else
					base = state->hw_state.hw_arrayproc.elmtArrOffset < state->hw_state.hw_arrayproc.baseAddr ?
						(state->hw_state.hw_arrayproc.elmtArrOffset & ~0xf) : state->hw_state.hw_arrayproc.baseAddr;
				if (base != state->hw_state.hw_arrayproc.baseAddr || curr_prog->ci_index_base || state->hw_state.hw_arrayproc.attrArrOffset[addrmap_top->attr_index] - base != curr_prog->ci_config[3]){
					unsigned offset = state->hw_state.hw_arrayproc.attrArrOffset[addrmap_top->attr_index] - base;
					for (i = 1; i < curr_prog->num_ci_array; i++)
						curr_prog->ci_config[i * 3 + 3] = curr_prog->ci_config[i * 3 + 3] - curr_prog->ci_config[3] + offset;
					curr_prog->ci_config[3] = offset;
					base_only_changed = GL_FALSE;
					if (base == state->hw_state.hw_arrayproc.baseAddr)
						curr_prog->ci_index_base = GL_FALSE;
				}
				state->hw_state.hw_arrayproc.baseAddr = base;
				curr_prog->ci_config[0] = base >> 3;
				if (base_only_changed)
					__cb_writeReg(PA_CI_BASE_ADDRESS, curr_prog->ci_config[0], 0xf);
				else{
					if (same_prog)
						__cb_writeRegs(PA_CI_BASE_ADDRESS, curr_prog->num_ci_array * 3 + 1, curr_prog->ci_config);
					else{
						__cb_writeRegs(PA_CI_BASE_ADDRESS, NUM_CI_ARRAY_REG, curr_prog->ci_config);
						__cb_writeReg(1 * GEO_VP_LEN + PA_RW_VP_INPUTMAP0, curr_prog->vs_attr_map[0], 0xf);
						__cb_writeReg(1 * GEO_VP_LEN + PA_RW_VP_INPUTMAP1, curr_prog->vs_attr_map[1], 0xf);
						if (curr_prog->geometry_shader_used)
						{
							__cb_writeReg(0 * GEO_VP_LEN + PA_RW_VP_INPUTMAP0, curr_prog->gs_attr_map[0], 0xf);
							__cb_writeReg(0 * GEO_VP_LEN + PA_RW_VP_INPUTMAP1, curr_prog->gs_attr_map[1], 0xf);
						}

						{
							attrib_addr_map_t *addrmap_curr;
							int j;
							addrmap_curr = curr_prog->attr_addr_map_top;
							state->hw_state.hw_shader.attrNumUsed = arr_attr_count;
							state->hw_state.hw_shader.attrNumTotalUsed = arr_attr_count + const_attr_count;
							for (i = 0; i < arr_attr_count; i++){
								state->hw_state.hw_shader.attrUseOrder[i] = addrmap_curr->attr_index;
								addrmap_curr = addrmap_curr->next;
							}
							for (j = 0; j < const_attr_count; j++, i++)
								state->hw_state.hw_shader.attrUseOrder[i] = const_list[j];
						}
					}
				}
			}
			else{
				{
					attrib_addr_map_t* addrmap_curr = addrmap_top;
					for (i = 0; i < arr_attr_count; i++){
						curr_prog->attr_addr_map[i].attr_index = addrmap_curr->attr_index;
						curr_prog->attr_offset[i] = state->hw_state.hw_arrayproc.attrArrOffset[addrmap_curr->attr_index];
						curr_prog->attr_stride[i] = state->gl_state.vertarray.array[addrmap_curr->attr_index].stride;
						if (i == arr_attr_count - 1)
							curr_prog->attr_addr_map[i].next = 0;
						else
						{
							curr_prog->attr_addr_map[i].next = &curr_prog->attr_addr_map[i + 1];
							addrmap_curr = addrmap_curr->next;
						}
					}
					for (i = 0; i < const_attr_count; i++)
						curr_prog->ci_const_list[i] = const_list[i];
					curr_prog->num_array_attr = arr_attr_count;
					curr_prog->num_const_attr = const_attr_count;
					curr_prog->attr_addr_map_top = &curr_prog->attr_addr_map[0];
					if (state->gl_state.vertbuffer.elmtArrayID == 0 && !state->gl_state.rendermode.drawarray)
						state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
				}
				if (state->gl_state.rendermode.drawarray){
					curr_prog->ci_config[0] = state->hw_state.hw_arrayproc.baseAddr >> 3;
					curr_prog->ci_index_base = GL_FALSE;
				}
				else{
					if (state->hw_state.hw_arrayproc.elmtArrOffset < state->hw_state.hw_arrayproc.baseAddr){
						state->hw_state.hw_arrayproc.baseAddr = state->hw_state.hw_arrayproc.elmtArrOffset & ~0xf;
						curr_prog->ci_index_base = GL_TRUE;
					}
					else
						curr_prog->ci_index_base = GL_FALSE;
					curr_prog->ci_config[0] = state->hw_state.hw_arrayproc.baseAddr >> 3;
				}
				if (state->hw_state.hw_shader.useVertexBuffers){
					unsigned int arr_comp_count;
					unsigned int arr_byte_count;
					unsigned int actual_byte_cont;
					unsigned int structure_align;
					attrib_addr_map_t *addrmap_curr, *addrmap_next;
					unsigned* ci_arr_reg;
					GLsizeiptr ArrayFirstVertEnd;
					stt_vert_array_state_t* glvstate;
					stt_hw_array_proc_state_t* hwvstate;
					glvstate = &state->gl_state.vertarray;
					hwvstate = &state->hw_state.hw_arrayproc;
					curr_prog->num_ci_array = 0;
					arr_comp_count = arr_byte_count = actual_byte_cont = 0;
					ci_arr_reg = &curr_prog->ci_config[3];
					for (i = 1; i < NUM_CI_ARRAY_REG; i++)
						curr_prog->ci_config[i] = 0;
					structure_align = 1;
					addrmap_curr = addrmap_top;
					for (i = 0; i < arr_attr_count; i++){
						stt_vert_arr_t* array;
						unsigned int attrSetting = 0;
						unsigned int entrySize = 0;
						GLboolean bNewArr;
						unsigned diff;
						unsigned int attrArrOffset;
						addrmap_next = addrmap_curr->next;
						array = &state->gl_state.vertarray.array[addrmap_curr->attr_index];
						attrArrOffset = hwvstate->attrArrOffset[addrmap_curr->attr_index] - hwvstate->baseAddr;
						switch (array->type){
							case GL_FLOAT:			attrSetting = 0x3; entrySize = 4; break;
							case GL_SHORT:			attrSetting = 0x2; entrySize = 2; break;
							case GL_UNSIGNED_BYTE:	attrSetting = 0x1; entrySize = 1; break;
							case GL_BYTE:			attrSetting = 0x0; entrySize = 1; break;
						}
						attrSetting |= (array->size - 1) << 2;
						if (i < 8)
							curr_prog->ci_config[1] |= attrSetting << (i * 4);
						else
							curr_prog->ci_config[2] |= attrSetting << ((i-8) * 4);
						if (!array->stride){
							entrySize *= array->size;
							ci_arr_reg[0] = attrArrOffset;
							ci_arr_reg[1] |= i ;
							ci_arr_reg[2] |= (entrySize<<16) | (1<<28);
							curr_prog->num_ci_array++;
							ci_arr_reg += 3;
						}
						else{
							structure_align = structure_align < entrySize ? entrySize : structure_align;
							actual_byte_cont = (actual_byte_cont + (entrySize - 1)) & ~(entrySize - 1);
							entrySize *= array->size;
							actual_byte_cont += entrySize;
							if (!arr_comp_count){
								ci_arr_reg[0] = attrArrOffset;
								arr_byte_count = array->stride;
								ArrayFirstVertEnd = array->stride + hwvstate->attrArrOffset[addrmap_curr->attr_index];
							}
							if (arr_comp_count < 8)
								ci_arr_reg[1] |= i << (arr_comp_count * 4);
							else
								ci_arr_reg[2] |= i << ((arr_comp_count - 8) * 4);
							arr_comp_count++ ;
							bNewArr = (i + 1) == arr_attr_count
									|| (hwvstate->attrArrOffset[addrmap_next->attr_index] - (unsigned)hwvstate->baseAddr) <= attrArrOffset
									|| array->stride != glvstate->array[addrmap_next->attr_index].stride
									|| ArrayFirstVertEnd <= hwvstate->attrArrOffset[addrmap_next->attr_index]
									|| (MAX_BUFFERED_VERTEX_ATTRIBS == arr_comp_count);
							diff = ((bNewArr ? (ci_arr_reg[0] + arr_byte_count) : (hwvstate->attrArrOffset[addrmap_next->attr_index] - hwvstate->baseAddr)) - attrArrOffset - entrySize) >> 2 ;
							if (MAX_BUFFERED_VERTEX_ATTRIBS - arr_comp_count < (diff >> 2) + ((diff & 3) ? 1 : 0)){
								bNewArr = 1;
								diff = (ci_arr_reg[0] + arr_byte_count - attrArrOffset - entrySize) >> 2 ;
								state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
								break;
							}
							else if (diff){
								actual_byte_cont = (actual_byte_cont + 3) & ~0x3;
								actual_byte_cont += diff * 4;
								if (diff & 3){
									if (arr_comp_count < 8)
										ci_arr_reg[1] |= (0xb + (diff & 3)) << (arr_comp_count * 4);
									else
										ci_arr_reg[2] |= (0xb + (diff & 3)) << ((arr_comp_count - 8) * 4);
									arr_comp_count++;
									diff &= ~3 ;
								}
								while (diff){
									if (arr_comp_count < 8)
										ci_arr_reg[1] |= 0xf << (arr_comp_count * 4);
									else
										ci_arr_reg[2] |= 0xf << ((arr_comp_count - 8) * 4);
									arr_comp_count++ ;
									diff -= 4;
								}
							}
							if (bNewArr){
								actual_byte_cont = (actual_byte_cont + (structure_align - 1)) & ~(structure_align - 1);
								if (actual_byte_cont != arr_byte_count){
									state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
									break;
								}
								ci_arr_reg[2] |= (arr_byte_count << 16) | (arr_comp_count << 28);
								structure_align = 1;
								curr_prog->num_ci_array++;
								ci_arr_reg += 3;
								actual_byte_cont = arr_comp_count = 0;
							}
						}
						addrmap_curr = addrmap_next;
					}
					if (state->hw_state.hw_shader.useVertexBuffers){
						for (i = 0; i < const_attr_count; i++)
							curr_prog->ci_config[2] |= 0x10000 << (i + arr_attr_count);
						if (arr_attr_count)
							curr_prog->ci_config[2] |= ((arr_attr_count + const_attr_count - 1) << 28);
						if (!state->gl_state.rendermode.drawarray && curr_prog->num_ci_array == MAX_BUFFERED_VERTEX_ATTRIBS)
							state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
					}
				}

				{
					int j;
					attrib_addr_map_t *addrmap_curr;
					state->hw_state.hw_shader.attrNumUsed = arr_attr_count;
					state->hw_state.hw_shader.attrNumTotalUsed = arr_attr_count + const_attr_count;
					addrmap_curr = curr_prog->attr_addr_map_top;
					curr_prog->vs_attr_map[0] = 0;
					curr_prog->vs_attr_map[1] = 0;
					curr_prog->gs_attr_map[0] = 0x76543210;
					curr_prog->gs_attr_map[1] = 0xfedcba98;
					for (i = 0; i < arr_attr_count; i++){
						if (i < 8)
							curr_prog->vs_attr_map[0] |= (curr_prog->attribs[addrmap_curr->attr_index].reg_index & 0xf) << (i * 4);
						else
							curr_prog->vs_attr_map[1] |= (curr_prog->attribs[addrmap_curr->attr_index].reg_index & 0xf) << ((i - 8) * 4);
						state->hw_state.hw_shader.attrUseOrder[i] = addrmap_curr->attr_index;
						addrmap_curr = addrmap_curr->next;
					}
					for (j = 0; j < const_attr_count; j++, i++){
						if (i < 8)
							curr_prog->vs_attr_map[0] |= (curr_prog->attribs[const_list[j]].reg_index & 0xf) << (i * 4);
						else
							curr_prog->vs_attr_map[1] |= (curr_prog->attribs[const_list[j]].reg_index & 0xf) << ((i - 8) * 4);
						state->hw_state.hw_shader.attrUseOrder[i] = const_list[j];
					}
				}
				__cb_writeRegs(PA_CI_BASE_ADDRESS, NUM_CI_ARRAY_REG, curr_prog->ci_config);
				__cb_writeReg(1 * GEO_VP_LEN + PA_RW_VP_INPUTMAP0, curr_prog->vs_attr_map[0], 0xf);
				__cb_writeReg(1 * GEO_VP_LEN + PA_RW_VP_INPUTMAP1, curr_prog->vs_attr_map[1], 0xf);
				if (curr_prog->geometry_shader_used){
					__cb_writeReg(0 * GEO_VP_LEN + PA_RW_VP_INPUTMAP0, curr_prog->gs_attr_map[0], 0xf);
					__cb_writeReg(0 * GEO_VP_LEN + PA_RW_VP_INPUTMAP1, curr_prog->gs_attr_map[1], 0xf);
				}
			}
		}
		if (state->hw_state.hw_shader.useVertexBuffers && (
			GET_MASK(*_mask, IF_GL_VERT_ARRAY) ||
			GET_MASK(*_mask, IF_GL_VERTBUFFER) ||
			GET_MASK(*_mask, IF_GL_PROGRAM) ||
			GET_MASK(*_mask, IF_GL_VERT_CURRENT))
			)
		{
			unsigned i;
			for (i = state->hw_state.hw_shader.attrNumUsed; i < state->hw_state.hw_shader.attrNumTotalUsed; i++){
				int pos = state->hw_state.hw_shader.attrUseOrder[i];
				__cb_writeReg(PA_RW_CI_CONST_ATTR_NUMBER, i, 0xf);
				__cb_writeReg(PA_RW_CI_CONST_ATTR_DATA0, state->gl_state.vertcurr.attrib24[pos][0], 0xf);
				__cb_writeReg(PA_RW_CI_CONST_ATTR_DATA1, state->gl_state.vertcurr.attrib24[pos][1], 0xf);
				__cb_writeReg(PA_RW_CI_CONST_ATTR_DATA2, state->gl_state.vertcurr.attrib24[pos][2], 0xf);
			}
		}
	}
	if (state->forceValidateMask){
		unsigned i;
		if (state->forceValidateMask & NN_GX_STATE_VSUNIFORM){
			curr_prog->vs_floats_update_mask[0] = curr_prog->vs_floats_update_mask[1] = curr_prog->vs_floats_update_mask[2] = 0xffffffff;
			for (i = 0; s_vsuniform_regs[i] != RA_LAST; i++){
				if (curr_prog->ras_regs_be[s_vsuniform_regs[i]]){
					__shman->ras_regs[s_vsuniform_regs[i]] = ~curr_prog->ras_regs[s_vsuniform_regs[i]];
					SET_RASREGMASK(curr_prog->ras_reg_mask, s_vsuniform_regs[i]);
				}
			}
			if (curr_prog->geometry_shader_used){
				curr_prog->gs_floats_update_mask[0] = curr_prog->gs_floats_update_mask[1] = curr_prog->gs_floats_update_mask[2] = 0xffffffff;
				for (i = 0; s_gsuniform_regs[i] != RA_LAST; i++){
					if (curr_prog->ras_regs_be[s_gsuniform_regs[i]]){
						__shman->ras_regs[s_gsuniform_regs[i]] = ~curr_prog->ras_regs[s_gsuniform_regs[i]];
						SET_RASREGMASK(curr_prog->ras_reg_mask, s_gsuniform_regs[i]);
					}
				}
			}
		}
		if (state->forceValidateMask & NN_GX_STATE_FSUNIFORM){
			for (i = 0; s_fsuniform_regs[i] != RA_LAST; i++){
				if (curr_prog->ras_regs_be[s_fsuniform_regs[i]]){
					__shman->ras_regs[s_fsuniform_regs[i]] = ~curr_prog->ras_regs[s_fsuniform_regs[i]];
					SET_RASREGMASK(curr_prog->ras_reg_mask, s_fsuniform_regs[i]);
				}
			}
		}
		if (state->forceValidateMask & NN_GX_STATE_SHADERPROGRAM){
			for (i = 0; s_shaderprog_regs[i] != RA_LAST; i++){
				if (curr_prog->ras_regs_be[s_shaderprog_regs[i]]){
					__shman->ras_regs[s_shaderprog_regs[i]] = ~curr_prog->ras_regs[s_shaderprog_regs[i]];
					SET_RASREGMASK(curr_prog->ras_reg_mask, s_shaderprog_regs[i]);
				}
			}
		}
	}
	if (!(state->suppressStateMask & NN_GX_STATE_VSUNIFORM) &&
		(curr_prog->vs_floats_update_mask[0] || curr_prog->vs_floats_update_mask[1] || curr_prog->vs_floats_update_mask[2] ||
		curr_prog->gs_floats_update_mask[0] || curr_prog->gs_floats_update_mask[1] || curr_prog->gs_floats_update_mask[2])){
		if (curr_prog->gs_floats && (curr_prog->gs_floats_update_mask[0] || curr_prog->gs_floats_update_mask[1] || curr_prog->gs_floats_update_mask[2])){
			float_const_reg_t* src = curr_prog->gs_floats;
			float_const_reg_t* batch = 0;
			unsigned batch_addr = 0;
			unsigned batch_size = 0;
			unsigned addr = 0;
			while (!(curr_prog->gs_floats_update_mask[addr >> 5] & (1 << (addr & 0x1f)))){
				if (!(curr_prog->gs_floats_update_mask[addr >> 5] >> (addr & 0x1f)))
					addr = 32 + (addr & ~0x1f);
				else
					addr++;
			}
			if (addr < curr_prog->num_gs_floats){
				do{
					unsigned act_addr = curr_prog->gs_floats_addr[addr];
					if (batch && batch_size != act_addr - batch_addr){
						__cb_writeReg(PA_RW_VP_CONST_REG_ADDR, batch_addr | 0x80000000, 0xf);
						__cb_multiWriteReg(PA_RW_VP_CONST_REG_TOP, 4 * batch_size, (unsigned*)batch);
						batch = 0;
					}
					else
						batch_size++;
					if (!batch){
						batch_addr = act_addr;
						batch = src + addr;
						batch_size = 1;
					}
					addr++;
					while (!(curr_prog->gs_floats_update_mask[addr >> 5] & (1 << (addr & 0x1f))) && addr < curr_prog->num_gs_floats){
						if (!(curr_prog->gs_floats_update_mask[addr >> 5] >> (addr & 0x1f)))
							addr = 32 + (addr & ~0x1f);
						else
							addr++;
					}
				} while (addr < curr_prog->num_gs_floats);
				if (batch)
				{
					__cb_writeReg(PA_RW_VP_CONST_REG_ADDR, batch_addr | 0x80000000, 0xf);
					__cb_multiWriteReg(PA_RW_VP_CONST_REG_TOP, 4 * batch_size, (unsigned*)batch);
				}
			}
		}
		if (curr_prog->vs_floats &&
			(curr_prog->vs_floats_update_mask[0] || curr_prog->vs_floats_update_mask[1] || curr_prog->vs_floats_update_mask[2]))
		{
			float_const_reg_t* src = curr_prog->vs_floats;
			float_const_reg_t* batch = 0;
			unsigned batch_addr = 0;
			unsigned batch_size = 0;
			unsigned addr = 0;
			while (!(curr_prog->vs_floats_update_mask[addr >> 5] & (1 << (addr & 0x1f))))
			{
				if (!(curr_prog->vs_floats_update_mask[addr >> 5] >> (addr & 0x1f)))
					addr = 32 + (addr & ~0x1f);
				else
					addr++;
			}
			if (addr < curr_prog->num_vs_floats)
			{
				do
				{
					unsigned act_addr = curr_prog->vs_floats_addr[addr];
					if (batch && batch_size != act_addr - batch_addr)
					{
						__cb_writeReg(PA_RW_VP_CONST_REG_ADDR + GEO_VP_LEN, batch_addr | 0x80000000, 0xf);
						__cb_multiWriteReg(PA_RW_VP_CONST_REG_TOP + GEO_VP_LEN, 4 * batch_size, (unsigned*)batch);
						batch = 0;
					}
					else
						batch_size++;
					if (!batch)
					{
						batch_addr = act_addr;
						batch = src + addr;
						batch_size = 1;
					}
					addr++;
					while (!(curr_prog->vs_floats_update_mask[addr >> 5] & (1 << (addr & 0x1f))) && addr < curr_prog->num_vs_floats)
					{
						if (!(curr_prog->vs_floats_update_mask[addr >> 5] >> (addr & 0x1f)))
							addr = 32 + (addr & ~0x1f);
						else
							addr++;
					}
				} while (addr < curr_prog->num_vs_floats);
				if (batch)
				{
					__cb_writeReg(PA_RW_VP_CONST_REG_ADDR + GEO_VP_LEN, batch_addr | 0x80000000, 0xf);
					__cb_multiWriteReg(PA_RW_VP_CONST_REG_TOP + GEO_VP_LEN, 4 * batch_size, (unsigned*)batch);
				}
			}
		}
		curr_prog->vs_floats_update_mask[0] = 0;
		curr_prog->vs_floats_update_mask[1] = 0;
		curr_prog->vs_floats_update_mask[2] = 0;
		curr_prog->gs_floats_update_mask[0] = 0;
		curr_prog->gs_floats_update_mask[1] = 0;
		curr_prog->gs_floats_update_mask[2] = 0;
	}
	#define UPDATE_REG(_addr, _data, _be) \
	{ \
		curr_prog->ras_regs_be[_addr] |= ((_be) & 0xff ? 1 : 0) | ((_be) & 0xff00 ? 2 : 0) | ((_be) & 0xff0000 ? 4 : 0) | ((_be) & 0xff000000 ? 8 : 0); \
		if (state->forceMode) \
		{ \
			curr_prog->ras_regs[_addr] = (curr_prog->ras_regs[_addr] & ~(_be)) | ((_data) & (_be)); \
			SET_RASREGMASK(curr_prog->ras_reg_mask, (_addr)); \
			__shman->ras_regs[_addr] = ~curr_prog->ras_regs[_addr]; \
		} \
		else if ((curr_prog->ras_regs[_addr] & (_be)) != ((_data) & (_be))) \
		{ \
			curr_prog->ras_regs[_addr] = (curr_prog->ras_regs[_addr] & ~(_be)) | ((_data) & (_be)); \
			SET_RASREGMASK(curr_prog->ras_reg_mask, (_addr)); \
		} \
	}
	if (GET_MASK(*_mask, IF_GL_TRI_OFFSET) && !(state->suppressStateMask & NN_GX_STATE_TRIOFFSET))
	{
		unsigned u, u2;
		GLfloat scale, offset;
		if (curr_prog->fragment_state.operation.wscale != 0)
		{
			scale = -curr_prog->fragment_state.operation.wscale;
			offset = 0.f;
		}
		else
		{
			scale = state->gl_state.trioffset.depthRangeN - state->gl_state.trioffset.depthRangeF;
			offset = state->gl_state.trioffset.depthRangeN;
		}
		if (state->gl_state.trioffset.enabled && state->gl_state.trioffset.u_offset != 0.f)
		{
			switch (state->gl_state.fb_state.z_format)
			{
				case ZF_Z16:
					offset += state->gl_state.trioffset.u_offset / 65535.f;
					break;
				case ZF_Z24:
				case ZF_Z24S8:
				default:
					offset += state->gl_state.trioffset.u_offset / 16777215.f;
					break;
			}
		}
		UTL_F2F_16M7E(scale, u);
		if (offset != 0.f)
		{
			UTL_F2F_16M7E(offset, u2);
		}
		else
			u2 = 0;
		UPDATE_REG(RA_TS_ZSCALE, u, 0xffffffff);
		UPDATE_REG(RA_TS_ZBIAS, u2, 0xffffffff);
	}
	#undef UPDATE_REG
	if ((curr_prog->ras_regs[RA_TB_FOG_EN] & 0x7) == 0x7)
	{
		if (curr_prog->fragment_state.gas.autoacc)
		{
			__shman->ras_regs[RA_TB_GAS_ACC_MAX] = curr_prog->ras_regs[RA_TB_GAS_ACC_MAX];
			CLEAR_RASREGMASK(curr_prog->ras_reg_mask, RA_TB_GAS_ACC_MAX);
		}
		else
		{
			__shman->ras_regs[RA_TB_GAS_ACC_MAX] = ~curr_prog->ras_regs[RA_TB_GAS_ACC_MAX];
			SET_RASREGMASK(curr_prog->ras_reg_mask, RA_TB_GAS_ACC_MAX);
		}
	}
	{
		unsigned addr = 0;
		unsigned* batch = 0;
		unsigned batch_addr = 0;
		unsigned batch_size = 0;
		int i, j;
		if (state->suppressStateMask)
		{
			if (state->suppressStateMask & NN_GX_STATE_FSUNIFORM)
				CLEAR_PARTIAL_RASREGMASK(curr_prog->ras_reg_mask, s_fsuniform_regmask);
			if (state->suppressStateMask & NN_GX_STATE_SHADERPROGRAM)
				CLEAR_PARTIAL_RASREGMASK(curr_prog->ras_reg_mask, s_shaderprog_regmask);
			if (state->suppressStateMask & NN_GX_STATE_VSUNIFORM)
				CLEAR_PARTIAL_RASREGMASK(curr_prog->ras_reg_mask, s_vsuniform_regmask);
		}
		if ((curr_prog->ras_regs[RA_TU0_BEGIN_0F] & 1) && !(state->suppressStateMask & NN_GX_STATE_FSUNIFORM))
		{
			for (i = 0; i < MAX_PER_PIXEL_LIGHTS; i++)
			{
				if (curr_prog->fragment_state.lights[i].enabled)
					break;
			}
			if (i == MAX_PER_PIXEL_LIGHTS)
			{
				CLEAR_RASREGMASK(curr_prog->ras_reg_mask, RA_LR_L0_SPC0);
				CLEAR_RASREGMASK(curr_prog->ras_reg_mask, RA_LR_L0_SPC1);
				CLEAR_RASREGMASK(curr_prog->ras_reg_mask, RA_LR_L0_DIFF);
				CLEAR_RASREGMASK(curr_prog->ras_reg_mask, RA_LR_L0_AMB);
				__shman->ras_regs[RA_LR_L0_SPC0] = 0;
				__shman->ras_regs[RA_LR_L0_SPC1] = 0;
				__shman->ras_regs[RA_LR_L0_DIFF] = 0;
				__shman->ras_regs[RA_LR_L0_AMB] = 0;
				__cb_fillRegs(PA_WO_LR_L0_SPC0, 4, 0);
				offlight = GL_TRUE;
				if ((curr_prog->ras_regs[RA_LR_FUNC_MODE] & 0xf0) != 0)
				{
					CLEAR_RASREGMASK(curr_prog->ras_reg_mask, RA_LR_FUNC_MODE);
					__shman->ras_regs[RA_LR_FUNC_MODE] = curr_prog->ras_regs[RA_LR_FUNC_MODE] & ~0xf0;
					__cb_writeReg(PA_WO_LR_FUNC_MODE, __shman->ras_regs[RA_LR_FUNC_MODE], 0xf);
				}
			}
		}
		for (i = 0; i <= (RA_LAST >> 5); i++)
		{
			if (curr_prog->ras_reg_mask.mask_[i])
			{
				for (j = 0; !(curr_prog->ras_reg_mask.mask_[i] & (1 << j)); j++);
				addr += j;
				break;
			}
			addr += 32;
		}
		if (addr < RA_LAST)
		{
			do
			{
				unsigned* src = curr_prog->ras_regs + addr;
				unsigned* dst = __shman->ras_regs + addr;
				unsigned char be = curr_prog->ras_regs_be[addr];
				unsigned act_addr = s_ras_addr_map[addr];
				if (be && *src != *dst)
				{
					if (0xf != be)
					{
						if (batch)
						{
							__cb_writeRegs(batch_addr, batch_size, batch);
							batch = 0;
						}
						__cb_writeReg(act_addr, *src, be) ;
					}
					else
					{
						if (batch && act_addr - batch_addr != batch_size)
						{
							__cb_writeRegs(batch_addr, batch_size, batch);
							batch = 0;
						}
						else
							batch_size++;
						if (!batch)
						{
							batch = src;
							batch_addr = act_addr;
							batch_size = 1;
						}
					}
					*dst = *src;
				}
				else if (batch)
				{
					__cb_writeRegs(batch_addr, batch_size, batch);
					batch = 0;
				}
				addr++;
				if (addr >= RA_LAST)
					break;
				else if (curr_prog->ras_reg_mask.mask_[addr >> 5] >> (addr & 0x1f))
				{
					for (; !(curr_prog->ras_reg_mask.mask_[addr >> 5] & (1 << (addr & 0x1f))); addr++);
				}
				else
				{
					addr = 32 + (addr & ~0x1f);
					for (i = (addr >> 5); i <= (RA_LAST >> 5); i++)
					{
						if (curr_prog->ras_reg_mask.mask_[i])
						{
							for (j = 0; !(curr_prog->ras_reg_mask.mask_[i] & (1 << j)); j++);
							addr += j;
							break;
						}
						addr += 32;
					}
				}
			}
			while (addr < RA_LAST);
			if (batch)
			{
				__cb_writeRegs(batch_addr, batch_size, batch);
			}
			CLEAR_ALL_RASREGMASK(curr_prog->ras_reg_mask);
		}
		if (offlight)
		{
			SET_RASREGMASK(curr_prog->ras_reg_mask, RA_LR_L0_SPC0);
			SET_RASREGMASK(curr_prog->ras_reg_mask, RA_LR_L0_SPC1);
			SET_RASREGMASK(curr_prog->ras_reg_mask, RA_LR_L0_DIFF);
			SET_RASREGMASK(curr_prog->ras_reg_mask, RA_LR_L0_AMB);
			if ((curr_prog->ras_regs[RA_LR_FUNC_MODE] & 0xf0) != 0)
				SET_RASREGMASK(curr_prog->ras_reg_mask, RA_LR_FUNC_MODE);
		}
	}
	if (!(state->suppressStateMask & NN_GX_STATE_LUT))
	{
		if ((curr_prog->ras_regs[RA_TU0_BEGIN_0F] & 1) && (GET_MASK(*_mask, IF_GL_TEXTURE_LUT) || GET_MASK(*_mask, IF_GL_LIGHTING_LUT_SAMPLERS)))
		{
			int i;
			static const int sampler_valid_bit[MAX_MATERIAL_LUTS] = {16, 17, 19, 20, 21, 22};
			static const int sampler_table_sel[MAX_MATERIAL_LUTS] = {0, 1, 3, 4, 5, 6};
			for (i = 0; i < MAX_MATERIAL_LUTS; i++)
			{
				GLuint tex;
				tx_lut_container_t* pTex;
				if (!((curr_prog->fragment_state.lighting.sampler_mask >> i) & 1) ||
					((curr_prog->ras_regs[RA_LR_FUNC_MODE2] >> sampler_valid_bit[i]) & 1))
					continue;
				if ((curr_prog->fragment_state.lighting.samplers[i] == -1) ||
					(tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.lighting.samplers[i]]) == 0 ||
					(pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.lighting.samplers[i])) == 0)
				{
					BASE_GL_FAIL_IF_NORET(!offlight, GL_INVALID_OPERATION);
					break;
				}
				if (tex == state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_START])
				{
					if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_START])
					{
						__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, (sampler_table_sel[i] << 8) | state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_START], 0xf);
						__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_START],
												&pTex->lr_plane[state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_START]]);
						state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_START] = 0;
					}
					continue;
				}
				if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_START])
					state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_START] = 0;
				state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_START] = tex;
				if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_LR))
				{
					int j;
					unsigned u;
					if (!pTex->lr_plane)
					{
						pTex->lr_plane = (GLuint*)malloc(MATERIAL_LUT_TABLE_SIZE * sizeof(GLuint));
						BASE_GL_FAIL_IF_BRK(!pTex->lr_plane, GL_OUT_OF_MEMORY);
					}
					for (j = 0; j < MATERIAL_LUT_TABLE_SIZE; j++)
					{
						UTL_F2UFX_12W_0I(*(pTex->src_plane + j), *(pTex->lr_plane + j));
						UTL_F2FX_12W_1I_F(*(pTex->src_plane + j + MATERIAL_LUT_TABLE_SIZE), u);
						pTex->lr_plane[j] |= u << 12;
					}
					pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_LR);
				}
				__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, sampler_table_sel[i] << 8, 0xf);
				__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, MATERIAL_LUT_TABLE_SIZE, pTex->lr_plane);
				tableupdate = GL_TRUE;
			}
			for (i = 0; i < MAX_PER_PIXEL_LIGHTS; i++)
			{
				GLuint tex;
				tx_lut_container_t* pTex;
				if (!(curr_prog->fragment_state.lights[i].enabled))
					continue;
				do
				{
					if (!((curr_prog->fragment_state.lighting.sampler_mask >> MS_ID_SP) & 1) ||
						((curr_prog->ras_regs[RA_LR_FUNC_MODE2] >> (8 + i)) & 1))
						break;
					BASE_GL_FAIL_IF_BRK(curr_prog->fragment_state.lights[i].spot_sampler == -1, GL_INVALID_OPERATION);
					tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.lights[i].spot_sampler];
					pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.lights[i].spot_sampler);
					BASE_GL_FAIL_IF_BRK(tex == 0 || pTex == 0, GL_INVALID_OPERATION);
					if(tex == state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_SP_START])
					{
						if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_SP_START])
						{
							__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, ((i + 8) << 8) | state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_SP_START], 0xf);
							__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_SP_START],
													&pTex->lr_plane[state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_SP_START]]);
							state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_SP_START] = 0;
						}
						break;
					}
					if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_SP_START])
						state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_SP_START] = 0;
					state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_SP_START] = tex;
					if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_LR))
					{
						int j;
						unsigned u;
						if (!pTex->lr_plane)
						{
							pTex->lr_plane = (GLuint*)malloc(MATERIAL_LUT_TABLE_SIZE * sizeof(GLuint));
							BASE_GL_FAIL_IF_BRK(!pTex->lr_plane, GL_OUT_OF_MEMORY);
						}
						for (j = 0; j < MATERIAL_LUT_TABLE_SIZE; j++)
						{
							UTL_F2UFX_12W_0I(*(pTex->src_plane + j), *(pTex->lr_plane + j));
							UTL_F2FX_12W_1I_F(*(pTex->src_plane + j + MATERIAL_LUT_TABLE_SIZE), u);
							pTex->lr_plane[j] |= u << 12;
						}
						pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_LR);
					}
					__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, (i + 8) << 8, 0xf);
					__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, MATERIAL_LUT_TABLE_SIZE, pTex->lr_plane);
					tableupdate = GL_TRUE;
					break;
				} while (0);
				if (!((curr_prog->ras_regs[RA_LR_FUNC_MODE2] >> (24 + i)) & 1))
				{
					BASE_GL_FAIL_IF_CONT(curr_prog->fragment_state.lights[i].da_sampler == -1, GL_INVALID_OPERATION);
					tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.lights[i].da_sampler];
					pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.lights[i].da_sampler);
					BASE_GL_FAIL_IF_CONT(tex == 0 || pTex == 0, GL_INVALID_OPERATION);
					if (tex == state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_DA_START])
					{
						if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_DA_START])
						{
							__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, ((i + 16) << 8) | state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_DA_START], 0xf);
							__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_DA_START],
													&pTex->lr_plane[state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_DA_START]]);
							state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_DA_START] = 0;
						}
						continue;
					}
					if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_DA_START])
						state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_DA_START] = 0;
					state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_DA_START] = tex;
					if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_LR))
					{
						int j;
						unsigned u;
						if (!pTex->lr_plane)
						{
							pTex->lr_plane = (GLuint*)malloc(MATERIAL_LUT_TABLE_SIZE * sizeof(GLuint));
							BASE_GL_FAIL_IF_CONT(!pTex->lr_plane, GL_OUT_OF_MEMORY);
						}
						for (j = 0; j < MATERIAL_LUT_TABLE_SIZE; j++)
						{
							UTL_F2UFX_12W_0I(*(pTex->src_plane + j), *(pTex->lr_plane + j));
							UTL_F2FX_12W_1I_F(*(pTex->src_plane + j + MATERIAL_LUT_TABLE_SIZE), u);
							pTex->lr_plane[j] |= u << 12;
						}
						pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_LR);
					}
					__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, (i + 16) << 8, 0xf);
					__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, MATERIAL_LUT_TABLE_SIZE, pTex->lr_plane);
					tableupdate = GL_TRUE;
				}
			}
		}
		if (curr_prog->fragment_state.proctex.sampler_type && (GET_MASK(*_mask, IF_GL_TEXTURE_LUT) || GET_MASK(*_mask, IF_GL_PROCTEX_LUT_SAMPLERS)))
		{
			int i;
			unsigned mapaddresses[] = {PT_LUT_IN_RGB, PT_LUT_IN_A};
			for (i = 0; i < 2; i++)
			{
				GLuint tex;
				tx_lut_container_t* pTex;
				if (1 == i && !(curr_prog->ras_regs[RA_PROC_TEX_M2REG_CONF_START_00] & 0x4000))
					continue;
				BASE_GL_FAIL_IF_BRK(curr_prog->fragment_state.proctex.samplers[i] == -1, GL_INVALID_OPERATION);
				tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.proctex.samplers[i]];
				pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.proctex.samplers[i]);
				BASE_GL_FAIL_IF_BRK(tex == 0 || pTex == 0, GL_INVALID_OPERATION);
				if (tex == state->gl_state.lutbindings.bindings[i + LAP_PT_LUT_RGB_MAP])
				{
					if (state->gl_state.lutbindings.subsize[i + LAP_PT_LUT_RGB_MAP])
					{
						__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, (mapaddresses[i] << 8) | state->gl_state.lutbindings.suboffset[i + LAP_PT_LUT_RGB_MAP], 0xf);
						__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, state->gl_state.lutbindings.subsize[i + LAP_PT_LUT_RGB_MAP],
													&pTex->pt_map_plane[state->gl_state.lutbindings.suboffset[i + LAP_PT_LUT_RGB_MAP]]);
						state->gl_state.lutbindings.subsize[i + LAP_PT_LUT_RGB_MAP] = 0;
					}
					continue;
				}
				if (state->gl_state.lutbindings.subsize[i + LAP_PT_LUT_RGB_MAP])
					state->gl_state.lutbindings.subsize[i + LAP_PT_LUT_RGB_MAP] = 0;
				state->gl_state.lutbindings.bindings[i + LAP_PT_LUT_RGB_MAP] = tex;
				if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_PT_MAP))
				{
					int j;
					unsigned u;
					if (!pTex->pt_map_plane)
					{
						pTex->pt_map_plane = (GLuint*)malloc(PROCTEX_LUT_TABLE_SIZE * sizeof(GLuint));
						BASE_GL_FAIL_IF_BRK(!pTex->pt_map_plane, GL_OUT_OF_MEMORY);
					}
					for (j = 0; j < PROCTEX_LUT_TABLE_SIZE; j++)
					{
						UTL_F2UFX_12W_0I(*(pTex->src_plane + j), *(pTex->pt_map_plane + j));
						UTL_F2FX_12W_1I_T(*(pTex->src_plane + j + PROCTEX_LUT_TABLE_SIZE), u);
						pTex->pt_map_plane[j] |= u << 12;
					}
					pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_PT_MAP);
				}
				__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, mapaddresses[i] << 8, 0xf);
				__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, PROCTEX_LUT_TABLE_SIZE, pTex->pt_map_plane);
				tableupdate = GL_TRUE;
			}
			do
			{
				GLuint tex;
				tx_lut_container_t* pTex;
				if (!(curr_prog->ras_regs[RA_PROC_TEX_M2REG_CONF_START_00] & 0x8000))
					break;
				BASE_GL_FAIL_IF_BRK(curr_prog->fragment_state.proctex.samplers[2] == -1, GL_INVALID_OPERATION);
				tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.proctex.samplers[2]];
				pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.proctex.samplers[2]);
				BASE_GL_FAIL_IF_BRK(tex == 0 || pTex == 0, GL_INVALID_OPERATION);
				if (tex == state->gl_state.lutbindings.bindings[LAP_PT_LUT_NOISE_MAP])
				{
					if (state->gl_state.lutbindings.subsize[LAP_PT_LUT_NOISE_MAP])
					{
						__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, (PT_LUT_HRDN << 8) | state->gl_state.lutbindings.suboffset[LAP_PT_LUT_NOISE_MAP], 0xf);
						__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, state->gl_state.lutbindings.subsize[LAP_PT_LUT_NOISE_MAP],
													&pTex->pt_noise_plane[state->gl_state.lutbindings.suboffset[LAP_PT_LUT_NOISE_MAP]]);
						state->gl_state.lutbindings.subsize[LAP_PT_LUT_NOISE_MAP] = 0;
					}
					break;
				}
				if (state->gl_state.lutbindings.subsize[LAP_PT_LUT_NOISE_MAP])
					state->gl_state.lutbindings.subsize[LAP_PT_LUT_NOISE_MAP] = 0;
				state->gl_state.lutbindings.bindings[LAP_PT_LUT_NOISE_MAP] = tex;
				if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_PT_NOISE))
				{
					int j;
					unsigned u;
					if (!pTex->pt_noise_plane)
					{
						pTex->pt_noise_plane = (GLuint*)malloc(PROCTEX_LUT_TABLE_SIZE * sizeof(GLuint));
						BASE_GL_FAIL_IF_BRK(!pTex->pt_noise_plane, GL_OUT_OF_MEMORY);
					}
					for (j = 0; j < PROCTEX_LUT_TABLE_SIZE; j++)
					{
						UTL_F2UFX_12W_0I(*(pTex->src_plane + j), *(pTex->pt_noise_plane + j));
						UTL_F2FX_12W_1I_T(*(pTex->src_plane + j + PROCTEX_LUT_TABLE_SIZE), u);
						pTex->pt_noise_plane[j] |= u << 12;
					}
					pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_PT_NOISE);
				}
				__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, PT_LUT_HRDN << 8, 0xf);
				__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, PROCTEX_LUT_TABLE_SIZE, pTex->pt_noise_plane);
				tableupdate = GL_TRUE;
			} while (0);
			do
			{
				GLuint res[PROCTEX_LUT_TABLE_SIZE * 4];

				for (i = 0; i < 4; i++)
				{
					if (state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.proctex.samplers[3 + i]] != state->gl_state.lutbindings.bindings[LAP_PT_LUT_R + i])
						break;
				}
				if (i == 4)
				{
					if (state->gl_state.lutbindings.subsize[LAP_PT_LUT_R] ||
						state->gl_state.lutbindings.subsize[LAP_PT_LUT_G] ||
						state->gl_state.lutbindings.subsize[LAP_PT_LUT_B] ||
						state->gl_state.lutbindings.subsize[LAP_PT_LUT_A])
					{
						unsigned start = PROCTEX_LUT_TABLE_SIZE * 4, end = 0;
						for (i = 0; i < 4; i++)
						{
							if (state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i])
							{
								if (start > state->gl_state.lutbindings.suboffset[LAP_PT_LUT_R + i])
									start = state->gl_state.lutbindings.suboffset[LAP_PT_LUT_R + i];
								if (end < (state->gl_state.lutbindings.suboffset[LAP_PT_LUT_R + i] + state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i] - 1))
									end = (state->gl_state.lutbindings.suboffset[LAP_PT_LUT_R + i] + state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i] - 1);
								state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i] = 0;
							}
						}
						for (i = 0; i < 4; i++)
						{
							unsigned j;
							tx_lut_container_t* pTex;
							pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.proctex.samplers[3 + i]);
							for (j = start; j <= end; j++)
								res[j] = (res[j] & (~(0xff << (8 * i)))) | ((GLuint)pTex->pt_rgba_plane[j] << (8 * i));
						}
						if (start < PROCTEX_LUT_TABLE_SIZE * 2 && end >= PROCTEX_LUT_TABLE_SIZE * 2)
						{
							__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, (PT_LUT_RGBA_LOD_0 << 8) | start, 0xf);
							__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, PROCTEX_LUT_TABLE_SIZE * 2 - start, &res[start]);
							__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, PT_LUT_RGBA_LOD_1 << 8, 0xf);
							__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, end + 1 - PROCTEX_LUT_TABLE_SIZE * 2, &res[PROCTEX_LUT_TABLE_SIZE * 2]);
						}
						else
						{
							if (start < PROCTEX_LUT_TABLE_SIZE * 2)
							{
								__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, (PT_LUT_RGBA_LOD_0 << 8) | start, 0xf);
								__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, end - start + 1, &res[start]);
							}
							else
							{
								__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, (PT_LUT_RGBA_LOD_1 << 8) | (start - PROCTEX_LUT_TABLE_SIZE * 2), 0xf);
								__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, end - start + 1, &res[start]);
							}
						}
					}
					break;
				}
				for (i = 0; i < 4; i++)
				{
					int j;
					tx_lut_container_t* pTex;
					pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.proctex.samplers[3 + i]);
					if (state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i])
						state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i] = 0;
					if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_PT_RGBA))
					{
						if (!pTex->pt_rgba_plane)
						{
							pTex->pt_rgba_plane = (GLubyte*)malloc(PROCTEX_LUT_TABLE_SIZE * 4 * sizeof(GLubyte));
							BASE_GL_FAIL_IF_BRK(!pTex->pt_rgba_plane, GL_OUT_OF_MEMORY);
						}
						for (j = 0; j < PROCTEX_LUT_TABLE_SIZE * 2; j++)
							pTex->pt_rgba_plane[j] = UTL_F2C_8(pTex->src_plane[j]);
						for (; j < PROCTEX_LUT_TABLE_SIZE * 4 - 1; j++)
							UTL_F2FX_8W_1I_T(pTex->src_plane[j], pTex->pt_rgba_plane[j]);
						pTex->pt_rgba_plane[j] = 0;
						pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_PT_RGBA);
					}
					for (j = 0; j < PROCTEX_LUT_TABLE_SIZE * 4; j++)
						res[j] = (res[j] & (~(0xff << (8 * i)))) | ((GLuint)pTex->pt_rgba_plane[j] << (8 * i));
				}
				if (i != 4)
					break;
				__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, PT_LUT_RGBA_LOD_0 << 8, 0xf);
				__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, PROCTEX_LUT_TABLE_SIZE * 2, res);
				__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, PT_LUT_RGBA_LOD_1 << 8, 0xf);
				__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, PROCTEX_LUT_TABLE_SIZE * 2, res + PROCTEX_LUT_TABLE_SIZE * 2);
				tableupdate = GL_TRUE;
				for (i = 0; i < 4; i++)
					state->gl_state.lutbindings.bindings[LAP_PT_LUT_R + i] = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.proctex.samplers[3 + i]];
			} while (0);
		}
		if ((curr_prog->ras_regs[RA_TB_FOG_EN] & 7) && (GET_MASK(*_mask, IF_GL_TEXTURE_LUT) || GET_MASK(*_mask, IF_GL_FOG_LUT_SAMPLER)))
		{
			do
			{
				GLuint tex;
				tx_lut_container_t* pTex;
				BASE_GL_FAIL_IF_BRK(curr_prog->fragment_state.fog.sampler == -1, GL_INVALID_OPERATION);
				tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.fog.sampler];
				pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.fog.sampler);
				BASE_GL_FAIL_IF_BRK(tex == 0 || pTex == 0, GL_INVALID_OPERATION);
				if (tex == state->gl_state.lutbindings.bindings[LAP_FG_LUT_FOG])
				{
					if (state->gl_state.lutbindings.subsize[LAP_FG_LUT_FOG])
					{
						__cb_writeReg(PA_WO_TB_FOG_RAM_ADDR, state->gl_state.lutbindings.suboffset[LAP_FG_LUT_FOG], 0xf);
						__cb_multiWriteReg(PA_WO_TB_FOG_RAM_DATA, state->gl_state.lutbindings.subsize[LAP_FG_LUT_FOG], &pTex->fog_plane[state->gl_state.lutbindings.suboffset[LAP_FG_LUT_FOG]]);
						state->gl_state.lutbindings.subsize[LAP_FG_LUT_FOG] = 0;
					}
					break;
				}
				if (state->gl_state.lutbindings.subsize[LAP_FG_LUT_FOG])
					state->gl_state.lutbindings.subsize[LAP_FG_LUT_FOG] = 0;
				state->gl_state.lutbindings.bindings[LAP_FG_LUT_FOG] = tex;
				if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_FOG))
				{
					int j;
					if (!pTex->fog_plane)
					{
						pTex->fog_plane = (GLuint*)malloc((MATERIAL_LUT_TABLE_SIZE / 2) * sizeof(GLuint));
						BASE_GL_FAIL_IF_BRK(!pTex->fog_plane, GL_OUT_OF_MEMORY);
					}
					for (j = 0; j < MATERIAL_LUT_TABLE_SIZE / 2; j++)
					{
						unsigned u;
						UTL_F2FX_13W_2I_T(pTex->src_plane[j + MATERIAL_LUT_TABLE_SIZE / 2], pTex->fog_plane[j]);
						UTL_F2UFX_11W_0I(pTex->src_plane[j], u);
						pTex->fog_plane[j] |= u << 13;
					}
					pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_FOG);
				}
				__cb_writeReg(PA_WO_TB_FOG_RAM_ADDR, 0, 0xf);
				__cb_multiWriteReg(PA_WO_TB_FOG_RAM_DATA, MATERIAL_LUT_TABLE_SIZE / 2, pTex->fog_plane);
				tableupdate = GL_TRUE;
			} while (0);
		}
		if ((curr_prog->ras_regs[RA_TB_FOG_EN] & 7) == 7 && (GET_MASK(*_mask, IF_GL_TEXTURE_LUT) || GET_MASK(*_mask, IF_GL_GAS_LUT_SAMPLERS)))
		{
			int i;
			unsigned res[GAS_LUT_TABLE_SIZE];
			do{
				for (i = 0; i < MAX_GAS_LUTS; i++)
				{
					if (state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.gas.samplers[i]] != state->gl_state.lutbindings.bindings[LAP_GS_LUT_START + i])
						break;
				}
				if (i == MAX_GAS_LUTS)
					break;
				memset(res, 0, sizeof(unsigned) * GAS_LUT_TABLE_SIZE);
				for (i = 0; i < MAX_GAS_LUTS; i++)
				{
					int j;
					tx_lut_container_t* pTex;
					pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.gas.samplers[i]);
					if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_GAS))
					{
						if (!pTex->gas_plane)
						{
							pTex->gas_plane = (GLuint*)malloc(GAS_LUT_TABLE_SIZE * sizeof(GLuint));
							BASE_GL_FAIL_IF_BRK(!pTex->gas_plane, GL_OUT_OF_MEMORY);
						}
						for (j = 0; j < GAS_LUT_TABLE_SIZE / 2; j++)
						{
							UTL_F2UFX_8W_8I((pTex->src_plane[j] * 255.f), pTex->gas_plane[j]);
							pTex->gas_plane[j + GAS_LUT_TABLE_SIZE / 2] = (unsigned int)(fabs(127.f * pTex->src_plane[j + GAS_LUT_TABLE_SIZE / 2])) & 0x7f;
							if (pTex->src_plane[j + GAS_LUT_TABLE_SIZE / 2] < 0.f)
								pTex->gas_plane[j + GAS_LUT_TABLE_SIZE / 2] |= 0x80;
						}
						pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_GAS);
					}
					for (j = 0; j < GAS_LUT_TABLE_SIZE / 2; j++)
					{
						res[j] |= (GLuint)pTex->gas_plane[j + GAS_LUT_TABLE_SIZE / 2] << (8 * i);
						res[j + GAS_LUT_TABLE_SIZE / 2] |= (GLuint)pTex->gas_plane[j] << (8 * i);
					}
				};
				if (i != MAX_GAS_LUTS)
					break;
				if (!tableupdate)
					__cb_addDummyWrite(PA_WO_TB_GLOBAL_START, 45);
				__cb_writeReg(PA_W0_GAS_TRFUNC_RAM_ADDR, 0, 0xf);
				__cb_multiWriteReg(PA_W0_GAS_TRFUNC_RAM_DATA, GAS_LUT_TABLE_SIZE, res);
				__cb_writeReg(PA_WO_CU_MODE, 0, 0);
				for (i = 0; i < MAX_GAS_LUTS; i++)
					state->gl_state.lutbindings.bindings[LAP_GS_LUT_START + i] = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.gas.samplers[i]];
			} while (0);
		}
	}
	if (!(state->suppressStateMask & NN_GX_STATE_FBACCESS) &&
		(GET_MASK(*_mask, IF_GL_FB_ACCESS) || curr_prog->fragment_state.operation.mode != state->gl_state.per_frag_op.mode))
	{
		unsigned maskbits;
		unsigned clmask = state->gl_state.fb_op.colorMask[0] | (state->gl_state.fb_op.colorMask[1] << 1) |
			(state->gl_state.fb_op.colorMask[2] << 2) | (state->gl_state.fb_op.colorMask[3] << 3);
		state->gl_state.per_frag_op.mode = curr_prog->fragment_state.operation.mode;
		__cb_writeReg(PA_WO_FU_CACHE_FLUSH, 1, 0xf);
		__cb_writeReg(PA_WO_FU_TAG_CLEAR, 1, 0xf);
		switch (state->gl_state.per_frag_op.mode)
		{
			case GL_FRAGOP_MODE_GL_DMP:
				#define CREAD_CHECK		(CWRITE_CHECK && ((state->gl_state.fb_state.color_format == CF_R8G8B8) || \
					(state->gl_state.per_frag_op.blendEnabled || (clmask != 0 && clmask != 0xf) || state->gl_state.per_frag_op.logicOpEnabled)))
				#define CWRITE_CHECK	(clmask)
				#define ZREAD_CHECK		(state->gl_state.per_frag_op.depthTestEnabled)
				#define ZWRITE_CHECK	(ZREAD_CHECK && state->gl_state.fb_op.depthMask)
				#define SREAD_CHECK		(state->gl_state.per_frag_op.stencilTestEnabled)
				#define SWRITE_CHECK	(SREAD_CHECK && state->gl_state.fb_op.stencilWriteMask)
				#define CHECK(type)	(maskbits & ##type##_BIT)
				#define CREAD_BIT	(0x1)
				#define CWRITE_BIT	(0x2)
				#define ZREAD_BIT	(0x4)
				#define ZWRITE_BIT	(0x8)
				#define SREAD_BIT	(0x10)
				#define SWRITE_BIT	(0x20)
				#define ZBUF_READ	((maskbits & ZREAD_BIT) && (maskbits & (ZWRITE_BIT | CWRITE_BIT)))
				#define SBUF_READ	((maskbits & SREAD_BIT) && (maskbits & (SWRITE_BIT | CWRITE_BIT)))
				maskbits =	(CREAD_CHECK?	CREAD_BIT  : 0)	|
							(CWRITE_CHECK?	CWRITE_BIT : 0)	|
							(ZREAD_CHECK?	ZREAD_BIT  : 0)	|
							(ZWRITE_CHECK?	ZWRITE_BIT : 0)	|
							(SREAD_CHECK?	SREAD_BIT  : 0)	|
							(SWRITE_CHECK?	SWRITE_BIT : 0);
				__cb_writeReg(PA_WO_FU_COLOR_READ_MASK, ((maskbits & CREAD_BIT) ? 0xf : 0x0), 0xf);
				__cb_writeReg(PA_WO_FU_COLOR_WRITE_MASK, ((maskbits & CWRITE_BIT) ? 0xf : 0x0), 0xf);
				__cb_writeReg(PA_WO_FU_ZCACHE_READ_MASK, (ZBUF_READ? 2 : 0) | (SBUF_READ ? 1 : 0), 0xf);
				__cb_writeReg(PA_WO_FU_ZCACHE_WRITE_MASK, ((maskbits & ZWRITE_BIT) ? 2 : 0) | ((maskbits & SWRITE_BIT) ? 1 : 0), 0xf);
				break;
			case GL_FRAGOP_MODE_GAS_ACC_DMP:
				__cb_writeReg(PA_WO_FU_COLOR_WRITE_MASK, 0xf, 0xf);
				__cb_writeReg(PA_WO_FU_COLOR_READ_MASK, 0xf, 0xf);
				__cb_writeReg(PA_WO_FU_ZCACHE_READ_MASK, 0x3, 0xf);
				__cb_writeReg(PA_WO_FU_ZCACHE_WRITE_MASK, 0x0, 0xf);
				break;
			case GL_FRAGOP_MODE_SHADOW_DMP:
				__cb_writeReg(PA_WO_FU_COLOR_WRITE_MASK, 0xf, 0xf);
				__cb_writeReg(PA_WO_FU_COLOR_READ_MASK, 0xf, 0xf);
				__cb_writeReg(PA_WO_FU_ZCACHE_READ_MASK, 0x0, 0xf);
				__cb_writeReg(PA_WO_FU_ZCACHE_WRITE_MASK, 0x0, 0xf);
				break;
		}
	}
	return;
}

void __shv_partialValidateShaderValidator(bit_mask_t* _mask, GLbitfield _statemask)
{
	GLboolean offlight = GL_FALSE;
	GLboolean tableupdate = GL_FALSE;
	pg_list_t* curr_prog;
	GET_CURRENT_STATE(state);
	if (_statemask & NN_GX_STATE_SCISSOR)
	{
		if (GET_MASK(*_mask, IF_GL_FB_SCISSOR))
		{
			GLint x, y, x2, y2, attr;
			if (!state->gl_state.per_frag_op.scissorTestEnabled)
			{
				x = y = 0;
				x2 = state->gl_state.fb_state.width - 1;
				y2 = state->gl_state.fb_state.height - 1;
				attr = 0;
			}
			else
			{
				x = state->gl_state.per_frag_op.scissorLeft;
				y = state->gl_state.per_frag_op.scissorBottom;
				x2 = state->gl_state.per_frag_op.scissorWidth + x - 1;
				y2 = state->gl_state.per_frag_op.scissorHeight + y - 1;
				if (x >= state->gl_state.fb_state.width)
					x = state->gl_state.fb_state.width - 1;
				else if (x < 0)
					x = 0;
				if (y >= state->gl_state.fb_state.height)
					y = state->gl_state.fb_state.height - 1;
				else if (y < 0)
					y = 0;
				if (x2 > state->gl_state.fb_state.width)
					x2 = state->gl_state.fb_state.width - 1;
				else if (x2 < 0)
					x2 = 0;
				if (y2 > state->gl_state.fb_state.height)
					y2 = state->gl_state.fb_state.height - 1;
				else if (y2 < 0)
					y2 = 0;
				attr = 3;
			}
			__cb_writeReg(PA_RW_TI_SCISSORING_ATTR, attr, 0xf);
			__cb_writeReg(PA_RW_TI_SCISSORING_POS0, x | (y << 16), 0xf);
			__cb_writeReg(PA_RW_TI_SCISSORING_POS1, x2 | (y2 << 16), 0xf);
		}
	}
	if (!__shman->current_program)
		return;
	curr_prog = __shman->current_program;
	if (_statemask & NN_GX_STATE_SHADERMODE)
	{
		int mode = curr_prog->geometry_shader_used ? 1 : 0;
		if (mode != s_vp_mode || (state->forceValidateMask & NN_GX_STATE_SHADERMODE))
		{
			__cb_addDummyWrite(0x251, 10);
			__cb_addDummyWrite(0x200, 30);
			__cb_writeReg(PA_RW_CI_VP_MODE, mode ? 2 : 0, 0x1);
			__cb_addDummyWrite(0x200, 30);
			s_vp_mode = mode;
			if (mode)
				__cb_writeReg(PA_RW_LB_VP0_WRITE_DISABLE, 1, 1);
			else
				__cb_writeReg(PA_RW_LB_VP0_WRITE_DISABLE, 0, 1);
		}
	}
	if (_statemask & NN_GX_STATE_SHADERBINARY)
	{
		if (GET_MASK(*_mask, IF_GL_BINARY_PACKAGE))
		{
			if (curr_prog->geometry_shader_used)
			{
				__cb_writeReg(PA_RW_VP_PRAM_ADDR, 0, 0xf);
				__cb_multiWriteReg(PA_RW_VP_PRAM_TOP, curr_prog->bin_list->num_program, curr_prog->bin_list->program);
				__cb_writeReg(PA_RW_VP_PROGRAM_RENEWAL, 1, 0xf);
				__cb_writeReg(PA_RW_VP_SWIZZLE_ADDR, 0, 0xf);
				__cb_multiWriteReg(PA_RW_VP_SWIZZLE_TOP, curr_prog->bin_list->num_swizzle, curr_prog->bin_list->swizzle);
			}
			else
			{
				if (curr_prog->bin_list->num_program > 512)
				{
					__cb_writeReg(PA_RW_VP_PRAM_ADDR, 512, 0xf);
					__cb_multiWriteReg(PA_RW_VP_PRAM_TOP, curr_prog->bin_list->num_program - 512, &curr_prog->bin_list->program[512]);
					__cb_writeReg(PA_RW_VP_PROGRAM_RENEWAL, 1, 0xf);
				}
			}
			__cb_writeReg(PA_RW_VP_PRAM_ADDR + GEO_VP_LEN, 0, 0xf);
			__cb_multiWriteReg(PA_RW_VP_PRAM_TOP + GEO_VP_LEN,
				(curr_prog->bin_list->num_program > 512) ? 512 : curr_prog->bin_list->num_program, curr_prog->bin_list->program);
			__cb_writeReg(PA_RW_VP_PROGRAM_RENEWAL + GEO_VP_LEN, 1, 0xf);
			__cb_writeReg(PA_RW_VP_SWIZZLE_ADDR + GEO_VP_LEN, 0, 0xf);
			__cb_multiWriteReg(PA_RW_VP_SWIZZLE_TOP + GEO_VP_LEN, curr_prog->bin_list->num_swizzle, curr_prog->bin_list->swizzle);
		}
	}
	if (_statemask & NN_GX_STATE_SHADERMODE)
	{
		if (curr_prog->geometry_shader_used && GET_MASK(*_mask, IF_GL_ATTACH_GS))
		{
			unsigned i;
			for (i = RA_VP0_BOOL_REG; i <= RA_VP0_OUTPUT_MASK; i++)
				__shman->ras_regs[i] = ~curr_prog->ras_regs[i];
		}
		if (GET_MASK(*_mask, IF_GL_DETACH_GS))
		{
			unsigned i;
			for (i = RA_VP1_BOOL_REG; i <= RA_VP1_OUTPUT_MASK; i++)
				__shman->ras_regs[i] = ~curr_prog->ras_regs[i];
		}
	}
	if (_statemask & NN_GX_STATE_SHADERFLOAT)
	{
		if (GET_MASK(*_mask, IF_GL_BINARY_EXE_VS) || GET_MASK(*_mask, IF_GL_BINARY_EXE_GS))
		{
			unsigned i;
			exe_info_t* exe_info;
			if (curr_prog->geometry_shader_used)
			{
				if (GET_MASK(*_mask, IF_GL_BINARY_EXE_GS))
				{
					exe_info = &curr_prog->bin_list->exe_infos[curr_prog->gs_exe_id];
					for (i = 0; i < exe_info->num_float_const; i++)
						__cb_writeRegs(PA_RW_VP_CONST_REG_ADDR, 4, (unsigned*)&exe_info->float_consts[i]);
				}
			}
			if (GET_MASK(*_mask, IF_GL_BINARY_EXE_VS))
			{
				exe_info = &curr_prog->bin_list->exe_infos[curr_prog->vs_exe_id];
				for (i = 0; i < exe_info->num_float_const; i++)
					__cb_writeRegs(PA_RW_VP_CONST_REG_ADDR + GEO_VP_LEN, 4, (unsigned*)&exe_info->float_consts[i]);
			}
		}
	}
	if (_statemask & NN_GX_STATE_VERTEX)
	{
		if (GET_MASK(*_mask, IF_GL_VERT_ARRAY) ||
			GET_MASK(*_mask, IF_GL_PROGRAM) ||
			GET_MASK(*_mask, IF_GL_VERTBUFFER))
		{
			int i;
			GLboolean same_prog, map_match;
			int const_list[MAX_VERTEX_ATTRIBS];
			int const_attr_count, arr_attr_count;
			unsigned const_flag;
			unsigned maxaddr, minaddr;
			attrib_addr_map_t addrmap[MAX_VERTEX_ATTRIBS];
			attrib_addr_map_t* addrmap_top = &addrmap[0];
			same_prog = __shman->last_validated_program == curr_prog ? GL_TRUE : GL_FALSE;
			__shman->last_validated_program = curr_prog;
			state->hw_state.hw_shader.useVertexBuffers = GL_TRUE;
			addrmap[0].next = 0;
			const_attr_count = arr_attr_count = 0;
			const_flag = 0;
			maxaddr = minaddr = 0;
			for (i = MAX_VERTEX_ATTRIBS - 1; i >= 0; i--)
			{
				if (curr_prog->attribs[i].reg_index == -1)
					continue;
				if (!state->gl_state.vertarray.array[i].enabled)
				{
					const_list[const_attr_count++] = i;
					const_flag |= 1 << i;
				}
				else
				{
					if (state->gl_state.vertarray.array[i].buffer == 0)
						state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
					else
						maxaddr = maxaddr < (unsigned)state->hw_state.hw_arrayproc.attrArrOffset[i] ? state->hw_state.hw_arrayproc.attrArrOffset[i] : maxaddr;
					addrmap[arr_attr_count].attr_index = i;
					if (arr_attr_count)
					{
						if (state->hw_state.hw_arrayproc.attrArrOffset[i] <= state->hw_state.hw_arrayproc.attrArrOffset[addrmap_top->attr_index])
						{
							addrmap[arr_attr_count].next = addrmap_top;
							addrmap_top = &addrmap[arr_attr_count];
						}
						else
						{
							attrib_addr_map_t* addrmap_tmp = addrmap_top;
							attrib_addr_map_t* addrmap_next = addrmap_top->next;
							for ( ; addrmap_next != 0; )
							{
								if (state->hw_state.hw_arrayproc.attrArrOffset[i] <= state->hw_state.hw_arrayproc.attrArrOffset[addrmap_next->attr_index])
								{
									addrmap_tmp->next = &addrmap[arr_attr_count];
									addrmap[arr_attr_count].next = addrmap_next;
									break;
								}
								addrmap_tmp = addrmap_next;
								addrmap_next = addrmap_next->next;
							}
							if (addrmap_next == 0)
							{
								addrmap_tmp->next = &addrmap[arr_attr_count];
								addrmap[arr_attr_count].next = 0;
							}
						}
					}
					arr_attr_count++;
				}
			}
			if (state->hw_state.hw_arrayproc.bUseDeletedBuffer || arr_attr_count + const_attr_count > MAX_BUFFERED_VERTEX_ATTRIBS)
				state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
			if (arr_attr_count)
			{
				minaddr = state->hw_state.hw_arrayproc.baseAddr = state->hw_state.hw_arrayproc.attrArrOffset[addrmap_top->attr_index] & ~0xf;
				if (state->hw_state.hw_shader.useVertexBuffers)
				{
					if (!state->gl_state.rendermode.drawarray)
					{
						minaddr = minaddr > (unsigned)state->hw_state.hw_arrayproc.elmtArrOffset ? state->hw_state.hw_arrayproc.elmtArrOffset : minaddr;
						maxaddr = maxaddr < (unsigned)state->hw_state.hw_arrayproc.elmtArrOffset ? state->hw_state.hw_arrayproc.elmtArrOffset : maxaddr;
					}
					if ((maxaddr - minaddr) >= 0x10000000)
					{
						BASE_GL_FAIL_IF_NORET(state->gl_state.rendermode.drawing, GL_INVALID_OPERATION);
						state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
					}
				}
			}
			else
				state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
			map_match = GL_TRUE;
			if (arr_attr_count != curr_prog->num_array_attr || const_attr_count != curr_prog->num_const_attr ||
				(state->forceValidateMask & NN_GX_STATE_VERTEX))
				map_match = GL_FALSE;
			else
			{
				attrib_addr_map_t* addrmap_curr = addrmap_top;
				attrib_addr_map_t* addrmap_prev = curr_prog->attr_addr_map_top;
				for (i = 0; i < arr_attr_count; i++)
				{
					if (addrmap_curr->attr_index != addrmap_prev->attr_index ||
						(state->hw_state.hw_arrayproc.attrArrOffset[addrmap_curr->attr_index] - state->hw_state.hw_arrayproc.attrArrOffset[addrmap_top->attr_index]) !=
						(curr_prog->attr_offset[i] - curr_prog->attr_offset[0]) ||
						state->gl_state.vertarray.array[addrmap_curr->attr_index].stride != curr_prog->attr_stride[i])
					{
						map_match = GL_FALSE;
						break;
					}
					{
						unsigned char prev_typesize, curr_typesize;
						switch (state->gl_state.vertarray.array[addrmap_curr->attr_index].type)
						{
							case GL_FLOAT:			curr_typesize = 3; break;
							case GL_SHORT:			curr_typesize = 2; break;
							case GL_UNSIGNED_BYTE:	curr_typesize = 1; break;
							default:				curr_typesize = 0; break;
						}
						curr_typesize |= (state->gl_state.vertarray.array[addrmap_curr->attr_index].size - 1) << 2;
						if (i < 8)
							prev_typesize = (curr_prog->ci_config[1] >> (i * 4)) & 0xf;
						else
							prev_typesize = (curr_prog->ci_config[2] >> ((i - 8) * 4)) & 0xf;
						if (curr_typesize != prev_typesize)
						{
							map_match = GL_FALSE;
							break;
						}
					}
					addrmap_curr = addrmap_curr->next;
					addrmap_prev = addrmap_prev->next;
				}
				for (i = 0; i < const_attr_count; i++)
				{
					if (const_list[i] != curr_prog->ci_const_list[i])
					{
						map_match = GL_FALSE;
						break;
					}
				}
			}
			if (map_match && state->hw_state.hw_shader.useVertexBuffers)
			{
				GLboolean base_only_changed = same_prog;
				unsigned base;
				if (state->gl_state.rendermode.drawarray)
					base = state->hw_state.hw_arrayproc.baseAddr;
				else
					base = state->hw_state.hw_arrayproc.elmtArrOffset < state->hw_state.hw_arrayproc.baseAddr ?
						(state->hw_state.hw_arrayproc.elmtArrOffset & ~0xf) : state->hw_state.hw_arrayproc.baseAddr;
				if (base != state->hw_state.hw_arrayproc.baseAddr ||
					curr_prog->ci_index_base ||
					state->hw_state.hw_arrayproc.attrArrOffset[addrmap_top->attr_index] - base != curr_prog->ci_config[3])
				{
					unsigned offset = state->hw_state.hw_arrayproc.attrArrOffset[addrmap_top->attr_index] - base;
					for (i = 1; i < curr_prog->num_ci_array; i++)
						curr_prog->ci_config[i * 3 + 3] = curr_prog->ci_config[i * 3 + 3] - curr_prog->ci_config[3] + offset;
					curr_prog->ci_config[3] = offset;
					base_only_changed = GL_FALSE;
					if (base == state->hw_state.hw_arrayproc.baseAddr)
						curr_prog->ci_index_base = GL_FALSE;
				}
				state->hw_state.hw_arrayproc.baseAddr = base;
				curr_prog->ci_config[0] = base >> 3;
				if (base_only_changed)
					__cb_writeReg(PA_CI_BASE_ADDRESS, curr_prog->ci_config[0], 0xf);
				else
				{
					if (same_prog)
						__cb_writeRegs(PA_CI_BASE_ADDRESS, curr_prog->num_ci_array * 3 + 1, curr_prog->ci_config);
					else
					{
						__cb_writeRegs(PA_CI_BASE_ADDRESS, NUM_CI_ARRAY_REG, curr_prog->ci_config);
						__cb_writeReg(1 * GEO_VP_LEN + PA_RW_VP_INPUTMAP0, curr_prog->vs_attr_map[0], 0xf);
						__cb_writeReg(1 * GEO_VP_LEN + PA_RW_VP_INPUTMAP1, curr_prog->vs_attr_map[1], 0xf);
						if (curr_prog->geometry_shader_used)
						{
							__cb_writeReg(0 * GEO_VP_LEN + PA_RW_VP_INPUTMAP0, curr_prog->gs_attr_map[0], 0xf);
							__cb_writeReg(0 * GEO_VP_LEN + PA_RW_VP_INPUTMAP1, curr_prog->gs_attr_map[1], 0xf);
						}
						{
							attrib_addr_map_t *addrmap_curr;
							int j;
							addrmap_curr = curr_prog->attr_addr_map_top;
							state->hw_state.hw_shader.attrNumUsed = arr_attr_count;
							state->hw_state.hw_shader.attrNumTotalUsed = arr_attr_count + const_attr_count;
							for (i = 0; i < arr_attr_count; i++)
							{
								state->hw_state.hw_shader.attrUseOrder[i] = addrmap_curr->attr_index;
								addrmap_curr = addrmap_curr->next;
							}
							for (j = 0; j < const_attr_count; j++, i++)
								state->hw_state.hw_shader.attrUseOrder[i] = const_list[j];
						}
					}
				}
			}
			else
			{
				{
					attrib_addr_map_t* addrmap_curr = addrmap_top;
					for (i = 0; i < arr_attr_count; i++)
					{
						curr_prog->attr_addr_map[i].attr_index = addrmap_curr->attr_index;
						curr_prog->attr_offset[i] = state->hw_state.hw_arrayproc.attrArrOffset[addrmap_curr->attr_index];
						curr_prog->attr_stride[i] = state->gl_state.vertarray.array[addrmap_curr->attr_index].stride;
						if (i == arr_attr_count - 1)
							curr_prog->attr_addr_map[i].next = 0;
						else
						{
							curr_prog->attr_addr_map[i].next = &curr_prog->attr_addr_map[i + 1];
							addrmap_curr = addrmap_curr->next;
						}
					}
					for (i = 0; i < const_attr_count; i++)
						curr_prog->ci_const_list[i] = const_list[i];
					curr_prog->num_array_attr = arr_attr_count;
					curr_prog->num_const_attr = const_attr_count;
					curr_prog->attr_addr_map_top = &curr_prog->attr_addr_map[0];
					if (state->gl_state.vertbuffer.elmtArrayID == 0 && !state->gl_state.rendermode.drawarray)
						state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
				}
				if (state->gl_state.rendermode.drawarray)
				{
					curr_prog->ci_config[0] = state->hw_state.hw_arrayproc.baseAddr >> 3;
					curr_prog->ci_index_base = GL_FALSE;
				}
				else
				{
					if (state->hw_state.hw_arrayproc.elmtArrOffset < state->hw_state.hw_arrayproc.baseAddr)
					{
						state->hw_state.hw_arrayproc.baseAddr = state->hw_state.hw_arrayproc.elmtArrOffset & ~0xf;
						curr_prog->ci_index_base = GL_TRUE;
					}
					else
						curr_prog->ci_index_base = GL_FALSE;
					curr_prog->ci_config[0] = state->hw_state.hw_arrayproc.baseAddr >> 3;
				}
				if (state->hw_state.hw_shader.useVertexBuffers)
				{
					unsigned int arr_comp_count;
					unsigned int arr_byte_count;
					unsigned int actual_byte_cont;
					unsigned int structure_align;
					attrib_addr_map_t *addrmap_curr, *addrmap_next;
					unsigned* ci_arr_reg;
					GLsizeiptr ArrayFirstVertEnd;
					stt_vert_array_state_t* glvstate;
					stt_hw_array_proc_state_t* hwvstate;
					glvstate = &state->gl_state.vertarray;
					hwvstate = &state->hw_state.hw_arrayproc;
					curr_prog->num_ci_array = 0;
					arr_comp_count = arr_byte_count = actual_byte_cont = 0;
					ci_arr_reg = &curr_prog->ci_config[3];
					for (i = 1; i < NUM_CI_ARRAY_REG; i++)
						curr_prog->ci_config[i] = 0;
					structure_align = 1;
					addrmap_curr = addrmap_top;
					for (i = 0; i < arr_attr_count; i++)
					{
						stt_vert_arr_t* array;
						unsigned int attrSetting = 0;
						unsigned int entrySize = 0;
						GLboolean bNewArr;
						unsigned diff;
						unsigned int attrArrOffset;
						addrmap_next = addrmap_curr->next;
						array = &state->gl_state.vertarray.array[addrmap_curr->attr_index];
						attrArrOffset = hwvstate->attrArrOffset[addrmap_curr->attr_index] - hwvstate->baseAddr;
						switch (array->type)
						{
							case GL_FLOAT:			attrSetting = 0x3; entrySize = 4; break;
							case GL_SHORT:			attrSetting = 0x2; entrySize = 2; break;
							case GL_UNSIGNED_BYTE:	attrSetting = 0x1; entrySize = 1; break;
							case GL_BYTE:			attrSetting = 0x0; entrySize = 1; break;
						}
						attrSetting |= (array->size - 1) << 2;
						if (i < 8)
							curr_prog->ci_config[1] |= attrSetting << (i * 4);
						else
							curr_prog->ci_config[2] |= attrSetting << ((i-8) * 4);
						if (!array->stride)
						{
							entrySize *= array->size;
							ci_arr_reg[0] = attrArrOffset;
							ci_arr_reg[1] |= i ;
							ci_arr_reg[2] |= (entrySize<<16) | (1<<28);
							curr_prog->num_ci_array++;
							ci_arr_reg += 3;
						}
						else
						{
							structure_align = structure_align < entrySize ? entrySize : structure_align;
							actual_byte_cont = (actual_byte_cont + (entrySize - 1)) & ~(entrySize - 1);
							entrySize *= array->size;
							actual_byte_cont += entrySize;
							if (!arr_comp_count)
							{
								ci_arr_reg[0] = attrArrOffset;
								arr_byte_count = array->stride;
								ArrayFirstVertEnd = array->stride + hwvstate->attrArrOffset[addrmap_curr->attr_index];
							}
							if (arr_comp_count < 8)
								ci_arr_reg[1] |= i << (arr_comp_count * 4);
							else
								ci_arr_reg[2] |= i << ((arr_comp_count - 8) * 4);
							arr_comp_count++ ;
							bNewArr = (i + 1) == arr_attr_count
									|| (hwvstate->attrArrOffset[addrmap_next->attr_index] - (unsigned)hwvstate->baseAddr) <= attrArrOffset
									|| array->stride != glvstate->array[addrmap_next->attr_index].stride
									|| ArrayFirstVertEnd <= hwvstate->attrArrOffset[addrmap_next->attr_index]
									|| (MAX_BUFFERED_VERTEX_ATTRIBS == arr_comp_count);
							diff = ((bNewArr ? (ci_arr_reg[0] + arr_byte_count) : (hwvstate->attrArrOffset[addrmap_next->attr_index] - hwvstate->baseAddr)) - attrArrOffset - entrySize) >> 2 ;
							if (MAX_BUFFERED_VERTEX_ATTRIBS - arr_comp_count < (diff >> 2) + ((diff & 3) ? 1 : 0))
							{
								bNewArr = 1;
								diff = (ci_arr_reg[0] + arr_byte_count - attrArrOffset - entrySize) >> 2 ;
								state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
								break;
							}
							else if (diff)
							{
								actual_byte_cont = (actual_byte_cont + 3) & ~0x3;
								actual_byte_cont += diff * 4;
								if (diff & 3)
								{
									if (arr_comp_count < 8)
										ci_arr_reg[1] |= (0xb + (diff & 3)) << (arr_comp_count * 4);
									else
										ci_arr_reg[2] |= (0xb + (diff & 3)) << ((arr_comp_count - 8) * 4);
									arr_comp_count++;
									diff &= ~3 ;
								}
								while (diff)
								{
									if (arr_comp_count < 8)
										ci_arr_reg[1] |= 0xf << (arr_comp_count * 4);
									else
										ci_arr_reg[2] |= 0xf << ((arr_comp_count - 8) * 4);
									arr_comp_count++ ;
									diff -= 4;
								}
							}
							if (bNewArr)
							{
								actual_byte_cont = (actual_byte_cont + (structure_align - 1)) & ~(structure_align - 1);
								if (actual_byte_cont != arr_byte_count)
								{
									state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
									break;
								}
								ci_arr_reg[2] |= (arr_byte_count << 16) | (arr_comp_count << 28);
								structure_align = 1;
								curr_prog->num_ci_array++;
								ci_arr_reg += 3;
								actual_byte_cont = arr_comp_count = 0;
							}
						}
						addrmap_curr = addrmap_next;
					}
					if (state->hw_state.hw_shader.useVertexBuffers)
					{
						for (i = 0; i < const_attr_count; i++)
							curr_prog->ci_config[2] |= 0x10000 << (i + arr_attr_count);
						if (arr_attr_count)
							curr_prog->ci_config[2] |= ((arr_attr_count + const_attr_count - 1) << 28);
						if (!state->gl_state.rendermode.drawarray && curr_prog->num_ci_array == MAX_BUFFERED_VERTEX_ATTRIBS)
							state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
					}
				}
				{
					int j;
					attrib_addr_map_t *addrmap_curr;
					state->hw_state.hw_shader.attrNumUsed = arr_attr_count;
					state->hw_state.hw_shader.attrNumTotalUsed = arr_attr_count + const_attr_count;
					addrmap_curr = curr_prog->attr_addr_map_top;
					curr_prog->vs_attr_map[0] = 0;
					curr_prog->vs_attr_map[1] = 0;
					curr_prog->gs_attr_map[0] = 0x76543210;
					curr_prog->gs_attr_map[1] = 0xfedcba98;
					for (i = 0; i < arr_attr_count; i++)
					{
						if (i < 8)
							curr_prog->vs_attr_map[0] |= (curr_prog->attribs[addrmap_curr->attr_index].reg_index & 0xf) << (i * 4);
						else
							curr_prog->vs_attr_map[1] |= (curr_prog->attribs[addrmap_curr->attr_index].reg_index & 0xf) << ((i - 8) * 4);
						state->hw_state.hw_shader.attrUseOrder[i] = addrmap_curr->attr_index;
						addrmap_curr = addrmap_curr->next;
					}
					for (j = 0; j < const_attr_count; j++, i++)
					{
						if (i < 8)
							curr_prog->vs_attr_map[0] |= (curr_prog->attribs[const_list[j]].reg_index & 0xf) << (i * 4);
						else
							curr_prog->vs_attr_map[1] |= (curr_prog->attribs[const_list[j]].reg_index & 0xf) << ((i - 8) * 4);
						state->hw_state.hw_shader.attrUseOrder[i] = const_list[j];
					}
				}
				__cb_writeRegs(PA_CI_BASE_ADDRESS, NUM_CI_ARRAY_REG, curr_prog->ci_config);
				__cb_writeReg(1 * GEO_VP_LEN + PA_RW_VP_INPUTMAP0, curr_prog->vs_attr_map[0], 0xf);
				__cb_writeReg(1 * GEO_VP_LEN + PA_RW_VP_INPUTMAP1, curr_prog->vs_attr_map[1], 0xf);
				if (curr_prog->geometry_shader_used)
				{
					__cb_writeReg(0 * GEO_VP_LEN + PA_RW_VP_INPUTMAP0, curr_prog->gs_attr_map[0], 0xf);
					__cb_writeReg(0 * GEO_VP_LEN + PA_RW_VP_INPUTMAP1, curr_prog->gs_attr_map[1], 0xf);
				}
			}
		}
		if (state->hw_state.hw_shader.useVertexBuffers && (
			GET_MASK(*_mask, IF_GL_VERT_ARRAY) ||
			GET_MASK(*_mask, IF_GL_VERTBUFFER) ||
			GET_MASK(*_mask, IF_GL_PROGRAM) ||
			GET_MASK(*_mask, IF_GL_VERT_CURRENT))
			)
		{
			unsigned i;
			for (i = state->hw_state.hw_shader.attrNumUsed; i < state->hw_state.hw_shader.attrNumTotalUsed; i++)
			{
				int pos = state->hw_state.hw_shader.attrUseOrder[i];
				__cb_writeReg(PA_RW_CI_CONST_ATTR_NUMBER, i, 0xf);
				__cb_writeReg(PA_RW_CI_CONST_ATTR_DATA0, state->gl_state.vertcurr.attrib24[pos][0], 0xf);
				__cb_writeReg(PA_RW_CI_CONST_ATTR_DATA1, state->gl_state.vertcurr.attrib24[pos][1], 0xf);
				__cb_writeReg(PA_RW_CI_CONST_ATTR_DATA2, state->gl_state.vertcurr.attrib24[pos][2], 0xf);
			}
		}
	}
	if (state->forceValidateMask)
	{
		unsigned i;
		if ((state->forceValidateMask & _statemask) & NN_GX_STATE_VSUNIFORM)
		{
			curr_prog->vs_floats_update_mask[0] = curr_prog->vs_floats_update_mask[1] = curr_prog->vs_floats_update_mask[2] = 0xffffffff;
			for (i = 0; s_vsuniform_regs[i] != RA_LAST; i++)
			{
				if (curr_prog->ras_regs_be[s_vsuniform_regs[i]])
				{
					__shman->ras_regs[s_vsuniform_regs[i]] = ~curr_prog->ras_regs[s_vsuniform_regs[i]];
					SET_RASREGMASK(curr_prog->ras_reg_mask, s_vsuniform_regs[i]);
				}
			}
			if (curr_prog->geometry_shader_used)
			{
				curr_prog->gs_floats_update_mask[0] = curr_prog->gs_floats_update_mask[1] = curr_prog->gs_floats_update_mask[2] = 0xffffffff;
				for (i = 0; s_gsuniform_regs[i] != RA_LAST; i++)
				{
					if (curr_prog->ras_regs_be[s_gsuniform_regs[i]])
					{
						__shman->ras_regs[s_gsuniform_regs[i]] = ~curr_prog->ras_regs[s_gsuniform_regs[i]];
						SET_RASREGMASK(curr_prog->ras_reg_mask, s_gsuniform_regs[i]);
					}
				}
			}
		}
		if ((state->forceValidateMask & _statemask) & NN_GX_STATE_FSUNIFORM)
		{
			for (i = 0; s_fsuniform_regs[i] != RA_LAST; i++)
			{
				if (curr_prog->ras_regs_be[s_fsuniform_regs[i]])
				{
					__shman->ras_regs[s_fsuniform_regs[i]] = ~curr_prog->ras_regs[s_fsuniform_regs[i]];
					SET_RASREGMASK(curr_prog->ras_reg_mask, s_fsuniform_regs[i]);
				}
			}
		}
		if ((state->forceValidateMask & _statemask) & NN_GX_STATE_SHADERPROGRAM)
		{
			for (i = 0; s_shaderprog_regs[i] != RA_LAST; i++)
			{
				if (curr_prog->ras_regs_be[s_shaderprog_regs[i]])
				{
					__shman->ras_regs[s_shaderprog_regs[i]] = ~curr_prog->ras_regs[s_shaderprog_regs[i]];
					SET_RASREGMASK(curr_prog->ras_reg_mask, s_shaderprog_regs[i]);
				}
			}
		}
	}
	if (_statemask & NN_GX_STATE_VSUNIFORM)
	{
		if (curr_prog->vs_floats_update_mask[0] || curr_prog->vs_floats_update_mask[1] || curr_prog->vs_floats_update_mask[2] ||
			curr_prog->gs_floats_update_mask[0] || curr_prog->gs_floats_update_mask[1] || curr_prog->gs_floats_update_mask[2])
		{
			if (curr_prog->gs_floats &&
				(curr_prog->gs_floats_update_mask[0] || curr_prog->gs_floats_update_mask[1] || curr_prog->gs_floats_update_mask[2]))
			{
				float_const_reg_t* src = curr_prog->gs_floats;
				float_const_reg_t* batch = 0;
				unsigned batch_addr = 0;
				unsigned batch_size = 0;
				unsigned addr = 0;
				while (!(curr_prog->gs_floats_update_mask[addr >> 5] & (1 << (addr & 0x1f))))
				{
					if (!(curr_prog->gs_floats_update_mask[addr >> 5] >> (addr & 0x1f)))
						addr = 32 + (addr & ~0x1f);
					else
						addr++;
				}
				if (addr < curr_prog->num_gs_floats)
				{
					do
					{
						unsigned act_addr = curr_prog->gs_floats_addr[addr];
						if (batch && batch_size != act_addr - batch_addr)
						{
							__cb_writeReg(PA_RW_VP_CONST_REG_ADDR, batch_addr | 0x80000000, 0xf);
							__cb_multiWriteReg(PA_RW_VP_CONST_REG_TOP, 4 * batch_size, (unsigned*)batch);
							batch = 0;
						}
						else
							batch_size++;
						if (!batch)
						{
							batch_addr = act_addr;
							batch = src + addr;
							batch_size = 1;
						}
						addr++;
						while (!(curr_prog->gs_floats_update_mask[addr >> 5] & (1 << (addr & 0x1f))) && addr < curr_prog->num_gs_floats)
						{
							if (!(curr_prog->gs_floats_update_mask[addr >> 5] >> (addr & 0x1f)))
								addr = 32 + (addr & ~0x1f);
							else
								addr++;
						}
					} while (addr < curr_prog->num_gs_floats);
					if (batch)
					{
						__cb_writeReg(PA_RW_VP_CONST_REG_ADDR, batch_addr | 0x80000000, 0xf);
						__cb_multiWriteReg(PA_RW_VP_CONST_REG_TOP, 4 * batch_size, (unsigned*)batch);
					}
				}
			}
			if (curr_prog->vs_floats &&
				(curr_prog->vs_floats_update_mask[0] || curr_prog->vs_floats_update_mask[1] || curr_prog->vs_floats_update_mask[2]))
			{
				float_const_reg_t* src = curr_prog->vs_floats;
				float_const_reg_t* batch = 0;
				unsigned batch_addr = 0;
				unsigned batch_size = 0;
				unsigned addr = 0;
				while (!(curr_prog->vs_floats_update_mask[addr >> 5] & (1 << (addr & 0x1f))))
				{
					if (!(curr_prog->vs_floats_update_mask[addr >> 5] >> (addr & 0x1f)))
						addr = 32 + (addr & ~0x1f);
					else
						addr++;
				}
				if (addr < curr_prog->num_vs_floats)
				{
					do
					{
						unsigned act_addr = curr_prog->vs_floats_addr[addr];
						if (batch && batch_size != act_addr - batch_addr)
						{
							__cb_writeReg(PA_RW_VP_CONST_REG_ADDR + GEO_VP_LEN, batch_addr | 0x80000000, 0xf);
							__cb_multiWriteReg(PA_RW_VP_CONST_REG_TOP + GEO_VP_LEN, 4 * batch_size, (unsigned*)batch);
							batch = 0;
						}
						else
							batch_size++;
						if (!batch)
						{
							batch_addr = act_addr;
							batch = src + addr;
							batch_size = 1;
						}
						addr++;
						while (!(curr_prog->vs_floats_update_mask[addr >> 5] & (1 << (addr & 0x1f))) && addr < curr_prog->num_vs_floats)
						{
							if (!(curr_prog->vs_floats_update_mask[addr >> 5] >> (addr & 0x1f)))
								addr = 32 + (addr & ~0x1f);
							else
								addr++;
						}
					} while (addr < curr_prog->num_vs_floats);
					if (batch)
					{
						__cb_writeReg(PA_RW_VP_CONST_REG_ADDR + GEO_VP_LEN, batch_addr | 0x80000000, 0xf);
						__cb_multiWriteReg(PA_RW_VP_CONST_REG_TOP + GEO_VP_LEN, 4 * batch_size, (unsigned*)batch);
					}
				}
			}
			curr_prog->vs_floats_update_mask[0] = 0;
			curr_prog->vs_floats_update_mask[1] = 0;
			curr_prog->vs_floats_update_mask[2] = 0;
			curr_prog->gs_floats_update_mask[0] = 0;
			curr_prog->gs_floats_update_mask[1] = 0;
			curr_prog->gs_floats_update_mask[2] = 0;
		}
	}
	#define UPDATE_REG(_addr, _data, _be) \
	{ \
		curr_prog->ras_regs_be[_addr] |= ((_be) & 0xff ? 1 : 0) | ((_be) & 0xff00 ? 2 : 0) | ((_be) & 0xff0000 ? 4 : 0) | ((_be) & 0xff000000 ? 8 : 0); \
		if (state->forceMode) \
		{ \
			curr_prog->ras_regs[_addr] = (curr_prog->ras_regs[_addr] & ~(_be)) | ((_data) & (_be)); \
			SET_RASREGMASK(curr_prog->ras_reg_mask, (_addr)); \
			SET_MASK(state->upd_mask, IF_HW_RASTERIZER_REGISTER); \
			__shman->ras_regs[_addr] = ~curr_prog->ras_regs[_addr]; \
		} \
		else if ((curr_prog->ras_regs[_addr] & (_be)) != ((_data) & (_be))) \
		{ \
			curr_prog->ras_regs[_addr] = (curr_prog->ras_regs[_addr] & ~(_be)) | ((_data) & (_be)); \
			SET_RASREGMASK(curr_prog->ras_reg_mask, (_addr)); \
			SET_MASK(state->upd_mask, IF_HW_RASTERIZER_REGISTER); \
		} \
	}
	if (_statemask & NN_GX_STATE_TRIOFFSET)
	{
		if (GET_MASK(*_mask, IF_GL_TRI_OFFSET))
		{
			unsigned u, u2;
			GLfloat scale, offset;
			if (curr_prog->fragment_state.operation.wscale != 0)
			{
				scale = -curr_prog->fragment_state.operation.wscale;
				offset = 0.f;
			}
			else
			{
				scale = state->gl_state.trioffset.depthRangeN - state->gl_state.trioffset.depthRangeF;
				offset = state->gl_state.trioffset.depthRangeN;
			}
			if (state->gl_state.trioffset.enabled && state->gl_state.trioffset.u_offset != 0.f)
			{
				switch (state->gl_state.fb_state.z_format)
				{
					case ZF_Z16:
						offset += state->gl_state.trioffset.u_offset / 65535.f;
						break;
					case ZF_Z24:
					case ZF_Z24S8:
					default:
						offset += state->gl_state.trioffset.u_offset / 16777215.f;
						break;
				}
			}
			UTL_F2F_16M7E(scale, u);
			if (offset != 0.f)
			{
				UTL_F2F_16M7E(offset, u2);
			}
			else
				u2 = 0;
			UPDATE_REG(RA_TS_ZSCALE, u, 0xffffffff);
			UPDATE_REG(RA_TS_ZBIAS, u2, 0xffffffff);
		}
	}
	#undef UPDATE_REG
	if (_statemask & NN_GX_STATE_FSUNIFORM)
	{
		if ((curr_prog->ras_regs[RA_TB_FOG_EN] & 0x7) == 0x7)
		{
			if (curr_prog->fragment_state.gas.autoacc)
			{
				__shman->ras_regs[RA_TB_GAS_ACC_MAX] = curr_prog->ras_regs[RA_TB_GAS_ACC_MAX];
				CLEAR_RASREGMASK(curr_prog->ras_reg_mask, RA_TB_GAS_ACC_MAX);
			}
			else
			{
				__shman->ras_regs[RA_TB_GAS_ACC_MAX] = ~curr_prog->ras_regs[RA_TB_GAS_ACC_MAX];
				SET_RASREGMASK(curr_prog->ras_reg_mask, RA_TB_GAS_ACC_MAX);
			}
		}
		if (curr_prog->ras_regs[RA_TU0_BEGIN_0F] & 1)
		{
			int i;
			for (i = 0; i < MAX_PER_PIXEL_LIGHTS; i++)
			{
				if (curr_prog->fragment_state.lights[i].enabled)
					break;
			}
			if (i == MAX_PER_PIXEL_LIGHTS)
			{
				offlight = GL_TRUE;
			}
		}
	}
	if (_statemask & NN_GX_STATE_VSUNIFORM)
	{
		int i;
		for (i = 0; s_gsuniform_regs[i] != RA_LAST; i++)
		{
			if (curr_prog->ras_reg_mask.mask_[s_gsuniform_regs[i] >> 5] & (1 << (s_gsuniform_regs[i] & 0x1f)))
			{
				unsigned* src = curr_prog->ras_regs + s_gsuniform_regs[i];
				unsigned* dst = __shman->ras_regs + s_gsuniform_regs[i];
				unsigned char be = curr_prog->ras_regs_be[s_gsuniform_regs[i]];
				if (be && *src != *dst)
				{
					__cb_writeReg(s_ras_addr_map[s_gsuniform_regs[i]], *src, be);
					*dst = *src;
				}
				CLEAR_RASREGMASK(curr_prog->ras_reg_mask, s_gsuniform_regs[i]);
			}
		}
		for (i = 0; s_vsuniform_regs[i] != RA_LAST; i++)
		{
			if (curr_prog->ras_reg_mask.mask_[s_vsuniform_regs[i] >> 5] & (1 << (s_vsuniform_regs[i] & 0x1f)))
			{
				unsigned* src = curr_prog->ras_regs + s_vsuniform_regs[i];
				unsigned* dst = __shman->ras_regs + s_vsuniform_regs[i];
				unsigned char be = curr_prog->ras_regs_be[s_vsuniform_regs[i]];
				if (be && *src != *dst)
				{
					__cb_writeReg(s_ras_addr_map[s_vsuniform_regs[i]], *src, be);
					*dst = *src;
				}
				CLEAR_RASREGMASK(curr_prog->ras_reg_mask, s_vsuniform_regs[i]);
			}
		}
	}
	if (_statemask & NN_GX_STATE_SHADERPROGRAM)
	{
		int i;
		for (i = 0; s_shaderprog_regs[i] != RA_LAST; i++)
		{
			if (curr_prog->ras_reg_mask.mask_[s_shaderprog_regs[i] >> 5] & (1 << (s_shaderprog_regs[i] & 0x1f)))
			{
				unsigned* src = curr_prog->ras_regs + s_shaderprog_regs[i];
				unsigned* dst = __shman->ras_regs + s_shaderprog_regs[i];
				unsigned char be = curr_prog->ras_regs_be[s_shaderprog_regs[i]];
				if (be && *src != *dst)
				{
					__cb_writeReg(s_ras_addr_map[s_shaderprog_regs[i]], *src, be);
					*dst = *src;
				}
				CLEAR_RASREGMASK(curr_prog->ras_reg_mask, s_shaderprog_regs[i]);
			}
		}
	}
	if (_statemask & NN_GX_STATE_FSUNIFORM)
	{
		int i;
		for (i = 0; s_fsuniform_regs[i] != RA_LAST; i++)
		{
			if (curr_prog->ras_reg_mask.mask_[s_fsuniform_regs[i] >> 5] & (1 << (s_fsuniform_regs[i] & 0x1f)))
			{
				unsigned* src = curr_prog->ras_regs + s_fsuniform_regs[i];
				unsigned* dst = __shman->ras_regs + s_fsuniform_regs[i];
				unsigned char be = curr_prog->ras_regs_be[s_fsuniform_regs[i]];
				if (be && *src != *dst)
				{
					__cb_writeReg(s_ras_addr_map[s_fsuniform_regs[i]], *src, be);
					*dst = *src;
				}
				CLEAR_RASREGMASK(curr_prog->ras_reg_mask, s_fsuniform_regs[i]);
			}
		}
	}
	if (_statemask & NN_GX_STATE_LUT)
	{
		if ((curr_prog->ras_regs[RA_TU0_BEGIN_0F] & 1) && (GET_MASK(*_mask, IF_GL_TEXTURE_LUT) || GET_MASK(*_mask, IF_GL_LIGHTING_LUT_SAMPLERS)))
		{
			int i;
			static const int sampler_valid_bit[MAX_MATERIAL_LUTS] = {16, 17, 19, 20, 21, 22};
			static const int sampler_table_sel[MAX_MATERIAL_LUTS] = {0, 1, 3, 4, 5, 6};
			for (i = 0; i < MAX_MATERIAL_LUTS; i++)
			{
				GLuint tex;
				tx_lut_container_t* pTex;
				if (!((curr_prog->fragment_state.lighting.sampler_mask >> i) & 1) ||
					((curr_prog->ras_regs[RA_LR_FUNC_MODE2] >> sampler_valid_bit[i]) & 1))
					continue;
				if ((curr_prog->fragment_state.lighting.samplers[i] == -1) ||
					(tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.lighting.samplers[i]]) == 0 ||
					(pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.lighting.samplers[i])) == 0)
				{
					BASE_GL_FAIL_IF_NORET(!offlight, GL_INVALID_OPERATION);
					break;
				}
				if (tex == state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_START])
				{
					if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_START])
					{
						__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, (sampler_table_sel[i] << 8) | state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_START], 0xf);
						__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_START],
												&pTex->lr_plane[state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_START]]);
						state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_START] = 0;
					}
					continue;
				}
				if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_START])
					state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_START] = 0;
				state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_START] = tex;
				if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_LR))
				{
					int j;
					unsigned u;
					if (!pTex->lr_plane)
					{
						pTex->lr_plane = (GLuint*)malloc(MATERIAL_LUT_TABLE_SIZE * sizeof(GLuint));
						BASE_GL_FAIL_IF_BRK(!pTex->lr_plane, GL_OUT_OF_MEMORY);
					}
					for (j = 0; j < MATERIAL_LUT_TABLE_SIZE; j++)
					{
						UTL_F2UFX_12W_0I(*(pTex->src_plane + j), *(pTex->lr_plane + j));
						UTL_F2FX_12W_1I_F(*(pTex->src_plane + j + MATERIAL_LUT_TABLE_SIZE), u);
						pTex->lr_plane[j] |= u << 12;
					}
					pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_LR);
				}
				__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, sampler_table_sel[i] << 8, 0xf);
				__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, MATERIAL_LUT_TABLE_SIZE, pTex->lr_plane);
				tableupdate = GL_TRUE;
			}
			for (i = 0; i < MAX_PER_PIXEL_LIGHTS; i++)
			{
				GLuint tex;
				tx_lut_container_t* pTex;
				if (!(curr_prog->fragment_state.lights[i].enabled))
					continue;
				do
				{
					if (!((curr_prog->fragment_state.lighting.sampler_mask >> MS_ID_SP) & 1) ||
						((curr_prog->ras_regs[RA_LR_FUNC_MODE2] >> (8 + i)) & 1))
						break;
					BASE_GL_FAIL_IF_BRK(curr_prog->fragment_state.lights[i].spot_sampler == -1, GL_INVALID_OPERATION);
					tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.lights[i].spot_sampler];
					pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.lights[i].spot_sampler);
					BASE_GL_FAIL_IF_BRK(tex == 0 || pTex == 0, GL_INVALID_OPERATION);
					if(tex == state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_SP_START])
					{
						if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_SP_START])
						{
							__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, ((i + 8) << 8) | state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_SP_START], 0xf);
							__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_SP_START],
													&pTex->lr_plane[state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_SP_START]]);
							state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_SP_START] = 0;
						}
						break;
					}
					if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_SP_START])
						state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_SP_START] = 0;
					state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_SP_START] = tex;
					if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_LR))
					{
						int j;
						unsigned u;
						if (!pTex->lr_plane)
						{
							pTex->lr_plane = (GLuint*)malloc(MATERIAL_LUT_TABLE_SIZE * sizeof(GLuint));
							BASE_GL_FAIL_IF_BRK(!pTex->lr_plane, GL_OUT_OF_MEMORY);
						}
						for (j = 0; j < MATERIAL_LUT_TABLE_SIZE; j++)
						{
							UTL_F2UFX_12W_0I(*(pTex->src_plane + j), *(pTex->lr_plane + j));
							UTL_F2FX_12W_1I_F(*(pTex->src_plane + j + MATERIAL_LUT_TABLE_SIZE), u);
							pTex->lr_plane[j] |= u << 12;
						}
						pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_LR);
					}
					__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, (i + 8) << 8, 0xf);
					__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, MATERIAL_LUT_TABLE_SIZE, pTex->lr_plane);
					tableupdate = GL_TRUE;
					break;
				} while (0);
				if (!((curr_prog->ras_regs[RA_LR_FUNC_MODE2] >> (24 + i)) & 1))
				{
					BASE_GL_FAIL_IF_CONT(curr_prog->fragment_state.lights[i].da_sampler == -1, GL_INVALID_OPERATION);
					tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.lights[i].da_sampler];
					pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.lights[i].da_sampler);
					BASE_GL_FAIL_IF_CONT(tex == 0 || pTex == 0, GL_INVALID_OPERATION);
					if (tex == state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_DA_START])
					{
						if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_DA_START])
						{
							__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, ((i + 16) << 8) | state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_DA_START], 0xf);
							__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_DA_START],
													&pTex->lr_plane[state->gl_state.lutbindings.suboffset[i + LAP_LR_LUT_DA_START]]);
							state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_DA_START] = 0;
						}
						continue;
					}
					if (state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_DA_START])
						state->gl_state.lutbindings.subsize[i + LAP_LR_LUT_DA_START] = 0;
					state->gl_state.lutbindings.bindings[i + LAP_LR_LUT_DA_START] = tex;
					if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_LR))
					{
						int j;
						unsigned u;
						if (!pTex->lr_plane)
						{
							pTex->lr_plane = (GLuint*)malloc(MATERIAL_LUT_TABLE_SIZE * sizeof(GLuint));
							BASE_GL_FAIL_IF_CONT(!pTex->lr_plane, GL_OUT_OF_MEMORY);
						}
						for (j = 0; j < MATERIAL_LUT_TABLE_SIZE; j++)
						{
							UTL_F2UFX_12W_0I(*(pTex->src_plane + j), *(pTex->lr_plane + j));
							UTL_F2FX_12W_1I_F(*(pTex->src_plane + j + MATERIAL_LUT_TABLE_SIZE), u);
							pTex->lr_plane[j] |= u << 12;
						}
						pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_LR);
					}
					__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, (i + 16) << 8, 0xf);
					__cb_multiWriteReg(PA_WO_LR_LUT_DATA0_ADR, MATERIAL_LUT_TABLE_SIZE, pTex->lr_plane);
					tableupdate = GL_TRUE;
				}
			}
		}
		if (curr_prog->fragment_state.proctex.sampler_type && (GET_MASK(*_mask, IF_GL_TEXTURE_LUT) || GET_MASK(*_mask, IF_GL_PROCTEX_LUT_SAMPLERS)))
		{
			int i;
			unsigned mapaddresses[] = {PT_LUT_IN_RGB, PT_LUT_IN_A};
			for (i = 0; i < 2; i++)
			{
				GLuint tex;
				tx_lut_container_t* pTex;
				if (1 == i && !(curr_prog->ras_regs[RA_PROC_TEX_M2REG_CONF_START_00] & 0x4000))
					continue;
				BASE_GL_FAIL_IF_BRK(curr_prog->fragment_state.proctex.samplers[i] == -1, GL_INVALID_OPERATION);
				tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.proctex.samplers[i]];
				pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.proctex.samplers[i]);
				BASE_GL_FAIL_IF_BRK(tex == 0 || pTex == 0, GL_INVALID_OPERATION);
				if (tex == state->gl_state.lutbindings.bindings[i + LAP_PT_LUT_RGB_MAP])
				{
					if (state->gl_state.lutbindings.subsize[i + LAP_PT_LUT_RGB_MAP])
					{
						__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, (mapaddresses[i] << 8) | state->gl_state.lutbindings.suboffset[i + LAP_PT_LUT_RGB_MAP], 0xf);
						__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, state->gl_state.lutbindings.subsize[i + LAP_PT_LUT_RGB_MAP],
													&pTex->pt_map_plane[state->gl_state.lutbindings.suboffset[i + LAP_PT_LUT_RGB_MAP]]);
						state->gl_state.lutbindings.subsize[i + LAP_PT_LUT_RGB_MAP] = 0;
					}
					continue;
				}
				if (state->gl_state.lutbindings.subsize[i + LAP_PT_LUT_RGB_MAP])
					state->gl_state.lutbindings.subsize[i + LAP_PT_LUT_RGB_MAP] = 0;
				state->gl_state.lutbindings.bindings[i + LAP_PT_LUT_RGB_MAP] = tex;
				if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_PT_MAP))
				{
					int j;
					unsigned u;
					if (!pTex->pt_map_plane)
					{
						pTex->pt_map_plane = (GLuint*)malloc(PROCTEX_LUT_TABLE_SIZE * sizeof(GLuint));
						BASE_GL_FAIL_IF_BRK(!pTex->pt_map_plane, GL_OUT_OF_MEMORY);
					}
					for (j = 0; j < PROCTEX_LUT_TABLE_SIZE; j++)
					{
						UTL_F2UFX_12W_0I(*(pTex->src_plane + j), *(pTex->pt_map_plane + j));
						UTL_F2FX_12W_1I_T(*(pTex->src_plane + j + PROCTEX_LUT_TABLE_SIZE), u);
						pTex->pt_map_plane[j] |= u << 12;
					}
					pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_PT_MAP);
				}
				__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, mapaddresses[i] << 8, 0xf);
				__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, PROCTEX_LUT_TABLE_SIZE, pTex->pt_map_plane);
				tableupdate = GL_TRUE;
			}
			do
			{
				GLuint tex;
				tx_lut_container_t* pTex;
				if (!(curr_prog->ras_regs[RA_PROC_TEX_M2REG_CONF_START_00] & 0x8000))
					break;
				BASE_GL_FAIL_IF_BRK(curr_prog->fragment_state.proctex.samplers[2] == -1, GL_INVALID_OPERATION);
				tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.proctex.samplers[2]];
				pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.proctex.samplers[2]);
				BASE_GL_FAIL_IF_BRK(tex == 0 || pTex == 0, GL_INVALID_OPERATION);
				if (tex == state->gl_state.lutbindings.bindings[LAP_PT_LUT_NOISE_MAP])
				{
					if (state->gl_state.lutbindings.subsize[LAP_PT_LUT_NOISE_MAP])
					{
						__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, (PT_LUT_HRDN << 8) | state->gl_state.lutbindings.suboffset[LAP_PT_LUT_NOISE_MAP], 0xf);
						__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, state->gl_state.lutbindings.subsize[LAP_PT_LUT_NOISE_MAP],
													&pTex->pt_noise_plane[state->gl_state.lutbindings.suboffset[LAP_PT_LUT_NOISE_MAP]]);
						state->gl_state.lutbindings.subsize[LAP_PT_LUT_NOISE_MAP] = 0;
					}
					break;
				}
				if (state->gl_state.lutbindings.subsize[LAP_PT_LUT_NOISE_MAP])
					state->gl_state.lutbindings.subsize[LAP_PT_LUT_NOISE_MAP] = 0;
				state->gl_state.lutbindings.bindings[LAP_PT_LUT_NOISE_MAP] = tex;
				if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_PT_NOISE))
				{
					int j;
					unsigned u;
					if (!pTex->pt_noise_plane)
					{
						pTex->pt_noise_plane = (GLuint*)malloc(PROCTEX_LUT_TABLE_SIZE * sizeof(GLuint));
						BASE_GL_FAIL_IF_BRK(!pTex->pt_noise_plane, GL_OUT_OF_MEMORY);
					}
					for (j = 0; j < PROCTEX_LUT_TABLE_SIZE; j++)
					{
						UTL_F2UFX_12W_0I(*(pTex->src_plane + j), *(pTex->pt_noise_plane + j));
						UTL_F2FX_12W_1I_T(*(pTex->src_plane + j + PROCTEX_LUT_TABLE_SIZE), u);
						pTex->pt_noise_plane[j] |= u << 12;
					}
					pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_PT_NOISE);
				}
				__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, PT_LUT_HRDN << 8, 0xf);
				__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, PROCTEX_LUT_TABLE_SIZE, pTex->pt_noise_plane);
				tableupdate = GL_TRUE;
			} while (0);
			do
			{
				GLuint res[PROCTEX_LUT_TABLE_SIZE * 4];
				for (i = 0; i < 4; i++)
				{
					if (state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.proctex.samplers[3 + i]] != state->gl_state.lutbindings.bindings[LAP_PT_LUT_R + i])
						break;
				}
				if (i == 4)
				{
					if (state->gl_state.lutbindings.subsize[LAP_PT_LUT_R] ||
						state->gl_state.lutbindings.subsize[LAP_PT_LUT_G] ||
						state->gl_state.lutbindings.subsize[LAP_PT_LUT_B] ||
						state->gl_state.lutbindings.subsize[LAP_PT_LUT_A])
					{
						unsigned start = PROCTEX_LUT_TABLE_SIZE * 4, end = 0;
						for (i = 0; i < 4; i++)
						{
							if (state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i])
							{
								if (start > state->gl_state.lutbindings.suboffset[LAP_PT_LUT_R + i])
									start = state->gl_state.lutbindings.suboffset[LAP_PT_LUT_R + i];
								if (end < (state->gl_state.lutbindings.suboffset[LAP_PT_LUT_R + i] + state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i] - 1))
									end = (state->gl_state.lutbindings.suboffset[LAP_PT_LUT_R + i] + state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i] - 1);
								state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i] = 0;
							}
						}
						for (i = 0; i < 4; i++)
						{
							unsigned j;
							tx_lut_container_t* pTex;
							pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.proctex.samplers[3 + i]);
							for (j = start; j <= end; j++)
								res[j] = (res[j] & (~(0xff << (8 * i)))) | ((GLuint)pTex->pt_rgba_plane[j] << (8 * i));
						}
						if (start < PROCTEX_LUT_TABLE_SIZE * 2 && end >= PROCTEX_LUT_TABLE_SIZE * 2)
						{
							__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, (PT_LUT_RGBA_LOD_0 << 8) | start, 0xf);
							__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, PROCTEX_LUT_TABLE_SIZE * 2 - start, &res[start]);
							__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, PT_LUT_RGBA_LOD_1 << 8, 0xf);
							__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, end + 1 - PROCTEX_LUT_TABLE_SIZE * 2, &res[PROCTEX_LUT_TABLE_SIZE * 2]);
						}
						else
						{
							if (start < PROCTEX_LUT_TABLE_SIZE * 2)
							{
								__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, (PT_LUT_RGBA_LOD_0 << 8) | start, 0xf);
								__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, end - start + 1, &res[start]);
							}
							else
							{
								__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, (PT_LUT_RGBA_LOD_1 << 8) | (start - PROCTEX_LUT_TABLE_SIZE * 2), 0xf);
								__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, end - start + 1, &res[start]);
							}
						}
					}
					break;
				}
				for (i = 0; i < 4; i++)
				{
					int j;
					tx_lut_container_t* pTex;
					pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.proctex.samplers[3 + i]);
					if (state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i])
						state->gl_state.lutbindings.subsize[LAP_PT_LUT_R + i] = 0;
					if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_PT_RGBA))
					{
						if (!pTex->pt_rgba_plane)
						{
							pTex->pt_rgba_plane = (GLubyte*)malloc(PROCTEX_LUT_TABLE_SIZE * 4 * sizeof(GLubyte));
							BASE_GL_FAIL_IF_BRK(!pTex->pt_rgba_plane, GL_OUT_OF_MEMORY);
						}
						for (j = 0; j < PROCTEX_LUT_TABLE_SIZE * 2; j++)
							pTex->pt_rgba_plane[j] = UTL_F2C_8(pTex->src_plane[j]);
						for (; j < PROCTEX_LUT_TABLE_SIZE * 4 - 1; j++)
							UTL_F2FX_8W_1I_T(pTex->src_plane[j], pTex->pt_rgba_plane[j]);
						pTex->pt_rgba_plane[j] = 0;
						pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_PT_RGBA);
					}
					for (j = 0; j < PROCTEX_LUT_TABLE_SIZE * 4; j++)
						res[j] = (res[j] & (~(0xff << (8 * i)))) | ((GLuint)pTex->pt_rgba_plane[j] << (8 * i));
				}
				if (i != 4)
					break;
				__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, PT_LUT_RGBA_LOD_0 << 8, 0xf);
				__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, PROCTEX_LUT_TABLE_SIZE * 2, res);
				__cb_writeReg(PA_WO_PROC_TEX_M2_LUT_SEL, PT_LUT_RGBA_LOD_1 << 8, 0xf);
				__cb_multiWriteReg(PA_WO_PROC_TEX_M2_LUT_DATA, PROCTEX_LUT_TABLE_SIZE * 2, res + PROCTEX_LUT_TABLE_SIZE * 2);
				tableupdate = GL_TRUE;
				for (i = 0; i < 4; i++)
					state->gl_state.lutbindings.bindings[LAP_PT_LUT_R + i] = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.proctex.samplers[3 + i]];
			} while (0);
		}
		if ((curr_prog->ras_regs[RA_TB_FOG_EN] & 7) && (GET_MASK(*_mask, IF_GL_TEXTURE_LUT) || GET_MASK(*_mask, IF_GL_FOG_LUT_SAMPLER)))
		{
			do
			{
				GLuint tex;
				tx_lut_container_t* pTex;
				BASE_GL_FAIL_IF_BRK(curr_prog->fragment_state.fog.sampler == -1, GL_INVALID_OPERATION);
				tex = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.fog.sampler];
				pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.fog.sampler);
				BASE_GL_FAIL_IF_BRK(tex == 0 || pTex == 0, GL_INVALID_OPERATION);
				if (tex == state->gl_state.lutbindings.bindings[LAP_FG_LUT_FOG])
				{
					if (state->gl_state.lutbindings.subsize[LAP_FG_LUT_FOG])
					{
						__cb_writeReg(PA_WO_TB_FOG_RAM_ADDR, state->gl_state.lutbindings.suboffset[LAP_FG_LUT_FOG], 0xf);
						__cb_multiWriteReg(PA_WO_TB_FOG_RAM_DATA, state->gl_state.lutbindings.subsize[LAP_FG_LUT_FOG], &pTex->fog_plane[state->gl_state.lutbindings.suboffset[LAP_FG_LUT_FOG]]);
						state->gl_state.lutbindings.subsize[LAP_FG_LUT_FOG] = 0;
					}
					break;
				}
				if (state->gl_state.lutbindings.subsize[LAP_FG_LUT_FOG])
					state->gl_state.lutbindings.subsize[LAP_FG_LUT_FOG] = 0;
				state->gl_state.lutbindings.bindings[LAP_FG_LUT_FOG] = tex;
				if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_FOG))
				{
					int j;
					if (!pTex->fog_plane)
					{
						pTex->fog_plane = (GLuint*)malloc((MATERIAL_LUT_TABLE_SIZE / 2) * sizeof(GLuint));
						BASE_GL_FAIL_IF_BRK(!pTex->fog_plane, GL_OUT_OF_MEMORY);
					}
					for (j = 0; j < MATERIAL_LUT_TABLE_SIZE / 2; j++)
					{
						unsigned u;
						UTL_F2FX_13W_2I_T(pTex->src_plane[j + MATERIAL_LUT_TABLE_SIZE / 2], pTex->fog_plane[j]);
						UTL_F2UFX_11W_0I(pTex->src_plane[j], u);
						pTex->fog_plane[j] |= u << 13;
					}
					pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_FOG);
				}
				__cb_writeReg(PA_WO_TB_FOG_RAM_ADDR, 0, 0xf);
				__cb_multiWriteReg(PA_WO_TB_FOG_RAM_DATA, MATERIAL_LUT_TABLE_SIZE / 2, pTex->fog_plane);
				tableupdate = GL_TRUE;
			} while (0);
		}
		if ((curr_prog->ras_regs[RA_TB_FOG_EN] & 7) == 7 && (GET_MASK(*_mask, IF_GL_TEXTURE_LUT) || GET_MASK(*_mask, IF_GL_GAS_LUT_SAMPLERS)))
		{
			int i;
			unsigned res[GAS_LUT_TABLE_SIZE];
			do
			{
				for (i = 0; i < MAX_GAS_LUTS; i++)
				{
					if (state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.gas.samplers[i]] != state->gl_state.lutbindings.bindings[LAP_GS_LUT_START + i])
						break;
				}
				if (i == MAX_GAS_LUTS)
					break;
				memset(res, 0, sizeof(unsigned) * GAS_LUT_TABLE_SIZE);
				for (i = 0; i < MAX_GAS_LUTS; i++)
				{
					int j;
					tx_lut_container_t* pTex;
					pTex = __tx_getBoundTextureLut(curr_prog->fragment_state.gas.samplers[i]);
					if (pTex->updated_flags & (1 << TX_UPDATE_FLAG_GAS))
					{
						if (!pTex->gas_plane)
						{
							pTex->gas_plane = (GLuint*)malloc(GAS_LUT_TABLE_SIZE * sizeof(GLuint));
							BASE_GL_FAIL_IF_BRK(!pTex->gas_plane, GL_OUT_OF_MEMORY);
						}
						for (j = 0; j < GAS_LUT_TABLE_SIZE / 2; j++)
						{
							UTL_F2UFX_8W_8I((pTex->src_plane[j] * 255.f), pTex->gas_plane[j]);
							pTex->gas_plane[j + GAS_LUT_TABLE_SIZE / 2] = (unsigned int)(fabs(127.f * pTex->src_plane[j + GAS_LUT_TABLE_SIZE / 2])) & 0x7f;
							if (pTex->src_plane[j + GAS_LUT_TABLE_SIZE / 2] < 0.f)
								pTex->gas_plane[j + GAS_LUT_TABLE_SIZE / 2] |= 0x80;
						}
						pTex->updated_flags &= ~(1 << TX_UPDATE_FLAG_GAS);
					}
					for (j = 0; j < GAS_LUT_TABLE_SIZE / 2; j++)
					{
						res[j] |= (GLuint)pTex->gas_plane[j + GAS_LUT_TABLE_SIZE / 2] << (8 * i);
						res[j + GAS_LUT_TABLE_SIZE / 2] |= (GLuint)pTex->gas_plane[j] << (8 * i);
					}
				};
				if (i != MAX_GAS_LUTS)
					break;
				if (!tableupdate)
					__cb_addDummyWrite(PA_WO_TB_GLOBAL_START, 45);
				__cb_writeReg(PA_W0_GAS_TRFUNC_RAM_ADDR, 0, 0xf);
				__cb_multiWriteReg(PA_W0_GAS_TRFUNC_RAM_DATA, GAS_LUT_TABLE_SIZE, res);
				__cb_writeReg(PA_WO_CU_MODE, 0, 0);
				for (i = 0; i < MAX_GAS_LUTS; i++)
					state->gl_state.lutbindings.bindings[LAP_GS_LUT_START + i] = state->gl_state.textureunits.boundLutTex[curr_prog->fragment_state.gas.samplers[i]];
			} while (0);
		}
	}
	if (_statemask & NN_GX_STATE_FBACCESS)
	{
		if (GET_MASK(*_mask, IF_GL_FB_ACCESS) ||
			curr_prog->fragment_state.operation.mode != state->gl_state.per_frag_op.mode)
		{
			unsigned maskbits;
			unsigned clmask = state->gl_state.fb_op.colorMask[0] | (state->gl_state.fb_op.colorMask[1] << 1) |
				(state->gl_state.fb_op.colorMask[2] << 2) | (state->gl_state.fb_op.colorMask[3] << 3);
			state->gl_state.per_frag_op.mode = curr_prog->fragment_state.operation.mode;
			__cb_writeReg(PA_WO_FU_CACHE_FLUSH, 1, 0xf);
			__cb_writeReg(PA_WO_FU_TAG_CLEAR, 1, 0xf);
			switch (state->gl_state.per_frag_op.mode)
			{
				case GL_FRAGOP_MODE_GL_DMP:
					#define CREAD_CHECK		(CWRITE_CHECK && ((state->gl_state.fb_state.color_format == CF_R8G8B8) || \
						(state->gl_state.per_frag_op.blendEnabled || (clmask != 0 && clmask != 0xf) || state->gl_state.per_frag_op.logicOpEnabled)))
					#define CWRITE_CHECK	(clmask)
					#define ZREAD_CHECK		(state->gl_state.per_frag_op.depthTestEnabled)
					#define ZWRITE_CHECK	(ZREAD_CHECK && state->gl_state.fb_op.depthMask)
					#define SREAD_CHECK		(state->gl_state.per_frag_op.stencilTestEnabled)
					#define SWRITE_CHECK	(SREAD_CHECK && state->gl_state.fb_op.stencilWriteMask)
					#define CHECK(type)	(maskbits & ##type##_BIT)
					#define CREAD_BIT	(0x1)
					#define CWRITE_BIT	(0x2)
					#define ZREAD_BIT	(0x4)
					#define ZWRITE_BIT	(0x8)
					#define SREAD_BIT	(0x10)
					#define SWRITE_BIT	(0x20)
					#define ZBUF_READ	((maskbits & ZREAD_BIT) && (maskbits & (ZWRITE_BIT | CWRITE_BIT)))
					#define SBUF_READ	((maskbits & SREAD_BIT) && (maskbits & (SWRITE_BIT | CWRITE_BIT)))
					maskbits =	(CREAD_CHECK?	CREAD_BIT  : 0)	|
								(CWRITE_CHECK?	CWRITE_BIT : 0)	|
								(ZREAD_CHECK?	ZREAD_BIT  : 0)	|
								(ZWRITE_CHECK?	ZWRITE_BIT : 0)	|
								(SREAD_CHECK?	SREAD_BIT  : 0)	|
								(SWRITE_CHECK?	SWRITE_BIT : 0);
					__cb_writeReg(PA_WO_FU_COLOR_READ_MASK, ((maskbits & CREAD_BIT) ? 0xf : 0x0), 0xf);
					__cb_writeReg(PA_WO_FU_COLOR_WRITE_MASK, ((maskbits & CWRITE_BIT) ? 0xf : 0x0), 0xf);
					__cb_writeReg(PA_WO_FU_ZCACHE_READ_MASK, (ZBUF_READ? 2 : 0) | (SBUF_READ ? 1 : 0), 0xf);
					__cb_writeReg(PA_WO_FU_ZCACHE_WRITE_MASK, ((maskbits & ZWRITE_BIT) ? 2 : 0) | ((maskbits & SWRITE_BIT) ? 1 : 0), 0xf);
					break;
				case GL_FRAGOP_MODE_GAS_ACC_DMP:
					__cb_writeReg(PA_WO_FU_COLOR_WRITE_MASK, 0xf, 0xf);
					__cb_writeReg(PA_WO_FU_COLOR_READ_MASK, 0xf, 0xf);
					__cb_writeReg(PA_WO_FU_ZCACHE_READ_MASK, 0x3, 0xf);
					__cb_writeReg(PA_WO_FU_ZCACHE_WRITE_MASK, 0x0, 0xf);
					break;
				case GL_FRAGOP_MODE_SHADOW_DMP:
					__cb_writeReg(PA_WO_FU_COLOR_WRITE_MASK, 0xf, 0xf);
					__cb_writeReg(PA_WO_FU_COLOR_READ_MASK, 0xf, 0xf);
					__cb_writeReg(PA_WO_FU_ZCACHE_READ_MASK, 0x0, 0xf);
					__cb_writeReg(PA_WO_FU_ZCACHE_WRITE_MASK, 0x0, 0xf);
					break;
			}
		}
	}
	return;
}

void __shv_preDraw(void){
	if (__shman->current_program == 0)
		return;

	if (__shman->current_program->fragment_state.operation.mode == GL_FRAGOP_MODE_GAS_ACC_DMP){
		if (__shman->last_fragopmode != GL_FRAGOP_MODE_GAS_ACC_DMP)
			__cb_writeReg(PA_WO_GAS_ACC_MAX_RESULT_INT, 0, 0xf);
	}
	else{
		if (__shman->last_fragopmode == GL_FRAGOP_MODE_GAS_ACC_DMP && __shman->current_program->fragment_state.gas.autoacc)
			__sys_markGasAcmMax();
	}
	__shman->last_fragopmode = __shman->current_program->fragment_state.operation.mode;
	
	return;
}

extern unsigned g_cuBlendFunc;
extern unsigned g_cuDepthTest;
extern unsigned g_cuStencilTest;
extern unsigned g_cuStencilOp;
extern unsigned g_cuLogicOp;
extern unsigned g_tiEarlyDepthFunc;
extern unsigned g_tiClearEarlyDepth;
extern unsigned g_cuBlendColor;
extern unsigned g_cuGasDepth;

void __shv_initializeShaderValidator(bit_mask_t* _mask){
	CLEAR_ALL_MASK(*_mask);
	
	SET_MASK(*_mask, IF_HW_VERTBUFFER);
	SET_MASK(*_mask, IF_GL_TRI_OFFSET);
	SET_MASK(*_mask, IF_GL_VERT_ARRAY);
	SET_MASK(*_mask, IF_GL_SHADER_UNIFORM);
	SET_MASK(*_mask, IF_GL_VERT_CURRENT);
	SET_MASK(*_mask, IF_GL_FB_ACCESS);
	SET_MASK(*_mask, IF_GL_FB_SCISSOR);
	SET_MASK(*_mask, IF_HW_RASTERIZER_REGISTER);
	SET_MASK(*_mask, IF_GL_BINARY_PACKAGE);
	SET_MASK(*_mask, IF_GL_BINARY_EXE_VS);
	SET_MASK(*_mask, IF_GL_BINARY_EXE_GS);
	SET_MASK(*_mask, IF_GL_ATTACH_GS);
	SET_MASK(*_mask, IF_GL_DETACH_GS);
	SET_MASK(*_mask, IF_GL_PROGRAM);
	SET_MASK(*_mask, IF_GL_TEXTURE_LUT);
	SET_MASK(*_mask, IF_GL_LIGHTING_LUT_SAMPLERS);
	SET_MASK(*_mask, IF_GL_PROCTEX_LUT_SAMPLERS);
	SET_MASK(*_mask, IF_GL_FOG_LUT_SAMPLER);
	SET_MASK(*_mask, IF_GL_GAS_LUT_SAMPLERS);

	g_cuBlendFunc = 0x01010000;
	g_cuDepthTest = 0x00001f40;
	g_cuStencilTest = 0xff00ff10;
	g_tiEarlyDepthFunc = 0x00000003;
	g_tiClearEarlyDepth = 0x00ffffff;
	g_cuStencilOp = 0;
	g_cuLogicOp = 3;
	g_cuBlendColor = 0;
	g_cuGasDepth = 0x03000000;

	__shv_initializeShaderValidatorHWRegister();

	return;
}	/* void __shv_initializeShaderValidator(bit_mask_t* _mask) */


void __shv_initializeShaderValidatorHWRegister(void){
	int i, j;

	__cb_writeReg(PA_RW_LB_MULTIVP_WRITE, 1, 1);
	__cb_writeReg(PA_RW_LB_VP0_WRITE_DISABLE, 0, 1);
	__cb_writeReg(PA_RW_VP_MODE, 0x80000000, 0x8);
	__cb_writeReg(PA_RW_CI_VP_MODE, 0, 0xb);
	__cb_writeReg(PA_RW_VC_SEND_MODE, 0, 0xf);
	__cb_writeReg(PA_RW_VC_ATTR_COUNT, 0, 0xf);
	__cb_writeReg(PA_RW_VC_SUBDIV_CONF, 0, 0xf);
	__cb_writeReg(PA_RW_VC_DRAW_MODE, 0, 0x1);
	__cb_writeReg(PA_RW_LB_ATTR_COUNT, 0, 0xf);
	__cb_writeReg(PA_RW_SYNC_ATTR_COUNT, 0, 0xf);
	__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 0x5);

	__cb_fillRegs(PA_WO_CU_BEGIN + 1, PA_WO_CU_END - PA_WO_CU_BEGIN, 0);
	__cb_writeReg(0x11f, 0x010140, 0xf);
	
	__cb_writeReg(PA_WO_CU_MODE, 0xE40100, 0xf);
	__cb_writeReg(PA_WO_CU_BLEND_FUNC, 0x01010000, 0xf);
	__cb_writeReg(PA_WO_CU_DEPTH_T, 0x00001f40, 0xf);
	__cb_writeReg(PA_WO_CU_STENCIL_T, 0xff00ff10, 0xf);
	__cb_writeReg(PA_RW_TI_ZCOMP_MODE, 0x00000003, 1);
	__cb_writeReg(PA_RW_TI_EARLYZ_MODE, 0, 1);
	__cb_writeReg(PA_RW_TI_SCISSORING_ATTR, 0, 0xf);
	__cb_writeReg(PA_RW_TI_SCISSORING_POS0, 0, 0xf);
	__cb_writeReg(PA_RW_TI_SCISSORING_POS1, 0, 0xf);
	__cb_writeReg(PA_WO_FU_EZ_EN, 0, 1);
	__cb_writeReg(PA_WO_FU_MEM_ADRS_MODE, 0, 0xf);
	__cb_writeReg(PA_RW_EZ_Z_CLEAR_VALUE, 0x00ffffff, 0x7);
	__cb_writeReg(PA_WO_CU_LOGIC, 3, 0xf);
	__cb_writeReg(PA_W0_GAS_DELTAZ_DEPTH, 0x03000000, 0x8);
	
	__cb_fillRegs(PA_TS_BEGIN, PA_RW_TS_WORD_NUM - PA_TS_BEGIN + 1, 0);
	__cb_fillRegs(PA_RW_TS_VTXWORD1, PA_RW_TS_VTXWORD7 - PA_RW_TS_VTXWORD1 + 1, 0x1f1f1f1f);

	__cb_writeReg(PA_RW_TS_W_BUFFER_FLAT_SHADING, 0x100, 0xf);
	__cb_writeReg(PA_RW_TS_POLYOFFSETENABLE, 1, 0xf);
	__cb_writeReg(PA_RW_TI_CLOCK_ENABLE, 0, 0xf);
	__cb_writeReg(PA_RW_TI_RASTER_RULE, 0, 2);
	__cb_writeReg(0x69, 0x00020000, 0xc);

	__cb_writeReg(PA_WO_FU_COLOR_WRITE_MASK, 0xf, 0xf);
	__cb_writeReg(PA_WO_FU_COLOR_READ_MASK, 0xf, 0xf);
	__cb_writeReg(PA_WO_FU_ZCACHE_READ_MASK, 0x3, 0xf);
	__cb_writeReg(PA_WO_FU_ZCACHE_WRITE_MASK, 0x3, 0xf);

	for (i = 0; i < 7; i++){
		__cb_writeReg(PA_WO_LR_LUT_SEL_ADR, i << 8, 0xf);
		for (j = 0; j < MATERIAL_LUT_TABLE_SIZE; j += 8)
			__cb_fillRegs(PA_WO_LR_LUT_DATA0_ADR, 8, 0);
	}

	__cb_writeReg(PA_RW_VP_CONST_REG_ADDR, 0x80000000, 0xf);
	for (j = 0; j < 0x60; j += 2)
		__cb_fillRegs(PA_RW_VP_CONST_REG_TOP, 8, 0);

	{
		unsigned *tmp = (unsigned*)malloc(0x4000);
		memset(tmp, 0, 0x4000);
		
		__cb_writeReg(PA_RW_VP_PRAM_ADDR + GEO_VP_LEN, 0, 0xf);
		__cb_multiWriteReg(PA_RW_VP_PRAM_TOP + GEO_VP_LEN, 0x200, tmp);
		
		__cb_writeReg(PA_RW_VP_PRAM_ADDR, 0x200, 0xf);
		__cb_multiWriteReg(PA_RW_VP_PRAM_TOP, 0xe00, tmp);
		
		__cb_writeReg(PA_RW_VP_PROGRAM_RENEWAL + GEO_VP_LEN, 0, 0xf);
		free(tmp);
	}
	for (i = 0; i < 16; i++)
		__cb_writeReg(PA_RW_VP_INT_REG_TOP + GEO_VP_LEN + i, 0, 0xf);
	
	__cb_fillRegs(PA_RW_VP_INPUTMAP0, 2, 0xffffffff);
	__cb_fillRegs(PA_RW_CI_ARRAY0_ATTR1, 12 * 3, 0);

	__cb_writeReg(PA_WO_LR_FUNC_MODE3, 1, 0xf);
	__cb_writeReg(PA_WO_PROC_TEX_M2REG_CONF_START + 6, 0xfefcf8f0, 0xf);

	for (i = 0; i < RA_LAST; i++){
		if (__shman->default_ras_regs_be[i])
			__cb_writeReg(s_ras_addr_map[i], __shman->default_ras_regs[i], __shman->default_ras_regs_be[i]);
	}

	return;
}