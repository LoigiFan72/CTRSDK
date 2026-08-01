#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2CTR.h>
#include <GLES2/gl2extern.h>
#include <nn/gx/CTR/gx_CTR.h>
#include <nn/gx/CTR/gx_CTRRaw.h>
#include <nn/gxlow/CTR/gxlow_CTR.h>
#include <nn/gx/CTR/gx_Vram.h>
#include <nn/gxlow/CTR/gxlow_InterruptReceiver.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define GL_ACCELERATE_TRIANGLES_DMP       0x6502

#define MAX_TEXTURE_UNITS				3
#define MAX_TEXTURE_SIZE				1024
#define MIN_TEXTURE_SIZE				8
#define MAX_TEXTURE_BLENDING_UNITS		6
#define MAX_RENDERBUFFER_SIZE			1024
#define MAX_PER_PIXEL_LIGHTS			8
#define MAX_MATERIAL_LUTS				6
#define MATERIAL_LUT_TABLE_SIZE 		256
#define MAX_VERTEX_ATTRIBS				16
#define MAX_BUFFERED_VERTEX_ATTRIBS		12
#define MAX_VERTEX_UNITS				4
#define PROCTEX_LUT_TABLE_SIZE			128
#define MAX_LUT_TEXTURES				32
#define MAX_LUT_ENTRIES					512
#define MAX_PROCTEX_LUTS				7
#define MAX_GAS_LUTS					3
#define GAS_LUT_TABLE_SIZE				16
#define MAX_LAYER_CONFIG				8

#define GL_NO_COPY_NO_DMA_DMP             (NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP)
#define GL_COPY_NO_DMA_DMP                (NN_GX_MEM_FCRAM | GL_COPY_FCRAM_DMP)
#define GL_NO_COPY_DMA_VRAMA_DMP          (NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP)
#define GL_NO_COPY_DMA_VRAMB_DMP          (NN_GX_MEM_VRAMB | GL_NO_COPY_FCRAM_DMP)
#define GL_COPY_DMA_VRAMA_DMP             (NN_GX_MEM_VRAMA | GL_COPY_FCRAM_DMP)
#define GL_COPY_DMA_VRAMB_DMP             (NN_GX_MEM_VRAMB | GL_COPY_FCRAM_DMP)

#define _PICA_MEM_NATIVE(x)		(x)

typedef void* (*dmpgl_allocator_t)(GLenum, GLenum, GLuint, GLsizei);
typedef void (*dmpgl_deallocator_t)(GLenum, GLenum ,GLuint, void*);
extern dmpgl_allocator_t __dmpgl_allocator;
extern dmpgl_deallocator_t __dmpgl_deallocator;
#define malloc(arg) ((__dmpgl_allocator) ? __dmpgl_allocator(NN_GX_MEM_FCRAM, NN_GX_MEM_SYSTEM, 0, arg) : 0)
#define malloc_ext(arg0, arg1, arg2, arg3)	(__dmpgl_allocator) ? __dmpgl_allocator(arg0, arg1, arg2, arg3) : 0
#define free(arg)		(__dmpgl_deallocator) ? __dmpgl_deallocator(NN_GX_MEM_FCRAM, NN_GX_MEM_SYSTEM, 0, arg) : (void)0
#define free_ext(arg0, arg1, arg2, arg3)	(__dmpgl_deallocator) ? __dmpgl_deallocator(arg0, arg1, arg2, arg3) : (void)0