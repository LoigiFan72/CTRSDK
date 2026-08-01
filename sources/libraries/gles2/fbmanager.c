// Filename: fbmanager.c
//
// Project: Horizon

#include "common.h"
#include "state.h"
#include "error.h"
#include "util.h"
#include "fbmanager.h"
#include "surface.h"
#include "texmanager.h"
#include "picaregmap.h"
#include "commandbuffer.h"
#include "statevalidator.h"

static fb_manager_t* __fbman = 0;

static fb_list_t* fb_createFramebuffer();
static rb_list_t* fb_createRenderbuffer();
static void fb_releaseRenderbuffer(rb_list_t* _rblist);
static void fb_releaseFramebuffer(fb_list_t* _fb);
static void fb_linkRBList(rb_list_t* newlist);
static void fb_linkFBList(fb_list_t* newlist);
static GLenum fb_status(fb_list_t* _fblist);

GLint __fb_initializeFBManager(fb_manager_t* fbman){
	GLint ret;

	__fbman = fbman;
	{
		fb_list_t* fblist;
		fblist = fb_createFramebuffer();
		if (!fblist)
			ret = -1;
		else{
			fblist->id = 0;
			fblist->next = 0;
			__fbman->fbListTop = fblist;
			__fbman->rbListTop = 0;
			__fbman->currFB = fblist;
			__fbman->currRB = 0;
			ret = 0;
		}
	}

	return ret;
}

void __fb_finalizeFBManager(){
	fb_list_t	*fblist, *fbnext;
	rb_list_t	*rblist, *rbnext;

	if (__fbman){
		for (rblist = __fbman->rbListTop; rblist != 0; rblist = rbnext){
			rbnext = rblist->next;
			fb_releaseRenderbuffer(rblist);
		}

		for (fblist = __fbman->fbListTop; fblist != 0; fblist = fbnext){
			fbnext = fblist->next;
			fb_releaseFramebuffer(fblist);
		}

		__fbman = 0;
	}

	return;
}

void __fbv_initializeFBValidator(bit_mask_t* _mask){
	CLEAR_ALL_MASK(*_mask);

	SET_MASK(*_mask, IF_GL_FRAMEBUFFER);

	return;
}

