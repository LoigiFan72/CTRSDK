// Filename: statevalidator.c
//
// Project: Horizon

#include "common.h"
#include "error.h"
#include "state.h"
#include "statevalidator.h"
#include "texmanager.h"
#include "fbmanager.h"
#include "vbmanager.h"
#include "shadermanager.h"
#include "commandbuffer.h"
#include "picaregmap.h"
#include "util.h"

vld_validator_frag_info_t* __finfo;

GLint __vldtr_initializeStateValidator(vld_validator_frag_info_t* finfo){
	int ret;
	
	__finfo = finfo;{
		__txv_initializeTextureValidator(&__finfo->texture);
		__fbv_initializeFBValidator(&__finfo->fbman);
		__vbv_initializeVBValidator(&__finfo->vbman);
		__shv_initializeShaderValidator(&__finfo->shman);
		ret = 0;
	}
	
	return ret;
}

void __vldtr_finalizeStateValidator(void){
	if (__finfo){
		__finfo = 0;
	}
	
	return;
}

extern unsigned g_cuBlendFunc;
extern unsigned g_cuDepthTest;
extern unsigned g_cuStencilTest;
extern unsigned g_cuStencilOp;
extern unsigned g_cuLogicOp;
extern unsigned g_cuBlendColor;
extern unsigned g_tiEarlyDepthFunc;
extern unsigned g_tiClearEarlyDepth;
extern unsigned g_cuGasDepth;

static void vldtr_validateOthers(void){
	GET_CURRENT_STATE(state);

	{
		unsigned u;
		UTL_F2F_16M7E(state->gl_state.viewport.width / 2.f, u);
		__cb_writeReg(PA_RW_TS_WIDTH2, u, 0xf);
		UTL_F2F_16M7E(state->gl_state.viewport.height / 2.f, u);
		__cb_writeReg(PA_RW_TS_HEIGHT2, u, 0xf);
		
		if (state->gl_state.viewport.width && state->gl_state.viewport.height){
			unsigned size[2];
			int i;
			size[0] = state->gl_state.viewport.width;
			size[1] = state->gl_state.viewport.height;
			for (i = 0; i < 2; i++){
				switch (size[i]){
					case 240:	size[i] = 0x38111111;	break;
					case 320:	size[i] = 0x37999999;	break;
					case 400:	size[i] = 0x3747ae14;	break;
					case 480:	size[i] = 0x37111111;	break;
					case 600:	size[i] = 0x36b4e81b;	break;
					case 640:	size[i] = 0x36999999;	break;
					case 800:	size[i] = 0x3647ae14;	break;
					case 1024:	size[i] = 0x36000000;	break;
					case 1200:	size[i] = 0x35b4e81b;	break;
					case 1280:	size[i] = 0x35999999;	break;
					default:
						UTL_F2F_23M7E((2.f / (float)size[i]), size[i]);
						size[i] <<= 1;
						break;
				}
				__cb_writeReg(PA_RW_TS_RWIDTH2 + 2 * i, size[i], 0xf);
			}
		}

		__cb_writeReg(PA_RW_TI_OFFSET, (state->gl_state.viewport.x | state->gl_state.viewport.y << 16), 0xf);
	}

	__cb_writeReg(PA_WO_CU_DEPTH_T, g_cuDepthTest, 3);
	__cb_writeReg(PA_W0_GAS_DELTAZ_DEPTH, g_cuGasDepth, 0x8);

	if (state->gl_state.per_frag_op.earlyDepthTestEnabled){
		__cb_writeReg(PA_RW_TI_EARLYZ_MODE, 1, 1);
		__cb_writeReg(PA_WO_FU_EZ_EN, 1, 1);
	}
	else{
		__cb_writeReg(PA_RW_TI_EARLYZ_MODE, 0, 1);
		__cb_writeReg(PA_WO_FU_EZ_EN, 0, 1);
	}

	__cb_writeReg(PA_RW_TI_ZCOMP_MODE, g_tiEarlyDepthFunc, 1);
	__cb_writeReg(PA_RW_EZ_Z_CLEAR_VALUE, g_tiClearEarlyDepth, 0x7);
	__cb_writeReg(PA_WO_CU_LOGIC, g_cuLogicOp, 0xf);

	if (state->gl_state.per_frag_op.logicOpEnabled){
		__cb_writeReg(PA_WO_CU_MODE, 0, 2);
	}
	else{
		__cb_writeReg(PA_WO_CU_BLEND_FUNC, state->gl_state.per_frag_op.blendEnabled ? g_cuBlendFunc : 0x1010000, 0xf);
		__cb_writeReg(PA_WO_CU_MODE, 0x100, 2);
	}
	
	if (state->gl_state.triorient.enabled)
		__cb_writeReg(PA_RW_TS_CULLMODE, (state->gl_state.triorient.cullFace == GL_FRONT) ^
			(state->gl_state.triorient.frontFace == GL_CCW) ? 2 : 1, 0xf);
	else
		__cb_writeReg(PA_RW_TS_CULLMODE, 0, 0xf);
	
	__cb_writeReg(PA_WO_FU_MEM_ADRS_MODE, state->gl_state.fb_state.block32Enabled ? 1 : 0, 1);
	__cb_writeReg(PA_WO_CU_STENCIL_T, g_cuStencilTest, 0xf);
	__cb_writeReg(PA_WO_CU_STENCIL_OP, g_cuStencilOp, 0xf);
	__cb_writeReg(PA_WO_CU_CONST_RGBA, g_cuBlendColor, 0xf);
	
	SET_MASK(state->upd_mask, IF_GL_FB_ACCESS);
	
	return;
}

