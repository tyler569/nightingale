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

#ifndef INCLUDE_MATH_H
#define INCLUDE_MATH_H

#include <sys/cdefs.h>

#ifndef __sortix_libm__
#define __sortix_libm__ 1
#endif

#if defined(__sortix__)
#include <__/limits.h>
#include <__/wordsize.h>
#elif defined(__GNU_LIBRARY__)
#include <bits/wordsize.h>
#endif

#include <__/math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __fpmacro_unary_floating(__name, __arg0)                        \
        /* LINTED */                                                    \
        ((sizeof (__arg0) == sizeof (float))                            \
        ?       __ ## __name ## f (__arg0)                              \
        : (sizeof (__arg0) == sizeof (double))                          \
        ?       __ ## __name ## d (__arg0)                              \
        :       __ ## __name ## l (__arg0))

/* ANSI/POSIX macros */
#define HUGE_VAL __builtin_huge_val()

/* C99 macros */
#if __USE_SORTIX || 1999 <= __USE_C

#define FP_ILOGB0 (-__INT_MAX)
#define FP_ILOGBNAN __INT_MAX

#define HUGE_VALF __builtin_huge_valf()
#define HUGE_VALL __builtin_huge_vall()
#define INFINITY __builtin_inff()
#define NAN __builtin_nanf("")

/* TODO: What does this entail and is that really how this libm does it? */
#define MATH_ERRNO 1
#define MATH_ERREXCEPT 2
#define math_errhandling MATH_ERREXCEPT

/* TODO: Is it really true that fma is faster in these cases? */
#if defined(__ia64__) || defined(__sparc64__)
#define FP_FAST_FMA 1
#endif
#ifdef __ia64__
#define FP_FAST_FMAL 1
#endif
#define FP_FAST_FMAF 1 /* TODO: Is this really true in all cases? */

/* Symbolic constants to classify floating point numbers. */
#define FP_INFINITE     0x00
#define FP_NAN          0x01
#define FP_NORMAL       0x02
#define FP_SUBNORMAL    0x03
#define FP_ZERO         0x04
/* NetBSD extensions */
#define _FP_LOMD        0x80 /* range for machine-specific classes */
#define _FP_HIMD        0xff

#define fpclassify(__x) __fpmacro_unary_floating(fpclassify, __x)
#define isfinite(__x)   __fpmacro_unary_floating(isfinite, __x)
#define isinf(__x)      __fpmacro_unary_floating(isinf, __x)
#define isnan(__x)      __fpmacro_unary_floating(isnan, __x)
#define isnormal(__x)   (fpclassify(__x) == FP_NORMAL)
#define signbit(__x)    __fpmacro_unary_floating(signbit, __x)

#define isgreater(x, y)         __builtin_isgreater((x), (y))
#define isgreaterequal(x, y)    __builtin_isgreaterequal((x), (y))
#define isless(x, y)            __builtin_isless((x), (y))
#define islessequal(x, y)       __builtin_islessequal((x), (y))
#define islessgreater(x, y)     __builtin_islessgreater((x), (y))
#define isunordered(x, y)       __builtin_isunordered((x), (y))

typedef __double_t double_t;
typedef __float_t float_t;

#endif /* __USE_SORTIX || 1999 <= __USE_C */

/* XOPEN/SVID macros */
#if __USE_SORTIX || __USE_XOPEN
#define M_E             2.7182818284590452354   /* e */
#define M_LOG2E         1.4426950408889634074   /* log 2e */
#define M_LOG10E        0.43429448190325182765  /* log 10e */
#define M_LN2           0.69314718055994530942  /* log e2 */
#define M_LN10          2.30258509299404568402  /* log e10 */
#define M_PI            3.14159265358979323846  /* pi */
#define M_PI_2          1.57079632679489661923  /* pi/2 */
#define M_PI_4          0.78539816339744830962  /* pi/4 */
#define M_1_PI          0.31830988618379067154  /* 1/pi */
#define M_2_PI          0.63661977236758134308  /* 2/pi */
#define M_2_SQRTPI      1.12837916709551257390  /* 2/sqrt(pi) */
#define M_SQRT2         1.41421356237309504880  /* sqrt(2) */
#define M_SQRT1_2       0.70710678118654752440  /* 1/sqrt(2) */

/* TODO: MAXFLOAT is obsoleted by FLT_MAX of float.h. */
#define MAXFLOAT        ((float)3.40282346638528860e+38)

extern int signgam;
#endif /* __USE_SORTIX || __USE_XOPEN */

/* Various extensions inherited from NetBSD libm. Perhaps we should get rid of
   some of them or make them private to libm itself, or just rename them. */
#if __USE_SORTIX
enum fdversion {fdlibm_ieee = -1, fdlibm_svid, fdlibm_xopen, fdlibm_posix};

#define _LIB_VERSION_TYPE enum fdversion
#define _LIB_VERSION _fdlib_version

/* if global variable _LIB_VERSION is not desirable, one may
 * change the following to be a constant by:
 *      #define _LIB_VERSION_TYPE const enum version
 * In that case, after one initializes the value _LIB_VERSION (see
 * s_lib_version.c) during compile time, it cannot be modified
 * in the middle of a program
 */
extern _LIB_VERSION_TYPE _LIB_VERSION;

#define _IEEE_  fdlibm_ieee
#define _SVID_  fdlibm_svid
#define _XOPEN_ fdlibm_xopen
#define _POSIX_ fdlibm_posix

#ifndef __cplusplus
struct exception
{
	int type;
	const char* name;
	double arg1;
	double arg2;
	double retval;
};
#endif

#define HUGE MAXFLOAT

/*
 * set X_TLOSS = pi*2**52, which is possibly defined in <values.h>
 * (one may replace the following line by "#include <values.h>")
 */

#define X_TLOSS 1.41484755040568800000e+16

#define DOMAIN          1
#define SING            2
#define OVERFLOW        3
#define UNDERFLOW       4
#define TLOSS           5
#define PLOSS           6

#endif /* __USE_SORTIX */

/*
 * Most of these functions depend on the rounding mode and have the side
 * effect of raising floating-point exceptions, so they are not declared
 * as __pure2.  In C99, FENV_ACCESS affects the purity of these functions.
 */

/*
 * ANSI/POSIX
 */
double acos(double);
double asin(double);
double atan(double);
double atan2(double, double);
double cos(double);
double sin(double);
double tan(double);

double cosh(double);
double sinh(double);
double tanh(double);

double exp(double);
double exp2(double);
double frexp(double, int*);
double ldexp(double, int);
double log(double);
double log2(double);
double log10(double);
double modf(double, double*);

double pow(double, double);
double sqrt(double);

double ceil(double);
double fabs(double);
double floor(double);
double fmod(double, double);

#if __USE_SORTIX || 1999 <= __USE_C || __USE_XOPEN
double erf(double);
double erfc(double);
double gamma(double);
double hypot(double, double);
int finite(double);
double j0(double);
double j1(double);
double jn(int, double);
double lgamma(double);
double y0(double);
double y1(double);
double yn(int, double);
#endif  /* __USE_SORTIX ||  1999 <= __USE_C || __USE_XOPEN */

#if __USE_SORTIX || 1999 <= __USE_C || 500 <= __USE_XOPEN
double acosh(double);
double asinh(double);
double atanh(double);
double cbrt(double);
double expm1(double);
int ilogb(double);
double log1p(double);
double logb(double);
double nextafter(double, double);
double remainder(double, double);
double rint(double);
double scalb(double, double);
#endif /* __USE_SORTIX || 1999 <= __USE_C || 500 <= __USE_XOPEN */

/*
 * ISO C99
 */
#if __USE_SORTIX || 1999 <= __USE_C
/* 7.12.3.1 int fpclassify(real-floating x) */
#define fpclassify(__x) __fpmacro_unary_floating(fpclassify, __x)

/* 7.12.3.2 int isfinite(real-floating x) */
#define isfinite(__x)   __fpmacro_unary_floating(isfinite, __x)

/* 7.12.3.5 int isnormal(real-floating x) */
#define isnormal(__x)   (fpclassify(__x) == FP_NORMAL)

/* 7.12.3.6 int signbit(real-floating x) */
#define signbit(__x)    __fpmacro_unary_floating(signbit, __x)

/* 7.12.4 trigonometric */

float acosf(float);
float asinf(float);
float atanf(float);
float atan2f(float, float);
float cosf(float);
float sinf(float);
float tanf(float);

/* 7.12.5 hyperbolic */

float acoshf(float);
float asinhf(float);
float atanhf(float);
float coshf(float);
float sinhf(float);
float tanhf(float);

/* 7.12.6 exp / log */

float expf(float);
float exp2f(float);
float expm1f(float);
float frexpf(float, int*);
int ilogbf(float);
float ldexpf(float, int);
float logf(float);
float log2f(float);
float log10f(float);
float log1pf(float);
float logbf(float);
float modff(float, float*);
float scalbnf(float, int);

/* 7.12.7 power / absolute */

float cbrtf(float);
float fabsf(float);
long double fabsl(long double);
float hypotf(float, float);
float powf(float, float);
float sqrtf(float);

/* 7.12.8 error / gamma */

float erff(float);
float erfcf(float);
float lgammaf(float);
float tgammaf(float);
double tgamma(double);

/* 7.12.9 nearest integer */

float ceilf(float);
float floorf(float);
float rintf(float);
double round(double);
float roundf(float);
double trunc(double);
float truncf(float);
long int lrint(double);
long int lrintf(float);
long long int llrint(double);
long long int llrintf(float);
long int lround(double);
long int lroundf(float);
long long int llround(double);
long long int llroundf(float);

/* 7.12.10 remainder */

float fmodf(float, float);
float remainderf(float, float);

/* 7.12.10.3 The remquo functions */
double remquo(double, double, int*);
float remquof(float, float, int*);

/* 7.12.11 manipulation */

float copysignf(float, float);
long double copysignl(long double, long double);
double nan(const char*);
float nanf(const char*);
long double nanl(const char*);
float nextafterf(float, float);
long double nextafterl(long double, long double);
double nexttoward(double, long double);

/* 7.12.14 comparison */

#define isunordered(x, y)       __builtin_isunordered((x), (y))
#define isgreater(x, y)         __builtin_isgreater((x), (y))
#define isgreaterequal(x, y)    __builtin_isgreaterequal((x), (y))
#define isless(x, y)            __builtin_isless((x), (y))
#define islessequal(x, y)       __builtin_islessequal((x), (y))
#define islessgreater(x, y)     __builtin_islessgreater((x), (y))

double fdim(double, double);
double fmax(double, double);
double fmin(double, double);
float fdimf(float, float);
float fmaxf(float, float);
float fminf(float, float);
long double fdiml(long double, long double);
long double fmaxl(long double, long double);
long double fminl(long double, long double);

/* 7.12.3.3 int isinf(real-floating x) */
#define isinf(__x)      __fpmacro_unary_floating(isinf, __x)

/* 7.12.3.4 int isnan(real-floating x) */
#define isnan(__x)      __fpmacro_unary_floating(isnan, __x)

#endif /* __USE_SORTIX || 1999 <= __USE_C */

#if __USE_SORTIX
#ifndef __cplusplus
int matherr(struct exception*);
#endif

/*
 * IEEE Test Vector
 */
double significand(double);

/*
 * Functions callable from C, intended to support IEEE arithmetic.
 */
double copysign(double, double);
double scalbn(double, int);

/*
 * BSD math library entry points
 */
double drem(double, double);

#endif /* __USE_SORTIX */

#if __USE_SORTIX /* or _REENTRANT */
/*
 * Reentrant version of gamma & lgamma; passes signgam back by reference
 * as the second argument; user must allocate space for signgam.
 */
double gamma_r(double, int*);
double lgamma_r(double, int*);
#endif /* __USE_SORTIX */


#if __USE_SORTIX

/* float versions of ANSI/POSIX functions */

float gammaf(float);
int isinff(float);
int isnanf(float);
int finitef(float);
float j0f(float);
float j1f(float);
float jnf(int, float);
float y0f(float);
float y1f(float);
float ynf(int, float);

float scalbf(float, float);

/*
 * float version of IEEE Test Vector
 */
float significandf(float);

/*
 * float versions of BSD math library entry points
 */
float dremf(float, float);
#endif /* __USE_SORTIX */

#if __USE_SORTIX /* or _REENTRANT */
/*
 * Float versions of reentrant version of gamma & lgamma; passes
 * signgam back by reference as the second argument; user must
 * allocate space for signgam.
 */
float gammaf_r(float, int*);
float lgammaf_r(float, int*);
#endif /* __USE_SORTIX */

/*
 * Library implementation
 */
int __fpclassifyf(float);
int __fpclassifyd(double);
int __isfinitef(float);
int __isfinited(double);
int __isinff(float);
int __isinfd(double);
int __isnanf(float);
int __isnand(double);
int __signbitf(float);
int __signbitd(double);

int __fpclassifyl(long double);
int __isfinitel(long double);
int __isinfl(long double);
int __isnanl(long double);
int __signbitl(long double);

int ilogbl(long double);
long double logbl(long double);
long double scalbnl(long double, int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
