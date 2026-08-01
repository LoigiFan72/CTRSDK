#pragma once

#include "texmanager.h"

typedef struct tag_texture_state_header_t{
	unsigned		signature;
	unsigned		version;
	unsigned		texcoll_state_offset;
	unsigned		texcoll_state_num;
	unsigned		tex_state_num;
} texture_state_header_t;

typedef struct tag_texture_collection_state_t{
	unsigned		tex_lut_id[3];
	unsigned		tex_2d_id[32];
	unsigned		tex_cube_id;
} texcoll_state_t;

typedef struct tag_texlut_state_t{
	GLfloat src_plane[32];
} texlut_state_t;

typedef struct tag_tex2d_state_t{
	tx_tex_container_base_t		texparam;
	srf_container_t				surface;
} tex2d_state_t;

typedef struct tag_texcube_state_t{
	tx_tex_container_base_t		texparam;
	srf_container_t				surface[6];
} texcube_state_t;