void __fbv_validateFBValidator(bit_mask_t* _mask){
	GLenum stat;
	GLsizei w, h;
	srf_container_t* pSurfColor;
	srf_container_t* pSurfZS;
	stt_fb_state_t* fbstate;
	stt_hw_framebuffer_state_t* hwfbstate;
	stt_viewport_state_t* vpstate;
	fb_list_t* _fb = __fbman->currFB;
	GET_CURRENT_STATE(state);

	stat = fb_status(_fb);
	fbstate = &state->gl_state.fb_state;

	if (stat != GL_FRAMEBUFFER_COMPLETE){
		fbstate->status = 0;
		fbstate->colorBufferEnabled = GL_FALSE;
		fbstate->depthBufferEnabled = GL_FALSE;
		fbstate->stencilBufferEnabled = GL_FALSE;

		return;
	}

	fbstate->status = stat;

	if (state->suppressStateMask & NN_GX_STATE_FRAMEBUFFER)
		return;

	hwfbstate = &state->hw_state.hw_framebuffer;

	w = 0;
	h = 0;

	pSurfColor = _fb->AttInfo[FB_AP_COLOR0].surfaceContainer;
	pSurfZS = _fb->AttInfo[FB_AP_DEPTH_STENCIL].surfaceContainer;

	if ((pSurfColor && hwfbstate->addrColor != (GLuint)pSurfColor->picaaddr) || (pSurfZS && hwfbstate->addrZ != (GLuint)pSurfZS->picaaddr) || (state->forceValidateMask & NN_GX_STATE_FRAMEBUFFER)){
		__cb_writeReg(PA_WO_FU_CACHE_FLUSH, 1, 0xf);
		__cb_writeReg(PA_WO_FU_TAG_CLEAR, 1, 0xf);
	}
	if (_fb->AttInfo[FB_AP_COLOR0].surfaceContainer){
		fbstate->colorBufferEnabled = GL_TRUE;

		hwfbstate->addrColor = (GLuint)pSurfColor->picaaddr;
		hwfbstate->sizeColor = pSurfColor->size;

		switch (pSurfColor->residentType){
			case TF_R8G8B8A8:
				fbstate->color_format = CF_R8G8B8A8;
				hwfbstate->bitWidthColor = 2;
				break;
			case TF_R5G5B5A1:
				fbstate->color_format = CF_R5G5B5A1;
				hwfbstate->bitWidthColor = 0;
				break;
			case TF_R5G6B5:
				fbstate->color_format = CF_R5G6B5;
				hwfbstate->bitWidthColor = 0;
				break;
			case TF_R4G4B4A4:
				fbstate->color_format = CF_R4G4B4A4;
				hwfbstate->bitWidthColor = 0;
				break;
			default:
				__err_setError(GL_INVALID_OPERATION);
				break;
		}

		__cb_writeReg(PA_WO_FU_BIT_WIDTH_COLOR, (fbstate->color_format << 16) | hwfbstate->bitWidthColor, 0xf);
		__cb_writeReg(PA_WO_FU_MEM_OFFSET_COLOR, nngxlowGetPhysicalAddr(hwfbstate->addrColor) >> 3, 0xf);
		
		w = pSurfColor->width;
		h = pSurfColor->height;
	}
	else{
		fbstate->colorBufferEnabled = GL_FALSE;
	}

	if (_fb->AttInfo[FB_AP_DEPTH_STENCIL].surfaceContainer){
		enum Z_FORMAT prevz_format = state->gl_state.fb_state.z_format;
		
		fbstate->depthBufferEnabled = GL_TRUE;
		hwfbstate->addrZ = (unsigned)pSurfZS->picaaddr;

		switch (pSurfZS->pixelSize){
			case 32:
				hwfbstate->bitWidthZ = 3;
				state->gl_state.fb_state.z_format = ZF_Z24S8;
				break;
			case 24:
				hwfbstate->bitWidthZ = 2;
				state->gl_state.fb_state.z_format = ZF_Z24;
				break;
			case 16:
				hwfbstate->bitWidthZ = 0;
				state->gl_state.fb_state.z_format = ZF_Z16;
				break;
			default:
				__err_setError(GL_INVALID_OPERATION);
				break;
		}

		if (state->gl_state.trioffset.enabled){
			if ((state->gl_state.fb_state.z_format == ZF_Z16 && prevz_format != ZF_Z16)
				|| (state->gl_state.fb_state.z_format != ZF_Z16 && prevz_format == ZF_Z16))
				SET_MASK(state->upd_mask, IF_GL_TRI_OFFSET);
		}

		fbstate->stencilBufferEnabled = (state->gl_state.fb_state.z_format == ZF_Z24S8) ? GL_TRUE : GL_FALSE;
		hwfbstate->sizeZ = pSurfZS->size;

		__cb_writeReg(PA_WO_FU_BIT_WIDTH_Z, hwfbstate->bitWidthZ, 0xf);
		__cb_writeReg(PA_WO_FU_MEM_OFFSET_Z, nngxlowGetPhysicalAddr(hwfbstate->addrZ) >> 3, 0xf);

		w = pSurfZS->width;
		h = pSurfZS->height;
	}
	else{
		fbstate->depthBufferEnabled = GL_FALSE;
		fbstate->stencilBufferEnabled = GL_FALSE;
	}

	if (fbstate->width != w || fbstate->height != h || (state->forceValidateMask & NN_GX_STATE_FRAMEBUFFER))
	{
		unsigned int property;

		property = (((h - 1) & 0xfff) << 12) | (w & 0xfff);
		__cb_writeReg(PA_WO_FU_DISPLAY_PROPERTY, property | 0x1000000, 0xf);
		__cb_writeReg(PA_WO_TI_DISPLAY_PROPERTY, property | 0x1000000, 0xf);
		fbstate->width = w;
		fbstate->height = h;
		if (state->gl_state.per_frag_op.scissorTestEnabled)
			SET_MASK(state->upd_mask, IF_GL_FB_SCISSOR);
	}

	vpstate = &state->gl_state.viewport;

	if (!vpstate->bInitted)
	{
		vpstate->bInitted = GL_TRUE;
		glViewport(0, 0, w, h);
	}

	return;
}

