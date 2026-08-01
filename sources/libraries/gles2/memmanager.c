// Filename: memmanager.c
//
// Project: Horizon

#include "memmanager.h"

void __mem_readMemory(GLuint _addr, GLubyte* _data, GLuint _count){
	if (_count){
		memcpy(_data, (void*)_PICA_MEM_NATIVE(_addr), _count);
	}
	
	return;
}