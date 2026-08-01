// Filename: getvalue.c
//
// Project: Horizon

#include "common.h"
#include "state.h"
#include "error.h"
#include "util.h"
#include "fbmanager.h"

void GL_APIENTRY glGetIntegerv(GLenum pname, GLint* params){
	unsigned int i;
	GET_CURRENT_STATE(state);
	
	switch (pname){
		case GL_COLOR_WRITEMASK:
			for (i = 0; i < 4; i++)
				params[i] = UTL_BOOL2INT(state->gl_state.fb_op.colorMask[i]);
			break;
		case GL_DEPTH_WRITEMASK:
			params[0] = UTL_BOOL2INT(state->gl_state.fb_op.depthMask);
			break;
		case GL_COLOR_CLEAR_VALUE:
			for (i = 0; i < 4; i++)
				params[i] = UTL_FLOAT2INTCOL(state->gl_state.fb_op.clearColor[i]);
			break;
		case GL_STENCIL_CLEAR_VALUE:
			params[0] = state->gl_state.fb_op.clearStencil;
			break;
		case GL_DEPTH_CLEAR_VALUE:
			params[0] = UTL_FLOAT2INT(state->gl_state.fb_op.clearDepth);
			break;
		case GL_EARLY_DEPTH_CLEAR_VALUE_DMP:
			params[0] = state->gl_state.fb_op.clearEarlyDepth;
			break;
		case GL_BLEND_COLOR:
			for (i = 0; i < 4; i++)
				params[i] = UTL_FLOAT2INTCOL(state->gl_state.per_frag_op.blendColor[i]);
			break;
		case GL_SCISSOR_BOX:
			params[0] = state->gl_state.per_frag_op.scissorLeft;
			params[1] = state->gl_state.per_frag_op.scissorBottom;
			params[2] = state->gl_state.per_frag_op.scissorWidth;
			params[3] = state->gl_state.per_frag_op.scissorHeight;
			break;
		case GL_DEPTH_FUNC:
			params[0] = UTL_ENUM2INT(state->gl_state.per_frag_op.depthFunc);
			break;
		case GL_EARLY_DEPTH_FUNC_DMP:
			params[0] = UTL_ENUM2INT(state->gl_state.per_frag_op.earlyDepthFunc);
			break;
		case GL_STENCIL_FUNC:
			params[0] = UTL_ENUM2INT(state->gl_state.per_frag_op.stencilFunc);
			break;
		case GL_STENCIL_FAIL:
			params[0] = UTL_ENUM2INT(state->gl_state.per_frag_op.stencilOpFail);
			break;
		case GL_STENCIL_PASS_DEPTH_FAIL:
			params[0] = UTL_ENUM2INT(state->gl_state.per_frag_op.stencilOpDepthFail);
			break;
		case GL_STENCIL_PASS_DEPTH_PASS:
			params[0] = UTL_ENUM2INT(state->gl_state.per_frag_op.stencilOpDepthPass);
			break;
		case GL_STENCIL_REF:
			params[0] = state->gl_state.per_frag_op.stencilRefValue;
			break;
		case GL_STENCIL_VALUE_MASK:
			params[0] = state->gl_state.per_frag_op.stencilValueMask;
			break;
		case GL_BLEND_SRC_RGB:
			params[0] = UTL_ENUM2INT(state->gl_state.per_frag_op.blendFuncSrcRGB);
			break;
		case GL_BLEND_DST_RGB:
			params[0] = UTL_ENUM2INT(state->gl_state.per_frag_op.blendFuncDstRGB);
			break;
		case GL_BLEND_SRC_ALPHA:
			params[0] = state->gl_state.per_frag_op.blendFuncSrcAlpha;
			break;
		case GL_BLEND_DST_ALPHA:
			params[0] = state->gl_state.per_frag_op.blendFuncDstAlpha;
			break;
		case GL_BLEND_EQUATION_RGB:
			params[0] = state->gl_state.per_frag_op.blendEqRGB;
			break;
		case GL_BLEND_EQUATION_ALPHA:
			params[0] = state->gl_state.per_frag_op.blendEqAlpha;
			break;
		case GL_STENCIL_WRITEMASK:
			params[0] = state->gl_state.fb_op.stencilWriteMask;
			break;
				
		case GL_DEPTH_RANGE:
			params[0] = UTL_FLOAT2INT(state->gl_state.trioffset.depthRangeN);
			params[1] = UTL_FLOAT2INT(state->gl_state.trioffset.depthRangeF);
			break;
		case GL_POLYGON_OFFSET_FACTOR:
			params[0] = UTL_FLOAT2INT(state->gl_state.trioffset.m_offset);
			break;
		case GL_POLYGON_OFFSET_UNITS:
			params[0] = UTL_FLOAT2INT(state->gl_state.trioffset.u_offset);
			break;
		case GL_CULL_FACE_MODE:
			params[0] = UTL_ENUM2INT(state->gl_state.triorient.cullFace);
			break;
		case GL_FRONT_FACE:
			params[0] = UTL_ENUM2INT(state->gl_state.triorient.frontFace);
			break;
		case GL_VIEWPORT:
			params[0] = state->gl_state.viewport.x;
			params[1] = state->gl_state.viewport.y;
			params[2] = state->gl_state.viewport.width;
			params[3] = state->gl_state.viewport.height;
			break;
		case GL_MAX_VIEWPORT_DIMS:
			params[0] = MAX_RENDERBUFFER_SIZE;
			params[1] = MAX_RENDERBUFFER_SIZE;
			break;
		case GL_LOGIC_OP_MODE:
			params[0] = state->gl_state.per_frag_op.logicOp;
			break;
		
		/* vertbuffer state */
		case GL_ARRAY_BUFFER_BINDING:
			params[0] = state->gl_state.vertbuffer.arrayID;
			break;
		case GL_ELEMENT_ARRAY_BUFFER_BINDING:
			params[0] = state->gl_state.vertbuffer.elmtArrayID;
			break;
		case GL_VERTEX_STATE_COLLECTION_BINDING_DMP:
			params[0] = state->gl_state.vertexStateCollection;
			break;

		case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
			params[0] = MAX_TEXTURE_UNITS + 1;
			break;
		case GL_ACTIVE_TEXTURE:
			params[0] = state->gl_state.textureunits.activeTexture + GL_TEXTURE0;
			break;
		case GL_MAX_TEXTURE_SIZE:
			params[0] = MAX_TEXTURE_SIZE;
			break;
		case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
			params[0] = MAX_TEXTURE_SIZE;
			break;
		case GL_TEXTURE_BINDING_2D:
			params[0] = state->gl_state.textureunits.bound2DTex[state->gl_state.textureunits.activeTexture];
			break;
		case GL_TEXTURE_BINDING_CUBE_MAP:
			params[0] = state->gl_state.textureunits.boundCubeTex[state->gl_state.textureunits.activeTexture];
			break;
		case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
			params[0] = 1;
			break;
		case GL_COMPRESSED_TEXTURE_FORMATS:
			params[0] = GL_ETC1_RGB8_NATIVE_DMP;
			break;
		case GL_MAX_LUT_TEXTURES_DMP:
			params[0] = MAX_LUT_TEXTURES;
			break;
		case GL_MAX_LUT_ENTRIES_DMP:
			params[0] = MAX_LUT_ENTRIES;
			break;
		case GL_TEXTURE_COLLECTION_BINDING_DMP:
			params[0] = state->gl_state.textureunits.boundTexColl;
			break;
		case GL_TEXTURE_BINDING_LUT0_DMP:
		case GL_TEXTURE_BINDING_LUT1_DMP:
		case GL_TEXTURE_BINDING_LUT2_DMP:
		case GL_TEXTURE_BINDING_LUT3_DMP:
		case GL_TEXTURE_BINDING_LUT4_DMP:
		case GL_TEXTURE_BINDING_LUT5_DMP:
		case GL_TEXTURE_BINDING_LUT6_DMP:
		case GL_TEXTURE_BINDING_LUT7_DMP:
		case GL_TEXTURE_BINDING_LUT8_DMP:
		case GL_TEXTURE_BINDING_LUT9_DMP:
		case GL_TEXTURE_BINDING_LUT10_DMP:
		case GL_TEXTURE_BINDING_LUT11_DMP:
		case GL_TEXTURE_BINDING_LUT12_DMP:
		case GL_TEXTURE_BINDING_LUT13_DMP:
		case GL_TEXTURE_BINDING_LUT14_DMP:
		case GL_TEXTURE_BINDING_LUT15_DMP:
		case GL_TEXTURE_BINDING_LUT16_DMP:
		case GL_TEXTURE_BINDING_LUT17_DMP:
		case GL_TEXTURE_BINDING_LUT18_DMP:
		case GL_TEXTURE_BINDING_LUT19_DMP:
		case GL_TEXTURE_BINDING_LUT20_DMP:
		case GL_TEXTURE_BINDING_LUT21_DMP:
		case GL_TEXTURE_BINDING_LUT22_DMP:
		case GL_TEXTURE_BINDING_LUT23_DMP:
		case GL_TEXTURE_BINDING_LUT24_DMP:
		case GL_TEXTURE_BINDING_LUT25_DMP:
		case GL_TEXTURE_BINDING_LUT26_DMP:
		case GL_TEXTURE_BINDING_LUT27_DMP:
		case GL_TEXTURE_BINDING_LUT28_DMP:
		case GL_TEXTURE_BINDING_LUT29_DMP:
		case GL_TEXTURE_BINDING_LUT30_DMP:
		case GL_TEXTURE_BINDING_LUT31_DMP:
			params[0] = state->gl_state.textureunits.boundLutTex[pname - GL_TEXTURE_BINDING_LUT0_DMP];
			break;
		
		/* fbmanager */
		case GL_SUBPIXEL_BITS:
			params[0] = 0;
			break;
		case GL_RED_BITS: case GL_GREEN_BITS: case GL_BLUE_BITS: case GL_ALPHA_BITS:
		case GL_DEPTH_BITS:
		case GL_STENCIL_BITS:
		case GL_RENDERBUFFER_BINDING:
		case GL_FRAMEBUFFER_BINDING:
			__fb_getAttachmentParameteriv(pname, params);
			break;

		case GL_RENDER_BLOCK_MODE_DMP:
			params[0] = state->gl_state.fb_state.block32Enabled ? GL_RENDER_BLOCK32_MODE_DMP : GL_RENDER_BLOCK8_MODE_DMP;
			break;
		case GL_IMPLEMENTATION_COLOR_READ_TYPE:
			params[0] = GL_UNSIGNED_BYTE;
			break;
		case GL_IMPLEMENTATION_COLOR_READ_FORMAT:
			params[0] = GL_RGBA;
			break;
		case GL_MAX_RENDERBUFFER_SIZE:
			params[0] = MAX_RENDERBUFFER_SIZE;
			break;


		case GL_SHADER_COMPILER:
			params[0] = GL_FALSE;
			break;
		case GL_NUM_SHADER_BINARY_FORMATS:
			params[0] = 1;
			break;
		case GL_SHADER_BINARY_FORMATS:
			params[0] = GL_PLATFORM_BINARY_DMP;
			break;
		case GL_MAX_VERTEX_ATTRIBS:
			params[0] = MAX_VERTEX_ATTRIBS;
			break;
		case GL_CURRENT_PROGRAM:
			params[0] = state->gl_state.rendermode.currentProgram;
			break;

		case GL_COLOR_LOGIC_OP:
			params[0] = UTL_BOOL2INT(state->gl_state.per_frag_op.logicOpEnabled);
			break;
		case GL_BLEND:
			params[0] = UTL_BOOL2INT(state->gl_state.per_frag_op.blendEnabled);
			break;
		case GL_DEPTH_TEST:
			params[0] = UTL_BOOL2INT(state->gl_state.per_frag_op.depthTestEnabled);
			break;
		case GL_STENCIL_TEST:
			params[0] = UTL_BOOL2INT(state->gl_state.per_frag_op.stencilTestEnabled);
			break;
		case GL_SCISSOR_TEST:
			params[0] = UTL_BOOL2INT(state->gl_state.per_frag_op.scissorTestEnabled);
			break;
		case GL_POLYGON_OFFSET_FILL:
			params[0] = UTL_BOOL2INT(state->gl_state.trioffset.enabled);
			break;
		case GL_CULL_FACE:
			params[0] = UTL_BOOL2INT(state->gl_state.triorient.enabled);
			break;
		
		default:
			__err_setError(GL_INVALID_ENUM);
			break;
	}
	
	return;
}