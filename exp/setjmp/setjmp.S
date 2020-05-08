
#if __x86_64__

.global _setjmp
.global setjmp
.type _setjmp, @function
.type setjmp, @function
_setjmp:
setjmp:
	mov %rbx, (%rdi)
	mov %rbp, 8(%rdi)
	mov %r12, 16(%rdi)
	mov %r13, 24(%rdi)
	mov %r14, 32(%rdi)
	mov %r15, 40(%rdi)
	lea 8(%rsp), %rdx
	mov %rdx, 48(%rdi)
	mov (%rsp), %rdx
	mov %rdx, 56(%rdi)
	xor %rax, %rax
	ret

.global _longjmp
.global longjmp
.type _longjmp, @function
.type longjmp, @function
_longjmp:
longjmp:
	mov %rsi, %rax
	test %rax, %rax
	jnz 1f
	inc %rax
1:
	mov (%rdi), %rbx
	mov 8(%rdi), %rbp
	mov 16(%rdi), %r12
	mov 24(%rdi), %r13
	mov 32(%rdi), %r14
	mov 40(%rdi), %r15
	mov 48(%rdi), %rdx
	mov %rdx, %rsp
	mov 56(%rdi), %rdx
	jmp *%rdx

#elif I686

.global __setjmp
.global _setjmp
.global setjmp
.type _setjmp,@function
.type setjmp,@function
__setjmp:
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
.type _longjmp,@function
.type longjmp,@function
_longjmp:
longjmp:
	mov  4(%esp),%edx
	mov  8(%esp),%eax
	test    %eax,%eax
	jnz 1f
	inc     %eax
1:
	mov   (%edx),%ebx
	mov  4(%edx),%esi
	mov  8(%edx),%edi
	mov 12(%edx),%ebp
	mov 16(%edx),%ecx
	mov     %ecx,%esp
	mov 20(%edx),%ecx
	jmp *%ecx

#else
#error "unimplemented"
#endif