srf_container_t* __fb_getCurrentFBColorSurface(void){
	return __fbman->currFB->AttInfo[FB_AP_COLOR0].surfaceContainer;
}

void GL_APIENTRY glBindRenderbuffer(GLenum _target, GLuint _rb)
{
	rb_list_t* rblist;
	GET_CURRENT_STATE(state);

	BASE_GL_FAIL_IF(_target != GL_RENDERBUFFER, GL_INVALID_ENUM);

	if (__fbman->currRB && _rb == __fbman->currRB->id)
		return;

	if (_rb){
		for (rblist = __fbman->rbListTop; rblist != 0; rblist = rblist->next){
			if (rblist->id >= _rb)
				break;
		}
		if (rblist && rblist->id != _rb)
			rblist = 0;
		if (rblist == 0){
			rblist = fb_createRenderbuffer();
			rblist->id = _rb;
			BASE_GL_FAIL_IF(!rblist, GL_OUT_OF_MEMORY);
			fb_linkRBList(rblist);
		}
	}

	__fbman->currRB = _rb ? rblist : 0;

	SET_MASK(state->upd_mask, IF_GL_FRAMEBUFFER);

	return;
}

static rb_list_t* fb_createRenderbuffer(void){
	rb_list_t* rblist;

	rblist = (rb_list_t*)malloc(sizeof(rb_list_t));
	if (rblist){
		memset(rblist, 0, sizeof(rb_list_t));
	}

	return rblist;
}

static void fb_linkRBList(rb_list_t* newlist){
	rb_list_t *rblist, *tmp;

	if (__fbman->rbListTop == 0)
		__fbman->rbListTop = newlist;
	else{
		if (__fbman->rbListTop->id > newlist->id){
			newlist->next = __fbman->rbListTop;
			__fbman->rbListTop = newlist;
		}
		else{
			for (rblist = __fbman->rbListTop->next, tmp = __fbman->rbListTop; rblist != 0; rblist = rblist->next){
				if (rblist->id > newlist->id){
					tmp->next = newlist;
					newlist->next = rblist;
					break;
				}
				tmp = rblist;
			}
			if (rblist == 0)
				tmp->next = newlist;
		}
	}

	return;
}

void GL_APIENTRY glDeleteRenderbuffers(GLsizei _n, const GLuint *_prb){
	GLsizei i;
	rb_list_t*	rblist;
	rb_list_t*	tmp;
	GET_CURRENT_STATE(state);

	BASE_GL_FAIL_IF(_n < 0, GL_INVALID_VALUE);
	for (i = 0; i < _n; i++){
		if (!_prb[i])
			continue;

		tmp = 0;
		for (rblist = __fbman->rbListTop; rblist != 0; rblist = rblist->next){
			if (rblist->id >= _prb[i])
				break;
			tmp = rblist;
		}
		if (rblist == 0 || rblist->id != _prb[i])
			continue;

		if (tmp == 0)
			__fbman->rbListTop = __fbman->rbListTop->next;
		else
			tmp->next = rblist->next;
		
		if (__fbman->currRB == rblist){
			__fbman->currRB = 0;
		}
		fb_releaseRenderbuffer(rblist);
	}

	SET_MASK(state->upd_mask, IF_GL_FRAMEBUFFER);

	return;
}

