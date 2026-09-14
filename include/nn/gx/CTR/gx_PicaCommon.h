#pragma once

#include <GLES2/gl2.h>
#include "nn/gx/CTR/gx_PicaReg.h"

#define CMD_PADDING_NAME2( x, name ) u32 padding_##name : x
#define CMD_PADDING_NAME1( x, name ) CMD_PADDING_NAME2( x, name )
#define CMD_PADDING( x )  CMD_PADDING_NAME1( x, __LINE__ )

#define PICA_CMD_HEADER( addr, size, be, seq )  ((addr) | (be) << 16 | (size) << 20 | ((seq) ? 0x80000000 : 0))

#define PICA_CMD_HEADER_BURST( addr, size )            PICA_CMD_HEADER(addr, size - 1, 0xf, 0)
#define PICA_CMD_HEADER_BURST_BE( addr, size, be )     PICA_CMD_HEADER(addr, size - 1,  be, 0)
#define PICA_CMD_HEADER_BURSTSEQ( addr, size )         PICA_CMD_HEADER(addr, size - 1, 0xf, 1)
#define PICA_CMD_HEADER_BURSTSEQ_BE( addr, size, be )  PICA_CMD_HEADER(addr, size - 1,  be, 1)

#define PICA_CMD_HEADER_SINGLE( addr )                 PICA_CMD_HEADER(addr, 0, 0xf, 0)
#define PICA_CMD_HEADER_SINGLE_BE( addr, be )          PICA_CMD_HEADER(addr, 0,  be, 0)

#define PICA_CMD_HEADER_VS_F32( size )           PICA_CMD_HEADER(PICA_REG_VS_FLOAT0, (size) * 4 -1, 0xf, 0)

#define PICA_CMD_HEADER_GS_F32( size )           PICA_CMD_HEADER(PICA_REG_GS_FLOAT0, (size) * 4 -1, 0xf, 0)

#define PICA_CMD_DATA_ZERO() 0x00000000

typedef struct _color8
{
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
} color8;

#define PICA_CMD_DATA_COLOR_RGB8( r, g, b ) \
    ( r | g << 8 | b << 16 )

#define PICA_CMD_DATA_COLOR_RGBA8( r, g, b, a ) \
    ( r | g << 8 | b << 16 | a << 24 )