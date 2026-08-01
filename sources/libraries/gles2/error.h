#pragma once

#include "common.h"

/* ERR Macro */

#if !defined(NN_PLATFORM_CTR) || defined(NN_BUILD_DEBUG) || defined(NN_BUILD_VERBOSE)

void __err_setError(GLenum err);

#define BASE_GL_FAIL_IF(cond, err)			\
	if (cond)								\
	{										\
		__err_setError(err);				\
		return;								\
	}

#define BASE_GL_FAIL_IF_RET(cond, err, r)	\
	if (cond)								\
	{										\
		__err_setError(err);				\
		return r;							\
	}

#define BASE_GL_FAIL_IF_CONT(cond, err)		\
	if (cond)								\
	{										\
		__err_setError(err);				\
		continue;							\
	}

#define BASE_GL_FAIL_IF_BRK(cond, err)		\
	if (cond)								\
	{										\
		__err_setError(err);				\
		break;								\
	}

#define BASE_GL_FAIL_IF_NORET(cond, err)	\
	if (cond)								\
	{										\
		__err_setError(err);				\
	}

#else

#pragma diag_suppress 550,826
#define BASE_GL_FAIL_IF(cond, err)
#define BASE_GL_FAIL_IF_RET(cond, err, r)
#define BASE_GL_FAIL_IF_CONT(cond, err)
#define BASE_GL_FAIL_IF_BRK(cond, err)
#define BASE_GL_FAIL_IF_NORET(cond, err)
#define __err_setError(err)


#endif