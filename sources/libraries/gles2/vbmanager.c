// Filename: vbmanager.c
//
// Project: Horizon

#include "common.h"
#include "state.h"
#include "error.h"
#include "vbmanager.h"
#include "util.h"
#include "systemmanager.h"
#include "memmanager.h"
#include "vertexstatecache.h"

static vb_manager_t* __vbman = 0;
static GLuint __miniID;

static GLboolean vb_allocVertBufferMemory(vb_list_t* _vblist);
static void vb_freeVertBufferMemory(vb_list_t* _vblist);
static vb_vertbuffer_t* vb_createVertBuffer(void);
static vb_collection_t* vb_createCollection(void);
static void vb_releaseVertBuffer(vb_list_t* vblist);
static void vb_releaseCollection(vb_list_t* vblist);
static void vbv_validateVertBuffer(vb_vertbuffer_t* _vb, GLuint _target);
static void vbv_validateZeroBuffer(GLuint _target);

#define __GET_VB_LIST(buffer, vblist)	\
{	\
	for (vblist = __vbman->vb_list_table[buffer & VB_LIST_TABLE_SIZE]; vblist != 0; vblist = vblist->next)	\
	{	\
		if (vblist->id == buffer)	\
			break;	\
	}	\
}

GLint __vb_initializeVBManager(vb_manager_t* vbman){
	GLint ret;
	
	__vbman = vbman;
	{
		memset(__vbman, 0, sizeof(vb_manager_t));
		__vbman->defaultCollection = vb_createCollection();
		if (__vbman->defaultCollection == 0){
			free(__vbman);
			__vbman = 0;
			ret = -1;
		}
		else{
			ret = 0;
			__miniID = 1;
		}
	}
	
	return ret;
}

void __vb_finalizeVBManager(){
	if (__vbman){
		int i;
		GET_CURRENT_STATE(state);
		
		for (i = 0; i < VB_LIST_TABLE_SIZE + 1; i++){
			vb_list_t* vblist;
			vb_list_t* next;
			for (vblist = __vbman->vb_list_table[i]; vblist != 0; vblist = next){
				next = vblist->next;
				switch (vblist->type){
					case VT_BUF:
						vb_releaseVertBuffer(vblist);
						break;
					case VT_COLL:
						vb_releaseCollection(vblist);
						break;
				}
			}
		}
		free(__vbman->defaultCollection);
		state->gl_state.vertbuffer.arrayID = 0;
		state->gl_state.vertbuffer.elmtArrayID = 0;
		__vbman = 0;
	}
	
	return;
}

void __vb_setAttribArrayBuffer(GLuint index){
	__vbman->boundattribarray[index] = __vbman->boundarray;
}

