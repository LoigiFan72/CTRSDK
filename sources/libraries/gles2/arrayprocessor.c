// Filename: arrayprocessor.c
//
// Project: Horizon

#include "common.h"
#include "state.h"
#include "error.h"
#include "util.h"
#include "statevalidator.h"
#include "commandbuffer.h"
#include "memmanager.h"
#include "picaregmap.h"
#include "georegmap.h"
#include "shadermanager.h"

static void arp_drawHostArrays(GLuint _count, GLuint _idxsize, GLuint _first, GLboolean _bDrawElements);

static void arp_drawHostArrays(GLuint _count, GLuint _idxsize, GLuint _first, GLboolean _bDrawElements){
	GLuint i, j;
	stt_hw_shader_state_t* shaderstate;
	stt_vert_array_state_t* glvstate;
	stt_hw_array_proc_state_t* hwvstate;
	GLboolean bContinue;
	GLint size[MAX_VERTEX_ATTRIBS];
	GLenum type[MAX_VERTEX_ATTRIBS];
	GET_CURRENT_STATE(state);

	shaderstate = &state->hw_state.hw_shader;
	glvstate = &state->gl_state.vertarray;
	hwvstate = &state->hw_state.hw_arrayproc;

	__cb_writeReg(PA_RW_CI_CONST_ATTR_NUMBER, 0xf, 0xf); /* direct data writing */
	__cb_writeReg(PA_RW_VC_DRAW_MODE, 1, 1);
	__cb_writeReg(PA_RW_LB_MULTIVP_WRITE, 0, 1);

	bContinue = GL_TRUE;

	for (j = 0; j < shaderstate->attrNumTotalUsed; j++){
		unsigned attrno = shaderstate->attrUseOrder[j];
		if (glvstate->array[attrno].enabled){
			size[j] = glvstate->array[attrno].size;
			type[j] = glvstate->array[attrno].type;
		}
		else{
			size[j] = 4;
			type[j] = GL_FLOAT;
		}
	}

	for (i = 0; bContinue; i++){
		int idx = 0;
		if (_bDrawElements){
			GLubyte idxdata[2];
			switch (_idxsize){
				case 1:
					if (state->gl_state.vertbuffer.elmtArrayID){
						__mem_readMemory(_first + hwvstate->elmtArrOffset + i, idxdata, 1);
						idx = idxdata[0];
					}
					else
						idx = *((GLubyte*)(_first + hwvstate->hostElmtArrOffset) + i);
					break;
				case 2:
					if (state->gl_state.vertbuffer.elmtArrayID){
						__mem_readMemory(hwvstate->elmtArrOffset + _first + i * 2, idxdata, 2);
						idx = *(GLushort*)idxdata;
					}
					else
						idx = *((GLushort*)(_first + hwvstate->hostElmtArrOffset) + i);
					break;
			}
		}
		else
			idx = i + _first;

		for (j = 0; j < shaderstate->attrNumTotalUsed; j++){
			int k;
			unsigned attrno = shaderstate->attrUseOrder[j];
			GLuint data[4];
			GLfloat tdata[4];
			GLubyte vertbuf[16];
			GLubyte* rawdata;
			GLuint buf[3];
			
			if (glvstate->array[attrno].buffer && glvstate->array[attrno].enabled){
				__mem_readMemory(hwvstate->attrArrOffset[attrno] + hwvstate->hostStride[attrno] * idx, vertbuf, 16);
				rawdata = vertbuf;
			}
			else
				rawdata = (GLubyte*)(hwvstate->hostAttrArrOffset[attrno] + hwvstate->hostStride[attrno] * idx);

			switch (type[j]){
				case GL_FLOAT:
					tdata[0] = ((GLfloat*)rawdata)[0];
					tdata[1] = ((GLfloat*)rawdata)[1];
					tdata[2] = ((GLfloat*)rawdata)[2];
					tdata[3] = ((GLfloat*)rawdata)[3];
					break;
				case GL_SHORT:
					tdata[0] = (GLfloat)((GLshort*)rawdata)[0];
					tdata[1] = (GLfloat)((GLshort*)rawdata)[1];
					tdata[2] = (GLfloat)((GLshort*)rawdata)[2];
					tdata[3] = (GLfloat)((GLshort*)rawdata)[3];
					break;
				case GL_BYTE:
					tdata[0] = (GLfloat)((GLbyte*)rawdata)[0];
					tdata[1] = (GLfloat)((GLbyte*)rawdata)[1];
					tdata[2] = (GLfloat)((GLbyte*)rawdata)[2];
					tdata[3] = (GLfloat)((GLbyte*)rawdata)[3];
					break;
				case GL_UNSIGNED_BYTE:
					tdata[0] = (GLfloat)((GLubyte*)rawdata)[0];
					tdata[1] = (GLfloat)((GLubyte*)rawdata)[1];
					tdata[2] = (GLfloat)((GLubyte*)rawdata)[2];
					tdata[3] = (GLfloat)((GLubyte*)rawdata)[3];
					break;
			}
			
			switch (size[j]){
				case 1:
					data[1] = 0;
				case 2:
					data[2] = 0;
				case 3:
					data[3] = 0x3f0000;
					break;
			}
			
			for (k = 0; k < size[j]; k++)
				UTL_F2F_16M7E(tdata[k], data[k]);

			buf[0] = (data[2] >> 16) | (data[3] <<  8);
			buf[1] = ((data[1] >> 8) & 0xffff) | ((data[2] & 0xffff) << 16);
			buf[2] = data[0] | ((data[1] & 0xff) << 24);
			__cb_writeRegs(PA_RW_CI_CONST_ATTR_DATA0, 3, buf);
		}

		bContinue = i < _count - 1;
	}
	
	__cb_writeReg(PA_RW_LB_MULTIVP_WRITE, 1, 1);
	__cb_writeReg(PA_RW_CI_CACHE_CLEAR, 1, 0xf);
	__cb_writeReg(PA_RW_VC_DRAW_MODE, 0, 1);
	__cb_writeReg(PA_WO_FU_CACHE_FLUSH, 1, 0xf);

	return;
}

