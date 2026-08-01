// Filename: vertarraystate.c
//
// Project: Horizon

#include "common.h"
#include "state.h"
#include "util.h"
#include "error.h"
#include "vbmanager.h"

void GL_APIENTRY glEnableVertexAttribArray(GLuint index){
	GET_CURRENT_STATE(state);

	BASE_GL_FAIL_IF(index >= 16, GL_INVALID_VALUE);

	if (state->gl_state.vertarray.array[index].enabled)
		return;
	state->gl_state.vertarray.array[index].enabled = GL_TRUE;
	
	SET_MASK(state->upd_mask, IF_GL_VERT_ARRAY);
	
	return;
}

void GL_APIENTRY glDisableVertexAttribArray(GLuint index){
	GET_CURRENT_STATE(state);

	BASE_GL_FAIL_IF(index >= 16, GL_INVALID_VALUE);

	if (!state->gl_state.vertarray.array[index].enabled)
		return;
	state->gl_state.vertarray.array[index].enabled = GL_FALSE;
	
	SET_MASK(state->upd_mask, IF_GL_VERT_ARRAY);
	
	return;
}

void GL_APIENTRY glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean norm, GLsizei stride, const void* ptr){
	GET_CURRENT_STATE(state);

	BASE_GL_FAIL_IF(index >= 16 || size < 1 || size > 4 || stride < 0, GL_INVALID_VALUE);
	BASE_GL_FAIL_IF(type != GL_FLOAT && type != GL_SHORT && type != GL_BYTE && type != GL_UNSIGNED_BYTE, GL_INVALID_ENUM);
	BASE_GL_FAIL_IF((type == GL_FLOAT && ((unsigned)ptr & 0x3)) || (type == GL_SHORT && ((unsigned)ptr & 0x1)), GL_INVALID_VALUE);
	BASE_GL_FAIL_IF(norm, GL_INVALID_OPERATION);

	CHECK_ASSIGN(state->gl_state.vertarray.array[index].size, size, IF_GL_VERT_ARRAY);
	CHECK_ASSIGN(state->gl_state.vertarray.array[index].type, type, IF_GL_VERT_ARRAY);
	CHECK_ASSIGN(state->gl_state.vertarray.array[index].norm, norm, IF_GL_VERT_ARRAY);
	CHECK_ASSIGN(state->gl_state.vertarray.array[index].stride, stride, IF_GL_VERT_ARRAY);
	CHECK_ASSIGN(state->gl_state.vertarray.array[index].ptr, ptr, IF_GL_VERT_ARRAY);
	CHECK_ASSIGN(state->gl_state.vertarray.array[index].buffer, state->gl_state.vertbuffer.arrayID, IF_GL_VERT_ARRAY);
	__vb_setAttribArrayBuffer(index);
	
	return;
}

