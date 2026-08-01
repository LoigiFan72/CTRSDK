#pragma once

#include "common.h"
#include "state.h"

typedef struct tag_vertex_state_header_t{
	unsigned		signature;
	unsigned		version;
	unsigned		vscoll_state_offset;
	unsigned		vscoll_state_num;
	unsigned		vb_state_num;
} vertex_state_header_t;

typedef struct tag_vscoll_state_t{
	unsigned					vb_array_index;
	unsigned					vb_element_index;
	stt_vert_array_state_t		arraystate;
	stt_vert_current_state_t	attrstate;
} vscoll_state_t;

typedef struct tag_vb_state_t{
	GLsizeiptr				size;
	GLenum					usage;
	GLenum					allocarea;
	GLenum					transtype;
} vb_state_t;