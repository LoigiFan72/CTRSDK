#pragma once

#include "bitmask.h"
#include "surface.h"
#include "rasdefs.h"

typedef struct ras_textureunit_regmap_t		tex_reg_map_t;

typedef enum TX_TRILIN_MODE_T{
	TX_TLM_LOW  = 0x0,
	TX_TLM_HIGH = 0x1,
	TX_TLM_ALT  = 0x2,
	TX_TLM_NON  = 0x3,
	
	TX_TLM_MAX_BIT = (1u << 30)
} tx_trilin_mode_t;

typedef enum TX_ADDRSS_MODE_T{
	TX_ADM_CLMP = 0x0,
	TX_ADM_CLBD = 0x1,
	TX_ADM_REPT = 0x2,
	TX_ADM_MIRR = 0x3,
	TX_ADM_MIR1 = 0x4,
	TX_ADM_MR1B = 0x5,
	TX_ADM_ERR0 = 0x6,
	TX_ADM_ERR1 = 0x7,
	
	TX_ADM_MAX_BIT = (1u << 30)
} tx_addrss_mode_t;

typedef enum TX_TEXTYP_MODE_T{
	TX_TTM_TEX  = 0x0,
	TX_TTM_ENV  = 0x1,
	TX_TTM_SDW  = 0x2,
	TX_TTM_PRJT = 0x3,
	TX_TTM_ENV_SDW = 0x4,
	
	TX_TTM_MAX_BIT = (1u << 30)
} tx_textyp_mode_t;

typedef enum TX_TEXDAT_MODE_T{
	TX_TDM_RGBA = 0x0,
	TX_TDM_RGB  = 0x1,
	TX_TDM_ETC  = 0x2,
	
	TX_TDM_MAX_BIT = (1u << 30)
} tx_texdat_mode_t;

typedef enum TX_MINFILTER_SEL_T{
	TX_PCS_NEAREST					= 0x0,
	TX_PCS_LINEAR					= 0x1,
	TX_PCS_NEAREST_MIPMAP_NEAREST	= 0x2,
	TX_PCS_LINEAR_MIPMAP_NEAREST	= 0x3,
	TX_PCS_NEAREST_MIPMAP_LINEAR	= 0x4,
	TX_PCS_LINEAR_MIPMAP_LINEAR		= 0x5,
	
	TX_PCS_MAX_BIT = (1u << 30)
} tx_minfilter_sel_t;

typedef enum TX_LUT_UPDATE_FLAG_T{
	TX_UPDATE_FLAG_LR = 0,
	TX_UPDATE_FLAG_FOG,
	TX_UPDATE_FLAG_GAS,
	TX_UPDATE_FLAG_PT_MAP,
	TX_UPDATE_FLAG_PT_NOISE,
	TX_UPDATE_FLAG_PT_RGBA,
	
	TX_UPDATE_FLAG_MAX_BIT = (1u << 30)
} tx_lut_update_flag_t;

typedef struct TX_TEXTURE_T{
	srf_container_t		surfaceContainer;
	GLuint				id;
} tx_texture_t;

typedef struct TX_TEX_CONTAINER_BASE_T{
	GLenum				magFilter;
	GLenum				minFilter;
	GLenum				wrapS;
	GLenum				wrapT;
	int					numLevels;
	GLint				minLOD;
	GLfloat				BorderColor[4];
	GLfloat				lodBias;
	GLenum				transType;
	GLboolean			genMipmap;
	char                padding3[3];
} tx_tex_container_base_t;

typedef struct TX_TEX_CONTAINER_T{
	tx_tex_container_base_t		container;
	tx_texture_t				texture;
} tx_tex_container_t;

typedef struct TX_CUBE_TEX_CONTAINER_T{
	tx_tex_container_base_t		container;
	tx_texture_t				texture[6];
} tx_cube_tex_container_t;

typedef struct TX_LUT_CONTAINER_T{
	GLuint			id;
	GLfloat			src_plane[512];
	GLuint*			lr_plane;
	GLuint*			fog_plane;
	GLuint*			gas_plane;
	GLuint*			pt_map_plane;
	GLuint*			pt_noise_plane;
	GLubyte*		pt_rgba_plane;
	unsigned		updated_flags;
} tx_lut_container_t;

typedef struct TX_COLLECTION_CONTAINER_T{
	GLuint			id;
	GLuint			tex2d[3];
	GLuint			texcube[3];
	GLuint			luts[32];
} tx_collection_container_t;

typedef enum TX_TEX_TYPE_T{
	TT_2D,
	TT_CUBE,
	TT_TEX_COLL,
	TT_LUT,
	
	TT_MAX_BIT = (1u << 30)
} tx_tex_type_t;

typedef struct TEX_LIST_T{
	void*					container;
	tx_tex_type_t			type;
	GLuint					id;
	struct TEX_LIST_T*		next;
} tex_list_t;

#define TX_LIST_TABLE_SIZE		0x1ff
typedef struct TEX_MANAGER_T{
	tx_tex_container_t*			defaultTexContainer;
	tx_cube_tex_container_t*	defaultCubeTexContainer;
	tx_collection_container_t*	defaultCollectionContainer;
	GLuint						deleteCollection;
	tex_list_t*					tex_list_table[TX_LIST_TABLE_SIZE + 1];
	tex_list_t*					bound2DTex[3];
	tex_list_t*					boundCubeTex[3];
	tex_list_t*					boundLutTex[32];
	tex_list_t*					boundTexColl;
} tex_manager_t;

GLint __tx_initializeTexManager(tex_manager_t* texman);
void __tx_finalizeTexManager();
tx_tex_container_t* __tx_getTexture2D(GLuint _tex);
tx_cube_tex_container_t* __tx_getTextureCube(GLuint _tex);
tx_lut_container_t* __tx_getBoundTextureLut(GLuint _lut);

void __txv_initializeTextureValidator(bit_mask_t* _mask);
void __txv_validateTextureValidator(bit_mask_t* _mask);
void __txv_initializeTextureValidatorHWRegister(void);