void GL_APIENTRY glBindBuffer(GLenum _target, GLuint _buffer){
	vb_list_t*		vblist = 0;
	GET_CURRENT_STATE(state);
	
	BASE_GL_FAIL_IF(_target != GL_ARRAY_BUFFER && _target != GL_ELEMENT_ARRAY_BUFFER && _target != GL_VERTEX_STATE_COLLECTION_DMP, GL_INVALID_ENUM);
	
	if (_buffer){
		__GET_VB_LIST(_buffer, vblist)

		if (vblist == 0){
			vb_list_t* tmp;
			vb_list_t* newlist = (vb_list_t*)malloc(sizeof(vb_list_t));
			BASE_GL_FAIL_IF(!newlist, GL_OUT_OF_MEMORY);
			newlist->id = _buffer;
			newlist->next = 0;
			switch (_target){
				case GL_VERTEX_STATE_COLLECTION_DMP:
					newlist->container = (void*)vb_createCollection();
					newlist->type = VT_COLL;
					break;
				default:
					newlist->container = (void*)vb_createVertBuffer();
					newlist->type = VT_BUF;
					break;
			}

			if (__vbman->vb_list_table[_buffer & VB_LIST_TABLE_SIZE] == 0)
				__vbman->vb_list_table[_buffer & VB_LIST_TABLE_SIZE] = newlist;
			else{
				if (__vbman->vb_list_table[_buffer & VB_LIST_TABLE_SIZE]->id > _buffer){
					newlist->next = __vbman->vb_list_table[_buffer & VB_LIST_TABLE_SIZE];
					__vbman->vb_list_table[_buffer & VB_LIST_TABLE_SIZE] = newlist;
				}
				else{
					for (vblist = __vbman->vb_list_table[_buffer & VB_LIST_TABLE_SIZE]->next, tmp = __vbman->vb_list_table[_buffer & VB_LIST_TABLE_SIZE]; vblist != 0; vblist = vblist->next){
						if (vblist->id > _buffer){
							tmp->next = newlist;
							newlist->next = vblist;
							break;
						}
						tmp = vblist;
					}
					if (vblist == 0)
						tmp->next = newlist;
				}
			}
			vblist = newlist;
		}
		else if (vblist->container == 0){
			switch (_target){
				case GL_VERTEX_STATE_COLLECTION_DMP:
					vblist->container = (void*)vb_createCollection();
					vblist->type = VT_COLL;
					break;
				default:
					vblist->container = (void*)vb_createVertBuffer();
					vblist->type = VT_BUF;
					break;
			}
			BASE_GL_FAIL_IF(vblist->container == 0, GL_OUT_OF_MEMORY);
		}
		else{
			switch (_target){
				case GL_VERTEX_STATE_COLLECTION_DMP:
					BASE_GL_FAIL_IF(vblist->type != VT_COLL, GL_INVALID_OPERATION);
					break;
				default:
					BASE_GL_FAIL_IF(vblist->type != VT_BUF, GL_INVALID_OPERATION);
					break;
			}
		}
	}

	switch (_target){
		case GL_ARRAY_BUFFER:
			state->gl_state.vertbuffer.arrayID = _buffer;
			__vbman->boundarray = vblist;
			break;
		case GL_ELEMENT_ARRAY_BUFFER:
			state->gl_state.vertbuffer.elmtArrayID = _buffer;
			__vbman->boundelementarray = vblist;
			break;
		case GL_VERTEX_STATE_COLLECTION_DMP:
			if (state->gl_state.vertexStateCollection != _buffer){
				int i;
				vb_collection_t* collection;

				if (state->gl_state.vertexStateCollection == 0){
					__vbman->defaultCollection->arraystate = state->gl_state.vertarray;
					__vbman->defaultCollection->bufstate = state->gl_state.vertbuffer;
					__vbman->defaultCollection->attrstate = state->gl_state.vertcurr;
				}
				else{
					collection = (vb_collection_t*)__vbman->boundstatecollection->container;
					collection->arraystate = state->gl_state.vertarray;
					collection->bufstate = state->gl_state.vertbuffer;
					collection->attrstate = state->gl_state.vertcurr;
				}
				if (_buffer == 0)
					collection = __vbman->defaultCollection;
				else
					collection = (vb_collection_t*)vblist->container;

				state->gl_state.vertarray = collection->arraystate;
				state->gl_state.vertbuffer = collection->bufstate;
				state->gl_state.vertcurr = collection->attrstate;
				state->gl_state.vertexStateCollection = _buffer;
				__vbman->boundstatecollection = vblist;
				for (i = 0; i < 16; i++){
					if (state->gl_state.vertarray.array[i].buffer){
						__GET_VB_LIST(state->gl_state.vertarray.array[i].buffer, vblist)
						__vbman->boundattribarray[i] = vblist;
					}
					else
						__vbman->boundattribarray[i] = 0;
				}
				if (state->gl_state.vertbuffer.arrayID){
					__GET_VB_LIST(state->gl_state.vertbuffer.arrayID, vblist)
					__vbman->boundarray = vblist;
				}
				else
					__vbman->boundarray = 0;
				if (state->gl_state.vertbuffer.elmtArrayID){
					__GET_VB_LIST(state->gl_state.vertbuffer.elmtArrayID, vblist)
					__vbman->boundelementarray = vblist;
				}
				else
					__vbman->boundelementarray = 0;
				SET_MASK(state->upd_mask, IF_GL_VERT_ARRAY);
				SET_MASK(state->upd_mask, IF_GL_VERT_CURRENT);
				
				if (__vbman->deleteCollection){
					GLuint del = __vbman->deleteCollection;
					glDeleteBuffers(1, &del);
					__vbman->deleteCollection = 0;
				}
			}
			break;
	}

	SET_MASK(state->upd_mask, IF_GL_VERTBUFFER);
	
	return;
}

static vb_vertbuffer_t* vb_createVertBuffer(void){
	vb_vertbuffer_t* vb;
	
	vb = (vb_vertbuffer_t*)malloc(sizeof(vb_vertbuffer_t));
	if (vb){
		memset(vb, 0, sizeof(vb_vertbuffer_t));
		vb->usage = GL_STATIC_DRAW;
	}
	
	return vb;
}

