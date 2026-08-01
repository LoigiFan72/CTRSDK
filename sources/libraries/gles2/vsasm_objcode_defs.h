#pragma once

#define VS_OBJ_HEADER_SIGN					"DVOJ"
#define VS_VPE_HEADER_SIGN					"DVPE"

typedef struct OBJ_HEADER_T{
	char				signature[4];
	char				version[4];
	unsigned int		setup_offset;
	unsigned int		setup_count;
	unsigned int		section_offset;
	unsigned int		section_count;
	unsigned int		label_offset;
	unsigned int		label_count;
	unsigned int		inst_offset;
	unsigned int		inst_count;
	unsigned int		swizzle_offset;
	unsigned int		swizzle_count;
	unsigned int		line_offset;
	unsigned int		line_count;
	unsigned int		reloc_offset;
	unsigned int		reloc_count;
	unsigned int		outmap_offset;
	unsigned int		outmap_count;
	unsigned int		bsym_offset;
	unsigned int		bsym_count;
	unsigned int		string_offset;
	unsigned int		string_size;
} obj_header_t;

enum obj_shader_type_t{
	OBJ_SHADER_TYPE_VERTEX_SHADER = 0,
	OBJ_SHADER_TYPE_GEOMETRY_SHADER,
	
	OBJ_SHADER_TYPE_MAX_BIT = (1u << 30)
};

typedef struct OBJ_SETUP_T{
	unsigned short		type;
	unsigned short		index;
	unsigned int		value[4];
} obj_setup_t;

enum obj_setup_type_t{
	OBJ_SETUP_TYPE_BOOL = 0,
	OBJ_SETUP_TYPE_INT,
	OBJ_SETUP_TYPE_CONST,
	
	OBJ_SETUP_TYPE_MAX_BIT = (1u << 30)
};

struct obj_intreg_t{
	unsigned int		count		: 8;
	unsigned int		init		: 8;
	unsigned int		step		: 8;
	unsigned int		padding		: 8;
};

typedef struct OBJ_LABEL_T{
	int					index;
	unsigned int		address;
	unsigned int		length;
	unsigned int		str_index;
} obj_label_t;

typedef struct OBJ_SECTION_T{
	unsigned int		inst_offset;
	unsigned int		inst_count;
	unsigned int		label_offset;
	unsigned int		label_count;
	unsigned int		outmap_offset;
	unsigned int		outmap_count;
	unsigned int		bsym_offset;
	unsigned int		bsym_count;
	unsigned int		execution_address;
	unsigned int		logical_address;
	unsigned short		input_mask;
	unsigned short		output_mask;
	unsigned short		last_pc;
	unsigned short		last_input_pc;
} obj_section_t;

typedef struct OBJ_LINE_T{
	unsigned int		str_index;
	unsigned int		line_no;
} obj_line_t;

typedef struct OBJ_RELOC_T{
	unsigned int		address;
	unsigned short		type;
	unsigned short		reserve;
	unsigned int		index;
} obj_reloc_t;

enum obj_reloc_type_t{
	OBJ_RELOC_TYPE_ADDRESS = 0,
	OBJ_RELOC_TYPE_UNDEF_SR,
	OBJ_RELOC_TYPE_UNDEF_ADDR,
	OBJ_RELOC_TYPE_SWIZZLE,
	
	OBJ_RELOC_TYPE_MAX_BIT = (1u << 30)
};

typedef struct OBJ_OUTMAP_T{
	unsigned short		type;
	unsigned short		index;
	unsigned short		mask;
	unsigned short		reserve;
} obj_outmap_t;

enum obj_outmap_type_t{
	OBJ_OUTMAP_TYPE_POS = 0,
	OBJ_OUTMAP_TYPE_QC,
	OBJ_OUTMAP_TYPE_COLOR,
	OBJ_OUTMAP_TYPE_TEX0,
	OBJ_OUTMAP_TYPE_TEX0W,
	OBJ_OUTMAP_TYPE_TEX1,
	OBJ_OUTMAP_TYPE_TEX2,
	OBJ_OUTMAP_TYPE_TEX3,
	OBJ_OUTMAP_TYPE_VIEW,
	OBJ_OUTMAP_TYPE_GEN,
	
	OBJ_OUTMAP_TYPE_MAX_BIT = (1u << 30)
};

typedef struct OBJ_BSYM_T{
	unsigned int		name_index;
	unsigned short		start_index;
	unsigned short		end_index;
} obj_bsym_t;

typedef unsigned int	obj_inst_t;

typedef struct OBJ_SWZ_T{
	unsigned int			value;
	unsigned short			used_info;
	unsigned short			reserve;
} obj_swz_t;