#pragma once

#include "bitmask.h"
#include "state.h"

enum{
	RA_CI_VP_MODE = 0,
	RA_VP0_BOOL_REG,
	RA_VP0_INT_REG_TOP_00,
	RA_VP0_INT_REG_TOP_01,
	RA_VP0_INT_REG_TOP_02,
	RA_VP0_INT_REG_TOP_03,
	RA_VP0_MODE,
	RA_VP0_PROGRAM_ADDRESS,
	RA_VP0_OUTPUT_MASK,
	RA_VP1_BOOL_REG,
	RA_VP1_INT_REG_TOP_00,
	RA_VP1_INT_REG_TOP_01,
	RA_VP1_INT_REG_TOP_02,
	RA_VP1_INT_REG_TOP_03,
	RA_VP1_MODE,
	RA_VP1_PROGRAM_ADDRESS,
	RA_VP1_OUTPUT_MASK,
	RA_VC_ATTR_COUNT,
	RA_VC_SEND_MODE,
	RA_VC_SUBDIV_CONF,
	RA_LB_ATTR_COUNT,
	RA_SYNC_ATTR_COUNT,
	RA_RASIN_TRI_SETTING,
	RA_TS_ZSCALE,
	RA_TS_ZBIAS,
	RA_TS_WORD_NUM,
	RA_TS_VTXWORD1,
	RA_TS_VTXWORD2,
	RA_TS_VTXWORD3,
	RA_TS_VTXWORD4,
	RA_TS_VTXWORD5,
	RA_TS_VTXWORD6,
	RA_TS_VTXWORD7,
	RA_TS_USRCLIPENABLE,
	RA_TS_USRCLIPA,
	RA_TS_USRCLIPB,
	RA_TS_USRCLIPC,
	RA_TS_USRCLIPD,
	RA_TI_TEX_DERIVS_ENABLE,
	RA_TI_Z_DIV_W,
	RA_TI_CLOCK_ENABLE,
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
	RA_TB_FOG_EN,
	RA_TB_FOG_RGB,
	RA_TB_FOG_ATTN,
	RA_TB_GAS_ACC_MAX,
	RA_TB_BUFFER_COLOR,
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
	RA_LAST,
	
	RA_MAX_BIT = (1u << 30)
};

typedef struct FLOAT_CONST_T{
	unsigned				index;
	unsigned				value[3];
} float_const_t;

typedef struct FLOAT_CONST_REG_T{
	unsigned				value[4];
} float_const_reg_t;

typedef struct UNIFORM_INFO_T{
	GLenum		type;
	int			addr;
	int			index;
	int			name_index;
	int			size;
} uniform_info_t;

#define FLOAT_UNIFORM_OFFSET		0x10
#define LOOP_UNIFORM_OFFSET			0x70
#define BOOL_UNIFORM_OFFSET			0x78

typedef struct FIXED_UNIFORM_INFO_T{
	GLsizei					size;
	GLenum					type;
	char*					name;
} fixed_uniform_info_t;

typedef struct ATTRIB_INFO_T{
	GLenum					type;
	int						name_index;
} attrib_info_t;

typedef struct UNI_LOCATION_T{
	union{
		unsigned int		value;
		struct uni_location_common_t{
			unsigned reserved0		: 18;
			unsigned fragment		: 1;
			unsigned prog			: 13;
		} common;

		struct uni_location_vs_t{
			unsigned useroffset		: 7;
			unsigned mapindex		: 11;
			unsigned fragment		: 1;
			unsigned prog			: 13;
		} vs;

		struct uni_location_fs_t{
			unsigned size			: 2;
			unsigned offset			: 16;
			unsigned fragment		: 1;
			unsigned prog			: 13;
		} fs;

	} location;

	unsigned		name_index;

	struct uni_location_vs_extinfo_t{
		unsigned		reserved0		: 10;
		unsigned		geometry		: 1;
		unsigned		index			: 2;
		unsigned		size			: 2;
		unsigned		matrix			: 1;
		unsigned		intbool			: 1;
		unsigned		regsize			: 7;
		unsigned		offset			: 8;
	} vs_ext;
} uni_location_t;

#define MAX_SHM_IMPL_UNIFORMS		0x07ff
#define MAX_SHM_IMPL_PROGRAMS		0x1fff

typedef struct ATTRIB_LOCATION_T{
	char*						name;
	GLuint						index;
	struct ATTRIB_LOCATION_T*	next;
} attrib_location_t;