void GL_APIENTRY glDrawArrays(GLenum _mode, GLint _first, GLsizei _count){
	stt_vert_array_state_t* arrstate;
	stt_hw_shader_state_t* shaderstate;
	unsigned texenabled;
	GET_CURRENT_STATE(state);
	
	BASE_GL_FAIL_IF(_count < 0 || _first < 0, GL_INVALID_VALUE);

	switch (_mode){
		case GL_TRIANGLES: case GL_TRIANGLE_STRIP: case GL_TRIANGLE_FAN:
			BASE_GL_FAIL_IF(state->gl_state.rendermode.bGeometryShaderUsed, GL_INVALID_OPERATION);
			break;
		case GL_GEOMETRY_PRIMITIVE_DMP:
			BASE_GL_FAIL_IF(!state->gl_state.rendermode.bGeometryShaderUsed, GL_INVALID_OPERATION);
			SET_MASK(state->upd_mask, IF_HW_RASTERIZER_REGISTER);
			break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}

	arrstate = &state->gl_state.vertarray;

	state->gl_state.rendermode.prevmode = state->gl_state.rendermode.mode;
	state->gl_state.rendermode.mode = _mode;
	state->gl_state.rendermode.drawing = GL_TRUE;
	state->gl_state.rendermode.drawarray = GL_TRUE;

	__vldtr_validateAllState();
	BASE_GL_FAIL_IF(state->gl_state.fb_state.status != GL_FRAMEBUFFER_COMPLETE, GL_INVALID_FRAMEBUFFER_OPERATION);

	if ((state->gl_state.triorient.enabled == GL_TRUE && state->gl_state.triorient.cullFace == GL_FRONT_AND_BACK) || (_count == 0))
		return;
	
	__shv_preDraw();

	switch (_mode){
		case GL_TRIANGLES:
	        __cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 2);
			break;
		case GL_TRIANGLE_STRIP:
	        __cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0x100, 2);
			break;
		case GL_TRIANGLE_FAN:
	        __cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0x200, 2);
			break;
		default:
			__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0x300, 2);
			break;
	}

	__cb_writeReg(PA_RW_RASIN_START_ARRAY, 1, 0xf);

	texenabled = state->hw_state.hw_texunits.regs[0] & 0x7;
	if (texenabled)
		__cb_writeReg(PA_WO_TU_BEGIN, texenabled, 0x1);

	shaderstate = &state->hw_state.hw_shader;

	if (shaderstate->useVertexBuffers){
		if (!(state->suppressStateMask & NN_GX_STATE_VERTEX)){
			if (((((_count > 0x10) ? (_count - 0x10) * 2 : 0) + state->hw_state.hw_arrayproc.baseAddr) & 0xfff) >= 0xfe0)
				__cb_writeReg(PA_RW_CI_INDEXARRAY_TYPE, 0x80000020, 0xf);
			else
				__cb_writeReg(PA_RW_CI_INDEXARRAY_TYPE, 0x80000000, 0xf);
		}
		__cb_writeReg(PA_RW_VC_DRAW_MODE, 1, 1);

		__cb_writeReg(PA_RW_CI_INDEXARRAY_COUNT, _count, 0xf);
		__cb_writeReg(PA_RW_CI_DRAWARRAY_FIRST, _first, 0xf);

		__cb_writeReg(PA_RW_LB_MULTIVP_WRITE, 0, 1);
		__cb_writeReg(PA_RW_CI_START_DRAW_ARRAY, 1, 0xf);
		__cb_writeReg(PA_RW_LB_MULTIVP_WRITE, 1, 1);

		__cb_writeReg(PA_RW_CI_CACHE_CLEAR, 1, 0xf);
		__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 8);
		__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 8);
		__cb_writeReg(PA_RW_VC_DRAW_MODE, 0, 1);
		__cb_writeReg(PA_WO_FU_CACHE_FLUSH, 1, 0xf);
	}
	else{
		#ifdef DMPGL_GEN_ERROR
		int i;
		GLboolean bBuffered = GL_FALSE;
		for (i = 0; i < MAX_VERTEX_ATTRIBS; i++){
			if (arrstate->array[i].enabled && arrstate->array[i].buffer){
				bBuffered = GL_TRUE;
				break;
			}
		}
		
		if (bBuffered)
			__err_setError(GL_INVALID_OPERATION);
		else
		#endif
			arp_drawHostArrays(_count, 2, _first, GL_FALSE);
	}

	if (texenabled)
		__cb_writeReg(PA_WO_TU_BEGIN, 0, 0x1);
	__cb_writeReg(PA_RW_VP_PROGRAM_ADDRESS + GEO_VP_LEN, 0x7fff0000, 0xc);
	if (_mode == GL_GEOMETRY_PRIMITIVE_DMP)
		__cb_writeReg(PA_RW_VP_PROGRAM_ADDRESS, 0x7fff0000, 0xc);

	state->gl_state.rendermode.drawing = GL_FALSE;
	state->gl_state.rendermode.drawarray = GL_FALSE;
	BASE_GL_FAIL_IF(!__cb_isCommandbufferEnabled, GL_ERROR_COMMANDBUFFER_FULL_DMP);
	if (state->forceValidateMask)
		state->forceValidateMask = 0;
	
	return;
}