static vb_collection_t* vb_createCollection(void){
	vb_collection_t* coll;
	
	coll = (vb_collection_t*)malloc(sizeof(vb_collection_t));
	if (coll){
		int i;
		memset(coll, 0, sizeof(vb_collection_t));
		for (i = 0; i < 16; i++){
			coll->arraystate.array[i].size = 4;
			coll->arraystate.array[i].type = GL_FLOAT;
			coll->attrstate.attrib[i][0] = coll->attrstate.attrib[i][1] = coll->attrstate.attrib[i][2] = 0.f;
			coll->attrstate.attrib[i][3] = 1.f;

			{
				unsigned v[4];
				int j;
				for (j = 0; j < 4; j++)
					UTL_F2F_16M7E(coll->attrstate.attrib[i][j], v[j]);
				
				coll->attrstate.attrib24[i][0] = (v[2] >> 16) | (v[3] <<  8);
				coll->attrstate.attrib24[i][1] = ((v[1] >> 8) & 0xffff) | ((v[2] & 0xffff) << 16);
				coll->attrstate.attrib24[i][2] = v[0] | ((v[1] & 0xff) << 24);
			}
		}
	}
	
	return coll;
}

void GL_APIENTRY glDeleteBuffers(GLsizei _n, const GLuint *_buffers){
	GLsizei i;
	vb_list_t* vblist;
	vb_list_t* tmp;
	
	GET_CURRENT_STATE(state);
	BASE_GL_FAIL_IF(_n < 0, GL_INVALID_VALUE);

	for (i = 0; i < _n; i++){
		if (!_buffers[i])
			continue;
		
		tmp = 0;
		for (vblist = __vbman->vb_list_table[_buffers[i] & VB_LIST_TABLE_SIZE]; vblist != 0; vblist = vblist->next){
			if (vblist->id >= _buffers[i])
				break;
			tmp = vblist;
		}
		if (vblist == 0 || vblist->id != _buffers[i])
			continue;

		if (state->gl_state.vertexStateCollection == _buffers[i]){
			__vbman->deleteCollection = _buffers[i];
			continue;
		}

		if (state->gl_state.vertbuffer.arrayID == _buffers[i]){
			SET_MASK(state->upd_mask, IF_GL_VERTBUFFER);
			state->gl_state.vertbuffer.arrayID = 0;
			__vbman->boundarray = 0;
		}

		if (state->gl_state.vertbuffer.elmtArrayID == _buffers[i]){
			SET_MASK(state->upd_mask, IF_GL_VERTBUFFER);
			state->gl_state.vertbuffer.elmtArrayID = 0;
			__vbman->boundelementarray = 0;
		}
		
		{
			int j;
			for (j = 0; j < 16; j++){
				if (state->gl_state.vertarray.array[j].buffer == _buffers[i])
					__vbman->boundattribarray[j] = 0;
			}
		}
		
		if (_buffers[i] < __miniID)
			__miniID = _buffers[i];

		if (tmp == 0)
			__vbman->vb_list_table[_buffers[i] & VB_LIST_TABLE_SIZE] = __vbman->vb_list_table[_buffers[i] & VB_LIST_TABLE_SIZE]->next;
		else
			tmp->next = vblist->next;
		
		/* free object */
		switch (vblist->type){
			case VT_BUF:
				vb_releaseVertBuffer(vblist);
				break;
			case VT_COLL:
				vb_releaseCollection(vblist);
				break;
		}
	}
	
	return;
}

static void vb_releaseVertBuffer(vb_list_t* _vblist){
	if (_vblist->container){
		vb_freeVertBufferMemory(_vblist);
		free(_vblist->container);
	}
	free(_vblist);

	return;
}

static void vb_releaseCollection(vb_list_t* _vblist){
	if (_vblist->container){
		free(_vblist->container);
	}
	free(_vblist);

	return;
}

