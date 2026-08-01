#pragma once

#include "bitmask.h"
#include "texform.h"
#include "common.h"

typedef enum STT_LUT_ATTACHMENT_POINT_T{
	LAP_LR_LUT_START,
	LAP_LR_LUT_LR_D0 = LAP_LR_LUT_START,
	LAP_LR_LUT_LR_D1,
	LAP_LR_LUT_LR_FR,
	LAP_LR_LUT_LR_RB,
	LAP_LR_LUT_LR_RG,
	LAP_LR_LUT_LR_RR,
	LAP_LR_LUT_SP_START,
	LAP_LR_LUT_SP0 = LAP_LR_LUT_SP_START,
	LAP_LR_LUT_SP1,
	LAP_LR_LUT_SP2,
	LAP_LR_LUT_SP3,
	LAP_LR_LUT_SP4,
	LAP_LR_LUT_SP5,
	LAP_LR_LUT_SP6,
	LAP_LR_LUT_SP7,
	LAP_LR_LUT_DA_START,
	LAP_LR_LUT_DA0 = LAP_LR_LUT_DA_START,
	LAP_LR_LUT_DA1,
	LAP_LR_LUT_DA2,
	LAP_LR_LUT_DA3,
	LAP_LR_LUT_DA4,
	LAP_LR_LUT_DA5,
	LAP_LR_LUT_DA6,
	LAP_LR_LUT_DA7,
	LAP_PT_LUT_START,
	LAP_PT_LUT_RGB_MAP = LAP_PT_LUT_START,
	LAP_PT_LUT_A_MAP,
	LAP_PT_LUT_NOISE_MAP,
	LAP_PT_LUT_R,
	LAP_PT_LUT_G,
	LAP_PT_LUT_B,
	LAP_PT_LUT_A,
	LAP_FG_LUT_START,
	LAP_FG_LUT_FOG = LAP_FG_LUT_START,
	LAP_GS_LUT_START,
	LAP_GS_LUT_R = LAP_GS_LUT_START,
	LAP_GS_LUT_G,
	LAP_GS_LUT_B,
	LAP_LUT_COUNT,
	
	LAP_MAX_BIT = (1u << 30)
} stt_lut_attachment_point_t;

typedef struct STT_RENDER_MODE_T
{
	GLenum mode;
	GLenum prevmode;
	GLboolean drawing;
	GLboolean drawarray;
	GLboolean bAccerelateTriangle;
	GLboolean bGeometryShaderUsed;
	GLint currentProgram;

} stt_render_mode_t;


typedef struct STT_VIEWPORT_STATE_T
{
	GLint x;
	GLint y;
	GLsizei width;
	GLsizei height;
	GLboolean bInitted;
	char padding3[3];

} stt_viewport_state_t;


typedef struct STT_TRI_ORIENT_STATE_T
{
	GLenum frontFace;
	GLenum cullFace;
	GLboolean enabled;
	char padding3[3];

} stt_tri_orient_state_t;


typedef struct STT_TRI_OFFSET_STATE_T
{
	GLfloat m_offset;
	GLfloat u_offset;
	GLfloat w_scale;
	GLfloat depthRangeN;
	GLfloat depthRangeF;
	GLboolean enabled;
	char padding3[3];

} stt_tri_offset_state_t;


typedef struct STT_TEXTURE_UNITS_STATE_T
{
	GLenum activeTexture;

	GLuint bound2DTex[3];
	GLuint boundCubeTex[3];
	GLuint boundLutTex[32];
	GLuint boundTexColl;

	GLenum samplerType[3];

	GLboolean enabled2DTex[3];
	GLboolean enabledCubeTex[3];

	char padding2[2];

} stt_texture_units_state_t;


typedef struct STT_LUT_BINDINGS_T
{
	GLuint bindings[LAP_LUT_COUNT];
	GLuint subsize[LAP_LUT_COUNT];
	GLuint suboffset[LAP_LUT_COUNT];

} stt_lut_bindings_t;


typedef struct STT_VERT_CURRENT_STATE_T
{
	GLfloat attrib[16][4];
	GLuint attrib24[16][3];

} stt_vert_current_state_t;


typedef struct STT_VERT_ARR_T
{
	const GLvoid* ptr;
	GLint size;
	GLenum type;
	GLsizei stride;
	GLuint buffer;
	GLboolean norm;
	GLboolean enabled;
	char padding2[2];

} stt_vert_arr_t;


typedef struct STT_VERT_ARRAY_STATE_T
{
	stt_vert_arr_t array[16];

} stt_vert_array_state_t;


typedef struct STT_VERT_BUF_STATE_T
{
	GLuint elmtArrayID;
	GLuint arrayID;

} stt_vert_buf_state_t;