static void fb_releaseRenderbuffer(rb_list_t* _rblist){
	if (_rblist){
		__fb_detachSurfaceContainer(&_rblist->surfaceContainer);
		if (_rblist->surfaceContainer.picaaddr)
			free_ext(_rblist->surfaceContainer.allocarea, NN_GX_MEM_RENDERBUFFER, _rblist->id, _rblist->surfaceContainer.picaaddr);
		free(_rblist);
	}

	return;
}

void __fb_detachSurfaceContainer(srf_container_t* _pCont){
	int i;
	fb_list_t* fblist;

	if (!_pCont)
		return;

	for (fblist = __fbman->fbListTop; fblist != 0; fblist = fblist->next){
		for (i = 0; i < FB_AP_MAX_POINTS; i++){
			if (fblist->AttInfo[i].surfaceContainer == _pCont){
				if (fblist == __fbman->currFB){
					GET_CURRENT_STATE(state);
					SET_MASK(state->upd_mask, IF_GL_FRAMEBUFFER);
				}

				fblist->AttInfo[i].surfaceContainer = 0;
				fblist->AttInfo[i].type = FB_AT_RENDERBUFFER;
				fblist->AttInfo[i].id = 0;
				fblist->AttInfo[i].target = 0;
			}
		}
	}

	return;
}

void GL_APIENTRY glGenRenderbuffers(GLsizei _n, GLuint *_prb){
	unsigned int i;
	rb_list_t* tmp;
	rb_list_t* newlist;

	BASE_GL_FAIL_IF(_n < 0, GL_INVALID_VALUE);
	if (_n == 0)
		return;

	if (__fbman->rbListTop == 0){
		tmp = 0;
		for (i = 0; i < (unsigned)_n; i++){
			newlist = fb_createRenderbuffer();
			BASE_GL_FAIL_IF(!newlist, GL_OUT_OF_MEMORY);
			if (tmp)
				tmp->next = newlist;
			else
				__fbman->rbListTop = newlist;
			tmp = newlist;
			_prb[i] = i+1;
			newlist->id = i+1;
		}
	}
	else{
		int idx = 0;
		GLuint id = 2;
		rb_list_t* rblist;
		
		if (__fbman->rbListTop->id != 1){
			newlist = fb_createRenderbuffer();
			BASE_GL_FAIL_IF(!newlist, GL_OUT_OF_MEMORY);
			_prb[idx++] = 1;
			newlist->id = 1;
			newlist->next = __fbman->rbListTop;
			__fbman->rbListTop = newlist;
			_n--;
		}

		for (rblist = __fbman->rbListTop->next, tmp = __fbman->rbListTop; rblist != 0 && _n != 0; rblist = rblist->next){
			if (rblist->id > id){
				for (i = 0; i < (rblist->id - id) && _n != 0; i++){
					newlist = fb_createRenderbuffer();
					BASE_GL_FAIL_IF(!newlist, GL_OUT_OF_MEMORY);
					newlist->id = i + id;
					_prb[idx] = i + id;
					tmp->next = newlist;
					newlist->next = rblist;
					tmp = newlist;
					idx++;
					_n--;
				}
				id = rblist->id;
			}
			id++;
			tmp = rblist;
		}

		if (_n != 0){
			for (i = 0; i < (unsigned)_n; i++){
				newlist = fb_createRenderbuffer();
				BASE_GL_FAIL_IF(!newlist, GL_OUT_OF_MEMORY);
				tmp->next = newlist;
				newlist->id = id;
				tmp = newlist;
				_prb[idx] = id;
				id++;
				idx++;
			}
			tmp->next = 0;
		}
	}

	return;
}

