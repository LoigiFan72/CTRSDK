// Filename: texmanager.c
//
// Project: Horizon

#include "common.h"
#include "state.h"
#include "error.h"
#include "util.h"
#include "surface.h"
#include "texmanager.h"
#include "fbmanager.h"
#include "commandbuffer.h"
#include "statevalidator.h"
#include "memmanager.h"
#include "systemmanager.h"
#include "picaregmap.h"
#include "texstatecache.h"
#include "garent.h"

static tex_manager_t*	__texman = 0;
static GLuint __miniID;

/******************************************
* static function prototypes
*******************************************/
static GLboolean tx_allocTextureMemory(GLenum _transtype, tx_texture_t* _tex);
static void tx_freeTextureMemory(GLenum _transtype, tx_texture_t* _tex);
static tx_tex_container_t* tx_createTexContainer(GLuint _id);
static tx_cube_tex_container_t* tx_createCubeTexContainer(GLuint _id);
static tx_lut_container_t* tx_createLutContainer(GLuint _id);
static tx_collection_container_t* tx_createCollectionContainer(GLuint _id);
static void tx_releaseTexContainer(tx_tex_container_t* _tc);
static void tx_releaseCubeTexContainer(tx_cube_tex_container_t* _ctc);
static void tx_releaseLutTexContainer(tx_lut_container_t* _ctc);
static void tx_releaseCollectionContainer(tx_collection_container_t* _ctc);
static void tx_linkList(tex_list_t* newlist, GLuint _texture);
static void tx_copyTexImage(tx_tex_container_base_t* _tcbase, tx_texture_t* _tex, GLint _numLevel, GLenum _internalformat, GLint _x, GLint _y, GLsizei _width, GLsizei _height, GLenum _transtype);
static void tx_copyTexSubImage(tx_tex_container_base_t* _tcbase, tx_texture_t* _tex, GLint _xoffset, GLint _yoffset, GLint _x, GLint _y, GLsizei _width, GLsizei _height);
static void tx_texImage(tx_tex_container_base_t* _tcbase, tx_texture_t* _tex, GLint _numLevel, GLsizei _width, GLsizei _height, GLenum _format, GLenum _type, const GLvoid *_pixels, GLint _size, GLboolean isNative, GLenum _transtype);
static void tx_getParameteri(tx_tex_container_base_t* _tc, GLenum _pname, GLint* _params);
static void tx_getParameterf(tx_tex_container_base_t* _tc, GLenum _pname, GLfloat* _params);
static GLenum tx_validate2DTextureWUnit(tx_tex_container_t* _tc, GLuint _unit);
static GLenum tx_validate2DTexture(tx_tex_container_t* _tc);
static GLenum tx_validateCubeTextureWUnit(tx_cube_tex_container_t* _ctc, GLuint _unit);
static GLenum tx_validateCubeTexture(tx_cube_tex_container_t* _ctc);
static void tx_getActualFormat(GLenum _format, GLenum* _actFormat, GLboolean* _isNative);
static void tx_blockConvCopy(const GLuint* _sdata, GLuint* _ddata, enum TEXTURE_FORMAT _residentType, GLsizei _xoffs, GLsizei _yoffs,
	GLsizei _swidth, GLsizei _sheight, GLsizei _dwidth, GLsizei _dheight, GLint _scale, GLboolean _bBlockSrc);
static void tx_generateMipmap(srf_container_t* _surface);

#define __GET_TEX_LIST(texture, texlist)	\
{	\
	for (texlist = __texman->tex_list_table[texture & TX_LIST_TABLE_SIZE]; texlist != 0; texlist = texlist->next)	\
	{	\
		if (texlist->id == texture)	\
			break;	\
	}	\
}

GLint __tx_initializeTexManager(tex_manager_t* texman){
	GLint ret = -1;

	__texman = texman;
	{
		memset(__texman, 0, sizeof(tex_manager_t));
		__texman->defaultTexContainer = tx_createTexContainer(0);
		__texman->defaultCubeTexContainer = tx_createCubeTexContainer(0);
		__texman->defaultCollectionContainer = tx_createCollectionContainer(0);
		if (!__texman->defaultTexContainer || !__texman->defaultCubeTexContainer || !__texman->defaultCollectionContainer){
			if (__texman->defaultTexContainer)
				tx_releaseTexContainer(__texman->defaultTexContainer);
			if (__texman->defaultCubeTexContainer)
				tx_releaseCubeTexContainer(__texman->defaultCubeTexContainer);
			if (__texman->defaultCollectionContainer)
				tx_releaseCollectionContainer(__texman->defaultCollectionContainer);
			free(__texman);
		}
		else
			ret = 0;
		__miniID = 1;

		memset(__texman->defaultCollectionContainer, 0, sizeof(tx_collection_container_t));
	}

	return ret;
}

void __tx_finalizeTexManager(void){
	if (__texman){
		int i, j;
		GET_CURRENT_STATE(state);
		
		for (i = 0; i < TX_LIST_TABLE_SIZE + 1; i++){
			tex_list_t* texlist;
			tex_list_t* next;
			for (texlist = __texman->tex_list_table[i]; texlist != 0; texlist = next){
				next = texlist->next;
				if (texlist->container){
					switch (texlist->type){
						case TT_2D:
							tx_releaseTexContainer((tx_tex_container_t*)texlist->container);
							texlist->container = 0;
							break;
						case TT_CUBE:
							tx_releaseCubeTexContainer((tx_cube_tex_container_t*)texlist->container);
							texlist->container = 0;
							break;
						case TT_TEX_COLL:
							tx_releaseCollectionContainer((tx_collection_container_t*)texlist->container);
							texlist->container = 0;
							break;
						case TT_LUT:
							tx_releaseLutTexContainer((tx_lut_container_t*)texlist->container);
							texlist->container = 0;
							break;
					}
				}
				free(texlist);
			}
		}
		for (j = 0; j < MAX_TEXTURE_UNITS; j++){
			state->gl_state.textureunits.bound2DTex[j] = 0;
			state->gl_state.textureunits.boundCubeTex[j] = 0;
		}
		for (j = 0; j < MAX_LUT_TEXTURES; j++)
			state->gl_state.textureunits.boundLutTex[j] = 0;
		state->gl_state.textureunits.boundTexColl = 0;
		tx_releaseTexContainer(__texman->defaultTexContainer);
		tx_releaseCubeTexContainer(__texman->defaultCubeTexContainer);
		tx_releaseCollectionContainer(__texman->defaultCollectionContainer);
		__texman = 0;
	}

	return;
}

tx_lut_container_t* __tx_getBoundTextureLut(GLuint _lut){
	if (_lut < MAX_LUT_TEXTURES && __texman->boundLutTex[_lut])
		return (tx_lut_container_t*)__texman->boundLutTex[_lut]->container;
	return 0;
}

static void tx_linkList(tex_list_t* newlist, GLuint _texture){
	tex_list_t *texlist, *tmp;

	if (__texman->tex_list_table[_texture & TX_LIST_TABLE_SIZE] == 0)
		__texman->tex_list_table[_texture & TX_LIST_TABLE_SIZE] = newlist;
	else{
		if (__texman->tex_list_table[_texture & TX_LIST_TABLE_SIZE]->id > _texture){
			newlist->next = __texman->tex_list_table[_texture & TX_LIST_TABLE_SIZE];
			__texman->tex_list_table[_texture & TX_LIST_TABLE_SIZE] = newlist;
		}
		else{
			for (texlist = __texman->tex_list_table[_texture & TX_LIST_TABLE_SIZE]->next, tmp = __texman->tex_list_table[_texture & TX_LIST_TABLE_SIZE]; texlist != 0; texlist = texlist->next){
				if (texlist->id > _texture){
					/* insert */
					tmp->next = newlist;
					newlist->next = texlist;
					break;
				}
				tmp = texlist;
			}
			if (texlist == 0)
				tmp->next = newlist;
		}
	}

	return;
}

void GL_APIENTRY glActiveTexture(GLenum texture){
	GET_CURRENT_STATE(state);
	
	BASE_GL_FAIL_IF(texture < GL_TEXTURE0 || texture > GL_TEXTURE0 + MAX_TEXTURE_UNITS - 1, GL_INVALID_ENUM);
	
	state->gl_state.textureunits.activeTexture = texture - GL_TEXTURE0;
	
	return;
}

