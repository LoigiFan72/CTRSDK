#pragma once

#include "common.h"

#define UTL_CLAMP(x)		(((x) <= 0.f) ? 0.f : ((x) >= 1.f) ? 1.f : (x))

#define UTL_FLOAT2BOOL(x)	(((x) == 0.f) ? GL_FALSE : GL_TRUE)
#define UTL_INT2BOOL(x)		((x) ? GL_TRUE : GL_FALSE)
#define UTL_ENUM2BOOL(x)	((x) ? GL_TRUE : GL_FALSE)
#define UTL_FIXED2BOOL(x)	((x) ? GL_TRUE : GL_FALSE)

#define UTL_BOOL2FLOAT(x)	(((x) == GL_TRUE) ? 1.f : 0.f)
#define UTL_INT2FLOAT(x)	((GLfloat)(x))
#define UTL_FIXED2FLOAT(x)	((GLfloat)(x)/65536.f)
#define UTL_ENUM2FLOAT(x)	((GLfloat)(x))

#define UTL_BOOL2INT(x)		(((x) == GL_TRUE) ? 1 : 0)
#define UTL_ENUM2INT(x)		((GLint)(x))
#define UTL_FLOAT2INT(x)	((GLint)(x))
#define UTL_FLOAT2INTCOL(x)	((GLint)(((x) * 4294967295.f - 1.f) * 0.5f))
#define UTL_FIXED2INT(x)	((GLint)(x))

#define UTL_FLOAT2FIXED(x)	((GLfixed)((x) * 65536.f))
#define UTL_INT2FIXED(x)	((GLfixed)(x))
#define UTL_BOOL2FIXED(x)	(((x) == GL_TRUE) ? 1 : 0)
#define UTL_ENUM2FIXED(x)	((GLfixed)(x))

#define UTL_FLOAT2ENUM(x)	((GLenum)(x))
#define UTL_FIXED2ENUM(x)	((GLenum)(x))
#define UTL_INT2ENUM(x)		(x)


#define UTL_F2C_8(_c)		((unsigned)(0.5f + (_c > 1.0f ? 1.f : _c) * (float)((1 << 8) - 1)))
#define UTL_F2C_8b(_c)		((unsigned)(0.5f + (_c) * (float)((1 << 8) - 1)))

#define UTL_F2F_12M_7E(_inarg, _outarg) \
	{ \
		unsigned uval_, m_; \
		int e_; \
		float f_; \
		static const int bias_ = 128 - (1 << (7 - 1)); \
		f_ = (_inarg); \
		uval_ = *(unsigned*)&f_; \
		e_ = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0; \
		m_ = (uval_ & 0x7fffff) >> (23 - 12); \
		if (e_ >= 0) \
			_outarg = m_ | (e_ << 12) | ((uval_ >> 31) << (12 + 7)); \
		else \
			_outarg = ((uval_ >> 31) << (12 + 7)); \
	}

#define UTL_F2FX_13W_5I_T(_inarg, _outarg) \
	{ \
		float f_; \
		unsigned v_; \
		f_ = (_inarg); \
		v_ = *(unsigned*)&f_; \
		if (f_ == 0.f || (v_ & 0x7f800000) == 0x7f800000) \
			_outarg = 0; \
		else \
		{ \
			f_ += 0.5f * (1 << 5); \
			f_ *= 1 << (13 - 5); \
			if (f_ < 0) \
				f_ = 0; \
			else if (f_ >= (1 << 13)) \
				f_ = (1 << 13) - 1; \
			if (f_ >= (1 << (13 - 1))) \
				_outarg = (unsigned)(f_ - (1 << (13 - 1))); \
			else \
				_outarg = (unsigned)(f_ + (1 << (13 - 1))); \
		} \
	}

#define UTL_F2F_10M5E(_inarg, _outarg) \
	{ \
		unsigned uval_, m_; \
		int e_; \
		float f_; \
		static const int bias_ = 128 - (1 << (5 - 1)); \
		f_ = (_inarg); \
		uval_ = *(unsigned*)&f_; \
		e_ = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0; \
		m_ = (uval_ & 0x7fffff) >> (23 - 10); \
		if (e_ >= 0) \
			_outarg = m_ | (e_ << 10) | ((uval_ >> 31) << (10 + 5)); \
		else \
			_outarg = ((uval_ >> 31) << (10 + 5)); \
	}

