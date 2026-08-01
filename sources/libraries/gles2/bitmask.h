#pragma once

typedef enum inv_flag{
	IF_GL_FRAMEBUFFER,
	IF_GL_VERTBUFFER,
	IF_GL_TRI_OFFSET,
	IF_GL_LIGHTING_LUT_SAMPLERS,
	IF_GL_FOG_LUT_SAMPLER,
	IF_GL_PROCTEX_LUT_SAMPLERS,
	IF_GL_VERT_ARRAY,
	IF_GL_VERT_CURRENT,
	IF_GL_FB_ACCESS,
	IF_GL_FB_SCISSOR,
	IF_GL_TEXTURE0,
	IF_GL_TEXTURE1,
	IF_GL_TEXTURE2,
	IF_GL_TEXTURE3,
	IF_GL_TEXTURE_LUT,
	IF_GL_PROGRAM,
	IF_GL_SHADER_UNIFORM,
	IF_HW_TEXTURE,
	IF_HW_VERTBUFFER,
	IF_HW_RASTERIZER_REGISTER,
	IF_GL_BINARY_PACKAGE,
	IF_GL_BINARY_EXE_VS,
	IF_GL_BINARY_EXE_GS,
	IF_GL_ATTACH_GS,
	IF_GL_DETACH_GS,
	IF_GL_GAS_LUT_SAMPLERS,
	
	IF_MAX_BIT = (1u << 30)
} inv_flags_t;

typedef struct bit_mask{
	unsigned		mask_[1];
} bit_mask_t;

/* MACROS */

#define SET_MASK(mask, flag)	((mask).mask_[(flag) >> 5] |= (1 << ((flag) & 0x1f)))

#define GET_MASK(mask, flag)	((mask).mask_[(flag) >> 5] & (1 << ((flag) & 0x1f)))

#define COPY_MASK(lhs, rhs)				\
{										\
	(lhs).mask_[0] = (rhs).mask_[0];	\
}

#define ASSIGN_OR_MASK(lhs, rhs)		\
{										\
	(lhs).mask_[0] |= (rhs).mask_[0];	\
}

#define CLEAR_ALL_MASK(mask)			\
{										\
	(mask).mask_[0] = 0;				\
}

#define CLEAR_MASK(lhs, rhs)			\
{										\
	(lhs).mask_[0] &= ~(rhs).mask_[0];	\
}

#define SET_ALL_MASK(mask)				\
{										\
	(mask).mask_[0] = ~0;				\
}

#define AND_MASK(lhs, rhs)		(((lhs).mask_[0] & (rhs).mask_[0]))