void GL_APIENTRY glBindTexture(GLenum _target, GLuint _texture)
{
	tex_list_t *texlist = 0;
	GET_CURRENT_STATE(state);

	switch (_target){
		case GL_TEXTURE_2D:
			if (_texture == state->gl_state.textureunits.bound2DTex[state->gl_state.textureunits.activeTexture])
				return;
			else if (_texture){
				__GET_TEX_LIST(_texture, texlist)
				if (texlist && texlist->id != _texture)
					texlist = 0;
				BASE_GL_FAIL_IF(texlist && texlist->type != TT_2D && texlist->container != 0, GL_INVALID_OPERATION);
				if (texlist == 0){
					texlist = (tex_list_t*)malloc(sizeof(tex_list_t));
					BASE_GL_FAIL_IF(!texlist, GL_OUT_OF_MEMORY);
					texlist->container = (void*)tx_createTexContainer(_texture);
					#ifdef DMPGL_GEN_ERROR
					if (!texlist->container){
						free(texlist);
						__err_setError(GL_OUT_OF_MEMORY);
						return;
					}
					#endif

					texlist->type = TT_2D;
					texlist->id = _texture;
					texlist->next = 0;
					tx_linkList(texlist, _texture);
				}
				else if (texlist->container == 0){
					texlist->container = (void*)tx_createTexContainer(_texture);
					BASE_GL_FAIL_IF(!texlist->container, GL_OUT_OF_MEMORY);
					texlist->type = TT_2D;
				}
			}
			if (__texman->boundTexColl)
				((tx_collection_container_t*)__texman->boundTexColl->container)->tex2d[state->gl_state.textureunits.activeTexture] = _texture;
			else
				__texman->defaultCollectionContainer->tex2d[state->gl_state.textureunits.activeTexture] = _texture;
			state->gl_state.textureunits.bound2DTex[state->gl_state.textureunits.activeTexture] = _texture;
			__texman->bound2DTex[state->gl_state.textureunits.activeTexture] = texlist;
			SET_MASK(state->upd_mask, (IF_GL_TEXTURE0 + state->gl_state.textureunits.activeTexture));
			break;
		case GL_TEXTURE_CUBE_MAP:
			if (_texture == state->gl_state.textureunits.boundCubeTex[state->gl_state.textureunits.activeTexture])
				return;
			else if (_texture){
				__GET_TEX_LIST(_texture, texlist)
				if (texlist && texlist->id != _texture)
					texlist = 0;
				BASE_GL_FAIL_IF(texlist && texlist->type != TT_CUBE && texlist->container != 0, GL_INVALID_OPERATION);
				if (texlist == 0){
					texlist = (tex_list_t*)malloc(sizeof(tex_list_t));
					BASE_GL_FAIL_IF(!texlist, GL_OUT_OF_MEMORY);
					texlist->container = (void*)tx_createCubeTexContainer(_texture);

					texlist->type = TT_CUBE;
					texlist->id = _texture;
					texlist->next = 0;
					tx_linkList(texlist, _texture);
				}
				else if (texlist->container == 0){
					texlist->container = (void*)tx_createCubeTexContainer(_texture);
					BASE_GL_FAIL_IF(!texlist->container, GL_OUT_OF_MEMORY);
					texlist->type = TT_CUBE;
				}
			}
			if (__texman->boundTexColl)
				((tx_collection_container_t*)__texman->boundTexColl->container)->texcube[state->gl_state.textureunits.activeTexture] = _texture;
			else
				__texman->defaultCollectionContainer->texcube[state->gl_state.textureunits.activeTexture] = _texture;
			state->gl_state.textureunits.boundCubeTex[state->gl_state.textureunits.activeTexture] = _texture;
			__texman->boundCubeTex[state->gl_state.textureunits.activeTexture] = texlist;
			SET_MASK(state->upd_mask, (IF_GL_TEXTURE0 + state->gl_state.textureunits.activeTexture));
			break;
		case GL_TEXTURE_COLLECTION_DMP:
			if (_texture == state->gl_state.textureunits.boundTexColl)
				return;
			else if (_texture){
				__GET_TEX_LIST(_texture, texlist)
				if (texlist && texlist->id != _texture)
					texlist = 0;
				BASE_GL_FAIL_IF(texlist && texlist->type != TT_TEX_COLL && texlist->container != 0, GL_INVALID_OPERATION);
				if (texlist == 0){
					texlist = (tex_list_t*)malloc(sizeof(tex_list_t));
					BASE_GL_FAIL_IF(!texlist, GL_OUT_OF_MEMORY);
					texlist->container = (void*)tx_createCollectionContainer(_texture);

					texlist->type = TT_TEX_COLL;
					texlist->id = _texture;
					texlist->next = 0;
					tx_linkList(texlist, _texture);
				}
				else if (texlist->container == 0){
					texlist->container = (void*)tx_createCollectionContainer(_texture);
					BASE_GL_FAIL_IF(!texlist->container, GL_OUT_OF_MEMORY);
					texlist->type = TT_TEX_COLL;
				}
			}
			{
				int i;
				tex_list_t* boundlist;
				int updateLut = 0;
				for (i = 0; i < MAX_LUT_TEXTURES; i++){
					GLuint newboundlut = _texture ? ((tx_collection_container_t*)texlist->container)->luts[i] : __texman->defaultCollectionContainer->luts[i];
					if (state->gl_state.textureunits.boundLutTex[i] == newboundlut)
						continue;
					
					state->gl_state.textureunits.boundLutTex[i] = newboundlut;
					if (newboundlut)
					{
						__GET_TEX_LIST(newboundlut, boundlist)
						__texman->boundLutTex[i] = boundlist;
					}
					else
						__texman->boundLutTex[i] = 0;
					updateLut = 1;
				}
				if (updateLut)
					SET_MASK(state->upd_mask, IF_GL_TEXTURE_LUT);
				
				for (i = 0; i < MAX_TEXTURE_UNITS; i++){
					GLuint newbound2D, newboundCube;
					if (_texture){
						newbound2D = ((tx_collection_container_t*)texlist->container)->tex2d[i];
						newboundCube = ((tx_collection_container_t*)texlist->container)->texcube[i];
					}
					else{
						newbound2D = __texman->defaultCollectionContainer->tex2d[i];
						newboundCube = __texman->defaultCollectionContainer->texcube[i];
					}
					
					if (state->gl_state.textureunits.bound2DTex[i] == newbound2D
					 && state->gl_state.textureunits.boundCubeTex[i] == newboundCube)
						continue;
					
					state->gl_state.textureunits.bound2DTex[i] = newbound2D;
					if (newbound2D)
					{
						__GET_TEX_LIST(newbound2D, boundlist)
						__texman->bound2DTex[i] = boundlist;
					}
					else
						__texman->bound2DTex[i] = 0;
					
					state->gl_state.textureunits.boundCubeTex[i] = newboundCube;
					if (newboundCube){
						__GET_TEX_LIST(newboundCube, boundlist)
						__texman->boundCubeTex[i] = boundlist;
					}
					else
						__texman->boundCubeTex[i] = 0;
					SET_MASK(state->upd_mask, IF_GL_TEXTURE0 + i);
				}
			}
			state->gl_state.textureunits.boundTexColl = _texture;
			__texman->boundTexColl = texlist;
			if (__texman->deleteCollection){
				GLuint del = __texman->deleteCollection;
				glDeleteTextures(1, &del);
				__texman->deleteCollection = 0;
			}
			break;
		case GL_LUT_TEXTURE0_DMP:
		case GL_LUT_TEXTURE1_DMP:
		case GL_LUT_TEXTURE2_DMP:
		case GL_LUT_TEXTURE3_DMP:
		case GL_LUT_TEXTURE4_DMP:
		case GL_LUT_TEXTURE5_DMP:
		case GL_LUT_TEXTURE6_DMP:
		case GL_LUT_TEXTURE7_DMP:
		case GL_LUT_TEXTURE8_DMP:
		case GL_LUT_TEXTURE9_DMP:
		case GL_LUT_TEXTURE10_DMP:
		case GL_LUT_TEXTURE11_DMP:
		case GL_LUT_TEXTURE12_DMP:
		case GL_LUT_TEXTURE13_DMP:
		case GL_LUT_TEXTURE14_DMP:
		case GL_LUT_TEXTURE15_DMP:
		case GL_LUT_TEXTURE16_DMP:
		case GL_LUT_TEXTURE17_DMP:
		case GL_LUT_TEXTURE18_DMP:
		case GL_LUT_TEXTURE19_DMP:
		case GL_LUT_TEXTURE20_DMP:
		case GL_LUT_TEXTURE21_DMP:
		case GL_LUT_TEXTURE22_DMP:
		case GL_LUT_TEXTURE23_DMP:
		case GL_LUT_TEXTURE24_DMP:
		case GL_LUT_TEXTURE25_DMP:
		case GL_LUT_TEXTURE26_DMP:
		case GL_LUT_TEXTURE27_DMP:
		case GL_LUT_TEXTURE28_DMP:
		case GL_LUT_TEXTURE29_DMP:
		case GL_LUT_TEXTURE30_DMP:
		case GL_LUT_TEXTURE31_DMP:
			if (_texture == state->gl_state.textureunits.boundLutTex[_target - GL_LUT_TEXTURE0_DMP])
				return;
			else if (_texture){
				__GET_TEX_LIST(_texture, texlist)
				if (texlist && texlist->id != _texture)
					texlist = 0;
				BASE_GL_FAIL_IF(texlist && texlist->type != TT_LUT && texlist->container != 0, GL_INVALID_OPERATION);
				if (texlist == 0){
					texlist = (tex_list_t*)malloc(sizeof(tex_list_t));
					BASE_GL_FAIL_IF(!texlist, GL_OUT_OF_MEMORY);
					texlist->container = (void*)tx_createLutContainer(_texture);

					texlist->type = TT_LUT;
					texlist->id = _texture;
					texlist->next = 0;
					tx_linkList(texlist, _texture);
				}
				else if (texlist->container == 0){
					texlist->container = (void*)tx_createLutContainer(_texture);
					BASE_GL_FAIL_IF(!texlist->container, GL_OUT_OF_MEMORY);
					texlist->type = TT_LUT;
				}
			}
			if (__texman->boundTexColl)
				((tx_collection_container_t*)__texman->boundTexColl->container)->luts[_target - GL_LUT_TEXTURE0_DMP] = _texture;
			else
				__texman->defaultCollectionContainer->luts[_target - GL_LUT_TEXTURE0_DMP] = _texture;

			state->gl_state.textureunits.boundLutTex[_target - GL_LUT_TEXTURE0_DMP] = _texture;
			__texman->boundLutTex[_target - GL_LUT_TEXTURE0_DMP] = texlist;
			SET_MASK(state->upd_mask, IF_GL_TEXTURE_LUT);
			break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}

	return;
}

static tx_tex_container_t* tx_createTexContainer(GLuint _id){
	tx_tex_container_t* tc;

	tc = (tx_tex_container_t*)malloc(sizeof(tx_tex_container_t));
	if (tc){
		memset(tc, 0, sizeof(tx_tex_container_t));
		tc->container.minFilter = GL_NEAREST;
		tc->container.magFilter = GL_NEAREST;
		tc->container.wrapS = GL_REPEAT;
		tc->container.wrapT = GL_REPEAT;
		tc->container.BorderColor[0] = tc->container.BorderColor[1] = tc->container.BorderColor[2] = tc->container.BorderColor[3] = 0;
		tc->container.lodBias = 0.f;
		tc->container.transType = 0;
		tc->container.minLOD = -1000;
		tc->texture.id = _id;
	}

	return tc;
}

static tx_cube_tex_container_t* tx_createCubeTexContainer(GLuint _id){
	int i;
	tx_cube_tex_container_t* ctc;

	ctc = (tx_cube_tex_container_t*)malloc(sizeof(tx_cube_tex_container_t));
	if (ctc){
		memset(ctc, 0, sizeof(tx_cube_tex_container_t));
		ctc->container.minFilter = GL_NEAREST;
		ctc->container.magFilter = GL_NEAREST;
		ctc->container.wrapS = GL_REPEAT;
		ctc->container.wrapT = GL_REPEAT;
		ctc->container.lodBias = 0.f;
		ctc->container.BorderColor[0] = ctc->container.BorderColor[1] = ctc->container.BorderColor[2] = ctc->container.BorderColor[3] = 0;
		ctc->container.transType = 0;
		ctc->container.minLOD = -1000;

		for (i = 0; i < 6; i++)
			ctc->texture[i].id = _id;
	}

	return ctc;
}

static tx_collection_container_t* tx_createCollectionContainer(GLuint _id){
	tx_collection_container_t* tc;

	tc = (tx_collection_container_t*)malloc(sizeof(tx_collection_container_t));
	if (tc){
		memset(tc, 0, sizeof (tx_collection_container_t));
		tc->id = _id;
	}

	return tc;
}

static tx_lut_container_t* tx_createLutContainer(GLuint _id){
	tx_lut_container_t* tc;

	tc = (tx_lut_container_t*)malloc(sizeof(tx_lut_container_t));
	if (tc){
		memset(tc, 0, sizeof (tx_lut_container_t));
		tc->id = _id;
		tc->updated_flags = 0xffffffff;
	}

	return tc;
}

static GLboolean tx_allocTextureMemory(GLenum _transtype, tx_texture_t* _tex){
	GLboolean ret = GL_TRUE;
	
	switch (_transtype){
		case GL_NO_COPY_NO_DMA_DMP:
			break;
		case GL_NO_COPY_DMA_VRAMA_DMP:
		case GL_NO_COPY_DMA_VRAMB_DMP:
			_tex->surfaceContainer.allocarea = (_transtype == GL_NO_COPY_DMA_VRAMA_DMP) ? NN_GX_MEM_VRAMA : NN_GX_MEM_VRAMB;
			_tex->surfaceContainer.picaaddr = malloc_ext(_tex->surfaceContainer.allocarea, NN_GX_MEM_TEXTURE, _tex->id, _tex->surfaceContainer.size);
			if (!_tex->surfaceContainer.picaaddr)
				ret = GL_FALSE;
			break;
		case GL_COPY_NO_DMA_DMP:
			_tex->surfaceContainer.copyaddr = malloc_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_TEXTURE, _tex->id, _tex->surfaceContainer.size);
			if (!_tex->surfaceContainer.copyaddr)
				ret = GL_FALSE;
			_tex->surfaceContainer.picaaddr = _tex->surfaceContainer.copyaddr;	/* pica access address is copy address */
			break;
		case GL_COPY_DMA_VRAMA_DMP:
		case GL_COPY_DMA_VRAMB_DMP:
			_tex->surfaceContainer.allocarea = (_transtype == GL_COPY_DMA_VRAMA_DMP) ? NN_GX_MEM_VRAMA : NN_GX_MEM_VRAMB;
			_tex->surfaceContainer.picaaddr = malloc_ext(_tex->surfaceContainer.allocarea, NN_GX_MEM_TEXTURE, _tex->id, _tex->surfaceContainer.size);
			_tex->surfaceContainer.copyaddr = malloc_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_TEXTURE, _tex->id, _tex->surfaceContainer.size);
			if (!_tex->surfaceContainer.picaaddr || !_tex->surfaceContainer.copyaddr){
				ret = GL_FALSE;
				if (_tex->surfaceContainer.picaaddr)
					free_ext(_tex->surfaceContainer.allocarea, NN_GX_MEM_TEXTURE, _tex->id, _tex->surfaceContainer.picaaddr);
				if (_tex->surfaceContainer.copyaddr)
					free_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_TEXTURE, _tex->id, _tex->surfaceContainer.copyaddr);
			}
			break;
		default:
			break;
	}
	
	return ret;
}

static void tx_freeTextureMemory(GLenum _transtype, tx_texture_t* _tex){
	switch (_transtype){
		case GL_NO_COPY_NO_DMA_DMP:
			break;
		case GL_NO_COPY_DMA_VRAMA_DMP:
		case GL_NO_COPY_DMA_VRAMB_DMP:
			if (_tex->surfaceContainer.picaaddr)
				free_ext(_tex->surfaceContainer.allocarea, NN_GX_MEM_TEXTURE, _tex->id, _tex->surfaceContainer.picaaddr);
			break;
		case GL_COPY_NO_DMA_DMP:
			if (_tex->surfaceContainer.copyaddr)
				free_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_TEXTURE, _tex->id, _tex->surfaceContainer.copyaddr);
			break;
		case GL_COPY_DMA_VRAMA_DMP:
		case GL_COPY_DMA_VRAMB_DMP:
			if (_tex->surfaceContainer.picaaddr)
				free_ext(_tex->surfaceContainer.allocarea, NN_GX_MEM_TEXTURE, _tex->id, _tex->surfaceContainer.picaaddr);
			if (_tex->surfaceContainer.copyaddr)
				free_ext(NN_GX_MEM_FCRAM, NN_GX_MEM_TEXTURE, _tex->id, _tex->surfaceContainer.copyaddr);
			break;
		default:
			break;
	}
	_tex->surfaceContainer.picaaddr = 0;
	_tex->surfaceContainer.copyaddr = 0;
	_tex->surfaceContainer.appaddr = 0;
	
	return;
}

