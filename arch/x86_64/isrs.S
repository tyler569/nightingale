.intel_syntax noprefix
.extern c_interrupt_shim

.section .text

interrupt_shim:
	push rax
	push rcx
	push rbx
	push rdx
	push rsi
	push rdi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	mov ebp, ds
	push rbp    // push data segment

	mov ebp, 0
	mov ds, ebp // set kernel data segment

	mov rdi, rsp
	mov rax, offset c_interrupt_shim

	push rsp    // align stack for C
	and rsp, ~15
    cld
	call rax    // call C
	pop rsp

.global return_from_interrupt
return_from_interrupt:
	pop rbp
	mov ds, ebp // restore data segment

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rbx
	pop rcx
	pop rax
	add rsp, 16
	iretq

// jmp_to_userspace(ip, sp, ...)
// rdi: user ip
// rsi: user sp
// rdx: arg 1
// rcx: arg 2
// r8 : arg 3
.global jmp_to_userspace
jmp_to_userspace:
	// TODO: 0 GPRs to not leak kernel data
	push 0x20 | 3   // SS
	push rsi        // RSP
	push 0x200      // RFLAGS (IF)
	push 0x18 | 3   // CS
	push rdi        // RIP
	mov rdi, rdx    // user arg 1
	mov rsi, rcx    // user arg 2
	mov rdx, r8     // user arg 3
	mov rbp, rdi    // set user_rbp == user_rsp
	iretq

.macro isrnoerr name num
.global \name
\name:
	push 0
	push \num
	jmp interrupt_shim
.endm

.macro isrerr name num
.global \name
\name:
	push \num
	jmp interrupt_shim
.endm

isrnoerr isr0 0
isrnoerr isr1 1
isrnoerr isr2 2
isrnoerr isr3 3
isrnoerr isr4 4
isrnoerr isr5 5
isrnoerr isr6 6
isrnoerr isr7 7
isrerr isr8 8
isrnoerr isr9 9
isrerr isr10 10
isrerr isr11 11
isrerr isr12 12
isrerr isr13 13
isrerr isr14 14
isrnoerr isr15 15
isrnoerr isr16 16
isrerr isr17 17
isrnoerr isr18 18
isrnoerr isr19 19
isrnoerr isr20 20
isrnoerr isr21 21
isrnoerr isr22 22
isrnoerr isr23 23
isrnoerr isr24 24
isrnoerr isr25 25
isrnoerr isr26 26
isrnoerr isr27 27
isrnoerr isr28 28
isrnoerr isr29 29
isrerr isr30 30
isrnoerr isr31 31
isrnoerr irq0 32
isrnoerr irq1 33
isrnoerr irq2 34
isrnoerr irq3 35
isrnoerr irq4 36
isrnoerr irq5 37
isrnoerr irq6 38
isrnoerr irq7 39
isrnoerr irq8 40
isrnoerr irq9 41
isrnoerr irq10 42
isrnoerr irq11 43
isrnoerr irq12 44
isrnoerr irq13 45
isrnoerr irq14 46
isrnoerr irq15 47
isrnoerr isr_double_fault 127
isrnoerr isr_syscall 128
isrnoerr isr_yield 129
isrnoerr isr_panic 130
isrnoerr isr_halt 131