void GL_APIENTRY glGetRenderbufferParameteriv(GLenum _target, GLenum _pname, GLint* _params){
	srf_container_t* pSurf;
	rb_list_t* _rblist;

	BASE_GL_FAIL_IF(!__fbman->currRB, GL_INVALID_OPERATION);
	BASE_GL_FAIL_IF(_target != GL_RENDERBUFFER, GL_INVALID_ENUM);

	_rblist = __fbman->currRB;
	pSurf = &_rblist->surfaceContainer;

	switch (_pname){
		case GL_RENDERBUFFER_WIDTH:
			_params[0] = pSurf->width;
			break;
		case GL_RENDERBUFFER_HEIGHT:
			_params[0] = pSurf->height;
			break;
		case GL_RENDERBUFFER_INTERNAL_FORMAT:
			_params[0] = pSurf->format;
			break;
		case GL_RENDERBUFFER_RED_SIZE:
		case GL_RENDERBUFFER_GREEN_SIZE:
		case GL_RENDERBUFFER_BLUE_SIZE:
		case GL_RENDERBUFFER_ALPHA_SIZE:
			if (pSurf->format == GL_RGBA8_OES || pSurf->format == GL_RGB5_A1
			 || pSurf->format == GL_RGB565 || pSurf->format == GL_RGBA4 || pSurf->format == GL_RGBA || pSurf->format == GL_RGB)
				_params[0] = pSurf->compSize[_pname - GL_RENDERBUFFER_RED_SIZE];
			else
				_params[0] = 0;
			break;
		case GL_RENDERBUFFER_DEPTH_SIZE:
		case GL_RENDERBUFFER_STENCIL_SIZE:
			if (pSurf->format == GL_DEPTH_COMPONENT16 || pSurf->format == GL_DEPTH_COMPONENT24_OES
			 || pSurf->format == GL_DEPTH24_STENCIL8_EXT)
				_params[0] = pSurf->compSize[_pname - GL_RENDERBUFFER_DEPTH_SIZE];
			else
				_params[0] = 0;
			break;
		case GL_RENDERBUFFER_DATA_ADDR_DMP:
			_params[0] = (GLint)pSurf->picaaddr;
			break;
		default:
			__err_setError(GL_INVALID_ENUM);
			break;
	}

	return;
}

static void fb_linkFBList(fb_list_t* newlist){
	fb_list_t *fblist, *tmp;

	if (__fbman->fbListTop == 0)
		__fbman->fbListTop = newlist;
	else{
		if (__fbman->fbListTop->id > newlist->id){
			newlist->next = __fbman->fbListTop;
			__fbman->fbListTop = newlist;
		}
		else{
			for (fblist = __fbman->fbListTop->next, tmp = __fbman->fbListTop; fblist != 0; fblist = fblist->next){
				if (fblist->id > newlist->id){
					tmp->next = newlist;
					newlist->next = fblist;
					break;
				}
				tmp = fblist;
			}
			if (fblist == 0)
				tmp->next = newlist;
		}
	}

	return;
}

void GL_APIENTRY glBindFramebuffer(GLenum _target, GLuint _fb){
	fb_list_t* fblist;
	GET_CURRENT_STATE(state);

	BASE_GL_FAIL_IF(_target != GL_FRAMEBUFFER, GL_INVALID_ENUM);

	if (_fb == __fbman->currFB->id)
		return;

	for (fblist = __fbman->fbListTop; fblist != 0; fblist = fblist->next){
		if (fblist->id >= _fb)
			break;
	}
	if (fblist && fblist->id != _fb)
		fblist = 0;
	if (fblist == 0){
		fblist = fb_createFramebuffer();
		BASE_GL_FAIL_IF(!fblist, GL_OUT_OF_MEMORY);
		fblist->id = _fb;
		fblist->next = 0;
		fb_linkFBList(fblist);
	}

	__fbman->currFB = fblist;

	SET_MASK(state->upd_mask, IF_GL_FRAMEBUFFER);

	return;
}

