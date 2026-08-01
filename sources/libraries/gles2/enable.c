// Filename: enable.c
//
// Project: Horizon

#include "common.h"
#include "state.h"
#include "error.h"
#include "util.h"
#include "commandbuffer.h"
#include "picaregmap.h"

static void ena_enableStatus(GLenum cap, GLboolean ena);

extern unsigned g_cuBlendFunc;
extern unsigned g_cuDepthTest;
extern unsigned g_cuStencilTest;
extern unsigned g_tiEarlyDepthFunc;

void GL_APIENTRY glDisable(GLenum cap){
	ena_enableStatus(cap, GL_FALSE);
}

void GL_APIENTRY glEnable(GLenum cap){
	ena_enableStatus(cap, GL_TRUE);
}

static void ena_enableStatus(GLenum param, GLboolean bEnable){
	GET_CURRENT_STATE(state);
	
	switch (param){		
		case GL_COLOR_LOGIC_OP:
			if (state->gl_state.per_frag_op.logicOpEnabled == bEnable && !state->forceMode)
				break;
			if (bEnable){
				__cb_writeReg(PA_WO_CU_MODE, 0, 2);
			}
			else{
				__cb_writeReg(PA_WO_CU_BLEND_FUNC, state->gl_state.per_frag_op.blendEnabled ? g_cuBlendFunc : 0x1010000, 0xf);
				__cb_writeReg(PA_WO_CU_MODE, 0x100, 2);
			}
			state->gl_state.per_frag_op.logicOpEnabled = bEnable;
			SET_MASK(state->upd_mask, IF_GL_FB_ACCESS);
			break;
		case GL_BLEND:
			if (state->gl_state.per_frag_op.blendEnabled == bEnable && !state->forceMode)
				break;
			if (state->gl_state.per_frag_op.logicOpEnabled){
				__cb_writeReg(PA_WO_CU_MODE, 0, 2);
			}
			else{
				__cb_writeReg(PA_WO_CU_BLEND_FUNC, bEnable ? g_cuBlendFunc : 0x1010000, 0xf);
				__cb_writeReg(PA_WO_CU_MODE, 0x100, 2);
			}
			state->gl_state.per_frag_op.blendEnabled = bEnable;
			SET_MASK(state->upd_mask, IF_GL_FB_ACCESS);
			break;
		case GL_DEPTH_TEST:
			if (state->gl_state.per_frag_op.depthTestEnabled == bEnable && !state->forceMode)
				break;
			bEnable ? (g_cuDepthTest |= 1) : (g_cuDepthTest &= ~1);
			__cb_writeReg(PA_WO_CU_DEPTH_T, g_cuDepthTest, 1);
			state->gl_state.per_frag_op.depthTestEnabled = bEnable;
			SET_MASK(state->upd_mask, IF_GL_FB_ACCESS);
			break;
		case GL_EARLY_DEPTH_TEST_DMP:
			if (state->gl_state.per_frag_op.earlyDepthTestEnabled == bEnable && !state->forceMode)
				break;
			__cb_writeReg(PA_RW_TI_EARLYZ_MODE, bEnable ? 1 : 0, 1);
			__cb_writeReg(PA_WO_FU_EZ_EN, bEnable ? 1 : 0, 1);
			state->gl_state.per_frag_op.earlyDepthTestEnabled = bEnable;
			SET_MASK(state->upd_mask, IF_GL_FB_ACCESS);
			break;
		case GL_STENCIL_TEST:
			if (state->gl_state.per_frag_op.stencilTestEnabled == bEnable && !state->forceMode)
				break;
			bEnable ? (g_cuStencilTest |= 1) : (g_cuStencilTest &= ~1);
			__cb_writeReg(PA_WO_CU_STENCIL_T, g_cuStencilTest, 1);
			state->gl_state.per_frag_op.stencilTestEnabled = bEnable;
			SET_MASK(state->upd_mask, IF_GL_FB_ACCESS);
			break;
		case GL_SCISSOR_TEST:
			CHECK_ASSIGN(state->gl_state.per_frag_op.scissorTestEnabled, bEnable, IF_GL_FB_SCISSOR);
			break;
		
		case GL_CULL_FACE:
			if (state->gl_state.triorient.enabled == bEnable && !state->forceMode)
				break;
			if (bEnable)
				__cb_writeReg(PA_RW_TS_CULLMODE, (state->gl_state.triorient.cullFace == GL_FRONT) ^
					(state->gl_state.triorient.frontFace == GL_CCW) ? 2 : 1, 0xf);
			else
				__cb_writeReg(PA_RW_TS_CULLMODE, 0, 0xf);
			state->gl_state.triorient.enabled = bEnable;
			break;
		
		case GL_POLYGON_OFFSET_FILL:
			CHECK_ASSIGN(state->gl_state.trioffset.enabled, bEnable, IF_GL_TRI_OFFSET);
			break;
		
		case GL_ACCELERATE_TRIANGLES_DMP:
			state->gl_state.rendermode.bAccerelateTriangle = bEnable;
			break;
		
		case GL_DEPTH_STENCIL_COPY_DMP:
			state->gl_state.fb_state.depthCopyMode = bEnable;
			break;
		
		default:
			__err_setError(GL_INVALID_ENUM);
			break;
	}
	
	BASE_GL_FAIL_IF(!__cb_isCommandbufferEnabled, GL_ERROR_COMMANDBUFFER_FULL_DMP);
	return;
}