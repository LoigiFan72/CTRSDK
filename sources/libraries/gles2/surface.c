// Filename: surface.c
//
// Project: Horizon

#include "common.h"
#include "state.h"
#include "error.h"
#include "surface.h"

void __srf_initSurface(GLsizei width, GLsizei height, GLenum format, GLenum type, GLint numLevels, GLsizei size, srf_container_t* container){
	assert(container->picaaddr == 0 && container->copyaddr == 0 && container->appaddr == 0);
	container->picaaddr = 0;
	container->copyaddr = 0;
	container->appaddr = 0;
	container->width = width;
	container->height = height;
	container->format = format;
	container->type = type;
	container->numLevels = numLevels;
	container->allocarea = 0;
	
	switch (format){
		case GL_RGBA8_OES:
			container->residentType = CF_R8G8B8A8;
			container->compSize[0] = 8;
			container->compSize[1] = 8;
			container->compSize[2] = 8;
			container->compSize[3] = 8;
			container->pixelSize = 32;
			break;
		case GL_RGBA:
			switch (type){
				case GL_UNSIGNED_SHORT_4_4_4_4:
					container->residentType = TF_R4G4B4A4;
					container->compSize[0] = 4;
					container->compSize[1] = 4;
					container->compSize[2] = 4;
					container->compSize[3] = 4;
					container->pixelSize = 16;
					break;
				case GL_UNSIGNED_SHORT_5_5_5_1:
					container->residentType = TF_R5G5B5A1;
					container->compSize[0] = 5;
					container->compSize[1] = 5;
					container->compSize[2] = 5;
					container->compSize[3] = 1;
					container->pixelSize = 16;
					break;
				case GL_UNSIGNED_BYTE:
					container->residentType = TF_R8G8B8A8;
					container->compSize[0] = 8;
					container->compSize[1] = 8;
					container->compSize[2] = 8;
					container->compSize[3] = 8;
					container->pixelSize = 32;
					break;
			}
			break;
		case GL_RGB8_OES:
			container->residentType = TF_R8G8B8;
			container->compSize[0] = 8;
			container->compSize[1] = 8;
			container->compSize[2] = 8;
			container->compSize[3] = 0;
			container->pixelSize = 24;
			break;
		case GL_RGB:
			switch (type){
				case GL_UNSIGNED_BYTE:
					container->residentType = TF_R8G8B8;
					container->compSize[0] = 8;
					container->compSize[1] = 8;
					container->compSize[2] = 8;
					container->compSize[3] = 0;
					container->pixelSize = 24;
					break;
				case GL_UNSIGNED_SHORT_5_6_5:
					container->residentType = TF_R5G6B5;
					container->compSize[0] = 5;
					container->compSize[1] = 6;
					container->compSize[2] = 5;
					container->compSize[3] = 0;
					container->pixelSize = 16;
					break;
			}
			break;
		case GL_SHADOW_DMP:
			container->residentType = TF_R8G8B8A8;
			container->compSize[0] = 8;
			container->compSize[1] = 8;
			container->compSize[2] = 8;
			container->compSize[3] = 8;
			container->pixelSize = 32;
			break;
		case GL_GAS_DMP:
			container->residentType = TF_R8G8B8A8;
			container->compSize[0] = 8;
			container->compSize[1] = 8;
			container->compSize[2] = 8;
			container->compSize[3] = 8;
			container->pixelSize = 32;
			break;
		case GL_ALPHA:
            switch(type){
				case GL_UNSIGNED_BYTE:
					container->residentType = TF_A8;
					container->compSize[0] = 0;
					container->compSize[1] = 0;
					container->compSize[2] = 0;
					container->compSize[3] = 8;
					container->pixelSize = 8;
					break;
				case GL_UNSIGNED_4BITS_DMP:
					container->residentType = TF_A4;
					container->compSize[0] = 0;
					container->compSize[1] = 0;
					container->compSize[2] = 0;
					container->compSize[3] = 4;
					container->pixelSize = 4;
					break;
			}
			break;
		case GL_LUMINANCE:
			switch (type){
				case GL_UNSIGNED_BYTE:
					container->residentType = TF_L8;
					container->compSize[0] = 8;
					container->compSize[1] = 8;
					container->compSize[2] = 8;
					container->compSize[3] = 0;
					container->pixelSize = 8;
					break;
				case GL_UNSIGNED_4BITS_DMP:
					container->residentType = TF_L4;
					container->compSize[0] = 4;
					container->compSize[1] = 4;
					container->compSize[2] = 4;
					container->compSize[3] = 0;
					container->pixelSize = 4;
					break;
			}
			break;
		case GL_LUMINANCE_ALPHA:
			switch (type){
				case GL_UNSIGNED_BYTE:
					container->residentType = TF_L8A8;
					container->compSize[0] = 8;
					container->compSize[1] = 8;
					container->compSize[2] = 8;
					container->compSize[3] = 8;
					container->pixelSize = 16;
					break;
				case GL_UNSIGNED_BYTE_4_4_DMP:
					container->residentType = TF_L4A4;
					container->compSize[0] = 4;
					container->compSize[1] = 4;
					container->compSize[2] = 4;
					container->compSize[3] = 4;
					container->pixelSize = 8;
					break;
			}
			break;
		case GL_HILO8_DMP:
			container->residentType = TF_R8G8;
			container->compSize[0] = 8;
			container->compSize[1] = 8;
			container->compSize[2] = 0;
			container->compSize[3] = 0;
			container->pixelSize = 16;
			break;
		case GL_DEPTH_COMPONENT16:
			container->residentType = ZF_Z16;
			container->compSize[0] = 16;
			container->compSize[1] = 0;
			container->compSize[2] = 0;
			container->compSize[3] = 0;
			container->pixelSize = 16;
			break;
		case GL_DEPTH_COMPONENT24_OES:
			container->residentType = ZF_Z24;
			container->compSize[0] = 24;
			container->compSize[1] = 0;
			container->compSize[2] = 0;
			container->compSize[3] = 0;
			container->pixelSize = 24;
			break;
		case GL_DEPTH24_STENCIL8_EXT:
			container->residentType = ZF_Z24S8;
			container->compSize[0] = 24;
			container->compSize[1] = 8;
			container->compSize[2] = 0;
			container->compSize[3] = 0;
			container->pixelSize = 32;
			break;
		case GL_RGB565:
			container->residentType = CF_R5G6B5;
			container->format = GL_RGB;
			container->type = GL_UNSIGNED_SHORT_5_6_5;
			container->compSize[0] = 5;
			container->compSize[1] = 6;
			container->compSize[2] = 5;
			container->compSize[3] = 0;
			container->pixelSize = 16;
			break;
		case GL_RGBA4:
			container->residentType = CF_R4G4B4A4;
			container->format = GL_RGBA;
			container->type = GL_UNSIGNED_SHORT_4_4_4_4;
			container->compSize[0] = 4;
			container->compSize[1] = 4;
			container->compSize[2] = 4;
			container->compSize[3] = 4;
			container->pixelSize = 16;
			break;
		case GL_RGB5_A1:
			container->residentType = CF_R5G5B5A1;
			container->format = GL_RGBA;
			container->type = GL_UNSIGNED_SHORT_5_5_5_1;
			container->compSize[0] = 5;
			container->compSize[1] = 5;
			container->compSize[2] = 5;
			container->compSize[3] = 1;
			container->pixelSize = 16;
			break;
		case GL_ETC1_RGB8_NATIVE_DMP:
			container->residentType = TF_ETC;
			container->compSize[0] = 8;
			container->compSize[1] = 8;
			container->compSize[2] = 8;
			container->compSize[3] = 0;
			container->pixelSize = 0;
			container->size = size;
			break;
		case GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP:
			container->residentType = TF_ETCA4;
			container->compSize[0] = 8;
			container->compSize[1] = 8;
			container->compSize[2] = 8;
			container->compSize[3] = 4;
			container->pixelSize = 0;
			container->size = size;
			break;
	}
	
	if (format != GL_ETC1_RGB8_NATIVE_DMP && format != GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP){
		int w, h;
		int i;
		
		container->size = 0;
		
		for (i = 0, w = width, h = height;
			i < numLevels && w >= MIN_TEXTURE_SIZE && h >= MIN_TEXTURE_SIZE;
				i++, w >>= 1, h >>= 1)
		{
			container->size += w * h * container->pixelSize / 8;
		}
	}
	
	return;
}