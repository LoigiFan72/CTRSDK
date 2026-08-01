// Filename: commandbuffer.c
//
// Project: Horizon

#include "common.h"
#include "error.h"
#include "commandbuffer.h"

unsigned* __cb_current_command_buffer = 0;
unsigned* __cb_current_max_command_buffer = 0;

void __cb_multiWriteReg(GLuint _addr, GLuint _count, GLuint* _data){
	GLuint offset;
	unsigned *dst = __cb_current_command_buffer;
	
	if ((unsigned)__cb_current_command_buffer + ((_count & ~1) + ((_count >> 7) + 1) * 2) * 4 >= (unsigned)__cb_current_max_command_buffer){
		__cb_current_command_buffer = __cb_current_max_command_buffer;
		return;
	}

	for (offset = 0; offset < _count; offset += 0x80){
		GLuint count;
		unsigned *src;

		src = (unsigned*)(_data + offset);

		*dst++ = *src++;
		count = ((_count - offset) > 0x80) ? 0x80 : _count - offset;
		*dst++ = _addr | (0xf << 16) | ((count - 1) << 20);		/* set burst header */

		count &= ~1;

		while (count & 0xe){
			count -= 2;
			*dst++ = *src++;
			*dst++ = *src++;
		}

		while (count){
			count -= 16;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
			*dst++ = *src++;
		}
	}

	__cb_current_command_buffer = dst;

	return;
}

void __cb_writeRegs(GLuint _addr, GLuint _count, GLuint* _data){
	GLuint offset;
	unsigned* dst = __cb_current_command_buffer;

	if ((unsigned)__cb_current_command_buffer + ((_count & ~1) + ((_count >> 7) + 1) * 2) * 4 >= (unsigned)__cb_current_max_command_buffer){
		__cb_current_command_buffer = __cb_current_max_command_buffer;
		return;
	}
	
	for (offset = 0; offset < _count; offset += 0x80){
		GLuint count;
		GLuint addr;
		unsigned* src;

		addr = _addr + offset;
		src = (unsigned*)(_data + offset);

		*dst++ = *src++;
		count = ((_count - offset) > 0x80) ? 0x80 : _count - offset;
		*dst++ = addr | (0xf << 16) | ((count - 1) << 20) | ((unsigned)0x1 << 31);

		count >>= 1;

		while (count){
			count--;
			*dst++ = *src++;
			*dst++ = *src++;
		}
	}

	__cb_current_command_buffer = dst;

	return;
}

void __cb_fillRegs(GLuint _addr, GLuint _count, GLuint _data){
	GLuint offset;
	unsigned* dst = __cb_current_command_buffer;

	if ((unsigned)__cb_current_command_buffer + ((_count & ~1) + ((_count >> 7) + 1) * 2) * 4 >= (unsigned)__cb_current_max_command_buffer){
		__cb_current_command_buffer = __cb_current_max_command_buffer;
		return;
	}
	
	for (offset = 0; offset < _count; offset += 0x80){
		GLuint count;
		GLuint addr;

		addr = _addr + offset;

		*dst++ = _data;
		count = ((_count - offset) > 0x80) ? 0x80 : _count - offset;
		*dst++ = addr | (0xf << 16) | ((count - 1) << 20) | ((unsigned)0x1 << 31);

		count >>= 1;

		while (count){
			count--;
			*dst++ = _data;
			*dst++ = _data;
		}
	}

	__cb_current_command_buffer = dst;

	return;
}

void __cb_addDummyWrite(GLuint _addr, GLuint _count){
	if ((unsigned)__cb_current_command_buffer + (_count + 2) * 4 >= (unsigned)__cb_current_max_command_buffer){
		__cb_current_command_buffer = __cb_current_max_command_buffer;
		return;
	}
	
	if (_count){
		_count -= 1;
		*__cb_current_command_buffer++ = 0;
		*__cb_current_command_buffer++ = _addr | (_count << 20);
		if (_count & 1)
			_count += 1;
		__cb_current_command_buffer += _count;
	}
	
	return;
}

unsigned* __cb_writeRegsBuffer(GLuint _addr, GLuint _count, GLuint* _data, unsigned* _command_buffer){
	GLuint offset;
	unsigned* dst = _command_buffer;

	for (offset = 0; offset < _count; offset += 0x80){
		GLuint count;
		GLuint addr;
		unsigned* src;

		addr = _addr + offset;
		src = (unsigned*)(_data + offset);

		*dst++ = *src++;
		count = ((_count - offset) > 0x80) ? 0x80 : _count - offset;
		*dst++ = addr | (0xf << 16) | ((count - 1) << 20) | ((unsigned)0x1 << 31);

		count >>= 1;

		while (count){
			count--;
			*dst++ = *src++;
			*dst++ = *src++;
		}
	}

	return dst;
}