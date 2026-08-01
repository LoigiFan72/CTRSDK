#pragma once

#include "bitmask.h"
#include "common.h"

typedef struct VLD_VALIDATOR_FRAG_INFO_T{
	bit_mask_t		texture;
	bit_mask_t		fbman;
	bit_mask_t		vbman;
	bit_mask_t		shman;
} vld_validator_frag_info_t;

GLint __vldtr_initializeStateValidator(vld_validator_frag_info_t* finfo);
void __vldtr_finalizeStateValidator(void);
void __vldtr_validateAllState(void);