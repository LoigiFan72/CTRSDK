#pragma once

#include "common.h"

#define DMPGL_PROG_STATE_SIGNATURE	0x44535044	/* DPSD */
#define DMPGL_PROG_STATE_VERSION	0x00000000

typedef struct tag_program_state_header_t{
	unsigned			signature;
	unsigned			version;
	unsigned			bin_state_offset;
	unsigned			bin_state_num;
	unsigned			program_offset;
	unsigned			program_num;
	unsigned			swizzle_offset;
	unsigned			swizzle_num;
	unsigned			exe_state_offset;
	unsigned			exe_state_num;
	unsigned			exe_float_offset;
	unsigned			exe_float_num;
	unsigned			exe_uniform_offset;
	unsigned			exe_uniform_num;
	unsigned			string_offset;
	unsigned			string_size;
	unsigned			sh_state_offset;
	unsigned			sh_state_num;
	unsigned			prog_state_offset;
	unsigned			prog_state_num;
	unsigned			prog_attrib_location_offset;
	unsigned			prog_attrib_location_num;
	unsigned			prog_uniform_location_offset;
	unsigned			prog_uniform_location_num;
	unsigned			prog_attrib_name_string_offset;
	unsigned			prog_attrib_name_string_num;
	unsigned			prog_vsgs_float_offset;
	unsigned			prog_vsgs_float_num;
} program_state_header_t;

typedef struct tag_binobj_state_t{
	unsigned			program_offset;
	unsigned			program_num;
	unsigned			swizzle_offset;
	unsigned			swizzle_num;
	unsigned			exe_offset;
	unsigned			exe_num;
} binobj_state_t;

typedef struct tag_exeobj_state_t{
	unsigned char		shader_type;
	unsigned char		merge_output_maps;
	unsigned char		geometry_data_mode;
	unsigned char		start_index;
	unsigned char		variable_sized_primitive_size;
	unsigned char		fixed_sized_primitive_size;
	unsigned short		input_mask;
	unsigned short		output_mask;
	short 				pad2;
	unsigned			num_input;
	unsigned			num_output;
	unsigned			main_addr;
	unsigned			end_addr;
	unsigned			bool_consts;
	unsigned			int_consts[4];
	unsigned			float_const_offset;
	unsigned			float_const_num;
	unsigned			output_format[7];
	unsigned			clock_enable;
	unsigned			uniform_offset;
	unsigned			uniform_num;
	attrib_info_t		attribs[16];
	unsigned			string_offset;
	unsigned			string_size;
} exeobj_state_t;

typedef struct tag_shobj_state_t{
	unsigned			binobj_index;
	unsigned			exeobj_index;
	GLenum				type;
} shobj_state_t;

typedef struct tag_progobj_state_t{
	int					vs_shobj_index;
	int					gs_shobj_index;
	GLboolean			is_fs_attached;
	GLboolean			shader_update_status;
	GLboolean			link_status;
	GLboolean			link_update_status;
	unsigned			attrib_location_offset;
	unsigned			attrib_location_num;
	unsigned			uniform_location_offset;
	unsigned			uniform_location_num;
	unsigned			gs_uniform_offset;
	unsigned			fs_uniform_offset;
	unsigned			vsgs_floats_offset;
	unsigned			vs_floats_num;
	unsigned			vs_floats_addr[96];
	unsigned			vs_floats_update_mask[3];
	unsigned			gs_floats_num;
	unsigned			gs_floats_addr[96];
	unsigned			gs_floats_update_mask[3];
	linked_attrib_t		attribs[16];
	unsigned			clock_enable;
	GLboolean			geometry_shader_used;
	GLboolean			ci_index_base;
	unsigned char		ras_regs_be[189];
	char				padding1;
	unsigned			ras_regs[189];
	ras_reg_mask_t		ras_reg_mask;
	unsigned			attr_addr_map_index[16];
	unsigned			attr_addr_map_num;
	unsigned			attr_offset[16];
	unsigned			attr_stride[16];
	int					num_array_attr;
	unsigned			ci_const_list[16];
	int					num_const_attr;
	int					num_ci_array;
	unsigned			ci_config[39];
	unsigned			vs_attr_map[2];
	unsigned			gs_attr_map[2];
	fragment_state_t	fragment_state;
} progobj_state_t;

typedef struct tag_attriblocation_state_t{
	unsigned			name_index;
	unsigned			index;
} attriblocation_state_t;