void GL_APIENTRY glDeleteTextures(GLsizei _n, const GLuint *_textures){
	int i, j;
	tex_list_t*	texlist;
	tex_list_t*	tmp;
	GET_CURRENT_STATE(state);

	BASE_GL_FAIL_IF(_n < 0, GL_INVALID_VALUE);
	
	for (i = 0; i < _n; i++){
		if (!_textures[i]) continue;

		tmp = 0;
		for (texlist = __texman->tex_list_table[_textures[i] & TX_LIST_TABLE_SIZE]; texlist != 0; texlist = texlist->next){
			if (texlist->id >= _textures[i])
				break;
			tmp = texlist;
		}
		if (texlist == 0 || texlist->id != _textures[i])
			continue;

		if (texlist->container){
			switch (texlist->type){
				case TT_2D:
					for (j = 0; j < MAX_TEXTURE_UNITS; j++)
						if (state->gl_state.textureunits.bound2DTex[j] == _textures[i]){
							state->gl_state.textureunits.bound2DTex[j] = 0;
							if (__texman->boundTexColl)
								((tx_collection_container_t*)__texman->boundTexColl->container)->tex2d[j] = 0;
							else
								__texman->defaultCollectionContainer->tex2d[j] = 0;
							__texman->bound2DTex[j] = 0;
							if (state->gl_state.textureunits.enabled2DTex[j])
								SET_MASK(state->upd_mask, IF_GL_TEXTURE0+j);
						}

					tx_releaseTexContainer((tx_tex_container_t*)texlist->container);
					texlist->container = 0;
					break;
				case TT_CUBE:
					for (j = 0; j < MAX_TEXTURE_UNITS; j++)
						if (state->gl_state.textureunits.boundCubeTex[j] == _textures[i]){
							state->gl_state.textureunits.boundCubeTex[j] = 0;
							if (__texman->boundTexColl)
								((tx_collection_container_t*)__texman->boundTexColl->container)->texcube[j] = 0;
							else
								__texman->defaultCollectionContainer->texcube[j] = 0;
							__texman->boundCubeTex[j] = 0;
							if (state->gl_state.textureunits.enabledCubeTex[j])
								SET_MASK(state->upd_mask, IF_GL_TEXTURE0 + j);
						}

					tx_releaseCubeTexContainer((tx_cube_tex_container_t*)texlist->container);
					texlist->container = 0;
					break;
				case TT_TEX_COLL:
					if (state->gl_state.textureunits.boundTexColl == _textures[i]){
						__texman->deleteCollection = _textures[i];
						continue;
					}
					tx_releaseCollectionContainer((tx_collection_container_t*)texlist->container);
					texlist->container = 0;
					break;
				case TT_LUT:
					for (j = 0; j < MAX_LUT_TEXTURES; j++){
						if (state->gl_state.textureunits.boundLutTex[j] == _textures[i]){
							state->gl_state.textureunits.boundLutTex[j] = 0;
							if (__texman->boundTexColl)
								((tx_collection_container_t*)__texman->boundTexColl->container)->luts[j] = 0;
							else
								__texman->defaultCollectionContainer->luts[j] = 0;
							__texman->boundLutTex[j] = 0;
						}
					}
					for (j = 0; j < LAP_LUT_COUNT; j++){
						if (state->gl_state.lutbindings.bindings[j] == _textures[i]){
							state->gl_state.lutbindings.bindings[j] = 0;
							state->gl_state.lutbindings.subsize[j] = 0;
							SET_MASK(state->upd_mask, IF_GL_TEXTURE_LUT);
						}
					}
					tx_releaseLutTexContainer((tx_lut_container_t*)texlist->container);
					texlist->container = 0;
					break;
			}
		}
		if (_textures[i] < __miniID)
			__miniID = _textures[i];

		if (tmp == 0)
			__texman->tex_list_table[_textures[i] & TX_LIST_TABLE_SIZE] = __texman->tex_list_table[_textures[i] & TX_LIST_TABLE_SIZE]->next;
		else
			tmp->next = texlist->next;
		free(texlist);
	}

	return;
}

static void tx_releaseTexContainer(tx_tex_container_t* _tc){
	__fb_detachSurfaceContainer(&_tc->texture.surfaceContainer);
	tx_freeTextureMemory(_tc->container.transType, &_tc->texture);
	free(_tc);
}

static void tx_releaseCubeTexContainer(tx_cube_tex_container_t* _ctc){
	int i;

	for (i = 0; i < 6; i++){
		__fb_detachSurfaceContainer(&_ctc->texture[i].surfaceContainer);
		tx_freeTextureMemory(_ctc->container.transType, &_ctc->texture[i]);
	}
	free(_ctc);

	return;
}

static void tx_releaseCollectionContainer(tx_collection_container_t* _tc){
	free(_tc);
}

static void tx_releaseLutTexContainer(tx_lut_container_t* _tc){
	if (_tc->lr_plane)
		free(_tc->lr_plane);
	if (_tc->fog_plane)
		free(_tc->fog_plane);
	if (_tc->gas_plane)
		free(_tc->gas_plane);
	if (_tc->pt_map_plane)
		free(_tc->pt_map_plane);
	if (_tc->pt_noise_plane)
		free(_tc->pt_noise_plane);
	if (_tc->pt_rgba_plane)
		free(_tc->pt_rgba_plane);

	free(_tc);
}

static void tx_blockConvCopy(const GLuint* _sdata, GLuint* _ddata, enum TEXTURE_FORMAT _residentType, 
	GLsizei _xoffs, GLsizei _yoffs,GLsizei _swidth, GLsizei _sheight, GLsizei _dwidth, GLsizei _dheight, GLint _scale, GLboolean _bBlockSrc){

	unsigned _pixelSize  = 4;
	unsigned _bit4 = 0;
	GLint    _bYflip = !_bBlockSrc;

	switch (_residentType){
		case TF_R8G8B8A8: _pixelSize=4; break;
		case TF_R8G8B8  : _pixelSize=3; break;
		case TF_L8A8    : _pixelSize=2; break;
		case TF_R8G8    : _pixelSize=2; break;
		case TF_R5G6B5  : _pixelSize=2; break;
		case TF_R5G5B5A1: _pixelSize=2; break;
		case TF_R4G4B4A4: _pixelSize=2; break;
		case TF_A8      : _pixelSize=1; break;
		case TF_L8      : _pixelSize=1; break;
		case TF_L4A4    : _pixelSize=1; break;
		case TF_L4      : _pixelSize=1; _bit4 = 1; break;
		case TF_A4      : _pixelSize=1; _bit4 = 1; break;
		case TF_ETC     : _pixelSize=8; break;
		case TF_ETCA4   : _pixelSize=16; break;
	}

	if (_residentType == TF_ETC || _residentType == TF_ETCA4){
		unsigned _wSrc = _swidth  / 4;
		unsigned _hSrc = _sheight / 4;
		unsigned _wDst = _dwidth  / 4;
		unsigned _hDst = _dheight / 4;
		unsigned _x, _y;
		unsigned _xOffset = _xoffs / 4;
		unsigned _yOffset = _hDst - (_yoffs / 4) - _hSrc;

		for (_y = 0; _y < _hSrc; _y++){
			unsigned _yd = _y + _yOffset;
			for (_x = 0; _x < _wSrc; _x++){
				unsigned _xd = _x + _xOffset;
				unsigned _offset = ((_xd / 2) + (_yd / 2) * _wDst / 2) * 2 * 2;
				unsigned _locate;
				
				_offset += ((_xd & 1) + (_yd & 1) * 2);
				if (_bBlockSrc){
					_locate  = ((_x / 2) + (_y / 2) * _wSrc / 2) * 2 * 2;
					_locate += ((_x & 1) + (_y & 1) * 2);
				}
				else{
					_locate  = _x + _y * _wSrc;
				}

				if (_offset >= _wDst * _hDst) continue;
				if (_locate >= _wSrc * _hSrc) continue;

				if (_residentType == TF_ETC){
					_ddata[_offset * 2]     = _sdata[_locate * 2];
					_ddata[_offset * 2 + 1] = _sdata[_locate * 2 + 1];
				}
				else{
					_ddata[_offset * 4    ] = _sdata[_locate * 4    ];
					_ddata[_offset * 4 + 1] = _sdata[_locate * 4 + 1];
					_ddata[_offset * 4 + 2] = _sdata[_locate * 4 + 2];
					_ddata[_offset * 4 + 3] = _sdata[_locate * 4 + 3];
				}
			}
		}
	}
	else{
		unsigned _wSrc = _swidth  << _scale;
		unsigned _hSrc = _sheight << _scale;
		unsigned _wDst = _dwidth  << _scale;
		unsigned _hDst = _dheight << _scale;
		unsigned _yOffset = _hDst - (_yoffs << _scale) - _hSrc;
		unsigned _xOffset = _xoffs << _scale;
		unsigned _x, _y;
		int i, _j, j;
		GLubyte* sourceData;
		
		if (!_bBlockSrc){
			int size = _swidth * _sheight * _pixelSize;
			if (_bit4)
				size /= 2;
			sourceData = (GLubyte*)malloc(size);

			switch (_residentType){
				case TF_R8G8B8A8:
					#define __SWAP4(x)			((((x) >> 24) & 0xff) | (((x) >> 8) & 0xff00) | (((x) << 8) & 0xff0000) | (((x) << 24) & 0xff000000))
					for (i = 0; i < _swidth * _sheight; i++)
						((GLuint*)sourceData)[i] = __SWAP4(((GLuint*)_sdata)[i]);
					break;
					#undef __SWAP4
				case TF_R8G8B8:
					for (i = 0; i < _swidth * _sheight; i++)
					{
						int p = i * 3;
						sourceData[p + 0] = ((GLubyte*)_sdata)[p + 2];
						sourceData[p + 1] = ((GLubyte*)_sdata)[p + 1];
						sourceData[p + 2] = ((GLubyte*)_sdata)[p + 0];
					}
					break;
				case TF_L8A8:
				case TF_R8G8:
					#define __SWAP2(x)			((((x) >> 8) & 0xff) | (((x) << 8) & 0xff00))
					for (i = 0; i < _swidth * _sheight; i++)
						((GLushort*)sourceData)[i] = __SWAP2(((GLushort*)_sdata)[i]);
					break;
					#undef __SWAP2
				case TF_R5G6B5:
				case TF_R5G5B5A1:
				case TF_R4G4B4A4:
					for (i = 0; i < _swidth * _sheight; i++)
						((GLushort*)sourceData)[i] = ((GLushort*)_sdata)[i];
					break;
				case TF_A8:
				case TF_L8:
				case TF_L4A4:
					for (i = 0; i < _swidth * _sheight; i++)
						((GLubyte*)sourceData)[i] = ((GLubyte*)_sdata)[i];
					break;
				case TF_L4:
				case TF_A4:
					for (i = 0; i < _swidth * _sheight / 2; i++)
						((GLubyte*)sourceData)[i] = ((GLubyte*)_sdata)[i];
					break;
			}
		}
		else
			sourceData = (GLubyte*)_sdata;

		for (_y = 0; _y < _hSrc; _y++){
			unsigned _yd = _y + _yOffset;
			unsigned ys = _y * _sheight / _hSrc;
			
			if (_bYflip)
				_yd = (_hSrc -_y - 1) + _yOffset;

			for (_x = 0; _x < _wSrc; _x++){
				unsigned _xd = _x + _xOffset;
				unsigned _offset = ((_xd / 8) + (_yd / 8) * _wDst / 8) * 8 * 8;
				unsigned xs = _x * _swidth / _wSrc;
				unsigned locate;

				for(i = 0, _j; (_j = (1 << i)) < 8; i++){
					_offset += ((_xd & _j) + (_yd & _j) * 2) * _j;
				}

				if (_bBlockSrc){
					locate = ((xs / 8) + (ys / 8) * _swidth / 8) * 8 * 8;
					for(i = 0, j; (j = (1 << i)) < 8; i++){
						locate += ((xs & j) + (ys & j) * 2) * j;
					}
				}
				else{
					locate = xs + ys * _swidth;
				}

				if (_offset >= _wDst * _hDst) continue;
				if (locate >= (unsigned)(_swidth *  _sheight)) continue;

				if (_bit4){
					GLubyte *dest, *src;
					dest = &((GLubyte*)_ddata)[_offset*_pixelSize / 2];
					src = &sourceData[locate * _pixelSize / 2];
					if (_x & 1)
						*dest = (*dest & 0x0f) | (*src & 0xf0);
					else
						*dest = (*dest & 0xf0) | (*src & 0x0f);
				}
				else{
					GLubyte *dest, *src;
					dest = &((GLubyte*)_ddata)[_offset * _pixelSize];
					src = &sourceData[locate * _pixelSize];
					for (i = 0; i < (int)_pixelSize; i++)
						*dest++ = *src++;
				}
			}
		}
		
		if (!_bBlockSrc)
			free(sourceData);
	}
	
	return;
}

