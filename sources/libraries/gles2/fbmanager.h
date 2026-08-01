#pragma once

#include "surface.h"
#include "bitmask.h"

typedef enum FB_ATTACH_POINT{
	FB_AP_COLOR0,
	FB_AP_DEPTH_STENCIL,
	FB_AP_MAX_POINTS,
	
	FB_AP_MAX_BIT = (1u << 30)
} fb_attach_point_t;

typedef enum FB_ATT_TYPE{
	FB_AT_RENDERBUFFER,
	FB_AT_TEXTURE,
	
	FB_AT_MAX_BIT = (1u << 30)
} fb_att_type_t;

typedef struct FB_ATT_INFO_T{
	fb_att_type_t type;
	GLint id;
	GLenum target;
	srf_container_t* surfaceContainer;
} fb_att_info_t;

typedef struct FB_LIST_T{
	fb_att_info_t AttInfo[FB_AP_MAX_POINTS];
	GLuint id;
	struct FB_LIST_T* next;
} fb_list_t;

typedef struct RB_LIST_T{
	srf_container_t	surfaceContainer;
	GLuint id;
	struct RB_LIST_T* next;
} rb_list_t;

typedef struct FB_MANAGER_T{
	fb_list_t* fbListTop;
	rb_list_t* rbListTop;
	fb_list_t* currFB;
	rb_list_t* currRB;
} fb_manager_t;

GLint __fb_initializeFBManager(fb_manager_t* fbman);
void __fb_finalizeFBManager(void);
void __fb_getAttachmentParameteriv(GLenum _pname, GLint* _params);
void __fbv_initializeFBValidator(bit_mask_t* _mask);
void __fbv_validateFBValidator(bit_mask_t* _mask);
void __fb_detachSurfaceContainer(srf_container_t* _pCont);
srf_container_t* __fb_getCurrentFBColorSurface(void);