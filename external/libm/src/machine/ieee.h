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

#ifndef INCLUDE_MACHINE_IEEE_H
#define INCLUDE_MACHINE_IEEE_H

#include <sys/ieee754.h>

/* float compatibility */

#define ieee_single_u ieee754_float
#define sngu_f f
#define sngu_sng ieee
#define sng_exp exponent
#define sng_frac mantissa
#define sng_sign negative

#define sngu_exp sngu_sng.sng_exp
#define sngu_sign sngu_sng.sng_sign
#define sngu_frac sngu_sng.sng_frac

/* double compatibility */
#define ieee_double_u ieee754_double
#define dblu_d d
#define dblu_dbl ieee
#define dbl_exp exponent
#define dbl_frach mantissa0
#define dbl_fracl mantissa1
#define dbl_sign negative

#define dblu_exp dblu_dbl.dbl_exp
#define dblu_sign dblu_dbl.dbl_sign
#define dblu_frach dblu_dbl.dbl_frach
#define dblu_fracl dblu_dbl.dbl_fracl

/* long double compatibility */
#define ieee_ext_u ieee754_long_double
#define extu_ld d
#define extu_ext ieee
#define ext_exp exponent
#define ext_frach mantissa0
#define ext_fracl mantissa1
#define ext_sign negative

#define extu_exp extu_ext.ext_exp
#define extu_sign extu_ext.ext_sign
#define extu_frach extu_ext.ext_frach
#define extu_fracl extu_ext.ext_fracl

#define LDBL_NBIT 0x80000000
#define mask_nbit_l(u) ((u).extu_frach &= ~LDBL_NBIT)

#endif