void GL_APIENTRY glCompressedTexImage2D(GLenum _target, GLint _level, GLenum _internalformat,
		GLsizei _width, GLsizei _height, GLint _border, GLsizei _imageSize, const GLvoid *_data)
{
	tx_tex_container_t*			pTex2D;
	tx_cube_tex_container_t*	pTexCube;
	GLenum						transtype;
	GLenum						actFormat;
	GLboolean					isNative;

	GLint						numMaxMipmap;

	GET_CURRENT_STATE(state);

	BASE_GL_FAIL_IF(_imageSize <= 0, GL_INVALID_OPERATION);
	BASE_GL_FAIL_IF(_border || _level > 0 || _level < -numMaxMipmap, GL_INVALID_VALUE);
	
	tx_getActualFormat(_internalformat, &actFormat, &isNative);
	transtype = _target & 0xffff0000;
	_target = _target & 0xffff;

	if ((transtype & 0xff000000) == 0)
		transtype |= GL_COPY_FCRAM_DMP;
	if ((transtype & 0x00ff0000) == 0)
		transtype |= NN_GX_MEM_FCRAM;

	switch (_target){
		case GL_TEXTURE_2D:
			if (!state->gl_state.textureunits.bound2DTex[state->gl_state.textureunits.activeTexture])
				pTex2D = __texman->defaultTexContainer;
			else
				pTex2D = (tx_tex_container_t*)__texman->bound2DTex[state->gl_state.textureunits.activeTexture]->container;
			tx_texImage(&pTex2D->container, &pTex2D->texture, (_level) ? -_level : 1,
							_width, _height, actFormat, GL_UNSIGNED_BYTE, _data, _imageSize, isNative, transtype);
			break;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X: case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y: case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			BASE_GL_FAIL_IF(0 != state->gl_state.textureunits.activeTexture, GL_INVALID_OPERATION);
			BASE_GL_FAIL_IF(_width != _height, GL_INVALID_VALUE);

			if (!state->gl_state.textureunits.boundCubeTex[state->gl_state.textureunits.activeTexture])
				pTexCube = __texman->defaultCubeTexContainer;
			else
				pTexCube = (tx_cube_tex_container_t*)__texman->boundCubeTex[state->gl_state.textureunits.activeTexture]->container;
			tx_texImage(&pTexCube->container, &pTexCube->texture[_target - GL_TEXTURE_CUBE_MAP_POSITIVE_X],
					(_level) ? -_level : 1, _width, _height, actFormat, GL_UNSIGNED_BYTE, _data, _imageSize, isNative, transtype);
			break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}

	SET_MASK(state->upd_mask, IF_GL_TEXTURE0 + state->gl_state.textureunits.activeTexture);

	return;
}

static void tx_getActualFormat(GLenum _format, GLenum* _actFormat, GLboolean* _isNative){
	*_isNative = GL_TRUE;
	switch (_format & 0xffff){
		case GL_RGBA_NATIVE_DMP:
			*_actFormat = GL_RGBA;
			break;
		case GL_RGB_NATIVE_DMP:
			*_actFormat = GL_RGB;
			break;
		case GL_ALPHA_NATIVE_DMP:
			*_actFormat = GL_ALPHA;
			break;
		case GL_LUMINANCE_NATIVE_DMP:
			*_actFormat = GL_LUMINANCE;
			break;
		case GL_LUMINANCE_ALPHA_NATIVE_DMP:
			*_actFormat = GL_LUMINANCE_ALPHA;
			break;
		case GL_HILO8_DMP_NATIVE_DMP:
			*_actFormat = GL_HILO8_DMP;
			break;
		case GL_ETC1_RGB8_NATIVE_DMP:
			*_actFormat = GL_ETC1_RGB8_NATIVE_DMP;
			break;
		case GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP:
			*_actFormat = GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP;
			break;
		case GL_SHADOW_NATIVE_DMP:
			*_actFormat = GL_SHADOW_DMP;
			break;
		case GL_GAS_NATIVE_DMP:
			*_actFormat = GL_GAS_DMP;
			break;
		default:
			*_actFormat = _format;
			*_isNative = GL_FALSE;
			break;
	}
	
	return;
}

void GL_APIENTRY glTexImage2D(GLenum _target, GLint _level, GLenum _internalformat, GLsizei _width,
	GLsizei _height, GLint _border, GLenum _format, GLenum _type, const GLvoid *_pixels){
	tx_tex_container_t*			pTex2D;
	tx_cube_tex_container_t*	pTexCube;
	GLenum						transtype;
	GLenum						actFormat;
	GLboolean					isNative;

	GET_CURRENT_STATE(state);
	
	BASE_GL_FAIL_IF(_format != _internalformat, GL_INVALID_OPERATION);
	
	tx_getActualFormat(_format, &actFormat, &isNative);
	transtype = _target & 0xffff0000;
	_target = _target & 0xffff;

	if ((transtype & 0xff000000) == 0)
		transtype |= _pixels ? GL_COPY_FCRAM_DMP : GL_NO_COPY_FCRAM_DMP;

	if ((transtype & 0x00ff0000) == 0)
		transtype |= _pixels ? NN_GX_MEM_FCRAM : NN_GX_MEM_VRAMB;


	BASE_GL_FAIL_IF((actFormat != GL_RGBA && actFormat != GL_RGBA8_OES) && (_type == GL_UNSIGNED_SHORT_5_5_5_1 || _type == GL_UNSIGNED_SHORT_4_4_4_4), GL_INVALID_OPERATION);
	BASE_GL_FAIL_IF((actFormat != GL_RGB && actFormat != GL_RGB8_OES) && _type == GL_UNSIGNED_SHORT_5_6_5, GL_INVALID_OPERATION);
	BASE_GL_FAIL_IF((actFormat == GL_RGB || actFormat == GL_RGB8_OES) && _type == GL_UNSIGNED_BYTE && _target != GL_TEXTURE_2D, GL_INVALID_OPERATION);
	BASE_GL_FAIL_IF(actFormat != GL_GAS_DMP && _type == GL_UNSIGNED_SHORT, GL_INVALID_OPERATION);
	BASE_GL_FAIL_IF(actFormat != GL_SHADOW_DMP && _type == GL_UNSIGNED_INT, GL_INVALID_OPERATION);
	BASE_GL_FAIL_IF(_type == GL_UNSIGNED_BYTE && (actFormat == GL_SHADOW_DMP || actFormat == GL_GAS_DMP), GL_INVALID_OPERATION);
	BASE_GL_FAIL_IF((actFormat != GL_ALPHA && actFormat != GL_LUMINANCE) && _type == GL_UNSIGNED_4BITS_DMP, GL_INVALID_OPERATION);
	BASE_GL_FAIL_IF(actFormat != GL_LUMINANCE_ALPHA && _type == GL_UNSIGNED_BYTE_4_4_DMP, GL_INVALID_OPERATION);

	switch (_target){
		case GL_TEXTURE_2D:
			if (!state->gl_state.textureunits.bound2DTex[state->gl_state.textureunits.activeTexture])
				pTex2D = __texman->defaultTexContainer;
			else
				pTex2D = (tx_tex_container_t*)__texman->bound2DTex[state->gl_state.textureunits.activeTexture]->container;
			tx_texImage(&pTex2D->container, &pTex2D->texture, (_level) ? -_level : 1,
								_width, _height, actFormat, _type, _pixels, 0, isNative, transtype);
			break;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X: case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y: case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			BASE_GL_FAIL_IF(0 != state->gl_state.textureunits.activeTexture, GL_INVALID_OPERATION);
			BASE_GL_FAIL_IF(_width != _height, GL_INVALID_VALUE);

			if (!state->gl_state.textureunits.boundCubeTex[state->gl_state.textureunits.activeTexture])
				pTexCube = __texman->defaultCubeTexContainer;
			else
				pTexCube = (tx_cube_tex_container_t*)__texman->boundCubeTex[state->gl_state.textureunits.activeTexture]->container;

			tx_texImage(&pTexCube->container, &pTexCube->texture[_target - GL_TEXTURE_CUBE_MAP_POSITIVE_X],
						(_level) ? -_level : 1, _width, _height, actFormat, _type, _pixels, 0, isNative, transtype);
			break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}
	SET_MASK(state->upd_mask, IF_GL_TEXTURE0 + state->gl_state.textureunits.activeTexture);

	return;
}

static void tx_texImage(tx_tex_container_base_t* _tcbase, tx_texture_t* _tex, GLint _numLevel, GLsizei _width,
	GLsizei _height, GLenum _format, GLenum _type, const GLvoid *_pixels, GLint _size, GLboolean isNative, GLenum _transtype){

	
	if (_tex->surfaceContainer.picaaddr && (_tex->surfaceContainer.width != _width || _tex->surfaceContainer.height != _height
		|| _tex->surfaceContainer.format != _format || _tcbase->transType != _transtype || _tex->surfaceContainer.numLevels != _numLevel
		|| _tex->surfaceContainer.type != _type)){
		tx_freeTextureMemory(_tcbase->transType, _tex);
	}
	
	if (!_tex->surfaceContainer.picaaddr){
		__srf_initSurface(_width, _height, _format, _type, _numLevel, _size, &_tex->surfaceContainer);

		if (tx_allocTextureMemory(_transtype, _tex) == GL_FALSE){
			__err_setError(GL_OUT_OF_MEMORY);
			return;
		}
	}
	
	if (_pixels){
		int updatesize = _tex->surfaceContainer.size;
		int updatelevel = _numLevel;
		if (_tcbase->genMipmap){
			updatesize = _width * _height * _tex->surfaceContainer.pixelSize / 8;
			updatelevel = 1;
		}
		
		_tex->surfaceContainer.appaddr = (void*)_pixels;

		switch (_transtype){
			case GL_NO_COPY_NO_DMA_DMP:
				_tex->surfaceContainer.picaaddr = _tex->surfaceContainer.appaddr;
				break;
			case GL_COPY_NO_DMA_DMP:
			case GL_COPY_DMA_VRAMA_DMP:
			case GL_COPY_DMA_VRAMB_DMP:
				#ifndef RUN_ON_DUMMY_GL
				if (isNative)
					memcpy(_PICA_MEM_NATIVE(_tex->surfaceContainer.copyaddr), _pixels, updatesize);
				else{
					int miplevel;
					int w, h;
					int offset;
					
					w = _width;
					h = _height;
					offset = 0;

					for (miplevel = 0; miplevel < updatelevel; miplevel++){
						tx_blockConvCopy((const GLuint*)((GLubyte*)_pixels + offset), (GLuint*)_PICA_MEM_NATIVE((GLubyte*)_tex->surfaceContainer.copyaddr + offset),
								(enum TEXTURE_FORMAT)_tex->surfaceContainer.residentType, 0, 0, w, h, w, h, 0, GL_FALSE);
						offset += w * h * _tex->surfaceContainer.pixelSize / 8;
						w >>= 1;
						h >>= 1;
					}
				}
				#endif
				break;
			default:
				break;
		}

		switch (_transtype){
			case GL_NO_COPY_DMA_VRAMA_DMP:
			case GL_NO_COPY_DMA_VRAMB_DMP:
				__sys_setDMACommandRequest(_tex->surfaceContainer.picaaddr, _tex->surfaceContainer.appaddr, updatesize);
				break;
			case GL_COPY_DMA_VRAMA_DMP:
			case GL_COPY_DMA_VRAMB_DMP:
				__sys_setDMACommandRequest(_tex->surfaceContainer.picaaddr, _tex->surfaceContainer.copyaddr, updatesize);
				break;
		}

		switch (_transtype){
			case GL_NO_COPY_NO_DMA_DMP:
			case GL_COPY_NO_DMA_DMP:
				nngxlowFlushDataCache(_tex->surfaceContainer.picaaddr, updatesize);
			default:
				break;
		}

		if (_tcbase->genMipmap){
			tx_generateMipmap(&_tex->surfaceContainer);
		}
	}
	
	_tcbase->transType = _transtype;

	return;
}