static fb_list_t* fb_createFramebuffer(void){
	fb_list_t* fblist;

	fblist = (fb_list_t*)malloc(sizeof(fb_list_t));
	if (fblist){
		int i;
		
		memset(fblist, 0, sizeof(fb_list_t));
		for (i = 0; i < FB_AP_MAX_POINTS; i++){
			fblist->AttInfo[i].type = FB_AT_RENDERBUFFER;
		}
	}

	return fblist;
}

void GL_APIENTRY glDeleteFramebuffers(GLsizei _n, const GLuint *_pfb){
	GLsizei i;
	fb_list_t*	fblist;
	fb_list_t*	tmp;
	GET_CURRENT_STATE(state);

	BASE_GL_FAIL_IF(_n < 0, GL_INVALID_VALUE);
	for (i = 0; i < _n; i++){
		if (!_pfb[i])
			continue;

		tmp = 0;
		for (fblist = __fbman->fbListTop; fblist != 0; fblist = fblist->next){
			if (fblist->id >= _pfb[i])
				break;
			tmp = fblist;
		}
		if (fblist == 0 || fblist->id != _pfb[i])
			continue;

		if (__fbman->currFB == fblist)
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		if (tmp == 0)
			__fbman->fbListTop = __fbman->fbListTop->next;
		else
			tmp->next = fblist->next;
		fb_releaseFramebuffer(fblist);
	}

	SET_MASK(state->upd_mask, IF_GL_FRAMEBUFFER);

	return;
}

static void fb_releaseFramebuffer(fb_list_t* fb){
	if (fb)
		free(fb);

	return;
}

void GL_APIENTRY glGenFramebuffers(GLsizei _n, GLuint *_pfb){
	unsigned int i;
	fb_list_t* tmp;
	fb_list_t* newlist;

	BASE_GL_FAIL_IF(_n < 0, GL_INVALID_VALUE);
	if (_n == 0)
		return;

	if (__fbman->fbListTop == 0){
		tmp = 0;
		for (i = 0; i < (unsigned)_n; i++){
			newlist = fb_createFramebuffer();
			BASE_GL_FAIL_IF(!newlist, GL_OUT_OF_MEMORY);
			if (tmp)
				tmp->next = newlist;
			else
				__fbman->fbListTop = newlist;
			tmp = newlist;
			_pfb[i] = i+1;
			newlist->id = i+1;
		}
	}
	else{
    	int idx = 0;
		GLuint id = 1;
		fb_list_t* fblist;
		
		for (fblist = __fbman->fbListTop->next, tmp = __fbman->fbListTop; fblist != 0 && _n != 0; fblist = fblist->next){
			if (fblist->id > id){
				for (i = 0; i < (fblist->id - id) && _n != 0; i++){
					newlist = fb_createFramebuffer();
					BASE_GL_FAIL_IF(!newlist, GL_OUT_OF_MEMORY);
					newlist->id = i + id;
					_pfb[idx] = i + id;
					tmp->next = newlist;
					newlist->next = fblist;
					tmp = newlist;
					idx++;
					_n--;
				}
				id = fblist->id;
			}
			id++;
			tmp = fblist;
		}

		if (_n != 0){
			for (i = 0; i < (unsigned)_n; i++){
				newlist = fb_createFramebuffer();
				BASE_GL_FAIL_IF(!newlist, GL_OUT_OF_MEMORY);
				tmp->next = newlist;
				newlist->id = id;
				tmp = newlist;
				_pfb[idx] = id;
				id++;
				idx++;
			}
			tmp->next = 0;
		}
	}

	return;
}

GLenum GL_APIENTRY glCheckFramebufferStatus(GLenum _target){
	BASE_GL_FAIL_IF_RET(_target != GL_FRAMEBUFFER, GL_INVALID_ENUM, GL_INVALID_ENUM);

	return fb_status(__fbman->currFB);
}


