// Filename: fragopstate.c
//
// Project: Horizon

#include "common.h"
#include "state.h"
#include "picaregmap.h"
#include "error.h"
#include "util.h"
#include "commandbuffer.h"

extern unsigned g_cuBlendFunc;
extern unsigned g_cuDepthTest;
extern unsigned g_cuStencilTest;
extern unsigned g_cuStencilOp;
extern unsigned g_cuLogicOp;
extern unsigned g_cuBlendColor;
extern unsigned g_tiEarlyDepthFunc;
extern unsigned g_tiClearEarlyDepth;
extern unsigned g_cuGasDepth;

void GL_APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor){
	glBlendFuncSeparate(sfactor, dfactor, sfactor, dfactor);
	
	return;
}

void GL_APIENTRY glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
	unsigned srcrgb = 0, dstrgb = 0, srca = 0, dsta = 0;

	GET_CURRENT_STATE(state);
	
	if (state->gl_state.per_frag_op.blendFuncSrcRGB == srcRGB &&
		state->gl_state.per_frag_op.blendFuncDstRGB == dstRGB &&
		state->gl_state.per_frag_op.blendFuncSrcAlpha == srcAlpha &&
		state->gl_state.per_frag_op.blendFuncDstAlpha == dstAlpha &&
		!state->forceMode)
		return;
	
	switch (srcRGB){
		case GL_ZERO:						srcrgb = 0;		break;
		case GL_ONE:						srcrgb = 1;		break;
		case GL_SRC_COLOR:					srcrgb = 2;		break;
		case GL_ONE_MINUS_SRC_COLOR:		srcrgb = 3;		break;
		case GL_DST_COLOR:					srcrgb = 4;		break;
		case GL_ONE_MINUS_DST_COLOR:		srcrgb = 5;		break;
		case GL_SRC_ALPHA:					srcrgb = 6;		break;
		case GL_ONE_MINUS_SRC_ALPHA:		srcrgb = 7;		break;
		case GL_DST_ALPHA:					srcrgb = 8;		break;
		case GL_ONE_MINUS_DST_ALPHA:		srcrgb = 9;		break;
		case GL_CONSTANT_COLOR:				srcrgb = 0xa;	break;
		case GL_ONE_MINUS_CONSTANT_COLOR:	srcrgb = 0xb;	break;
		case GL_CONSTANT_ALPHA:				srcrgb = 0xc;	break;
		case GL_ONE_MINUS_CONSTANT_ALPHA:	srcrgb = 0xd;	break;
		case GL_SRC_ALPHA_SATURATE:			srcrgb = 0xe;	break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}

	switch (dstRGB){
		case GL_ZERO:						dstrgb = 0;		break;
		case GL_ONE:						dstrgb = 1;		break;
		case GL_SRC_COLOR:					dstrgb = 2;		break;
		case GL_ONE_MINUS_SRC_COLOR:		dstrgb = 3;		break;
		case GL_DST_COLOR:					dstrgb = 4;		break;
		case GL_ONE_MINUS_DST_COLOR:		dstrgb = 5;		break;
		case GL_SRC_ALPHA:					dstrgb = 6;		break;
		case GL_ONE_MINUS_SRC_ALPHA:		dstrgb = 7;		break;
		case GL_DST_ALPHA:					dstrgb = 8;		break;
		case GL_ONE_MINUS_DST_ALPHA:		dstrgb = 9;		break;
		case GL_CONSTANT_COLOR:				dstrgb = 0xa;	break;
		case GL_ONE_MINUS_CONSTANT_COLOR:	dstrgb = 0xb;	break;
		case GL_CONSTANT_ALPHA:				dstrgb = 0xc;	break;
		case GL_ONE_MINUS_CONSTANT_ALPHA:	dstrgb = 0xd;	break;
		case GL_SRC_ALPHA_SATURATE:			dstrgb = 0xe;	break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}

	switch (srcAlpha){
		case GL_ZERO:						srca = 0;		break;
		case GL_ONE:						srca = 1;		break;
		case GL_SRC_COLOR:					srca = 2;		break;
		case GL_ONE_MINUS_SRC_COLOR:		srca = 3;		break;
		case GL_DST_COLOR:					srca = 4;		break;
		case GL_ONE_MINUS_DST_COLOR:		srca = 5;		break;
		case GL_SRC_ALPHA:					srca = 6;		break;
		case GL_ONE_MINUS_SRC_ALPHA:		srca = 7;		break;
		case GL_DST_ALPHA:					srca = 8;		break;
		case GL_ONE_MINUS_DST_ALPHA:		srca = 9;		break;
		case GL_CONSTANT_COLOR:				srca = 0xa;		break;
		case GL_ONE_MINUS_CONSTANT_COLOR:	srca = 0xb;		break;
		case GL_CONSTANT_ALPHA:				srca = 0xc;		break;
		case GL_ONE_MINUS_CONSTANT_ALPHA:	srca = 0xd;		break;
		case GL_SRC_ALPHA_SATURATE:			srca = 0xe;		break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}

	switch (dstAlpha){
		case GL_ZERO:						dsta = 0;		break;
		case GL_ONE:						dsta = 1;		break;
		case GL_SRC_COLOR:					dsta = 2;		break;
		case GL_ONE_MINUS_SRC_COLOR:		dsta = 3;		break;
		case GL_DST_COLOR:					dsta = 4;		break;
		case GL_ONE_MINUS_DST_COLOR:		dsta = 5;		break;
		case GL_SRC_ALPHA:					dsta = 6;		break;
		case GL_ONE_MINUS_SRC_ALPHA:		dsta = 7;		break;
		case GL_DST_ALPHA:					dsta = 8;		break;
		case GL_ONE_MINUS_DST_ALPHA:		dsta = 9;		break;
		case GL_CONSTANT_COLOR:				dsta = 0xa;		break;
		case GL_ONE_MINUS_CONSTANT_COLOR:	dsta = 0xb;		break;
		case GL_CONSTANT_ALPHA:				dsta = 0xc;		break;
		case GL_ONE_MINUS_CONSTANT_ALPHA:	dsta = 0xd;		break;
		case GL_SRC_ALPHA_SATURATE:			dsta = 0xe;		break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}

	g_cuBlendFunc = (g_cuBlendFunc & 0xffff) | (srcrgb << 16) | (dstrgb << 20) | (srca << 24) | (dsta << 28);

    state->gl_state.per_frag_op.blendFuncSrcRGB = srcRGB;
	state->gl_state.per_frag_op.blendFuncDstRGB = dstRGB;
	state->gl_state.per_frag_op.blendFuncSrcAlpha = srcAlpha;
	state->gl_state.per_frag_op.blendFuncDstAlpha = dstAlpha;

	if (state->gl_state.per_frag_op.blendEnabled && !state->gl_state.per_frag_op.logicOpEnabled)
		__cb_writeReg(PA_WO_CU_BLEND_FUNC, g_cuBlendFunc, 0xc);
	else if (state->forceMode)
		__cb_writeReg(PA_WO_CU_BLEND_FUNC, 0x1010000, 0xc);
	BASE_GL_FAIL_IF(!__cb_isCommandbufferEnabled, GL_ERROR_COMMANDBUFFER_FULL_DMP);
	
	return;
}