static void tx_generateMipmap(srf_container_t* _surface){
	int miplevel;
	int w, h, ppfformat;
	unsigned srcaddr, dstaddr;
	
	srcaddr = (unsigned)_surface->picaaddr;
	w = _surface->width;
	h = _surface->height;
	switch (_surface->residentType){
		case TF_R8G8B8A8:	ppfformat = GARNET_PPF_FORMAT_R8G8B8A8;	break;
		case TF_R8G8B8:		ppfformat = GARNET_PPF_FORMAT_R8G8B8;	break;
		case TF_R4G4B4A4:	ppfformat = GARNET_PPF_FORMAT_R4G4B4A4;	break;
		case TF_R5G5B5A1:	ppfformat = GARNET_PPF_FORMAT_R5G5B5A1;	break;
		case TF_R5G6B5:		ppfformat = GARNET_PPF_FORMAT_R5G6B5;	break;
	}
	for (miplevel = 1; miplevel < _surface->numLevels; miplevel++){
		dstaddr = srcaddr + w * h * _surface->pixelSize / 8;
		__sys_setB2BCommandRequest((void*)srcaddr, (void*)dstaddr, w, h, ppfformat);
		srcaddr = dstaddr;
		w >>= 1;
		h >>= 1;
	}
	
	return;
}

void GL_APIENTRY glGenTextures(GLsizei _n, GLuint *_textures){
	GLint i;
	tex_list_t* texlist;
	tex_list_t* tmp;
	tex_list_t* newlist;
	GLuint id = 0;

	BASE_GL_FAIL_IF(_n < 0, GL_INVALID_VALUE);

	for (i = 0; i < _n; i++){
		newlist = (tex_list_t*)malloc(sizeof(tex_list_t));
		BASE_GL_FAIL_IF(!newlist, GL_OUT_OF_MEMORY);
		memset(newlist, 0, sizeof(tex_list_t));
		
		id = __miniID;
		for (;; id++){
			if (__texman->tex_list_table[id & TX_LIST_TABLE_SIZE] == 0){
				_textures[i] = id;
				newlist->id = id;
				newlist->next = 0;
				__texman->tex_list_table[id & TX_LIST_TABLE_SIZE] = newlist;
				break;
			}
			else if (__texman->tex_list_table[id & TX_LIST_TABLE_SIZE]->id == id){
				continue;
			}
			else if (__texman->tex_list_table[id & TX_LIST_TABLE_SIZE]->id > id){
				_textures[i] = id;
				newlist->id = id;
				newlist->next = __texman->tex_list_table[id & TX_LIST_TABLE_SIZE];
				__texman->tex_list_table[id & TX_LIST_TABLE_SIZE] = newlist;
				break;
			}
			else{
				int bCont, bBreak;
				bCont = bBreak = 0;
				for (texlist = __texman->tex_list_table[id & TX_LIST_TABLE_SIZE]->next, tmp = __texman->tex_list_table[id & TX_LIST_TABLE_SIZE]; texlist != 0; texlist = texlist->next){
					if (texlist->id == id){
						bCont = 1;
						break;
					}
					else if (texlist->id > id){
						_textures[i] = id;
						newlist->id = id;
						tmp->next = newlist;
						newlist->next = texlist;
						bBreak = 1;
						break;
					}
					tmp = texlist;
				}
				if (bBreak)
					break;
				if (bCont)
					continue;

				if (!texlist){
					tmp->next = newlist;
					newlist->id = id;
					newlist->next = 0;
					_textures[i] = id;
					break;
				}
				assert(0);
			}
		}
		__miniID = id + 1;
	}

	return;
}

void GL_APIENTRY glGetTexParameteriv(GLenum _target, GLenum _pname, GLint *_params){
	tx_tex_container_t*			pTex2D;
	tx_cube_tex_container_t*	pTexCube;
	GET_CURRENT_STATE(state);

	switch (_target){
		case GL_TEXTURE_2D:
			if (!state->gl_state.textureunits.bound2DTex[state->gl_state.textureunits.activeTexture])
				pTex2D = __texman->defaultTexContainer;
			else
				pTex2D = (tx_tex_container_t*)__texman->bound2DTex[state->gl_state.textureunits.activeTexture]->container;
			if (_pname == GL_TEXTURE_DATA_ADDR_DMP)
				_params[0] = (GLint)pTex2D->texture.surfaceContainer.picaaddr;
			else
				tx_getParameteri(&pTex2D->container, _pname, _params);
			break;
		case GL_TEXTURE_CUBE_MAP:
			if (!state->gl_state.textureunits.boundCubeTex[state->gl_state.textureunits.activeTexture])
				pTexCube = __texman->defaultCubeTexContainer;
			else
				pTexCube = (tx_cube_tex_container_t*)__texman->boundCubeTex[state->gl_state.textureunits.activeTexture]->container;
			if (_pname == GL_TEXTURE_DATA_ADDR_DMP){
				int i;
				for (i = 0; i < 6; i++)
					_params[i] = (GLint)pTexCube->texture[i].surfaceContainer.picaaddr;
			}
			else
				tx_getParameteri(&pTexCube->container, _pname, _params);
			break;
		default:
			__err_setError(GL_INVALID_ENUM);
			break;
	}

	return;
}

static void tx_getParameteri(tx_tex_container_base_t* _tc, GLenum _pname, GLint* _params){
	int i;
	switch (_pname){
		case GL_TEXTURE_MIN_FILTER:
			_params[0] = _tc->minFilter;
			break;
		case GL_TEXTURE_MAG_FILTER:
			_params[0] = _tc->magFilter;
			break;
		case GL_TEXTURE_WRAP_S:
			_params[0] = _tc->wrapS;
			break;
		case GL_TEXTURE_WRAP_T:
			_params[0] = _tc->wrapT;
			break;
		case GL_TEXTURE_BORDER_COLOR:
			for (i = 0; i < 4; i++)
				_params[i] = UTL_FLOAT2INT(_tc->BorderColor[i]);
			break;
		case GL_TEXTURE_LOD_BIAS:
			_params[0] = UTL_FLOAT2INT(_tc->lodBias);
			break;
		case GL_TEXTURE_MIN_LOD:
			_params[0] = _tc->minLOD;
			break;
		case GL_GENERATE_MIPMAP:
			_params[0] = _tc->genMipmap;
		    break;
		default:
			__err_setError(GL_INVALID_ENUM);
			break;
	}

	return;
}

void GL_APIENTRY glTexParameteri(GLenum _target, GLenum _pname, GLint _param){
	glTexParameteriv(_target, _pname, &_param);
}

void GL_APIENTRY glTexParameteriv(GLenum _target, GLenum _pname, const GLint *_params){
	int i;
	tx_tex_container_t*			pTex2D;
	tx_cube_tex_container_t*	pTexCube;
	GET_CURRENT_STATE(state);

	switch (_target){
		case GL_TEXTURE_2D:
			if (!state->gl_state.textureunits.bound2DTex[state->gl_state.textureunits.activeTexture])
				pTex2D = __texman->defaultTexContainer;
			else
				pTex2D = (tx_tex_container_t*)__texman->bound2DTex[state->gl_state.textureunits.activeTexture]->container;
			switch (_pname){
				case GL_TEXTURE_MIN_FILTER:
					BASE_GL_FAIL_IF(!(
						_params[0] == GL_NEAREST ||
						_params[0] == GL_LINEAR ||
						_params[0] == GL_NEAREST_MIPMAP_NEAREST ||
						_params[0] == GL_LINEAR_MIPMAP_NEAREST ||
						_params[0] == GL_NEAREST_MIPMAP_LINEAR ||
						_params[0] == GL_LINEAR_MIPMAP_LINEAR), GL_INVALID_ENUM);
					pTex2D->container.minFilter = _params[0];
					break;
				case GL_TEXTURE_MAG_FILTER:
					BASE_GL_FAIL_IF(!(
						_params[0] == GL_NEAREST ||
						_params[0] == GL_LINEAR), GL_INVALID_ENUM);
					pTex2D->container.magFilter = _params[0];
					break;
				case GL_TEXTURE_WRAP_S:
					BASE_GL_FAIL_IF(!(
						_params[0] == GL_CLAMP_TO_EDGE ||
						_params[0] == GL_REPEAT ||
						_params[0] == GL_CLAMP_TO_BORDER ||
						_params[0] == GL_MIRRORED_REPEAT), GL_INVALID_ENUM);
					pTex2D->container.wrapS = _params[0];
					break;
				case GL_TEXTURE_WRAP_T:
					BASE_GL_FAIL_IF(!(
						_params[0] == GL_CLAMP_TO_EDGE ||
						_params[0] == GL_REPEAT ||
						_params[0] == GL_CLAMP_TO_BORDER ||
						_params[0] == GL_MIRRORED_REPEAT), GL_INVALID_ENUM);
					pTex2D->container.wrapT = _params[0];
					break;
				case GL_TEXTURE_BORDER_COLOR:
					for (i = 0; i < 4; i++)
						pTex2D->container.BorderColor[i] = UTL_CLAMP(UTL_INT2FLOAT(_params[i]));
					break;
				case GL_TEXTURE_LOD_BIAS:
					BASE_GL_FAIL_IF(_params[0] < -16 || _params[0] >= 16, GL_INVALID_VALUE);
					pTex2D->container.lodBias = UTL_INT2FLOAT(_params[0]);
					break;
				case GL_TEXTURE_MIN_LOD:
					pTex2D->container.minLOD = _params[0];
					break;
				case GL_GENERATE_MIPMAP:
					pTex2D->container.genMipmap = _params[0];
					break;
				default:
					__err_setError(GL_INVALID_ENUM);
					return;
			}
			break;
		case GL_TEXTURE_CUBE_MAP:
			if (!state->gl_state.textureunits.boundCubeTex[state->gl_state.textureunits.activeTexture])
				pTexCube = __texman->defaultCubeTexContainer;
			else
				pTexCube = (tx_cube_tex_container_t*)__texman->boundCubeTex[state->gl_state.textureunits.activeTexture]->container;
			switch (_pname){
				case GL_TEXTURE_MIN_FILTER:
					BASE_GL_FAIL_IF(!(
						_params[0] == GL_NEAREST ||
						_params[0] == GL_LINEAR ||
						_params[0] == GL_NEAREST_MIPMAP_NEAREST ||
						_params[0] == GL_LINEAR_MIPMAP_NEAREST ||
						_params[0] == GL_NEAREST_MIPMAP_LINEAR ||
						_params[0] == GL_LINEAR_MIPMAP_LINEAR), GL_INVALID_ENUM);
					pTexCube->container.minFilter = _params[0];
					break;
				case GL_TEXTURE_MAG_FILTER:
					BASE_GL_FAIL_IF(!(
						_params[0] == GL_NEAREST ||
						_params[0] == GL_LINEAR), GL_INVALID_ENUM);
					pTexCube->container.magFilter = _params[0];
					break;
				case GL_TEXTURE_WRAP_S:
					BASE_GL_FAIL_IF(!(
						_params[0] == GL_CLAMP_TO_EDGE ||
						_params[0] == GL_REPEAT ||
						_params[0] == GL_CLAMP_TO_BORDER ||
						_params[0] == GL_MIRRORED_REPEAT), GL_INVALID_ENUM);
					pTexCube->container.wrapS = _params[0];
					break;
				case GL_TEXTURE_WRAP_T:
					BASE_GL_FAIL_IF(!(
						_params[0] == GL_CLAMP_TO_EDGE ||
						_params[0] == GL_REPEAT ||
						_params[0] == GL_CLAMP_TO_BORDER ||
						_params[0] == GL_MIRRORED_REPEAT), GL_INVALID_ENUM);
					pTexCube->container.wrapT = _params[0];
					break;
				case GL_TEXTURE_BORDER_COLOR:
					for (i = 0; i < 4; i++)
						pTexCube->container.BorderColor[i] = UTL_CLAMP(UTL_INT2FLOAT(_params[i]));
					break;
				case GL_TEXTURE_LOD_BIAS:
					BASE_GL_FAIL_IF(_params[0] < -16 || _params[0] >= 16, GL_INVALID_VALUE);
					pTexCube->container.lodBias = UTL_INT2FLOAT(_params[0]);
					break;
				case GL_TEXTURE_MIN_LOD:
					pTexCube->container.minLOD = _params[0];
					break;
				case GL_GENERATE_MIPMAP:
					pTexCube->container.genMipmap = _params[0];
					break;
				default:
					__err_setError(GL_INVALID_ENUM);
					return;
			}
			break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}
	SET_MASK(state->upd_mask, IF_GL_TEXTURE0 + state->gl_state.textureunits.activeTexture);

	return;
}

