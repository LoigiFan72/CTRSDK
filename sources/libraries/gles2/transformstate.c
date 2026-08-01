// Filename: transformstate.c
//
// Project: Horizon

#include "common.h"
#include "state.h"
#include "error.h"
#include "util.h"
#include "picaregmap.h"
#include "georegmap.h"
#include "commandbuffer.h"

void GL_APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height){
	unsigned u ;
	GET_CURRENT_STATE(state);
	BASE_GL_FAIL_IF(x < 0 || y < 0 || width < 0 || height < 0, GL_INVALID_VALUE);
	
	UTL_F2F_16M7E(width / 2.f, u);
	__cb_writeReg(PA_RW_TS_WIDTH2, u, 0xf);
	UTL_F2F_16M7E(height / 2.f, u);
	__cb_writeReg(PA_RW_TS_HEIGHT2, u, 0xf);

	if (width && height){
		unsigned size[2];
		int i;
		size[0] = width;
		size[1] = height;
		for (i = 0; i < 2; i++){
			switch (size[i]){
				case 240:	size[i] = 0x38111111;	break;
				case 320:	size[i] = 0x37999999;	break;
				case 400:	size[i] = 0x3747ae14;	break;
				case 480:	size[i] = 0x37111111;	break;
				case 600:	size[i] = 0x36b4e81b;	break;
				case 640:	size[i] = 0x36999999;	break;
				case 800:	size[i] = 0x3647ae14;	break;
				case 1024:	size[i] = 0x36000000;	break;
				case 1200:	size[i] = 0x35b4e81b;	break;
				case 1280:	size[i] = 0x35999999;	break;
				default:
					UTL_F2F_23M7E((2.f / (float)size[i]), size[i]);
					size[i] <<= 1;
					break;
			}
			__cb_writeReg(PA_RW_TS_RWIDTH2 + 2 * i, size[i], 0xf);
		}
	}

	__cb_writeReg(PA_RW_TI_OFFSET, (x | y << 16), 0xf);

	state->gl_state.viewport.x = x;
	state->gl_state.viewport.y = y;
	state->gl_state.viewport.width = width;
	state->gl_state.viewport.height = height;
	if (!state->gl_state.viewport.bInitted)
		state->gl_state.viewport.bInitted = GL_TRUE;
	BASE_GL_FAIL_IF(!__cb_isCommandbufferEnabled, GL_ERROR_COMMANDBUFFER_FULL_DMP);

	return;
}

