// Filename: state.c
//
// Project: Horizon

#include "common.h"
#include "state.h"
#include "util.h"

stt_state_t* __gl_state = 0;

unsigned g_cuBlendFunc;
unsigned g_cuDepthTest;
unsigned g_cuStencilTest;
unsigned g_cuStencilOp;
unsigned g_cuLogicOp;
unsigned g_cuBlendColor;
unsigned g_tiEarlyDepthFunc;
unsigned g_tiClearEarlyDepth;
unsigned g_cuGasDepth;

GLint __sta_initializeState(stt_state_t* glstate){
	int i, j;
	
	__gl_state = glstate;
	memset(__gl_state, 0, sizeof(stt_state_t));
	
	__gl_state->gl_state.rendermode.drawing = GL_FALSE;
	__gl_state->gl_state.rendermode.drawarray = GL_FALSE;
	__gl_state->gl_state.rendermode.mode = GL_TRIANGLES;
	__gl_state->gl_state.rendermode.prevmode = GL_TRIANGLES;
	__gl_state->gl_state.rendermode.bAccerelateTriangle = GL_TRUE;
	__gl_state->gl_state.rendermode.bGeometryShaderUsed = GL_FALSE;
	__gl_state->gl_state.rendermode.currentProgram = 0;
	
	__gl_state->gl_state.viewport.bInitted = GL_FALSE;
	__gl_state->gl_state.viewport.x = 0;
	__gl_state->gl_state.viewport.y = 0;
	__gl_state->gl_state.viewport.width = 0;
	__gl_state->gl_state.viewport.height = 0;
	
	__gl_state->gl_state.triorient.enabled = GL_FALSE;
	__gl_state->gl_state.triorient.frontFace = GL_CCW;
	__gl_state->gl_state.triorient.cullFace = GL_BACK;
	
	__gl_state->gl_state.trioffset.m_offset = 0;
	__gl_state->gl_state.trioffset.u_offset = 0;
	__gl_state->gl_state.trioffset.enabled = GL_FALSE;
	__gl_state->gl_state.trioffset.depthRangeN = 0;
	__gl_state->gl_state.trioffset.depthRangeF = 1.f;
	__gl_state->gl_state.trioffset.w_scale = 0;
	
	__gl_state->gl_state.textureunits.activeTexture = 0;
	for (i = 0; i < MAX_TEXTURE_UNITS; i++){
		__gl_state->gl_state.textureunits.bound2DTex[i] = 0;
		__gl_state->gl_state.textureunits.boundCubeTex[i] = 0;
		__gl_state->gl_state.textureunits.enabled2DTex[i] = GL_FALSE;
		__gl_state->gl_state.textureunits.enabledCubeTex[i] = GL_FALSE;
		__gl_state->gl_state.textureunits.samplerType[i] = GL_FALSE;
	}

	for (i = 0; i < MAX_LUT_TEXTURES; i++)
		__gl_state->gl_state.textureunits.boundLutTex[i] = 0;

	__gl_state->gl_state.textureunits.boundTexColl = 0;
	
	for (i = 0; i < MAX_VERTEX_ATTRIBS; i++){
		__gl_state->gl_state.vertcurr.attrib[i][0] = __gl_state->gl_state.vertcurr.attrib[i][1] = __gl_state->gl_state.vertcurr.attrib[i][2] = 0.f;
		__gl_state->gl_state.vertcurr.attrib[i][3] = 1.f;
	}
	
	for (i = 0; i < MAX_VERTEX_ATTRIBS; i++){
		unsigned v[4];
		for (j = 0; j < 4; j++)
			UTL_F2F_16M7E(__gl_state->gl_state.vertcurr.attrib[i][j], v[j]);
		
		__gl_state->gl_state.vertcurr.attrib24[i][0] = (v[2] >> 16) | (v[3] <<  8);
		__gl_state->gl_state.vertcurr.attrib24[i][1] = ((v[1] >> 8) & 0xffff) | ((v[2] & 0xffff) << 16);
		__gl_state->gl_state.vertcurr.attrib24[i][2] = v[0] | ((v[1] & 0xff) << 24);
	}
	
	for (i = 0; i < MAX_VERTEX_ATTRIBS; i++){
		__gl_state->gl_state.vertarray.array[i].enabled = 0;
		__gl_state->gl_state.vertarray.array[i].buffer = 0;
		__gl_state->gl_state.vertarray.array[i].size = 4;
		__gl_state->gl_state.vertarray.array[i].type = GL_FLOAT;
	}
	
	__gl_state->gl_state.vertbuffer.elmtArrayID = 0;
	__gl_state->gl_state.vertbuffer.arrayID = 0;
	
	__gl_state->gl_state.vertexStateCollection = 0;
	
	__gl_state->gl_state.per_frag_op.scissorTestEnabled = GL_FALSE;
	__gl_state->gl_state.per_frag_op.scissorLeft = 0;
	__gl_state->gl_state.per_frag_op.scissorBottom = 0;
	__gl_state->gl_state.per_frag_op.scissorWidth = 0;
	__gl_state->gl_state.per_frag_op.scissorHeight = 0;
	__gl_state->gl_state.per_frag_op.alphaTestEnabled = GL_FALSE;
	__gl_state->gl_state.per_frag_op.alphaFunc = GL_ALWAYS;
	__gl_state->gl_state.per_frag_op.alphaRefValue = 0;
	__gl_state->gl_state.per_frag_op.stencilTestEnabled = GL_FALSE;
	__gl_state->gl_state.per_frag_op.stencilFunc = GL_ALWAYS;
	__gl_state->gl_state.per_frag_op.stencilRefValue = 0;
	__gl_state->gl_state.per_frag_op.stencilValueMask = 0xffffffff;
	__gl_state->gl_state.per_frag_op.stencilOpFail = GL_KEEP;
	__gl_state->gl_state.per_frag_op.stencilOpDepthFail = GL_KEEP;
	__gl_state->gl_state.per_frag_op.stencilOpDepthPass = GL_KEEP;
	__gl_state->gl_state.per_frag_op.depthTestEnabled = GL_FALSE;
	__gl_state->gl_state.per_frag_op.depthFunc = GL_LESS;
	__gl_state->gl_state.per_frag_op.earlyDepthTestEnabled = GL_FALSE;
	__gl_state->gl_state.per_frag_op.earlyDepthFunc = GL_LESS;
	__gl_state->gl_state.per_frag_op.blendEnabled = GL_FALSE;
	__gl_state->gl_state.per_frag_op.blendFuncSrcRGB = GL_ONE;
	__gl_state->gl_state.per_frag_op.blendFuncSrcAlpha = GL_ONE;
	__gl_state->gl_state.per_frag_op.blendFuncDstRGB = GL_ZERO;
	__gl_state->gl_state.per_frag_op.blendFuncDstAlpha = GL_ZERO;
	__gl_state->gl_state.per_frag_op.logicOpEnabled = GL_FALSE;
	__gl_state->gl_state.per_frag_op.logicOp = GL_COPY;
	__gl_state->gl_state.per_frag_op.mode = GL_FRAGOP_MODE_GL_DMP;
	__gl_state->gl_state.per_frag_op.blendEqRGB = GL_FUNC_ADD;
	__gl_state->gl_state.per_frag_op.blendEqAlpha = GL_FUNC_ADD;
	__gl_state->gl_state.per_frag_op.blendColor[0] = __gl_state->gl_state.per_frag_op.blendColor[1] =
		__gl_state->gl_state.per_frag_op.blendColor[2] = __gl_state->gl_state.per_frag_op.blendColor[3] = 0;
	
	__gl_state->gl_state.fb_op.depthMask = GL_TRUE;
	__gl_state->gl_state.fb_op.stencilWriteMask = 0xffffffff;
	__gl_state->gl_state.fb_op.clearStencil = 0;
	__gl_state->gl_state.fb_op.clearDepth = 1.f;
	UTL_F2UFX_24W_0I(1.f, __gl_state->gl_state.fb_op.clearDepth24);
	UTL_F2UFX_16W_0I(1.f, __gl_state->gl_state.fb_op.clearDepth16);
	__gl_state->gl_state.fb_op.clearEarlyDepth = 0xffffff;
	__gl_state->gl_state.fb_op.colorMask[0] = __gl_state->gl_state.fb_op.colorMask[1]
		= __gl_state->gl_state.fb_op.colorMask[2] = __gl_state->gl_state.fb_op.colorMask[3] = GL_TRUE;
	__gl_state->gl_state.fb_op.clearColor[0] = __gl_state->gl_state.fb_op.clearColor[1] =
		__gl_state->gl_state.fb_op.clearColor[2] = __gl_state->gl_state.fb_op.clearColor[3] = 0;
	
	__gl_state->gl_state.fb_state.status = 0;
	__gl_state->gl_state.fb_state.colorBufferEnabled = GL_FALSE;
	__gl_state->gl_state.fb_state.depthBufferEnabled = GL_FALSE;
	__gl_state->gl_state.fb_state.stencilBufferEnabled = GL_FALSE;
	__gl_state->gl_state.fb_state.block32Enabled = GL_FALSE;
	__gl_state->gl_state.fb_state.width = 0;
	__gl_state->gl_state.fb_state.height = 0;
	__gl_state->gl_state.fb_state.color_format = CF_R8G8B8A8;
	__gl_state->gl_state.fb_state.z_format = ZF_Z24S8;
	
	__gl_state->hw_state.hw_arrayproc.elmtArrOffset = 0;
	__gl_state->hw_state.hw_arrayproc.hostElmtArrOffset = 0;
	__gl_state->hw_state.hw_arrayproc.baseAddr = 0;
	for (i = 0; i < MAX_VERTEX_ATTRIBS; i++){
		__gl_state->hw_state.hw_arrayproc.hostAttrArrOffset[i] =
			__gl_state->hw_state.hw_arrayproc.attrArrOffset[i] = __gl_state->hw_state.hw_arrayproc.hostStride[i] = 0;
	}
	
	__gl_state->hw_state.hw_texunits.regs[0] = 0;		/* r_ON_0, r_ON_1, r_ON_PT	*/
	
	__gl_state->hw_state.hw_shader.useVertexBuffers = GL_FALSE;
	__gl_state->hw_state.hw_shader.attrNumUsed = 0;
	__gl_state->hw_state.hw_shader.attrNumTotalUsed = 0;
	memset(__gl_state->hw_state.hw_shader.attrUseOrder, 0xff, sizeof(__gl_state->hw_state.hw_shader.attrUseOrder));
	
	return 0;
}

void __sta_finalizeState(void){
	if (__gl_state){
		__gl_state = 0;
	}
	return;
}