void GL_APIENTRY glTexParameterf(GLenum _target, GLenum _pname, GLfloat _param){
	glTexParameterfv(_target, _pname, &_param);
}

void GL_APIENTRY glTexParameterfv(GLenum _target, GLenum _pname, const GLfloat *_params){
	int i;
	tx_tex_container_t*			pTex2D;
	tx_cube_tex_container_t*	pTexCube;
	GET_CURRENT_STATE(state);

	switch (_target){
		case GL_TEXTURE_2D:
			if (!state->gl_state.textureunits.bound2DTex[state->gl_state.textureunits.activeTexture])
				pTex2D = __texman->defaultTexContainer;
			else
				pTex2D = (tx_tex_container_t*)__texman->bound2DTex[state->gl_state.textureunits.activeTexture]->container;
			switch (_pname){
				case GL_TEXTURE_MIN_FILTER:
					BASE_GL_FAIL_IF(!(
						UTL_FLOAT2INT(_params[0]) == GL_NEAREST ||
						UTL_FLOAT2INT(_params[0]) == GL_LINEAR ||
						UTL_FLOAT2INT(_params[0]) == GL_NEAREST_MIPMAP_NEAREST ||
						UTL_FLOAT2INT(_params[0]) == GL_LINEAR_MIPMAP_NEAREST ||
						UTL_FLOAT2INT(_params[0]) == GL_NEAREST_MIPMAP_LINEAR ||
						UTL_FLOAT2INT(_params[0]) == GL_LINEAR_MIPMAP_LINEAR), GL_INVALID_ENUM);
					pTex2D->container.minFilter = UTL_FLOAT2INT(_params[0]);
					break;
				case GL_TEXTURE_MAG_FILTER:
					BASE_GL_FAIL_IF(!(
						UTL_FLOAT2INT(_params[0]) == GL_NEAREST ||
						UTL_FLOAT2INT(_params[0]) == GL_LINEAR), GL_INVALID_ENUM);
					pTex2D->container.magFilter = UTL_FLOAT2INT(_params[0]);
					break;
				case GL_TEXTURE_WRAP_S:
					BASE_GL_FAIL_IF(!(
						UTL_FLOAT2INT(_params[0]) == GL_CLAMP_TO_EDGE ||
						UTL_FLOAT2INT(_params[0]) == GL_REPEAT ||
						UTL_FLOAT2INT(_params[0]) == GL_CLAMP_TO_BORDER ||
						UTL_FLOAT2INT(_params[0]) == GL_MIRRORED_REPEAT), GL_INVALID_ENUM);
					pTex2D->container.wrapS = UTL_FLOAT2INT(_params[0]);
					break;
				case GL_TEXTURE_WRAP_T:
					BASE_GL_FAIL_IF(!(
						UTL_FLOAT2INT(_params[0]) == GL_CLAMP_TO_EDGE ||
						UTL_FLOAT2INT(_params[0]) == GL_REPEAT ||
						UTL_FLOAT2INT(_params[0]) == GL_CLAMP_TO_BORDER ||
						UTL_FLOAT2INT(_params[0]) == GL_MIRRORED_REPEAT), GL_INVALID_ENUM);
					pTex2D->container.wrapT = UTL_FLOAT2INT(_params[0]);
					break;
				case GL_TEXTURE_BORDER_COLOR:
					for (i = 0; i < 4; i++)
						pTex2D->container.BorderColor[i] = UTL_CLAMP(_params[i]);
					break;
				case GL_TEXTURE_LOD_BIAS:
					BASE_GL_FAIL_IF(_params[0] < -16 || _params[0] >= 16, GL_INVALID_VALUE);
					pTex2D->container.lodBias = _params[0];
					break;
				case GL_TEXTURE_MIN_LOD:
					pTex2D->container.minLOD = UTL_FLOAT2INT(_params[0]);
					break;
				case GL_GENERATE_MIPMAP:
					pTex2D->container.genMipmap = UTL_FLOAT2BOOL(_params[0]);
					break;
				default:
					__err_setError(GL_INVALID_ENUM);
					return;
			}
			break;
		case GL_TEXTURE_CUBE_MAP:
			if (!state->gl_state.textureunits.boundCubeTex[state->gl_state.textureunits.activeTexture])
				pTexCube = __texman->defaultCubeTexContainer;
			else
				pTexCube = (tx_cube_tex_container_t*)__texman->boundCubeTex[state->gl_state.textureunits.activeTexture]->container;
			switch (_pname){
				case GL_TEXTURE_MIN_FILTER:
					BASE_GL_FAIL_IF(!(
						UTL_FLOAT2INT(_params[0]) == GL_NEAREST ||
						UTL_FLOAT2INT(_params[0]) == GL_LINEAR ||
						UTL_FLOAT2INT(_params[0]) == GL_NEAREST_MIPMAP_NEAREST ||
						UTL_FLOAT2INT(_params[0]) == GL_LINEAR_MIPMAP_NEAREST ||
						UTL_FLOAT2INT(_params[0]) == GL_NEAREST_MIPMAP_LINEAR ||
						UTL_FLOAT2INT(_params[0]) == GL_LINEAR_MIPMAP_LINEAR), GL_INVALID_ENUM);
					pTexCube->container.minFilter = UTL_FLOAT2INT(_params[0]);
					break;
				case GL_TEXTURE_MAG_FILTER:
					BASE_GL_FAIL_IF(!(
						UTL_FLOAT2INT(_params[0]) == GL_NEAREST ||
						UTL_FLOAT2INT(_params[0]) == GL_LINEAR), GL_INVALID_ENUM);
					pTexCube->container.magFilter = UTL_FLOAT2INT(_params[0]);
					break;
				case GL_TEXTURE_WRAP_S:
					BASE_GL_FAIL_IF(!(
						UTL_FLOAT2INT(_params[0]) == GL_CLAMP_TO_EDGE ||
						UTL_FLOAT2INT(_params[0]) == GL_REPEAT ||
						UTL_FLOAT2INT(_params[0]) == GL_CLAMP_TO_BORDER ||
						UTL_FLOAT2INT(_params[0]) == GL_MIRRORED_REPEAT), GL_INVALID_ENUM);
					pTexCube->container.wrapS = UTL_FLOAT2INT(_params[0]);
					break;
				case GL_TEXTURE_WRAP_T:
					BASE_GL_FAIL_IF(!(
						UTL_FLOAT2INT(_params[0]) == GL_CLAMP_TO_EDGE ||
						UTL_FLOAT2INT(_params[0]) == GL_REPEAT ||
						UTL_FLOAT2INT(_params[0]) == GL_CLAMP_TO_BORDER ||
						UTL_FLOAT2INT(_params[0]) == GL_MIRRORED_REPEAT), GL_INVALID_ENUM);
					pTexCube->container.wrapT = UTL_FLOAT2INT(_params[0]);
					break;
				case GL_TEXTURE_BORDER_COLOR:
					for (i = 0; i < 4; i++)
						pTexCube->container.BorderColor[i] = UTL_CLAMP(_params[i]);
					break;
				case GL_TEXTURE_LOD_BIAS:
					BASE_GL_FAIL_IF(_params[0] < -16 || _params[0] >= 16, GL_INVALID_VALUE);
					pTexCube->container.lodBias = _params[0];
					break;
				case GL_TEXTURE_MIN_LOD:
					pTexCube->container.minLOD = UTL_FLOAT2INT(_params[0]);
					break;
				case GL_GENERATE_MIPMAP:
					pTexCube->container.genMipmap = UTL_FLOAT2BOOL(_params[0]);
					break;
				default:
					__err_setError(GL_INVALID_ENUM);
					return;
			}
			break;
		default:
			__err_setError(GL_INVALID_ENUM);
			return;
	}
	SET_MASK(state->upd_mask, IF_GL_TEXTURE0 + state->gl_state.textureunits.activeTexture);

	return;
}

void __txv_initializeTextureValidator(bit_mask_t* _mask){
	int i;
	GET_CURRENT_STATE(state);

	CLEAR_ALL_MASK(*_mask);
	for (i = 0; i < MAX_TEXTURE_UNITS; i++)
		SET_MASK(*_mask, IF_GL_TEXTURE0 + i);

	{
		stt_hw_tex_units_state_t* hwtexstate = &state->hw_state.hw_texunits;
		tex_reg_map_t* regs = (tex_reg_map_t*)hwtexstate->regs;

		regs->r_NEW_PT = 1;
		regs->r_UV1_TEX2 = 0;
		regs->r_TEX_CACHE_FLUSH = 1;
	}

	__txv_initializeTextureValidatorHWRegister();

	return;
}

void __txv_initializeTextureValidatorHWRegister(void){
	int i;

	for (i = 0; i <= PA_WO_TU_END - PA_WO_TU_BEGIN; i++){
		switch (i){
			case 0:
				__cb_writeReg(i + PA_WO_TU_BEGIN, 0x00011000, 0xd);
				break;
			case 0x0c:
				__cb_writeReg(i + PA_WO_TU_BEGIN, 0x00ff0000, 0xf);
				break;
			case 0x0f:	/* register 0, 0xc, and 0xf is initialized by shader manager. */
				break;
			default:
				__cb_writeReg(i + PA_WO_TU_BEGIN, 0, 0xf);
				break;
		}
	}

	return;
}

void __txv_validateTextureValidator(bit_mask_t* _mask){
	int i;
	stt_texture_units_state_t* texstate;
	GLenum stat;
	int updateTex = 0;
	GET_CURRENT_STATE(state);

	if (state->suppressStateMask & NN_GX_STATE_TEXTURE)
		return;

	texstate = &state->gl_state.textureunits;

	stat = GL_NO_ERROR;
	for (i = 0; i < MAX_TEXTURE_UNITS; i++){
		if (!GET_MASK(*_mask, IF_GL_TEXTURE0 + i)) continue;

		if (texstate->enabledCubeTex[i]){
			if (!texstate->boundCubeTex[i])
				stat = tx_validateCubeTextureWUnit(__texman->defaultCubeTexContainer, i);
			else{
				if (__texman->boundCubeTex[i])
					stat = tx_validateCubeTextureWUnit((tx_cube_tex_container_t*)__texman->boundCubeTex[i]->container, i);
				else
					stat = GL_INVALID_OPERATION; /* texture object bount to texture collection is already deleted. */
			}
		}
		else{
			if (!texstate->bound2DTex[i])
				stat = tx_validate2DTextureWUnit(__texman->defaultTexContainer, i);
			else{
				if (__texman->bound2DTex[i])
					stat = tx_validate2DTextureWUnit((tx_tex_container_t*)__texman->bound2DTex[i]->container, i);
				else
					stat = GL_INVALID_OPERATION;
			}
		}
		updateTex = 1;
		BASE_GL_FAIL_IF_NORET(stat != GL_NO_ERROR, stat);
	}

	if (updateTex && (state->hw_state.hw_texunits.regs[0] & 0x7)){
		__cb_writeReg(PA_WO_TU_BEGIN, state->hw_state.hw_texunits.regs[0], 0x1);
		__cb_writeReg(PA_WO_TU_BEGIN, 0x10000, 0x5);
	}

	return;
}

