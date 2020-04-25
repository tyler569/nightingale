/*	$NetBSD: fpu.h,v 1.5 2008/04/16 21:51:03 cegger Exp $	*/

#ifndef	_AMD64_FPU_H_
#define	_AMD64_FPU_H_

/*
 * NetBSD/amd64 only uses the extended save/restore format used
 * by fxsave/fsrestore, to always deal with the SSE registers,
 * which are part of the ABI to pass floating point values.
 * Must be stored in memory on a 16-byte boundary.
 */

struct fxsave64 {
	__uint16_t  fx_fcw;
	__uint16_t  fx_fsw;
	__uint8_t   fx_ftw;
	__uint8_t   fx_unused1;
	__uint16_t  fx_fop;
	__uint64_t  fx_rip;
	__uint64_t  fx_rdp;
	__uint32_t  fx_mxcsr;
	__uint32_t  fx_mxcsr_mask;
	__uint64_t  fx_st[8][2];   /* 8 normal FP regs */
	__uint64_t  fx_xmm[16][2]; /* 16 SSE2 registers */
	__uint8_t   fx_unused3[96];
} __attribute__((__packed__));

struct savefpu {
	struct fxsave64 fp_fxsave;	/* see above */
	__uint16_t fp_ex_sw;		/* saved status from last exception */
	__uint16_t fp_ex_tw;		/* saved tag from last exception */
} __attribute__((__aligned__(16)));

/*
 * The i387 defaults to Intel extended precision mode and round to nearest,
 * with all exceptions masked.
 */
#define	__INITIAL_NPXCW__	0x037f
#define __INITIAL_MXCSR__ 	0x1f80
#define __INITIAL_MXCSR_MASK__	0xffbf

/* NetBSD uses IEEE double precision. */
#define	__NetBSD_NPXCW__	0x127f
/* Linux just uses the default control word. */
#define	__Linux_NPXCW__		0x037f

#endif /* _AMD64_FPU_H_ */
