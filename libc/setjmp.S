
#include <basic.h>

#if X86_64

/* Copyright 2011-2012 Nicholas J. Kain, licensed under standard MIT license */
.global _setjmp
.global setjmp
.type _setjmp, function
.type setjmp, function
_setjmp:
setjmp:
	mov %rbx, (%rdi)         /* rdi is jmp_buf, move registers onto it */
	mov %rbp, 8(%rdi)
	mov %r12, 16(%rdi)
	mov %r13, 24(%rdi)
	mov %r14, 32(%rdi)
	mov %r15, 40(%rdi)
	lea 8(%rsp), %rdx        /* this is our rsp WITHOUT current ret addr */
	mov %rdx, 48(%rdi)
	mov (%rsp), %rdx         /* save return addr ptr for new rip */
	mov %rdx, 56(%rdi)
	xor %rax, %rax           /* always return 0 */
	ret

/* Copyright 2011-2012 Nicholas J. Kain, licensed under standard MIT license */
.global _longjmp
.global longjmp
.type _longjmp, function
.type longjmp, function
_longjmp:
longjmp:
	mov %rsi, %rax           /* val will be longjmp return */
	test %rax, %rax
	jnz 1f
	inc %rax                /* if val==0, val=1 per longjmp semantics */
1:
	mov (%rdi), %rbx         /* rdi is the jmp_buf, restore regs from it */
	mov 8(%rdi), %rbp
	mov 16(%rdi), %r12
	mov 24(%rdi), %r13
	mov 32(%rdi), %r14
	mov 40(%rdi), %r15
	mov 48(%rdi), %rdx       /* this ends up being the stack pointer */
	mov %rdx, %rsp
	mov 56(%rdi), %rdx       /* this is the instruction pointer */
	jmp *%rdx               /* goto saved address without altering rsp */

#elif I686

.global _setjmp
.global setjmp
.type _setjmp, function
.type setjmp, function
_setjmp:
setjmp:
	mov 4(%esp), %eax
	mov %ebx, (%eax)
	mov %esi, 4(%eax)
	mov %edi, 8(%eax)
	mov %ebp, 12(%eax)
	lea 4(%esp), %ecx
	mov %ecx, 16(%eax)
	mov (%esp), %ecx
	mov %ecx, 20(%eax)
	xor %eax, %eax
	ret

.global _longjmp
.global longjmp
.type _longjmp, function
.type longjmp, function
_longjmp:
longjmp:
	mov 4(%esp), %edx
	mov 8(%esp), %eax
	test %eax, %eax
	jnz 1f
	inc %eax
1:
	mov  (%edx), %ebx
	mov 4(%edx), %esi
	mov 8(%edx), %edi
	mov 12(%edx), %ebp
	mov 16(%edx), %ecx
	mov %ecx, %esp
	mov 20(%edx), %ecx
	jmp *%ecx

#else
#error "unimplemented"
#endif