static GLenum tx_validate2DTextureWUnit(tx_tex_container_t* _tc, GLuint _unit){
	GLenum stat;
	stt_texture_units_state_t* texstate;
	stt_hw_tex_units_state_t* hwtexstate;
	tex_reg_map_t* regs;
	srf_container_t* surf;
	unsigned bcolor;
	unsigned tmp;
	GET_CURRENT_STATE(state);
	
	stat = tx_validate2DTexture(_tc);
	texstate = &state->gl_state.textureunits;
	hwtexstate = &state->hw_state.hw_texunits;
	regs = (tex_reg_map_t*)hwtexstate->regs;

	if (!texstate->enabled2DTex[_unit] || GL_NO_ERROR != stat){
		switch (_unit){
			case 0:
				regs->r_ON_0 = 0;
				break;
			case 1:
				regs->r_ON_1 = 0;
				break;
			case 2:
				regs->r_ON_2 = 0;
				break;
			default:
				return GL_INVALID_VALUE;
		}

		return texstate->enabled2DTex[_unit] ? stat : GL_NO_ERROR;
	}

	surf = &_tc->texture.surfaceContainer;

	bcolor = (unsigned)(_tc->container.BorderColor[0] * 255.f) | ((unsigned)(_tc->container.BorderColor[1] * 255.f) << 8) |
		((unsigned)(_tc->container.BorderColor[2] * 255.f) << 16) | ( (unsigned)(_tc->container.BorderColor[3] * 255.f) << 24);

	switch (_unit){
		case 0:
			regs->r_SHADOW_SPECIAL_ADRS = 0;
			regs->r_BORDER_0 = bcolor;
			regs->r_WIDTH_0 = surf->width;
			regs->r_HEIGHT_0 = surf->height;
			UTL_F2FX_13W_5I_T(_tc->container.lodBias, regs->r_LOD_BIAS_0);
			regs->r_TEX_FORMAT_0 = surf->residentType;
			switch (surf->format){
				case GL_RGBA: case GL_RGB: case GL_ALPHA: case GL_LUMINANCE:
				case GL_LUMINANCE_ALPHA: case GL_ETC1_RGB8_NATIVE_DMP: case GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP: case GL_HILO8_DMP:
					if (texstate->samplerType[0] == GL_TEXTURE_PROJECTION_DMP)
						regs->r_TMODE_0 = TX_TTM_PRJT;
					else
						regs->r_TMODE_0 = TX_TTM_TEX;

					switch (_tc->container.wrapS){
						case GL_CLAMP_TO_EDGE:			regs->r_ADR_MODE_U_0 = TX_ADM_CLMP;	break;
						case GL_REPEAT:					regs->r_ADR_MODE_U_0 = TX_ADM_REPT;	break;
						case GL_CLAMP_TO_BORDER:		regs->r_ADR_MODE_U_0 = TX_ADM_CLBD;	break;
						case GL_MIRRORED_REPEAT:		regs->r_ADR_MODE_U_0 = TX_ADM_MIRR;	break;
					}

					switch (_tc->container.wrapT){
						case GL_CLAMP_TO_EDGE:			regs->r_ADR_MODE_V_0 = TX_ADM_CLMP;	break;
						case GL_REPEAT:					regs->r_ADR_MODE_V_0 = TX_ADM_REPT;	break;
						case GL_CLAMP_TO_BORDER:		regs->r_ADR_MODE_V_0 = TX_ADM_CLBD;	break;
						case GL_MIRRORED_REPEAT:		regs->r_ADR_MODE_V_0 = TX_ADM_MIRR;	break;
					}

					switch (_tc->container.magFilter){
						case GL_NEAREST:	regs->r_MAX_FMODE_0 = 0;	break;
						case GL_LINEAR:		regs->r_MAX_FMODE_0 = 1;	break;
					}

					switch (_tc->container.minFilter){
						case GL_NEAREST:
							regs->r_MIN_FMODE_0 = 0;
							regs->r_MAX_LOD_0 = 0;
							regs->r_MIN_LOD_0 = 0;
							regs->r_TRILINEAR_0 = 0;
							break;
						case GL_NEAREST_MIPMAP_NEAREST:
							regs->r_MIN_FMODE_0 = 0;
							regs->r_MAX_LOD_0 = surf->numLevels - 1;
							regs->r_MIN_LOD_0 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_0 = 0;
							break;
						case GL_NEAREST_MIPMAP_LINEAR:
							regs->r_MIN_FMODE_0 = 0;
							regs->r_MAX_LOD_0 = surf->numLevels -1;
							regs->r_MIN_LOD_0 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_0 = 1;
							break;
						case GL_LINEAR:
							regs->r_MIN_FMODE_0 = 1;
							regs->r_MAX_LOD_0 = 0;
							regs->r_MIN_LOD_0 = 0;
							regs->r_TRILINEAR_0 = 0;
							break;
						case GL_LINEAR_MIPMAP_NEAREST:
							regs->r_MIN_FMODE_0 = 1;
							regs->r_MAX_LOD_0 = surf->numLevels - 1;
							regs->r_MIN_LOD_0 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_0 = 0;
							break;
						case GL_LINEAR_MIPMAP_LINEAR:
							regs->r_MIN_FMODE_0 = 1;
							regs->r_MAX_LOD_0 = surf->numLevels - 1;
							regs->r_MIN_LOD_0 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_0 = 1;
							break;
					}
					regs->r_TDMODE_0 = surf->format == GL_ETC1_RGB8_NATIVE_DMP ? TX_TDM_ETC : TX_TDM_RGBA;
					break;
				case GL_SHADOW_DMP:
					regs->r_TRILINEAR_0 = 0;
					regs->r_SHADOW_SPECIAL_ADRS = 1;
					regs->r_TMODE_0 = TX_TTM_SDW;
					regs->r_MAX_FMODE_0 = 1;
					regs->r_MIN_FMODE_0 = 1;
					regs->r_MAX_LOD_0 = 0;
					regs->r_MIN_LOD_0 = 0;
					regs->r_LOD_BIAS_0 = 0;
					regs->r_ADR_MODE_U_0 = TX_ADM_CLBD;
					regs->r_ADR_MODE_V_0 = TX_ADM_CLBD;
					regs->r_TDMODE_0 = TX_TDM_RGBA;
					break;
				case GL_GAS_DMP:
					regs->r_TMODE_0 = TX_TTM_TEX;
					regs->r_TDMODE_0 = TX_TDM_RGBA;
					regs->r_MAX_FMODE_0 = 0;
					regs->r_MIN_FMODE_0 = 0;
					regs->r_MAX_LOD_0 = 0;
					regs->r_MIN_LOD_0 = 0;
					regs->r_LOD_BIAS_0 = 0;
					regs->r_BORDER_0 = 0x0;
					regs->r_ADR_MODE_U_0 = TX_ADM_CLMP;
					regs->r_ADR_MODE_V_0 = TX_ADM_CLMP;
					break;
				default:
					return GL_INVALID_OPERATION;
			}
			regs->r_POINTER_0 = nngxlowGetPhysicalAddr((unsigned)surf->picaaddr) >> 3;
			regs->r_ON_0 = 1;

			__cb_writeRegs(PA_WO_TU_BEGIN + 1, 0xa, hwtexstate->regs + 1);
			__cb_writeReg(PA_WO_TU_BEGIN + 0xe, hwtexstate->regs[0xe], 0xf);
			break;
		case 1:
			UTL_F2FX_13W_5I_T(_tc->container.lodBias, tmp);
			regs->r_LOD_BIAS_1 = tmp;
			regs->r_TMODE_1 = TX_TTM_TEX;
			switch (surf->format){
				case GL_RGBA: case GL_RGB: case GL_ALPHA: case GL_LUMINANCE:
				case GL_LUMINANCE_ALPHA: case GL_ETC1_RGB8_NATIVE_DMP: case GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP: case GL_HILO8_DMP:
					switch (_tc->container.wrapS){
						case GL_CLAMP_TO_EDGE:			regs->r_ADR_MODE_U_1 = TX_ADM_CLMP;	break;
						case GL_REPEAT:					regs->r_ADR_MODE_U_1 = TX_ADM_REPT;	break;
						case GL_CLAMP_TO_BORDER:		regs->r_ADR_MODE_U_1 = TX_ADM_CLBD;	break;
						case GL_MIRRORED_REPEAT:		regs->r_ADR_MODE_U_1 = TX_ADM_MIRR;	break;
					}
					switch (_tc->container.wrapT){
						case GL_CLAMP_TO_EDGE:			regs->r_ADR_MODE_V_1 = TX_ADM_CLMP;	break;
						case GL_REPEAT:					regs->r_ADR_MODE_V_1 = TX_ADM_REPT;	break;
						case GL_CLAMP_TO_BORDER:		regs->r_ADR_MODE_V_1 = TX_ADM_CLBD;	break;
						case GL_MIRRORED_REPEAT:		regs->r_ADR_MODE_V_1 = TX_ADM_MIRR;	break;
					}
					switch (_tc->container.magFilter){
						case GL_NEAREST:	regs->r_MAX_FMODE_1 = 0;	break;
						case GL_LINEAR:		regs->r_MAX_FMODE_1 = 1;	break;
					}
					switch (_tc->container.minFilter){
						case GL_NEAREST:
							regs->r_MIN_FMODE_1 = 0;
							regs->r_MAX_LOD_1 = 0;
							regs->r_MIN_LOD_1 = 0;
							regs->r_TRILINEAR_1 = 0;
							break;
						case GL_NEAREST_MIPMAP_NEAREST:
							regs->r_MIN_FMODE_1 = 0;
							regs->r_MAX_LOD_1 = surf->numLevels - 1;
							regs->r_MIN_LOD_1 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_1 = 0;
							break;
						case GL_NEAREST_MIPMAP_LINEAR:
							regs->r_MIN_FMODE_1 = 0;
							regs->r_MAX_LOD_1 = surf->numLevels - 1;
							regs->r_MIN_LOD_1 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_1 = 1;
							break;
						case GL_LINEAR:
							regs->r_MIN_FMODE_1 = 1;
							regs->r_MAX_LOD_1 = 0;
							regs->r_MIN_LOD_1 = 0;
							regs->r_TRILINEAR_1 = 0;
							break;
						case GL_LINEAR_MIPMAP_NEAREST:
							regs->r_MIN_FMODE_1 = 1;
							regs->r_MAX_LOD_1 = surf->numLevels - 1;
							regs->r_MIN_LOD_1 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_1 = 0;
							break;
						case GL_LINEAR_MIPMAP_LINEAR:
							regs->r_MIN_FMODE_1 = 1;
							regs->r_MAX_LOD_1 = surf->numLevels - 1;
							regs->r_MIN_LOD_1 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_1 = 1;
							break;
					}
					regs->r_BORDER_1 = bcolor;
					break;
				case GL_GAS_DMP:
					regs->r_MAX_FMODE_1 = 0;
					regs->r_MIN_FMODE_1 = 0;
					regs->r_MAX_LOD_1 = 0;
					regs->r_MIN_LOD_1 = 0;
					regs->r_LOD_BIAS_1 = 0;
					regs->r_BORDER_1 = 0x0;
					regs->r_ADR_MODE_U_1 = TX_ADM_CLMP;
					regs->r_ADR_MODE_V_1 = TX_ADM_CLMP;
					break;
				default:
					return GL_INVALID_OPERATION;
			}
			regs->r_TDMODE_1 = surf->format == GL_ETC1_RGB8_NATIVE_DMP ? TX_TDM_ETC : TX_TDM_RGBA;
			regs->r_WIDTH_1 = surf->width;
			regs->r_HEIGHT_1 = surf->height;
			regs->r_POINTER_1 = nngxlowGetPhysicalAddr((unsigned)surf->picaaddr) >> 3;
			regs->r_ON_1 = 1;
			regs->r_TEX_FORMAT_1 = surf->residentType;
			__cb_writeRegs(PA_WO_TU1_BEGIN, PA_WO_TU2_BEGIN - PA_WO_TU1_BEGIN - 2, &hwtexstate->regs[PA_WO_TU1_BEGIN - PA_WO_TU_BEGIN]);
			break;
		case 2:
			UTL_F2FX_13W_5I_T(_tc->container.lodBias, tmp);
			regs->r_LOD_BIAS_2 = tmp;
			regs->r_TMODE_2 = TX_TTM_TEX;
			switch (surf->format){
				case GL_RGBA: case GL_RGB: case GL_ALPHA: case GL_LUMINANCE:
				case GL_LUMINANCE_ALPHA: case GL_ETC1_RGB8_NATIVE_DMP: case GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP: case GL_HILO8_DMP:
					switch (_tc->container.wrapS){
						case GL_CLAMP_TO_EDGE:			regs->r_ADR_MODE_U_2 = TX_ADM_CLMP;	break;
						case GL_REPEAT:					regs->r_ADR_MODE_U_2 = TX_ADM_REPT;	break;
						case GL_CLAMP_TO_BORDER:		regs->r_ADR_MODE_U_2 = TX_ADM_CLBD;	break;
						case GL_MIRRORED_REPEAT:		regs->r_ADR_MODE_U_2 = TX_ADM_MIRR;	break;
					}
					switch (_tc->container.wrapT){
						case GL_CLAMP_TO_EDGE:			regs->r_ADR_MODE_V_2 = TX_ADM_CLMP;	break;
						case GL_REPEAT:					regs->r_ADR_MODE_V_2 = TX_ADM_REPT;	break;
						case GL_CLAMP_TO_BORDER:		regs->r_ADR_MODE_V_2 = TX_ADM_CLBD;	break;
						case GL_MIRRORED_REPEAT:		regs->r_ADR_MODE_V_2 = TX_ADM_MIRR;	break;
					}
					switch (_tc->container.magFilter){
						case GL_NEAREST:				regs->r_MAX_FMODE_2 = 0;				break;
						case GL_LINEAR:					regs->r_MAX_FMODE_2 = 1;				break;
					}
					switch (_tc->container.minFilter){
						case GL_NEAREST:
							regs->r_MIN_FMODE_2 = 0;
							regs->r_MAX_LOD_2 = 0;
							regs->r_MIN_LOD_2 = 0;
							regs->r_TRILINEAR_2 = 0;
							break;
						case GL_NEAREST_MIPMAP_NEAREST:
							regs->r_MIN_FMODE_2 = 0;
							regs->r_MAX_LOD_2 = surf->numLevels - 1;
							regs->r_MIN_LOD_2 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_2 = 0;
							break;
						case GL_NEAREST_MIPMAP_LINEAR:
							regs->r_MIN_FMODE_2 = 0;
							regs->r_MAX_LOD_2 = surf->numLevels - 1;
							regs->r_MIN_LOD_2 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_2 = 1;
							break;
						case GL_LINEAR:
							regs->r_MIN_FMODE_2 = 1;
							regs->r_MAX_LOD_2 = 0;
							regs->r_MIN_LOD_2 = 0;
							regs->r_TRILINEAR_2 = 0;
							break;
						case GL_LINEAR_MIPMAP_NEAREST:
							regs->r_MIN_FMODE_2 = 1;
							regs->r_MAX_LOD_2 = surf->numLevels - 1;
							regs->r_MIN_LOD_2 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_2 = 0;
							break;
						case GL_LINEAR_MIPMAP_LINEAR:
							regs->r_MIN_FMODE_2 = 1;
							regs->r_MAX_LOD_2 = surf->numLevels - 1;
							regs->r_MIN_LOD_2 = (_tc->container.minLOD < 0) ? 0 : _tc->container.minLOD;
							regs->r_TRILINEAR_2 = 1;
							break;
					}
					regs->r_BORDER_2 = bcolor;
					break;
				case GL_GAS_DMP:
					regs->r_MAX_FMODE_2 = 0;
					regs->r_MIN_FMODE_2 = 0;
					regs->r_MAX_LOD_2 = 0;
					regs->r_MIN_LOD_2 = 0;
					regs->r_LOD_BIAS_2 = 0;
					regs->r_BORDER_2 = 0x0;
					regs->r_ADR_MODE_U_2 = TX_ADM_CLMP;
					regs->r_ADR_MODE_V_2 = TX_ADM_CLMP;
					break;
				default:
					return GL_INVALID_OPERATION;
			}
			regs->r_TDMODE_2 = surf->format == GL_ETC1_RGB8_NATIVE_DMP ? TX_TDM_ETC : TX_TDM_RGBA;
			regs->r_WIDTH_2 = surf->width;
			regs->r_HEIGHT_2 = surf->height;
			regs->r_POINTER_2 = nngxlowGetPhysicalAddr((unsigned)surf->picaaddr) >> 3;
			regs->r_ON_2 = 1;
			regs->r_TEX_FORMAT_2 = surf->residentType;
			__cb_writeRegs(PA_WO_TU2_BEGIN, PA_WO_TU_END - PA_WO_TU2_BEGIN + 1, &hwtexstate->regs[PA_WO_TU2_BEGIN - PA_WO_TU_BEGIN]);
			break;
	}

	return GL_NO_ERROR;
}