void GL_APIENTRY glGenBuffers(GLsizei _n, GLuint *_buffers){
	GLint i;
	vb_list_t* vblist;
	vb_list_t* newlist;
	vb_list_t* tmp;
	GLuint id = 0;
	
	BASE_GL_FAIL_IF(_n < 0, GL_INVALID_VALUE);
	
	for (i = 0; i < _n; i++){
		newlist = (vb_list_t*)malloc(sizeof(vb_list_t));
		BASE_GL_FAIL_IF(!newlist, GL_OUT_OF_MEMORY);
		memset(newlist, 0, sizeof(vb_list_t));
		
		id = __miniID;
		for (;; id++){
			if (__vbman->vb_list_table[id & VB_LIST_TABLE_SIZE] == 0){
				_buffers[i] = id;
				newlist->id = id;
				newlist->next = 0;
				__vbman->vb_list_table[id & VB_LIST_TABLE_SIZE] = newlist;
				break;
			}
			else if (__vbman->vb_list_table[id & VB_LIST_TABLE_SIZE]->id == id){
				continue;
			}
			else if (__vbman->vb_list_table[id & VB_LIST_TABLE_SIZE]->id > id){
				_buffers[i] = id;
				newlist->id = id;
				newlist->next = __vbman->vb_list_table[id & VB_LIST_TABLE_SIZE];
				__vbman->vb_list_table[id & VB_LIST_TABLE_SIZE] = newlist;
				break;
			}
			else{
				int bCont, bBreak;
				bCont = bBreak = 0;
				for (vblist = __vbman->vb_list_table[id & VB_LIST_TABLE_SIZE]->next, tmp = __vbman->vb_list_table[id & VB_LIST_TABLE_SIZE]; vblist != 0; vblist = vblist->next){
					if (vblist->id == id){
						bCont = 1;
						break;
					}
					else if (vblist->id > id){
						_buffers[i] = id;
						newlist->id = id;
						tmp->next = newlist;
						newlist->next = vblist;
						bBreak = 1;
						break;
					}
					tmp = vblist;
				}
				if (bBreak)
					break;
				if (bCont)
					continue;
				
				if (!vblist){
					tmp->next = newlist;
					newlist->id = id;
					newlist->next = 0;
					_buffers[i] = id;
					break;
				}
				assert(0);
			}
		}
		__miniID = id + 1;
	}
	
	return;
}

static GLboolean vb_allocVertBufferMemory(vb_list_t* _vblist){
	GLboolean ret = GL_TRUE;
	vb_vertbuffer_t* _vb = (vb_vertbuffer_t*)_vblist->container;
	
	switch (_vb->transtype){
		case GL_NO_COPY_NO_DMA_DMP:
			break;
		case GL_NO_COPY_DMA_VRAMA_DMP:
		case GL_NO_COPY_DMA_VRAMB_DMP:
			_vb->allocarea = (_vb->transtype == GL_NO_COPY_DMA_VRAMA_DMP) ? NN_GX_MEM_VRAMA : NN_GX_MEM_VRAMB;
			_vb->picaaddr = malloc_ext(_vb->allocarea, NN_GX_MEM_VERTEXBUFFER, _vblist->id, _vb->size);
			if (!_vb->picaaddr)
				ret = GL_FALSE;
			break;
		case GL_COPY_NO_DMA_DMP:
			_vb->copyaddr = malloc_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_VERTEXBUFFER, _vblist->id, _vb->size);
			if (!_vb->copyaddr)
				ret = GL_FALSE;
			_vb->picaaddr = _vb->copyaddr;
			break;
		case GL_COPY_DMA_VRAMA_DMP:
		case GL_COPY_DMA_VRAMB_DMP:
			_vb->allocarea = (_vb->transtype == GL_COPY_DMA_VRAMA_DMP) ? NN_GX_MEM_VRAMA : NN_GX_MEM_VRAMB;
			_vb->picaaddr = malloc_ext(_vb->allocarea, NN_GX_MEM_VERTEXBUFFER, _vblist->id, _vb->size);
			_vb->copyaddr = malloc_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_VERTEXBUFFER, _vblist->id, _vb->size);
			if (!_vb->picaaddr || !_vb->copyaddr){
				ret = GL_FALSE;
				if (_vb->picaaddr)
					free_ext(_vb->allocarea, NN_GX_MEM_VERTEXBUFFER, _vblist->id, _vb->picaaddr);
				if (_vb->copyaddr)
					free_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_VERTEXBUFFER, _vblist->id, _vb->copyaddr);
			}
			break;
		default:
			break;
	}
	
	return ret;
}