typedef struct LINKED_ATTRIB_T{
	GLenum					type;
	int						reg_index;
	int						name_index;
} linked_attrib_t;

typedef struct ATTRIB_ADDR_MAP_T{
	unsigned					attr_index;
	struct ATTRIB_ADDR_MAP_T*	next;
} attrib_addr_map_t;

typedef struct RAS_REG_MASK_T{
	unsigned		mask_[6];
} ras_reg_mask_t;

#define CLEAR_ALL_RASREGMASK(mask) \
{ \
	int masknum_; \
	for (masknum_ = 0; masknum_ < ((RA_LAST >> 5) + 1); masknum_++) \
	{ \
		(mask).mask_[masknum_] = 0; \
	} \
}
#define SET_ALL_RASREGMASK(mask) \
{ \
	int masknum_; \
	for (masknum_ = 0; masknum_ < ((RA_LAST >> 5) + 1); masknum_++) \
	{ \
		(mask).mask_[masknum_] = ~0; \
	} \
}
#define SET_RASREGMASK(mask, flag) \
{ \
	(mask).mask_[(flag) >> 5] |= ((unsigned)1 << ((flag) & 0x1f)); \
}
#define CLEAR_RASREGMASK(mask, flag) \
{ \
	(mask).mask_[(flag) >> 5] &= ~((unsigned)1 << ((flag) & 0x1f)); \
}

#define GET_RASREGMASK(mask, flag)	((mask).mask_[(flag) >> 5] & ((unsigned)1 << ((flag) & 0x1f)))

#define CLEAR_PARTIAL_RASREGMASK(mask, clearmask) \
{ \
	int masknum_; \
	for (masknum_ = 0; masknum_ < ((RA_LAST >> 5) + 1); masknum_++) \
	{ \
		(mask).mask_[masknum_] &= ~(clearmask).mask_[masknum_]; \
	} \
}

typedef struct EXE_INFO_T{
	unsigned char			shader_type;
	unsigned char			merge_output_maps;
	unsigned char			geometry_data_mode;
	unsigned char			start_index;
	unsigned char			variable_sized_primitive_size;
	unsigned char			fixed_sized_primitive_size;
	unsigned short			input_mask;
	unsigned short			output_mask;
	char                    padding2[2];
	unsigned int			num_input;
	unsigned int			num_output;
	unsigned int			main_addr;
	unsigned int			end_addr;
	unsigned int			bool_consts;
	unsigned int			int_consts[4];
	float_const_t*			float_consts;
	unsigned int			num_float_const;
	unsigned int			output_format[7];
	unsigned int			clock_enable;
	uniform_info_t*			uniforms;
	unsigned int			num_uniform;
	attrib_info_t			attribs[16];
	char*					strings;
	unsigned				string_size;
} exe_info_t;

typedef struct FRAGMENT_STATE_T{
	struct frag_lighting_t{
		GLboolean			use_const_specular2;
		GLboolean			shadow_primary;
		GLboolean			shadow_secondary;
		GLboolean			shadow_alpha;
		GLboolean			bump_renorm;
		char                padding3[3];
		GLint				samplers[6];
		GLuint				sampler_mask;
		GLclampf			ambient[4];
	} lighting;
	
	struct frag_light_t{
		GLboolean			enabled;
		char                padding3[3];
		GLclampf			ambient[4];
		GLclampf			diffuse[4];
		GLclampf			specular[4];
		GLfloat				specular2[4];
		GLfloat				position[4];
		GLfloat				spot_dir[3];
		GLint				spot_sampler;
		GLfloat				da_bias;
		GLfloat				da_scale;
		GLint				da_sampler;
	} lights[8];
	
	struct frag_material_t{
		GLclampf			ambient[4];
		GLclampf			diffuse[4];
		GLclampf			specular[4];
		GLfloat				specular2[4];
		GLclampf			emission[4];
	} material;
	
	struct frag_proctex_t{
		GLint				samplers[7];
		GLenum				sampler_type;
		GLfloat				bias;
		GLfloat				noiseU[3];
		GLfloat				noiseV[3];
	} proctex;
	
	struct frag_tex_t{
		GLenum				sampler_type[3];
	} tex;
	
	struct frag_operation_t{
		GLenum				mode;
		GLfloat				penumbra_bias;
		GLfloat				penumbra_scale;
		GLfloat				shadow_bias;
		GLfloat				shadow_scale;
		GLfloat				wscale;
		GLfloat				clipplane[4];
		GLfloat				alpha_ref;
	} operation;
	
	struct frag_fog_t{
		GLint				sampler;
		GLfloat				color[3];
	} fog;

	struct frag_gas_t{
		GLboolean			autoacc;
		char                padding3[3];
		GLint				samplers[3];
		GLfloat				lightxy[3];
		GLfloat				lightz[4];
		GLfloat				deltaz;
		GLfloat				accmax;
		GLfloat				attenuation;
	} gas;

	struct frag_texenv_t{
		GLfloat				constrgba[6][4];
		GLfloat				constrgba2[4];
	} texenv;

} fragment_state_t;