void __vldtr_validateAllState(void){
	GET_CURRENT_STATE(state);
	
	if (state->forceValidateMask & NN_GX_STATE_OTHERS)
		vldtr_validateOthers();
	
	if (AND_MASK(state->upd_mask, __finfo->texture))
		__txv_validateTextureValidator(&state->upd_mask);
	
	if (AND_MASK(state->upd_mask, __finfo->fbman))
		__fbv_validateFBValidator(&state->upd_mask);
	
	if (AND_MASK(state->upd_mask, __finfo->vbman))
		__vbv_validateVBValidator(&state->upd_mask);
	
	if (AND_MASK(state->upd_mask, __finfo->shman))
		__shv_validateShaderValidator(&state->upd_mask);
	
	CLEAR_ALL_MASK(state->upd_mask);
	
	return;
}

void nngxValidateState(GLbitfield statemask, GLboolean drawelements){
	GLenum err;
	bit_mask_t validmask;
	GET_CURRENT_STATE(state);
	
	BASE_GL_FAIL_IF(__cb_isCommandbufferEmpty, GL_ERROR_80B2_DMP);
	BASE_GL_FAIL_IF(state->gl_state.rendermode.currentProgram == 0 &&
		(statemask &
			(NN_GX_STATE_SHADERBINARY |
			NN_GX_STATE_SHADERPROGRAM |
			NN_GX_STATE_SHADERMODE |
			NN_GX_STATE_SHADERFLOAT |
			NN_GX_STATE_VSUNIFORM |
			NN_GX_STATE_FSUNIFORM |
			NN_GX_STATE_LUT |
			NN_GX_STATE_VERTEX |
			NN_GX_STATE_TRIOFFSET |
			NN_GX_STATE_FBACCESS)
		), GL_ERROR_80B3_DMP);

	
	err = glGetError();
	statemask &= ~state->suppressStateMask;
	
	if (statemask & state->forceValidateMask & NN_GX_STATE_OTHERS)
		vldtr_validateOthers();
	
	if (statemask & NN_GX_STATE_TEXTURE){
		CLEAR_ALL_MASK(validmask);
		SET_MASK(validmask, IF_GL_TEXTURE0);
		SET_MASK(validmask, IF_GL_TEXTURE1);
		SET_MASK(validmask, IF_GL_TEXTURE2);
		
		if (AND_MASK(state->upd_mask, validmask))
			__txv_validateTextureValidator(&validmask);
	}
	if (statemask & NN_GX_STATE_FRAMEBUFFER){
		CLEAR_ALL_MASK(validmask);
		SET_MASK(validmask, IF_GL_FRAMEBUFFER);
		
		if (AND_MASK(state->upd_mask, validmask))
			__fbv_validateFBValidator(&validmask);
	}
	if (statemask & NN_GX_STATE_VERTEX){
		if (AND_MASK(state->upd_mask, __finfo->vbman))
			__vbv_validateVBValidator(&state->upd_mask);
	}
	
	if (statemask & (NN_GX_STATE_SHADERBINARY | NN_GX_STATE_SHADERPROGRAM | NN_GX_STATE_SHADERMODE | NN_GX_STATE_SHADERFLOAT |NN_GX_STATE_VSUNIFORM |  NN_GX_STATE_FSUNIFORM | NN_GX_STATE_LUT | NN_GX_STATE_VERTEX | NN_GX_STATE_TRIOFFSET | NN_GX_STATE_FBACCESS | NN_GX_STATE_SCISSOR)){
		state->gl_state.rendermode.drawing = GL_TRUE;
		if (!drawelements)
			state->gl_state.rendermode.drawarray = GL_TRUE;
		__shv_partialValidateShaderValidator(&state->upd_mask, statemask);
		state->gl_state.rendermode.drawing = GL_FALSE;
		state->gl_state.rendermode.drawarray = GL_FALSE;
	}
	
	if (statemask & NN_GX_STATE_SHADERBINARY)
		state->upd_mask.mask_[0] &= ~(1 << IF_GL_BINARY_PACKAGE);
	
	if (statemask & NN_GX_STATE_SHADERMODE)
		state->upd_mask.mask_[0] &= ~((1 << IF_GL_ATTACH_GS) | (1 << IF_GL_DETACH_GS));
	
	if (statemask & NN_GX_STATE_SHADERFLOAT)
		state->upd_mask.mask_[0] &= ~((1 << IF_GL_BINARY_EXE_VS) | (1 << IF_GL_BINARY_EXE_GS));
	
	if (statemask & NN_GX_STATE_VSUNIFORM)
		state->upd_mask.mask_[0] &= ~(1 << IF_GL_SHADER_UNIFORM);
	
	if (statemask & NN_GX_STATE_FSUNIFORM)
		state->upd_mask.mask_[0] &= ~(1 << IF_HW_RASTERIZER_REGISTER);
	
	if (statemask & NN_GX_STATE_LUT)
		state->upd_mask.mask_[0] &= ~((1 << IF_GL_TEXTURE_LUT) | (1 << IF_GL_LIGHTING_LUT_SAMPLERS) |
			(1 << IF_GL_PROCTEX_LUT_SAMPLERS) | (1 << IF_GL_FOG_LUT_SAMPLER) | (1 << IF_GL_GAS_LUT_SAMPLERS));
	
	if (statemask & NN_GX_STATE_TEXTURE)
		state->upd_mask.mask_[0] &= ~((1 << IF_GL_TEXTURE0) | (1 << IF_GL_TEXTURE1) | (1 << IF_GL_TEXTURE2));
	
	if (statemask & NN_GX_STATE_FRAMEBUFFER)
		state->upd_mask.mask_[0] &= ~(1 << IF_GL_FRAMEBUFFER);
	
	if (statemask & NN_GX_STATE_VERTEX)
		state->upd_mask.mask_[0] &= ~((1 << IF_GL_VERTBUFFER) | (1 << IF_GL_VERT_ARRAY) | (1 << IF_GL_VERT_CURRENT) | (1 << IF_GL_PROGRAM));
	
	if (statemask & NN_GX_STATE_TRIOFFSET)
		state->upd_mask.mask_[0] &= ~(1 << IF_GL_TRI_OFFSET);
	
	if (statemask & NN_GX_STATE_FBACCESS)
		state->upd_mask.mask_[0] &= ~(1 << IF_GL_FB_ACCESS);
	
	if (statemask & NN_GX_STATE_SCISSOR)
		state->upd_mask.mask_[0] &= ~(1 << IF_GL_FB_SCISSOR);
	
	state->forceValidateMask &= ~statemask;

	if (err != GL_NO_ERROR){
		glGetError();
		__err_setError(err);
	}
	else if (glGetError() != GL_NO_ERROR){
		__err_setError(GL_ERROR_806C_DMP);
	}
	
	BASE_GL_FAIL_IF(!__cb_isCommandbufferEnabled, GL_ERROR_8066_DMP);
	
	return;
}