#define UTL_F2UFX_16W_0I(_inarg, _outarg) \
	{ \
		float f_ = (_inarg); \
		unsigned val_; \
		unsigned v_ = *(unsigned*)&f_; \
		if (f_ <= 0 || (v_ & 0x7f800000) == 0x7f800000) \
			val_ = 0; \
		else \
		{ \
			f_ *= 1 << (16 - 0); \
			if (f_ >= (1 << 16)) \
				val_ = (1 << 16) - 1; \
			else \
				val_ = (unsigned)(f_); \
		} \
		(_outarg) = val_; \
	}

#define UTL_F2UFX_24W_0I(_inarg, _outarg) \
	{ \
		float f_ = (_inarg); \
		unsigned val_; \
		unsigned v_ = *(unsigned*)&f_; \
		if (f_ <= 0 || (v_ & 0x7f800000) == 0x7f800000) \
			val_ = 0; \
		else \
		{ \
			f_ *= 1 << (24 - 0); \
			if (f_ >= (1 << 24)) \
				val_ = (1 << 24) - 1; \
			else \
				val_ = (unsigned)(f_); \
		} \
		(_outarg) = val_; \
	}

#define UTL_F2F_16M7E(_inarg, _outarg) \
	{ \
		unsigned uval_, m_; \
		int e_; \
		float f_; \
		static const int bias_ = 128 - (1 << (7 - 1)); \
		f_ = (_inarg); \
		uval_ = *(unsigned*)&f_; \
		e_ = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0; \
		m_ = (uval_ & 0x7fffff) >> (23 - 16); \
		if (e_ >= 0) \
			_outarg = m_ | (e_ << 16) | ((uval_ >> 31) << (16 + 7)); \
		else \
			_outarg = ((uval_ >> 31) << (16 + 7)); \
	}

#define UTL_F2F_23M7E(_inarg, _outarg) \
	{ \
		unsigned uval_, m_; \
		int e_; \
		float f_; \
		static const int bias_ = 128 - (1 << (7 - 1)); \
		f_ = (_inarg); \
		uval_ = *(unsigned*)&f_; \
		e_ = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0; \
		m_ = (uval_ & 0x7fffff) >> (23 - 23); \
		if (e_ >= 0) \
			_outarg = m_ | (e_ << 23) | ((uval_ >> 31) << (23 + 7)); \
		else \
			_outarg = ((uval_ >> 31) << (23 + 7)); \
	}

#define UTL_F2UFX_8W_8I(_inarg, _outarg) \
	{ \
		float f_ = (_inarg); \
		unsigned val_; \
		unsigned v_ = *(unsigned*)&f_; \
		if (f_ <= 0 || (v_ & 0x7f800000) == 0x7f800000) \
			val_ = 0; \
		else \
		{ \
			f_ *= 1 << (8 - 8); \
			if (f_ >= (1 << 8)) \
				val_ = (1 << 8) - 1; \
			else \
				val_ = (unsigned)(f_); \
		} \
		(_outarg) = val_; \
	}

#define UTL_F2UFX_11W_0I(_inarg, _outarg) \
	{ \
		float f_ = (_inarg); \
		unsigned val_; \
		unsigned v_ = *(unsigned*)&f_; \
		if (f_ <= 0 || (v_ & 0x7f800000) == 0x7f800000) \
			val_ = 0; \
		else \
		{ \
			f_ *= 1 << (11 - 0); \
			if (f_ >= (1 << 11)) \
				val_ = (1 << 11) - 1; \
			else \
				val_ = (unsigned)(f_); \
		} \
		(_outarg) = val_; \
	}

#define UTL_F2UFX_12W_0I(_inarg, _outarg) \
	{ \
		float f_ = (_inarg); \
		unsigned val_; \
		unsigned v_ = *(unsigned*)&f_; \
		if (f_ <= 0 || (v_ & 0x7f800000) == 0x7f800000) \
			val_ = 0; \
		else \
		{ \
			f_ *= 1 << (12 - 0); \
			if (f_ >= (1 << 12)) \
				val_ = (1 << 12) - 1; \
			else \
				val_ = (unsigned)(f_); \
		} \
		(_outarg) = val_; \
	}