typedef struct BIN_LIST_T{
	unsigned*				program;
	unsigned				num_program;
	unsigned*				swizzle;
	unsigned				num_swizzle;
	exe_info_t*				exe_infos;
	unsigned				num_exe_infos;
	int						ref_count;
	struct BIN_LIST_T*		prev;
	struct BIN_LIST_T*		next;
} bin_list_t;

typedef struct SH_LIST_T{
	bin_list_t*				bin_list;
	int						exe_id;
	GLuint					id;
	GLenum					type;
	int						ref_count;
	GLboolean				delete_status;
	char                    padding3[3];
	struct SH_LIST_T*		next;
} sh_list_t;

#define NUM_CI_ARRAY_REG	(PA_RW_CI_ARRAY11_ATTR1 - PA_CI_BASE_ADDRESS + 1)

typedef struct PG_LIST_T{
	struct PG_LIST_T*		next;
	GLuint					id;
	GLuint					fs;
	sh_list_t*				vs;
	sh_list_t*				gs;
	GLboolean				shader_update_status;
	GLboolean				delete_status;
	GLboolean				link_status;
	GLboolean				link_update_status;
	attrib_location_t*		attrib_location;
	uni_location_t*			uni_location_map;
	unsigned				num_uniform;
	unsigned				gs_uniform_offset;
	unsigned				fs_uniform_offset;
	float_const_reg_t*		vs_floats;
	unsigned				vs_floats_addr[96];
	unsigned				num_vs_floats;
	unsigned				vs_floats_update_mask[3];
	float_const_reg_t*		gs_floats;
	unsigned				gs_floats_addr[96];
	unsigned				num_gs_floats;
	unsigned				gs_floats_update_mask[3];
	linked_attrib_t			attribs[16];
	bin_list_t*				bin_list;
	int						vs_exe_id;
	int						gs_exe_id;
	unsigned				clock_enable;
	GLboolean				geometry_shader_used;
	GLboolean				ci_index_base;
	unsigned char			ras_regs_be[189];
	char                    padding1;
	unsigned				ras_regs[189];
	ras_reg_mask_t			ras_reg_mask;
	attrib_addr_map_t		attr_addr_map[16];
	attrib_addr_map_t*		attr_addr_map_top;
	unsigned				attr_offset[16];
	unsigned				attr_stride[16];
	int						num_array_attr;
	unsigned				ci_const_list[16];
	int						num_const_attr;
	int						num_ci_array;
	unsigned				ci_config[38];
	unsigned				vs_attr_map[2];
	unsigned				gs_attr_map[2];
	fragment_state_t		fragment_state;
} pg_list_t;

#define PG_LIST_TABLE_SIZE		0x1ff
#define SH_LIST_TABLE_SIZE		0x1ff

typedef struct SHADER_MANAGER_T{
	pg_list_t*		current_program;
	pg_list_t*		last_validated_program;
	pg_list_t*		pg_list_table[PG_LIST_TABLE_SIZE + 1];
	sh_list_t*		sh_list_table[SH_LIST_TABLE_SIZE + 1];
	bin_list_t*		bin_list_top;
	unsigned		ras_regs[189];
	unsigned		default_ras_regs[189];
	unsigned char	ras_regs_be[189];
	unsigned char	default_ras_regs_be[189];
	char            padding2[2];
	unsigned		last_fragopmode;
} shader_manager_t;

int __shm_initializeShaderManager(shader_manager_t* shman);
void __shm_finalizeShaderManager(void);

void __shv_initializeShaderValidator(bit_mask_t* _mask);
void __shv_validateShaderValidator(bit_mask_t* _mask);
void __shv_partialValidateShaderValidator(bit_mask_t* _mask, GLbitfield _statemask);
void __shv_getUpdatedState(GLbitfield* _statemask);
void __shv_invalidateState(GLbitfield _statemask);
void __shv_preDraw(void);
void __shv_initializeShaderValidatorHWRegister(void);