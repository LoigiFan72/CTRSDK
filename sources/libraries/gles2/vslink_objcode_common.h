#pragma once

#include "common.h"

#define VS_BIN_HEADER_SIGN					"DVLB" // Magic DVLB
#define VS_PKG_HEADER_SIGN					"DVLP" // Magic DVLP
#define VS_EXE_HEADER_SIGN					"DVLE" // Magic DVLE

typedef struct BIN_HEADER_T{
	unsigned int		signature;
	unsigned int		exe_number;
	unsigned int		exe_offset_top;
} bin_header_t;

typedef struct PKG_HEADER_T{
	unsigned int		signature;
	char				version[2];
	unsigned short		reserved0;
	unsigned int		inst_offset;
	unsigned int		inst_count;
	unsigned int		swizzle_offset;
	unsigned int		swizzle_count;
	unsigned int		line_offset;
	unsigned int		line_count;
	unsigned int		string_offset;
	unsigned int		string_size;
} pkg_header_t;

typedef struct EXE_HEADER_T{
	unsigned int		signature;
	char				version[2];
	unsigned char		shader_type;
	unsigned char		merge_output_maps;
	unsigned int		main_addr;
	unsigned int		end_addr;
	unsigned short		input_mask;
	unsigned short		output_mask;
	unsigned char		geometry_data_mode;
	unsigned char		start_index;
	unsigned char		variable_sized_primitive_size;
	unsigned char		fixed_sized_primitive_size;
	unsigned int		setup_offset;
	unsigned int		setup_count;
	unsigned int		label_offset;
	unsigned int		label_count;
	unsigned int		outmap_offset;
	unsigned int		outmap_count;
	unsigned int		bsym_offset;
	unsigned int		bsym_count;
	unsigned int		string_offset;
	unsigned int		string_size;
} exe_header_t;