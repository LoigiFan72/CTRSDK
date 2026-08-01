#pragma once

#include "common.h"

extern unsigned* __cb_current_command_buffer;
extern unsigned* __cb_current_max_command_buffer;

#define __cb_writeReg(_addr, _data, _be) \
	do \
	{ \
		if (__cb_current_command_buffer < __cb_current_max_command_buffer) { \
			*__cb_current_command_buffer++ = _data; \
			*__cb_current_command_buffer++ = _addr | (_be << 16); \
		} \
	} while (0)

void __cb_multiWriteReg(GLuint _addr, GLuint _count, GLuint* _data);
void __cb_writeRegs(GLuint _addr, GLuint _count, GLuint* _data);
void __cb_fillRegs(GLuint _addr, GLuint _count, GLuint _data);
void __cb_addDummyWrite(GLuint _addr, GLuint _count);

/* command buffer empty and overflow check */
#define __cb_isCommandbufferEnabled		((__cb_current_command_buffer != __cb_current_max_command_buffer) ? 1 : 0)
#define __cb_isCommandbufferEmpty		(__cb_current_command_buffer == 0)

#define __cb_writeRegBuffer(_addr, _data, _be, _command_buffer) \
	do \
	{ \
		*_command_buffer++ = _data; \
		*_command_buffer++ = _addr | (_be << 16); \
	} while (0)

unsigned* __cb_writeRegsBuffer(GLuint _addr, GLuint _count, GLuint* _data, unsigned* _command_buffer);