static void vb_freeVertBufferMemory(vb_list_t* _vblist){
	vb_vertbuffer_t* _vb = (vb_vertbuffer_t*)_vblist->container;
	
    switch (_vb->transtype){
		case GL_NO_COPY_NO_DMA_DMP:
			break;
		case GL_NO_COPY_DMA_VRAMA_DMP:
		case GL_NO_COPY_DMA_VRAMB_DMP:
			if (_vb->picaaddr)
				free_ext(_vb->allocarea, NN_GX_MEM_VERTEXBUFFER, _vblist->id, _vb->picaaddr);
			break;
		case GL_COPY_NO_DMA_DMP:
			if (_vb->copyaddr)
				free_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_VERTEXBUFFER, _vblist->id, _vb->copyaddr);
			break;
		case GL_COPY_DMA_VRAMA_DMP:
		case GL_COPY_DMA_VRAMB_DMP:
			if (_vb->picaaddr)
				free_ext(_vb->allocarea, NN_GX_MEM_VERTEXBUFFER, _vblist->id, _vb->picaaddr);
			if (_vb->copyaddr)
				free_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_VERTEXBUFFER, _vblist->id, _vb->copyaddr);
			break;
		default:
			break;
	}
	_vb->picaaddr = 0;
	_vb->copyaddr = 0;
	
	return;
}

void GL_APIENTRY glBufferData(GLenum _target, GLsizeiptr _size, const GLvoid *_data, GLenum _usage){
	vb_list_t* vblist;
	vb_vertbuffer_t* vb;
	GLenum transtype;
	
	GET_CURRENT_STATE(state);
	
	BASE_GL_FAIL_IF(_size < 0, GL_INVALID_VALUE);
	if (_size == 0)
		return;
	
	transtype = _target & 0xffff0000;
	_target = _target & 0xffff;
	
	switch (_target){
		case GL_ARRAY_BUFFER:
			vblist = __vbman->boundarray;
			break ;
		case GL_ELEMENT_ARRAY_BUFFER:
			vblist = __vbman->boundelementarray;
			break ;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}

	BASE_GL_FAIL_IF(!vblist, GL_INVALID_OPERATION);
	BASE_GL_FAIL_IF(_usage != GL_STATIC_DRAW, GL_INVALID_ENUM);
	
	if ((transtype & 0xff000000) == 0)
		transtype |= GL_COPY_FCRAM_DMP;

	if ((transtype & 0x00ff0000) == 0)
		transtype |= NN_GX_MEM_FCRAM;

	vb = (vb_vertbuffer_t*)vblist->container;
	if (vb->picaaddr && (_size != vb->size || transtype != vb->transtype)){
		vb_freeVertBufferMemory(vblist);
	}
	
	if (!vb->picaaddr){
		vb->transtype = transtype;
		vb->size = _size;
		vb->usage = _usage & 0xffff;
		if (vb_allocVertBufferMemory(vblist) == GL_FALSE){
			__err_setError(GL_OUT_OF_MEMORY);
			return;
		}
	}

	if (_data){
		vb->appaddr = (void*)_data;

		switch (transtype){
			case GL_NO_COPY_NO_DMA_DMP:
				vb->picaaddr = vb->appaddr;
				break;
			case GL_COPY_NO_DMA_DMP:
			case GL_COPY_DMA_VRAMA_DMP:
			case GL_COPY_DMA_VRAMB_DMP:
				#ifndef RUN_ON_DUMMY_GL
					memcpy(_PICA_MEM_NATIVE(vb->copyaddr), vb->appaddr, vb->size);
				#endif
				break;
		}
		
		switch (transtype){
			case GL_NO_COPY_DMA_VRAMA_DMP:
			case GL_NO_COPY_DMA_VRAMB_DMP:
				__sys_setDMACommandRequest(vb->picaaddr, vb->appaddr, vb->size);
				break;
			case GL_COPY_DMA_VRAMA_DMP:
			case GL_COPY_DMA_VRAMB_DMP:
				__sys_setDMACommandRequest(vb->picaaddr, vb->copyaddr, vb->size);
				break;
		}

		switch (transtype){
			case GL_NO_COPY_NO_DMA_DMP:
			case GL_COPY_NO_DMA_DMP:
				nngxlowFlushDataCache(vb->picaaddr, vb->size);
				break;
			default:
				break;
		}
	}
	
	SET_MASK(state->upd_mask, IF_GL_VERTBUFFER);
	
	return;
}

void __vbv_initializeVBValidator(bit_mask_t* _mask){
	CLEAR_ALL_MASK(*_mask);
	
	SET_MASK(*_mask, IF_GL_VERTBUFFER);
	SET_MASK(*_mask, IF_GL_VERT_ARRAY);
	__vbv_validateVBValidator(_mask);
	
	return;
}