void GL_APIENTRY  glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha){
	GET_CURRENT_STATE(state);
	
	if (state->gl_state.per_frag_op.blendColor[0] == red &&
		state->gl_state.per_frag_op.blendColor[1] == green &&
		state->gl_state.per_frag_op.blendColor[2] == blue &&
		state->gl_state.per_frag_op.blendColor[3] == alpha &&
		!state->forceMode)
		return;
	
	red   = UTL_CLAMP(red);
	green = UTL_CLAMP(green);
	blue  = UTL_CLAMP(blue);
	alpha = UTL_CLAMP(alpha);

	g_cuBlendColor = (unsigned)UTL_F2C_8b(red) | ((unsigned)UTL_F2C_8b(green) << 8) |
					((unsigned)UTL_F2C_8b(blue) << 16) | ((unsigned)UTL_F2C_8b(alpha) << 24);
	__cb_writeReg(PA_WO_CU_CONST_RGBA, g_cuBlendColor, 0xf);

	state->gl_state.per_frag_op.blendColor[0] = red;
	state->gl_state.per_frag_op.blendColor[1] = green;
	state->gl_state.per_frag_op.blendColor[2] = blue;
	state->gl_state.per_frag_op.blendColor[3] = alpha;
	BASE_GL_FAIL_IF(!__cb_isCommandbufferEnabled, GL_ERROR_COMMANDBUFFER_FULL_DMP);
	
	return;
}

void GL_APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha){
	GET_CURRENT_STATE(state);

	state->gl_state.fb_op.clearColor[0] = UTL_CLAMP(red);
	state->gl_state.fb_op.clearColor[1] = UTL_CLAMP(green);
	state->gl_state.fb_op.clearColor[2] = UTL_CLAMP(blue);
	state->gl_state.fb_op.clearColor[3] = UTL_CLAMP(alpha);
	
	return;
}

