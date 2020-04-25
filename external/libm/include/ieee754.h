/*-
 * Copyright (c) 2013 Jonas 'Sortie' Termansen.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef INCLUDE_IEEE754_H
#define INCLUDE_IEEE754_H

#include <sys/cdefs.h>

#ifndef __sortix_libm__
#define __sortix_libm__ 1
#endif

#include <endian.h>

#if !(BYTE_ORDER == LITTLE_ENDIAN || BYTE_ORDER == BIG_ENDIAN)
#error "Please add support for the endianness on your platform"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SNG_EXPBITS 8
#define SNG_FRACBITS 23
#define SNG_EXP_INFNAN 255
#define SNG_EXP_BIAS 127
#define IEEE754_FLOAT_BIAS SNG_EXP_BIAS

struct ieee754_float_single
{
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned int mantissa: SNG_FRACBITS;
	unsigned int exponent: SNG_EXPBITS;
	unsigned int negative: 1;
#elif BYTE_ORDER == BIG_ENDIAN
	unsigned int negative: 1;
	unsigned int exponent: SNG_EXPBITS;
	unsigned int mantissa: SNG_FRACBITS;
#endif
};

struct ieee754_float_single_nan
{
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned int mantissa: SNG_FRACBITS-1;
	unsigned int quiet_nan: 1;
	unsigned int exponent: SNG_EXPBITS;
	unsigned int negative: 1;
#elif BYTE_ORDER == BIG_ENDIAN
	unsigned int negative: 1;
	unsigned int exponent: SNG_EXPBITS;
	unsigned int quiet_nan: 1;
	unsigned int mantissa: SNG_FRACBITS-1;
#endif
};

union ieee754_float
{
	float f;
	struct ieee754_float_single ieee;
	struct ieee754_float_single_nan ieee_nan;
};

#define DBL_EXPBITS 11
#define DBL_FRACHBITS 20
#define DBL_FRACLBITS 32
#define DBL_FRACBITS (DBL_FRACHBITS + DBL_FRACLBITS)
#define DBL_EXP_INFNAN 2047
#define DBL_EXP_BIAS 1023
#define IEEE754_DOUBLE_BIAS DBL_EXP_BIAS

struct ieee754_float_double
{
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned int mantissa1: DBL_FRACLBITS;
	unsigned int mantissa0: DBL_FRACHBITS;
	unsigned int exponent: DBL_EXPBITS;
	unsigned int negative: 1;
#elif BYTE_ORDER == BIG_ENDIAN
	unsigned int negative: 1;
	unsigned int exponent: DBL_EXPBITS;
	unsigned int mantissa0: DBL_FRACHBITS;
	unsigned int mantissa1: DBL_FRACLBITS;
#endif
};

struct ieee754_float_double_nan
{
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned int mantissa1: DBL_FRACLBITS;
	unsigned int mantissa0: DBL_FRACHBITS-1;
	unsigned int quiet_nan: 1;
	unsigned int exponent: DBL_EXPBITS;
	unsigned int negative: 1;
#elif BYTE_ORDER == BIG_ENDIAN
	unsigned int negative: 1;
	unsigned int exponent: DBL_EXPBITS;
	unsigned int quiet_nan: 1;
	unsigned int mantissa0: DBL_FRACHBITS-1;
	unsigned int mantissa1: DBL_FRACLBITS;
#endif
};

union ieee754_double
{
	double d;
	struct ieee754_float_double ieee;
	struct ieee754_float_double_nan ieee_nan;
};

#define EXT_EXPBITS 15
#define EXT_FRACHBITS 32
#define EXT_FRACLBITS 32
#define EXT_FRACBITS (EXT_FRACHBITS + EXT_FRACLBITS)
#define EXT_EXP_INFNAN 32767
#define EXT_EXP_INF 32767
#define EXT_EXP_NAN 32767
#define EXT_EXP_BIAS 16383
#define IEEE854_LONG_DOUBLE_BIAS EXT_EXP_BIAS

struct ieee754_float_long_double
{
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned int mantissa1: EXT_FRACLBITS;
	unsigned int mantissa0: EXT_FRACHBITS;
	unsigned int exponent: EXT_EXPBITS;
	unsigned int negative: 1;
	unsigned int empty: 16;
#elif BYTE_ORDER == BIG_ENDIAN
	unsigned int empty: 16;
	unsigned int negative: 1;
	unsigned int exponent: EXT_EXPBITS;
	unsigned int mantissa0: EXT_FRACHBITS;
	unsigned int mantissa1: EXT_FRACLBITS;
#endif
};

struct ieee754_float_long_double_nan
{
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned int mantissa1: EXT_FRACLBITS;
	unsigned int mantissa0: EXT_FRACHBITS-2;
	unsigned int quiet_nan: 1;
	unsigned int one: 1;
	unsigned int exponent: EXT_EXPBITS;
	unsigned int negative: 1;
	unsigned int empty: 16;
#elif BYTE_ORDER == BIG_ENDIAN
	unsigned int empty: 16;
	unsigned int negative: 1;
	unsigned int exponent: EXT_EXPBITS;
	unsigned int one: 1;
	unsigned int quiet_nan: 1;
	unsigned int mantissa0: EXT_FRACHBITS-2;
	unsigned int mantissa1: EXT_FRACLBITS;
#endif
};

union ieee754_long_double
{
	long double d;
	struct ieee754_float_long_double ieee;
	struct ieee754_float_long_double_nan ieee_nan;
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