void __vbv_validateVBValidator(bit_mask_t* _mask){
	GLint i;
	stt_vert_buf_state_t* vertbuf;
	stt_vert_array_state_t* arrstate;
	stt_vert_current_state_t* vertcurr;
	stt_hw_array_proc_state_t* hwarrstate;
	vb_list_t* vblist;
	GET_CURRENT_STATE(state);
	
	vertbuf = &state->gl_state.vertbuffer;
	arrstate = &state->gl_state.vertarray;
	vertcurr = &state->gl_state.vertcurr;
	hwarrstate = &state->hw_state.hw_arrayproc;

	if (state->suppressStateMask & NN_GX_STATE_VERTEX)
		state->hw_state.hw_shader.useVertexBuffers = vertbuf->arrayID ? GL_TRUE : GL_FALSE;

	hwarrstate->bUseDeletedBuffer = GL_FALSE;
    if (vertbuf->elmtArrayID){
		if (__vbman->boundelementarray){
			vblist = __vbman->boundelementarray;
			vbv_validateVertBuffer((vb_vertbuffer_t*)vblist->container, 16);
		}
		else{
			hwarrstate->bUseDeletedBuffer = GL_TRUE;
			vbv_validateZeroBuffer(16);
		}
	}
	else
		vbv_validateZeroBuffer(16);

	for (i = 0; i < 16; i++){
		if (arrstate->array[i].enabled){
			if (arrstate->array[i].buffer){
		    	if (__vbman->boundattribarray[i]){
					vblist = __vbman->boundattribarray[i];
					vbv_validateVertBuffer((vb_vertbuffer_t*)vblist->container, i);
				}
				else{
					hwarrstate->bUseDeletedBuffer = GL_TRUE;
					vbv_validateZeroBuffer(i);
				}
			}
			else
				vbv_validateZeroBuffer(i);
		}
		else{
			hwarrstate->hostAttrArrOffset[i] = (GLsizeiptr)(vertcurr->attrib[i]);
			hwarrstate->hostStride[i] = 0 ;
		}
	}
	
	return;
}

static void vbv_validateVertBuffer(vb_vertbuffer_t* _vb, GLuint _target){
	stt_hw_array_proc_state_t* hw_arraystate;
	stt_vert_array_state_t* arrstate;
	GLuint typesize;
	GET_CURRENT_STATE(state);
	
	hw_arraystate = &state->hw_state.hw_arrayproc;

	if (_target == 16){
		hw_arraystate->elmtArrOffset = nngxlowGetPhysicalAddr((GLsizeiptr)_vb->picaaddr);
		hw_arraystate->hostElmtArrOffset = (GLsizeiptr)_vb->appaddr;
	}
	else{
		arrstate = &state->gl_state.vertarray;
		
		hw_arraystate->attrArrOffset[_target] = nngxlowGetPhysicalAddr((GLsizeiptr)_vb->picaaddr + (GLsizeiptr)arrstate->array[_target].ptr);
		hw_arraystate->hostAttrArrOffset[_target] = (GLsizeiptr)_vb->appaddr + (GLsizeiptr)arrstate->array[_target].ptr;
		UTL_TYPESIZE(arrstate->array[_target].type, typesize);
		hw_arraystate->hostStride[_target] = arrstate->array[_target].stride ? arrstate->array[_target].stride :
			typesize * arrstate->array[_target].size;
	}

	return;
}

static void vbv_validateZeroBuffer(GLuint _target){
	stt_hw_array_proc_state_t* hw_arraystate;
	stt_vert_array_state_t* arrstate;
	GLuint typesize;
	GET_CURRENT_STATE(state);
	
	hw_arraystate = &state->hw_state.hw_arrayproc;
	arrstate = &state->gl_state.vertarray ;

	switch (_target){
		case 16:  // WHAT THE FUCK IS 16???
			hw_arraystate->hostElmtArrOffset = hw_arraystate->elmtArrOffset = 0;
			break;
		default:
			hw_arraystate->hostAttrArrOffset[_target] = hw_arraystate->attrArrOffset[_target] = (GLsizeiptr)arrstate->array[_target].ptr;
			UTL_TYPESIZE(arrstate->array[_target].type, typesize);
			hw_arraystate->hostStride[_target] = arrstate->array[_target].stride ? arrstate->array[_target].stride :
				typesize*arrstate->array[_target].size;
			break;
	}
	
	return;
}