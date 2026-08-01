// Filename: error.c
//
// Project: Horizon

#include "common.h"
#include "error.h"

static GLenum __error = GL_NO_ERROR;

GLenum GL_APIENTRY glGetError(void){
	GLenum err = __error;
	__error = GL_NO_ERROR;
	
	return err;
}

#ifdef DMPGL_GEN_ERROR

void __err_setError(GLenum err){
	if (__error == GL_NO_ERROR)
		__error = err;
	
	return;
}

#endif