void GL_APIENTRY glDrawElements(GLenum _mode, GLsizei _count, GLenum _type, const GLvoid *_indices){
	stt_hw_shader_state_t* shaderstate;
	stt_hw_array_proc_state_t* hwvstate;
	unsigned texenabled;
	GLuint type;

	GET_CURRENT_STATE(state);
	
	BASE_GL_FAIL_IF(_count < 0, GL_INVALID_VALUE);
	
	switch (_mode){
		case GL_TRIANGLES: case GL_TRIANGLE_STRIP: case GL_TRIANGLE_FAN:
			BASE_GL_FAIL_IF(state->gl_state.rendermode.bGeometryShaderUsed, GL_INVALID_OPERATION);
			break;
		case  GL_GEOMETRY_PRIMITIVE_DMP:
			BASE_GL_FAIL_IF(!state->gl_state.rendermode.bGeometryShaderUsed, GL_INVALID_OPERATION);
			SET_MASK(state->upd_mask, IF_HW_RASTERIZER_REGISTER);
			break ;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}

	switch (_type){
		case GL_UNSIGNED_BYTE:
			type = 0;
			break;
		case GL_UNSIGNED_SHORT:
			type = 1;
			break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}

	state->gl_state.rendermode.prevmode = state->gl_state.rendermode.mode;
	state->gl_state.rendermode.mode = _mode;
	state->gl_state.rendermode.drawing = GL_TRUE;

	__vldtr_validateAllState();
	BASE_GL_FAIL_IF(state->gl_state.fb_state.status != GL_FRAMEBUFFER_COMPLETE, GL_INVALID_FRAMEBUFFER_OPERATION);

	if ((state->gl_state.triorient.enabled == GL_TRUE && state->gl_state.triorient.cullFace == GL_FRONT_AND_BACK) || (_count == 0))
		return;

	hwvstate = &state->hw_state.hw_arrayproc;
	
	__shv_preDraw();

	switch (_mode){
		case GL_TRIANGLES:
			if (state->gl_state.rendermode.bAccerelateTriangle){
				__cb_writeReg(PA_RW_CI_VP_MODE, 0x100, 2);
				__cb_writeReg(PA_RW_VC_DRAW_MODE, 0x100, 2);
				__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0x300, 2);
			}
			else{
				__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 2);
			}
			break;
		case GL_TRIANGLE_STRIP:
	        __cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0x100, 2);
			break;
		case GL_TRIANGLE_FAN:
	        __cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0x200, 2);
			break;
		default:
			__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0x300, 2);
			break;
	}

	__cb_writeReg(PA_RW_RASIN_START_ARRAY, 1, 0xf);

	texenabled = state->hw_state.hw_texunits.regs[0] & 0x7;
	if (texenabled)
		__cb_writeReg(PA_WO_TU_BEGIN, texenabled, 0x1);

	shaderstate = &state->hw_state.hw_shader;

	if (shaderstate->useVertexBuffers){
		if (!(state->suppressStateMask & NN_GX_STATE_VERTEX))
			__cb_writeReg(PA_RW_CI_INDEXARRAY_TYPE, ((hwvstate->elmtArrOffset - hwvstate->baseAddr + (GLsizei)_indices) & 0xfffffff) | (type << 31), 0xf);

		__cb_writeReg(PA_RW_CI_INDEXARRAY_COUNT, _count, 0xf);

		__cb_writeReg(PA_RW_LB_MULTIVP_WRITE, 0, 1);
		__cb_writeReg(PA_RW_CI_START_DRAW_ELEMENT, 1, 0xf);
		__cb_writeReg(PA_RW_LB_MULTIVP_WRITE, 1, 1);

		__cb_writeReg(PA_RW_CI_CACHE_CLEAR, 1, 0xf);
		__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 8);
		__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 8);
		__cb_writeReg(PA_WO_FU_CACHE_FLUSH, 1, 0xf);
	}
	else
	{
		#ifdef DMPGL_GEN_ERROR
		GLboolean bBuffered = GL_FALSE;
		if (state->gl_state.vertbuffer.elmtArrayID)
			bBuffered = GL_TRUE;
		else{
			int i;
			for (i = 0; i < MAX_VERTEX_ATTRIBS; i++){
				if (state->gl_state.vertarray.array[i].enabled && state->gl_state.vertarray.array[i].buffer){
					bBuffered = GL_TRUE;
					break;
				}
			}
		}
		#endif
		
		if (_mode == GL_TRIANGLES && state->gl_state.rendermode.bAccerelateTriangle){
			__cb_writeReg(PA_RW_CI_VP_MODE, 0, 2);
			__cb_writeReg(PA_RW_VC_DRAW_MODE, 0, 2);
			__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 2);
		}

		#ifdef DMPGL_GEN_ERROR
		if (bBuffered)
			__err_setError(GL_INVALID_OPERATION);
		else
		#endif
			arp_drawHostArrays(_count, type + 1, (GLuint)_indices, GL_TRUE);
	}

	if (_mode == GL_TRIANGLES && state->gl_state.rendermode.bAccerelateTriangle){
		__cb_writeReg(PA_RW_CI_VP_MODE, 0, 2);
		__cb_writeReg(PA_RW_VC_DRAW_MODE, 0, 2);
		__cb_writeReg(PA_RW_RASIN_TRI_SETTING, 0, 2);
	}

	if (texenabled)
		__cb_writeReg(PA_WO_TU_BEGIN, 0, 0x1);
	__cb_writeReg(PA_RW_VP_PROGRAM_ADDRESS + GEO_VP_LEN, 0x7fff0000, 0xc);
	if (_mode == GL_GEOMETRY_PRIMITIVE_DMP)
		__cb_writeReg(PA_RW_VP_PROGRAM_ADDRESS, 0x7fff0000, 0xc);

	state->gl_state.rendermode.drawing = GL_FALSE;
	BASE_GL_FAIL_IF(!__cb_isCommandbufferEnabled, GL_ERROR_COMMANDBUFFER_FULL_DMP);
	if (state->forceValidateMask)
		state->forceValidateMask = 0;

	return;
}