typedef struct STT_PER_FRAG_OP_STATE_T
{
	GLint scissorLeft;
	GLint scissorBottom;
	GLsizei scissorWidth;
	GLsizei scissorHeight;

	GLenum alphaFunc;
	GLclampf alphaRefValue;

	GLenum stencilFunc;
	GLint stencilRefValue;
	GLuint stencilValueMask;

	GLenum stencilOpFail;
	GLenum stencilOpDepthFail;
	GLenum stencilOpDepthPass;

	GLenum depthFunc;

	GLenum blendFuncSrcRGB;
	GLenum blendFuncSrcAlpha;
	GLenum blendFuncDstRGB;
	GLenum blendFuncDstAlpha;

	GLenum blendEqRGB;
	GLenum blendEqAlpha;

	GLclampf blendColor[4];

	GLenum logicOp;
	GLenum mode;

	GLboolean scissorTestEnabled;
	GLboolean alphaTestEnabled;
	GLboolean stencilTestEnabled;
	GLboolean depthTestEnabled;
	GLboolean blendEnabled;
	GLboolean logicOpEnabled;

	char padding1[1];

	GLboolean earlyDepthTestEnabled;
	GLenum earlyDepthFunc;

} stt_per_frag_op_state_t;


typedef struct STT_FB_OP_STATE_T
{
	GLboolean colorMask[4];
	GLboolean depthMask;

	char padding3[3];

	GLuint stencilWriteMask;

	GLclampf clearColor[4];

	GLclampf clearDepth;
	GLuint clearEarlyDepth;
	GLint clearStencil;

	unsigned clearDepth24;
	unsigned clearDepth16;

} stt_fb_op_state_t;


typedef struct STT_FB_STATE_T
{
	GLenum status;
	enum C_FORMAT color_format;
	enum Z_FORMAT z_format;

	GLboolean colorBufferEnabled;
	GLboolean depthBufferEnabled;
	GLboolean stencilBufferEnabled;
	GLboolean block32Enabled;
	GLboolean depthCopyMode;

	char padding3[3];

	GLsizei width;
	GLsizei height;

} stt_fb_state_t;



typedef struct STT_GL_STATE_T
{
	stt_render_mode_t rendermode;
	stt_viewport_state_t viewport;
	stt_tri_orient_state_t triorient;
	stt_tri_offset_state_t trioffset;

	stt_texture_units_state_t textureunits;
	stt_lut_bindings_t lutbindings;

	stt_vert_current_state_t vertcurr;
	stt_vert_array_state_t vertarray;
	stt_vert_buf_state_t vertbuffer;

	GLuint vertexStateCollection;

	stt_per_frag_op_state_t per_frag_op;
	stt_fb_op_state_t fb_op;
	stt_fb_state_t fb_state;

} stt_gl_state_t;



typedef struct STT_HW_ARRAY_PROC_STATE_T
{
	GLsizeiptr elmtArrOffset;
	GLsizeiptr attrArrOffset[16];

	GLsizeiptr baseAddr;

	GLsizeiptr hostElmtArrOffset;
	GLsizeiptr hostAttrArrOffset[16];
	GLsizeiptr hostStride[16];

	GLboolean bUseDeletedBuffer;
	char padding3[3];

} stt_hw_array_proc_state_t;


typedef struct STT_HW_TEX_UNITS_STATE_T
{
	GLuint regs[31];
	GLuint addresses[6];

} stt_hw_tex_units_state_t;


typedef struct STT_HW_FRAMEBUFFER_STATE_T
{
	GLuint bitWidthColor;
	GLuint bitWidthZ;

	GLuint addrZ;
	GLuint addrColor;

	GLuint sizeZ;
	GLuint sizeColor;

} stt_hw_framebuffer_state_t;


typedef struct STT_HW_SHADER_STATE_T
{
	GLboolean useVertexBuffers;
	char padding3[3];

	GLuint attrNumUsed;
	GLuint attrNumTotalUsed;

	GLuint attrUseOrder[16];

} stt_hw_shader_state_t;


typedef struct STT_HW_STATE_T
{
	stt_hw_array_proc_state_t hw_arrayproc;
	stt_hw_tex_units_state_t hw_texunits;
	stt_hw_framebuffer_state_t hw_framebuffer;
	stt_hw_shader_state_t hw_shader;

} stt_hw_state_t;



typedef struct STT_STATE_T
{
	bit_mask_t upd_mask;

	GLbitfield forceValidateMask;
	GLbitfield suppressStateMask;

	GLboolean forceMode;
	char padding3[3];

	stt_gl_state_t gl_state;
	stt_hw_state_t hw_state;

} stt_state_t;



GLint __sta_initializeState(stt_state_t* glstate);
void __sta_finalizeState();

extern stt_state_t* __gl_state;

#define GET_CURRENT_STATE(state) stt_state_t* state = __gl_state;