#define UTL_F2FX_13W_2I_T(_inarg, _outarg) \
	{ \
		float f_; \
		unsigned v_; \
		f_ = (_inarg); \
		v_ = *(unsigned*)&f_; \
		if (f_ == 0.f || (v_ & 0x7f800000) == 0x7f800000) \
			_outarg = 0; \
		else \
		{ \
			f_ += 0.5f * (1 << 2); \
			f_ *= 1 << (13 - 2); \
			if (f_ < 0) \
				f_ = 0; \
			else if (f_ >= (1 << 13)) \
				f_ = (1 << 13) - 1; \
			if (f_ >= (1 << (13 - 1))) \
				_outarg = (unsigned)(f_ - (1 << (13 - 1))); \
			else \
				_outarg = (unsigned)(f_ + (1 << (13 - 1))); \
		} \
	}

#define UTL_F2UFX_24W_16I(_inarg, _outarg) \
	{ \
		float f_ = (_inarg); \
		unsigned val_; \
		unsigned v_ = *(unsigned*)&f_; \
		if (f_ <= 0 || (v_ & 0x7f800000) == 0x7f800000) \
			val_ = 0; \
		else \
		{ \
			f_ *= 1 << (24 - 16); \
			if (f_ >= (1 << 24)) \
				val_ = (1 << 24) - 1; \
			else \
				val_ = (unsigned)(f_); \
		} \
		(_outarg) = val_; \
	}

#define UTL_F2FX_12W_1I_F(_inarg, _outarg) \
	{ \
		float f_; \
		unsigned v_; \
		f_ = (_inarg); \
		v_ = *(unsigned*)&f_; \
		if (f_ == 0.f || (v_ & 0x7f800000) == 0x7f800000) \
			_outarg = 0; \
		else \
		{ \
			f_ *= (1 << (12 - 1)); \
			if (f_ < 0) \
			{ \
				_outarg = 1 << (12 - 1); \
				f_ = -f_; \
			} \
			else \
				_outarg = 0; \
			if (f_ >= (1 << (12 - 1))) f_ = (1 << (12 - 1)) - 1; \
			_outarg |= (unsigned)(f_); \
		} \
	}

#define UTL_F2FX_8W_1I_T(_inarg, _outarg) \
	{ \
		float f_; \
		unsigned v_; \
		f_ = (_inarg); \
		v_ = *(unsigned*)&f_; \
		if (f_ == 0.f || (v_ & 0x7f800000) == 0x7f800000) \
			_outarg = 0; \
		else \
		{ \
			f_ += 0.5f * (1 << 1); \
			f_ *= 1 << (8 - 1); \
			if (f_ < 0) \
				f_ = 0; \
			else if (f_ >= (1 << 8)) \
				f_ = (1 << 8) - 1; \
			if (f_ >= (1 << (8 - 1))) \
				_outarg = (unsigned)(f_ - (1 << (8 - 1))); \
			else \
				_outarg = (unsigned)(f_ + (1 << (8 - 1))); \
		} \
	}

#define CHECK_ASSIGN(lhs, rhs, flag) \
	if ((lhs) != (rhs)) \
	{ \
		(lhs) = (rhs); \
		SET_MASK(state->upd_mask, (flag)); \
	}

#define UTL_TYPESIZE(type_, size_) \
	{ \
		switch (type_) \
		{ \
		case GL_FLOAT: \
			size_ = sizeof(GLfloat); \
			break; \
		case GL_SHORT: \
			size_ = sizeof(GLshort); \
			break; \
		case GL_BYTE: \
			size_ = sizeof(GLbyte); \
			break; \
		case GL_UNSIGNED_BYTE: \
			size_ = sizeof(GLubyte); \
			break; \
		default: \
			size_ = 0; \
			break; \
		} \
	}

#define UTL_F2FX_12W_1I_T(_inarg, _outarg) \
	{ \
		float f_; \
		unsigned v_; \
		f_ = (_inarg); \
		v_ = *(unsigned*)&f_; \
		if (f_ == 0.f || (v_ & 0x7f800000) == 0x7f800000) \
			_outarg = 0; \
		else \
		{ \
			f_ += 0.5f * (1 << 1); \
			f_ *= 1 << (12 - 1); \
			if (f_ < 0) \
				f_ = 0; \
			else if (f_ >= (1 << 12)) \
				f_ = (1 << 12) - 1; \
			if (f_ >= (1 << (12 - 1))) \
				_outarg = (unsigned)(f_ - (1 << (12 - 1))); \
			else \
				_outarg = (unsigned)(f_ + (1 << (12 - 1))); \
		} \
	}

