/*	$NetBSD: ieeefp.h,v 1.9 2011/03/27 05:13:15 mrg Exp $	*/

/*
 * Written by J.T. Conklin, Apr 6, 1995
 * Public domain.
 */

#ifndef INCLUDE_IEEEFP_H
#define INCLUDE_IEEEFP_H

#include <sys/cdefs.h>

#ifndef __sortix_libm__
#define __sortix_libm__ 1
#endif

#include <machine/fenv.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__i386__) || defined(__x86_64__)

typedef int fp_except;
#define FP_X_INV        FE_INVALID      /* invalid operation exception */
#define FP_X_DNML       FE_DENORMAL     /* denormalization exception */
#define FP_X_DZ         FE_DIVBYZERO    /* divide-by-zero exception */
#define FP_X_OFL        FE_OVERFLOW     /* overflow exception */
#define FP_X_UFL        FE_UNDERFLOW    /* underflow exception */
#define FP_X_IMP        FE_INEXACT      /* imprecise (loss of precision) */

typedef enum
{
        FP_RN=FE_TONEAREST,             /* round to nearest representable num */
        FP_RM=FE_DOWNWARD,              /* round toward negative infinity */
        FP_RP=FE_UPWARD,                /* round toward positive infinity */
        FP_RZ=FE_TOWARDZERO,            /* round to zero (truncate) */
} fp_rnd;

typedef enum
{
        FP_PS = 0,                      /* 24 bit (single-precision) */
        FP_PRS,                         /* reserved */
        FP_PD,                          /* 53 bit (double-precision) */
        FP_PE,                          /* 64 bit (extended-precision) */
} fp_prec;

typedef fp_prec fp_prec_t;
fp_prec_t fpgetprec(void);
fp_prec_t fpsetprec(fp_prec_t);

#endif

typedef fp_rnd fp_rnd_t;
typedef fp_except fp_except_t;

fp_rnd_t fpgetround(void);
fp_rnd_t fpsetround(fp_rnd_t);
fp_except_t fpgetmask(void);
fp_except_t fpsetmask(fp_except_t);
fp_except_t fpgetsticky(void);
fp_except_t fpsetsticky(fp_except_t);
fp_except_t fpresetsticky(fp_except_t);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* INCLUDE_IEEEFP_H */