static GLenum tx_validate2DTexture(tx_tex_container_t* _tc){
	if (!_tc->texture.surfaceContainer.picaaddr)
		return GL_INVALID_OPERATION;
	return GL_NO_ERROR;
}

static GLenum tx_validateCubeTextureWUnit(tx_cube_tex_container_t* _ctc, GLuint _unit){
	stt_texture_units_state_t* texstate;
	stt_hw_tex_units_state_t* hwtexstate;
	tex_reg_map_t* regs;
	GLenum stat;
	srf_container_t* surf;
	unsigned bcolor;
	GET_CURRENT_STATE(state);

	if (0 != _unit)
		return GL_INVALID_OPERATION;

	texstate = &state->gl_state.textureunits;
	hwtexstate = &state->hw_state.hw_texunits;
	regs = (tex_reg_map_t*)hwtexstate->regs;

	stat = tx_validateCubeTexture(_ctc);

	if (!texstate->enabledCubeTex[_unit] || GL_NO_ERROR != stat){
		switch (_unit){
			case 0:
				regs->r_ON_0 = 0;
				break;
			default:
				return GL_INVALID_VALUE;
		}

		return stat;
	}

	surf = &_ctc->texture[0].surfaceContainer;

	bcolor = (unsigned)(_ctc->container.BorderColor[0] * 255.f) | ((unsigned)(_ctc->container.BorderColor[1] * 255.f) << 8) |
		((unsigned)(_ctc->container.BorderColor[2] * 255.f) << 16) | ((unsigned)(_ctc->container.BorderColor[3] * 255.f) << 24);

	regs->r_TRILINEAR_0 = 0;
	regs->r_SHADOW_SPECIAL_ADRS = 0;
	regs->r_BORDER_0 = bcolor;
	regs->r_WIDTH_0 = surf->width;
	regs->r_HEIGHT_0 = surf->height;
	regs->r_SDW_Z_BIAS = 0;
	regs->r_TEX_FORMAT_0 = surf->residentType;
	switch (surf->format){
		case GL_RGBA: case GL_RGB: case GL_LUMINANCE_ALPHA: case GL_LUMINANCE: case GL_ALPHA:
		case GL_ETC1_RGB8_NATIVE_DMP: case GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP:
			regs->r_TMODE_0 = TX_TTM_ENV;

			switch (_ctc->container.wrapS){
				case GL_CLAMP_TO_EDGE:			regs->r_ADR_MODE_U_0 = TX_ADM_CLMP;	break;
				case GL_REPEAT:					regs->r_ADR_MODE_U_0 = TX_ADM_REPT;	break;
				case GL_CLAMP_TO_BORDER:		regs->r_ADR_MODE_U_0 = TX_ADM_CLBD;	break;
				case GL_MIRRORED_REPEAT:		regs->r_ADR_MODE_U_0 = TX_ADM_MIRR;	break;
			}

			switch (_ctc->container.wrapT){
				case GL_CLAMP_TO_EDGE:			regs->r_ADR_MODE_V_0 = TX_ADM_CLMP;	break;
				case GL_REPEAT:					regs->r_ADR_MODE_V_0 = TX_ADM_REPT;	break;
				case GL_CLAMP_TO_BORDER:		regs->r_ADR_MODE_V_0 = TX_ADM_CLBD;	break;
				case GL_MIRRORED_REPEAT:		regs->r_ADR_MODE_V_0 = TX_ADM_MIRR;	break;
			}

			switch (_ctc->container.magFilter){
				case GL_NEAREST:	regs->r_MAX_FMODE_0 = 0;	break;
				case GL_LINEAR:		regs->r_MAX_FMODE_0 = 1;	break;
			}

			switch (_ctc->container.minFilter){
				case GL_NEAREST:
					regs->r_MIN_FMODE_0 = 0;
					regs->r_MAX_LOD_0 = 0;
					regs->r_MIN_LOD_0 = 0;
					regs->r_TRILINEAR_0 = 0;
					break;
				case GL_NEAREST_MIPMAP_NEAREST:
					regs->r_MIN_FMODE_0 = 0;
					regs->r_MAX_LOD_0 = surf->numLevels - 1;
					regs->r_MIN_LOD_0 = (_ctc->container.minLOD < 0) ? 0 : _ctc->container.minLOD;
					regs->r_TRILINEAR_0 = 0;
					break;
				case GL_NEAREST_MIPMAP_LINEAR:
					regs->r_MIN_FMODE_0 = 0;
					regs->r_MAX_LOD_0 = surf->numLevels - 1;
					regs->r_MIN_LOD_0 = (_ctc->container.minLOD < 0) ? 0 : _ctc->container.minLOD;
					regs->r_TRILINEAR_0 = 1;
					break;
				case GL_LINEAR:
					regs->r_MIN_FMODE_0 = 1;
					regs->r_MAX_LOD_0 = 0;
					regs->r_MIN_LOD_0 = 0;
					regs->r_TRILINEAR_0 = 0;
					break;
				case GL_LINEAR_MIPMAP_NEAREST:
					regs->r_MIN_FMODE_0 = 1;
					regs->r_MAX_LOD_0 = surf->numLevels - 1;
					regs->r_MIN_LOD_0 = (_ctc->container.minLOD < 0) ? 0 : _ctc->container.minLOD;
					regs->r_TRILINEAR_0 = 0;
					break;
				case GL_LINEAR_MIPMAP_LINEAR:
					regs->r_MIN_FMODE_0 = 1;
					regs->r_MAX_LOD_0 = surf->numLevels - 1;
					regs->r_MIN_LOD_0 = (_ctc->container.minLOD < 0) ? 0 : _ctc->container.minLOD;
					regs->r_TRILINEAR_0 = 1;
					break;
			}
			regs->r_LOD_BIAS_0 = 0;
			regs->r_TDMODE_0 = surf->format == GL_ETC1_RGB8_NATIVE_DMP ? TX_TDM_ETC : TX_TDM_RGBA;
			break;
		case GL_SHADOW_DMP:
			regs->r_TMODE_0 = TX_TTM_ENV_SDW;
			regs->r_SHADOW_SPECIAL_ADRS = 1;
			regs->r_MAX_FMODE_0 = 1;
			regs->r_MIN_FMODE_0 = 1;
			regs->r_MAX_LOD_0 = 0;
			regs->r_MIN_LOD_0 = 0;
			regs->r_LOD_BIAS_0 = 0;
			regs->r_ADR_MODE_U_0 = TX_ADM_CLMP;
			regs->r_ADR_MODE_V_0 = TX_ADM_CLMP;
			regs->r_TDMODE_0 = TX_TDM_RGBA;
			break;
		default:
			return GL_INVALID_OPERATION;
	}
	regs->r_POINTER_0  = nngxlowGetPhysicalAddr((unsigned)_ctc->texture[0].surfaceContainer.picaaddr) >> 3;
	regs->r_POINTER1_0 = nngxlowGetPhysicalAddr((unsigned)_ctc->texture[1].surfaceContainer.picaaddr) >> 3;
	regs->r_POINTER2_0 = nngxlowGetPhysicalAddr((unsigned)_ctc->texture[2].surfaceContainer.picaaddr) >> 3;
	regs->r_POINTER3_0 = nngxlowGetPhysicalAddr((unsigned)_ctc->texture[3].surfaceContainer.picaaddr) >> 3;
	regs->r_POINTER4_0 = nngxlowGetPhysicalAddr((unsigned)_ctc->texture[4].surfaceContainer.picaaddr) >> 3;
	regs->r_POINTER5_0 = nngxlowGetPhysicalAddr((unsigned)_ctc->texture[5].surfaceContainer.picaaddr) >> 3;
	regs->r_ON_0 = 1;

	__cb_writeRegs(PA_WO_TU_BEGIN + 1, 0xa, hwtexstate->regs + 1);
	__cb_writeReg(PA_WO_TU_BEGIN + 0xe, hwtexstate->regs[0xe], 0xf);

	return GL_NO_ERROR;
}

static GLenum tx_validateCubeTexture(tx_cube_tex_container_t* _ctc){
	int face;
	srf_container_t* surf0;

	surf0 = &_ctc->texture[0].surfaceContainer;

	for (face = 1; face < 6; face++){
		srf_container_t* surf =  &_ctc->texture[face].surfaceContainer;
		if (!surf->picaaddr)
			break;

		if (surf0->numLevels != surf->numLevels || surf0->size != surf->size ||
			surf0->width != surf->width || surf0->height != surf->height ||
			surf0->format != surf->format || ((unsigned)surf0->picaaddr & 0xfe000000) != (((unsigned)surf->picaaddr + surf->size - 1) & 0xfe000000)
			|| (unsigned)surf0->picaaddr > (unsigned)surf->picaaddr)
			break;
	}
	if (face != 6)
		return GL_INVALID_OPERATION;
	
	return GL_NO_ERROR;
}