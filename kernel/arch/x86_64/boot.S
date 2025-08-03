// Nightingale-64
// A 64 bit kernel for x86_64
// Copyright (C) 2017, Tyler Philbrick

.intel_syntax noprefix

.section .text
.global start
start:
    mov rax, 0x5f345f365f345f36 // 6464
    mov [0xb8008], rax

// start_enable_sse:
    mov rax, cr0
    and rax, ~(1 << 2) // Clear CR0.EM
    or rax, 1 << 1     // Set CR0.MP
    mov cr0, rax

    mov rax, cr4
    or rax, 3 << 9     // Set CR4.OSFXSR and CR4.OSXMMEXCPT
    mov cr4, rax

// setup segments
    mov eax, 0
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

.extern idt_ptr
    lidt [idt_ptr]

    push 0          // rip = 0
    push 0          // rbp = 0
    mov rbp, rsp    // set up root of backtrace

    // rdi and rsi set above before jump to hh
.extern kernel_main
    call kernel_main
    nop

stop:
    hlt
    jmp stop

.global longjump_kcode
longjump_kcode:
    push 0                      // ss
    push rsi                    // sp
    pushf                       // flags
    push 0x8                    // cs
    push rdi                    // ip
    iretq

cpu_id:
    push rbx
    push rcx
    push rdx
    mov eax, 1
    cpuid
    shr ebx, 24
    mov eax, ebx
    pop rdx
    pop rcx
    pop rbx
    ret

// .global set_kernel_stack
// set_kernel_stack:
//     mov [tss64_stack], rdi
//     ret

.global read_ip
read_ip:
    mov rax, [rsp]
    ret

.section .bss

.align 0x1000
.global hhstack_guard_page
.global hhstack_X
.global hhstack_top
hhstack_guard_page:
    .skip 0x1000
hhstack_X:
    .skip 0x2000
hhstack_top:

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