static GLenum fb_status(fb_list_t* _fblist){
	int i;
	srf_container_t* pSurf;
	GLint w = 0, h = 0;

	if (!_fblist->AttInfo[FB_AP_COLOR0].surfaceContainer && !_fblist->AttInfo[FB_AP_DEPTH_STENCIL].surfaceContainer)
		return GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;

	for (i = 0; i < FB_AP_MAX_POINTS; i++){
		if (!_fblist->AttInfo[i].surfaceContainer)
			continue;

		pSurf = _fblist->AttInfo[i].surfaceContainer;

		if (!pSurf->picaaddr || !pSurf->width || !pSurf->height)
			return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;

		if (w == 0){
			w = pSurf->width;
			h = pSurf->height;
		}
		else if (w != pSurf->width || h != pSurf->height)
			return GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS;
	}

	if (_fblist->AttInfo[FB_AP_COLOR0].surfaceContainer == _fblist->AttInfo[FB_AP_DEPTH_STENCIL].surfaceContainer)
		return GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;

	return GL_FRAMEBUFFER_COMPLETE;
}

void GL_APIENTRY glFramebufferRenderbuffer(GLenum _target, GLenum _attach, GLenum _rbtarget, GLuint _rb){
	int pos;
	rb_list_t* rblist;
	fb_list_t* fblist;
	GET_CURRENT_STATE(state);

	BASE_GL_FAIL_IF(_target != GL_FRAMEBUFFER || (_rbtarget != GL_RENDERBUFFER && _rb), GL_INVALID_ENUM);

	for (rblist = __fbman->rbListTop; rblist != 0; rblist = rblist->next){
		if (rblist->id >= _rb)
			break;
	}
	if (rblist && rblist->id != _rb)
		rblist = 0;
	BASE_GL_FAIL_IF(_rb && !rblist, GL_INVALID_OPERATION);

	fblist = __fbman->currFB;

	switch (_attach){
		case GL_COLOR_ATTACHMENT0:
			pos = FB_AP_COLOR0;
			BASE_GL_FAIL_IF(rblist && rblist->surfaceContainer.format != GL_RGBA8_OES && rblist->surfaceContainer.residentType != CF_R5G5B5A1
				&& rblist->surfaceContainer.residentType != CF_R5G6B5 && rblist->surfaceContainer.residentType != CF_R4G4B4A4
				&& rblist->surfaceContainer.format != GL_GAS_DMP, GL_INVALID_OPERATION);
			BASE_GL_FAIL_IF(rblist && 
				(rblist->surfaceContainer.format == GL_DEPTH24_STENCIL8_EXT ||
				 rblist->surfaceContainer.format == GL_DEPTH_COMPONENT16 ||
				 rblist->surfaceContainer.format == GL_DEPTH_COMPONENT24_OES), GL_INVALID_OPERATION);
			break;
		case GL_DEPTH_ATTACHMENT:
			pos = FB_AP_DEPTH_STENCIL;
			BASE_GL_FAIL_IF(rblist && rblist->surfaceContainer.format != GL_DEPTH24_STENCIL8_EXT
				&& rblist->surfaceContainer.format != GL_DEPTH_COMPONENT16 && rblist->surfaceContainer.format != GL_DEPTH_COMPONENT24_OES, GL_INVALID_OPERATION);
			break;
		case GL_DEPTH_STENCIL_ATTACHMENT:
			pos = FB_AP_DEPTH_STENCIL;
			BASE_GL_FAIL_IF(rblist && rblist->surfaceContainer.format != GL_DEPTH24_STENCIL8_EXT, GL_INVALID_OPERATION);
			break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}

	fblist->AttInfo[pos].surfaceContainer = _rb ? &rblist->surfaceContainer : 0;
	fblist->AttInfo[pos].id = _rb ? rblist->id : 0;
	fblist->AttInfo[pos].type = FB_AT_RENDERBUFFER;
	fblist->AttInfo[pos].target = 0;

	SET_MASK(state->upd_mask, IF_GL_FRAMEBUFFER);

	return;
}