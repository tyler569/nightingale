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

#ifndef INCLUDE____MATH_H
#define INCLUDE____MATH_H

#include <sys/cdefs.h>

#if defined(__sortix__)
#include <__/wordsize.h>
#elif defined(__GNU_LIBRARY__)
#include <bits/wordsize.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if __WORDSIZE == 64 || (defined __FLT_EVAL_METHOD__ && __FLT_EVAL_METHOD__ == 0)

/* The x86-64 architecture computes values with the precission of the
   used type.  Similarly for -m32 -mfpmath=sse.  */

/* `float' expressions are evaluated as `float'.  */
typedef float __float_t;

/* `double' expressions are evaluated as `double'.  */
typedef double __double_t;

#else

/* The ix87 FPUs evaluate all values in the 80 bit floating-point format
   which is also available for the user as `long double'.  Therefore we
   define:  */

/* `float' expressions are evaluated as `long double'.  */
typedef long double __float_t;

/* `double' expressions are evaluated as `long double'.  */
typedef long double __double_t;

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
