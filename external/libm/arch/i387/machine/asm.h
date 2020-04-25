/*	$NetBSD: asm.h,v 1.40 2011/06/16 13:16:20 joerg Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)asm.h	5.5 (Berkeley) 5/7/91
 */

/* This is a cut down version of netbsd <machine/asm.h>. */

#ifndef INCLUDE_MACHINE_ASM_H
#define INCLUDE_MACHINE_ASM_H

#ifdef PIC
#define PIC_PROLOGUE	\
	pushl	%ebx;	\
	call	1f;	\
1:			\
	popl	%ebx;	\
	addl	$_GLOBAL_OFFSET_TABLE_+[.-1b], %ebx
#define PIC_EPILOGUE	\
	popl	%ebx
#define PIC_PLT(x)	x@PLT
#define PIC_GOT(x)	x@GOT(%ebx)
#define PIC_GOTOFF(x)	x@GOTOFF(%ebx)
#else
#define PIC_PROLOGUE
#define PIC_EPILOGUE
#define PIC_PLT(x)	x
#define PIC_GOT(x)	x
#define PIC_GOTOFF(x)	x
#endif

#define _C_LABEL(x)	x
#define	_ASM_LABEL(x)	x

#define CVAROFF(x, y)		_C_LABEL(x) + y

#define __CONCAT(x,y)	x ## y
#define __STRING(x)	#x

/* let kernels and others override entrypoint alignment */
#if !defined(_ALIGN_TEXT) && !defined(_KERNEL)
# ifdef _STANDALONE
#  define _ALIGN_TEXT .align 1
# elif defined __ELF__
#  define _ALIGN_TEXT .align 16
# else
#  define _ALIGN_TEXT .align 4
# endif
#endif

#define _ENTRY(x) \
	.text; _ALIGN_TEXT; .globl x; .type x,@function; x:
#define _LABEL(x) \
	.globl x; x:


#ifdef GPROF
# define _PROF_PROLOGUE	\
	pushl %ebp; movl %esp,%ebp; call PIC_PLT(__mcount); popl %ebp
#else
# define _PROF_PROLOGUE
#endif

#define	ENTRY(y)	_ENTRY(_C_LABEL(y)); _PROF_PROLOGUE
#define	NENTRY(y)	_ENTRY(_C_LABEL(y))
#define	ASENTRY(y)	_ENTRY(_ASM_LABEL(y)); _PROF_PROLOGUE
#define	LABEL(y)	_LABEL(_C_LABEL(y))
#define	END(y)		.size y, . - y

#define	ASMSTR		.asciz

#ifdef __ELF__
#define RCSID(x)	.pushsection ".ident"; .asciz x; .popsection
#else
#define RCSID(x)	.text; .asciz x
#endif

#define	WEAK_ALIAS(alias,sym)						\
	.weak alias;							\
	alias = sym

/*
 * STRONG_ALIAS: create a strong alias.
 */
#define STRONG_ALIAS(alias,sym)						\
	.globl alias;							\
	alias = sym

#define	WARN_REFERENCES(sym,msg)					\
	.pushsection .gnu.warning. ## sym;				\
	.ascii msg;							\
	.popsection

#endif /* !INCLUDE_MACHINE_ASM_H */
