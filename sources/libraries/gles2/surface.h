#pragma once

#include "common.h"

typedef struct SRF_CONTAINER_T{
	void* picaaddr;
	void* copyaddr;
	void* appaddr;
	GLsizei	width;
	GLsizei	height;
	GLenum format;
	GLenum type;
	unsigned residentType;
	GLsizei size;
	GLint compSize[4];
	GLint numLevels;
	GLint pixelSize;
	GLenum allocarea;
} srf_container_t;

void __srf_initSurface(GLsizei width, GLsizei height, GLenum format, GLenum type, GLint numLevels, GLsizei size, srf_container_t* container);