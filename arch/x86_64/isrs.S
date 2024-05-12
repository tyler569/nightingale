#include "asm.h"

.macro isrnoerr name num
.global \name
\name:
	push $0
	push \num
	jmp interrupt_entry
.endm

.macro isrerr name num
.global \name
\name:
	push \num
	jmp interrupt_entry
.endm

.macro MAYBE_SWAPGS
    cmpq $0x23, 24(%rsp)
    jne 1f
    swapgs
1:
.endm

isrnoerr isr0   $0
isrnoerr isr1   $1
isrnoerr isr2   $2
isrnoerr isr3   $3
isrnoerr isr4   $4
isrnoerr isr5   $5
isrnoerr isr6   $6
isrnoerr isr7   $7
isrerr   isr8   $8
isrnoerr isr9   $9
isrerr   isr10 $10
isrerr   isr11 $11
isrerr   isr12 $12
isrerr   isr13 $13
isrerr   isr14 $14
isrnoerr isr15 $15
isrnoerr isr16 $16
isrerr   isr17 $17
isrnoerr isr18 $18
isrnoerr isr19 $19
isrnoerr isr20 $20
isrnoerr isr21 $21
isrnoerr isr22 $22
isrnoerr isr23 $23
isrnoerr isr24 $24
isrnoerr isr25 $25
isrnoerr isr26 $26
isrnoerr isr27 $27
isrnoerr isr28 $28
isrnoerr isr29 $29
isrerr   isr30 $30
isrnoerr isr31 $31
isrnoerr irq0  $32
isrnoerr irq1  $33
isrnoerr irq2  $34
isrnoerr irq3  $35
isrnoerr irq4  $36
isrnoerr irq5  $37
isrnoerr irq6  $38
isrnoerr irq7  $39
isrnoerr irq8  $40
isrnoerr irq9  $41
isrnoerr irq10 $42
isrnoerr irq11 $43
isrnoerr irq12 $44
isrnoerr irq13 $45
isrnoerr irq14 $46
isrnoerr irq15 $47
isrnoerr isr_double_fault $127
isrnoerr isr_syscall $128
isrnoerr isr_yield $129
isrnoerr isr_panic $130
isrnoerr isr_halt $131

.global interrupt_entry
interrupt_entry:
    MAYBE_SWAPGS
    PUSH_ALL
    mov %rsp, %rdi

    call c_interrupt_entry

    // never implement `jump_to_frame` (or similar) by jumping here.
    // if there is a problem with the new frame and the stack pointer
    // points to the user stack or godforbid unrelated kernel data we're
    // going to have a bad day. The only way to get here is to return
    // from a valid interrupt frame on a real kernel stack.
    //
    // to jump to a novel frame, see `jump_to_frame` - it sets up the
    // new frame without moving the stack pointer.

    POP_ALL
    MAYBE_SWAPGS
    add $16, %rsp
    iretq


.global jump_to_frame
jump_to_frame:
    // mov registers out of the struct
    mov 0(%rdi), %r15
    mov 8(%rdi), %r14
    mov 16(%rdi), %r13
    mov 24(%rdi), %r12
    mov 32(%rdi), %r11
    mov 40(%rdi), %r10
    mov 48(%rdi), %r9
    mov 56(%rdi), %r8
    // 64(%rdi) is %rdi, moved after %rdi is needed
    mov 72(%rdi), %rsi
    mov 80(%rdi), %rbp
    mov 88(%rdi), %rbx
    mov 96(%rdi), %rdx
    mov 104(%rdi), %rcx
    mov 112(%rdi), %rax

    // create interrupt frame on the stack
    push 168(%rdi)
    push 160(%rdi)
    push 152(%rdi)
    push 144(%rdi)
    push 136(%rdi)

    mov 64(%rdi), %rdi

    // maybe_swapgs expects to see the extra 2 words for interrupt number
    // and error code
    sub $16, %rsp
    MAYBE_SWAPGS
    add $16, %rsp

    iretq
    

.section .bss

.align 0x1000
.global hhstack_guard_page
.global hhstack_X
.global hhstack_top
hhstack_guard_page:
    .skip 0x1000
hhstack_X:
    .skip 0x2000
hhstack_top: // TODO remove

.align 0x10
.global int_stack_X
.global int_stack_top
int_stack_X:
    .skip 0x2000
int_stack_top:

.align 0x10
.global df_stack_X
.global df_stack_top
df_stack_X:
    .skip 0x1000
df_stack_top:
