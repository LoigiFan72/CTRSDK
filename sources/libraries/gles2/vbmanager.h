#pragma once

#include "common.h"
#include "state.h"
#include "bitmask.h"
#include "shadermanager.h"

typedef enum VB_TYPE_T{
	VT_BUF = 0,
	VT_COLL,
	
	VT_MAX_BIT = (1u << 30)
} vb_type_t;

typedef struct VB_VERTBUFFER_T{
	void*					picaaddr;
	void*					copyaddr;
	void*					appaddr;
	GLsizeiptr				size;
	GLenum					usage;
	GLenum					allocarea;
	GLenum					transtype;
} vb_vertbuffer_t;

typedef struct VB_COLLECTION_T{
	stt_vert_array_state_t		arraystate;
	stt_vert_buf_state_t		bufstate;
	stt_vert_current_state_t	attrstate;
} vb_collection_t;

typedef struct VB_LIST_T{
	GLuint					id;
	vb_type_t				type;
	void*					container;
	struct VB_LIST_T*		next;
} vb_list_t;

#define VB_LIST_TABLE_SIZE		0x1ff

typedef struct VB_MANAGER_T{
	vb_list_t*			vb_list_table[VB_LIST_TABLE_SIZE + 1];
	vb_collection_t*	defaultCollection;
	GLuint				deleteCollection;
	vb_list_t*			boundarray;
	vb_list_t*			boundelementarray;
	vb_list_t*			boundattribarray[16];
	vb_list_t*			boundstatecollection;
} vb_manager_t;


GLint __vb_initializeVBManager(vb_manager_t* vbman);
void __vb_finalizeVBManager(void);
void __vb_setAttribArrayBuffer(GLuint index);

void __vbv_initializeVBValidator(bit_mask_t* _mask);
void __vbv_validateVBValidator(bit_mask_t* _mask);