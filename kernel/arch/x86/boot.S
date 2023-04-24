// Nightingale-64
// A 64 bit kernel for x86_64
// Copyright (C) 2017, Tyler Philbrick

.intel_syntax noprefix

.section .text
.global start
start:
    lgdt [gdt64_pointer]    // higher half gdt

    mov rsp, offset hhstack_top

    mov eax, 0
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

    mov rax, 0x5f345f365f345f36 // 6464
    mov [0xb8008], rax

// load tss
    lea rax, tss64
    mov [gdt64_tss + 2], ax
    shr rax, 16
    mov [gdt64_tss + 4], al
    shr rax, 8
    mov [gdt64_tss + 7], al
    shr rax, 8
    mov [gdt64_tss + 8], eax


    mov ax, offset TSSDESC
    ltr ax

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

cpu_id:
    push rbx
    push rcx
    push rdx
    mov     eax, 1
    cpuid
    shr    ebx, 24
    mov eax, ebx
    pop rdx
    pop rcx
    pop rbx
    ret

.global set_kernel_stack
set_kernel_stack:
    mov [tss64_stack], rdi
    ret

.global read_ip
read_ip:
    mov rax, [rsp]
    ret

.section .bss

.align 0x1000
.global boot_kernel_stack
.global hhstack_guard_page
hhstack_guard_page:
    .skip 0x1000
boot_kernel_stack:
hhstack:
    .skip 0x2000
hhstack_top:

.align 0x10
.global int_stack
int_stack:
    .skip 0x2000
int_stack_top:

.align 0x10
df_stack:
    .skip 0x1000
df_stack_top:

.section .data

tss64:
    .long 0              // reserved 0
tss64_stack:
    .quad int_stack_top  // RSP0
    .quad 0              // RSP1
    .quad 0              // RSP2
    .quad 0              // reserved 0
tss64_ist1:
    .quad df_stack_top
tss64_ist2:
    .quad 0
tss64_ist3:
    .quad 0
tss64_ist4:
    .quad 0
tss64_ist5:
    .quad 0
tss64_ist6:
    .quad 0
tss64_ist7:
    .quad 0
    .quad 0             // reserved 0
    .word 0             // reserved 0
tss64_iomap:
    .word tss64_end - tss64
tss64_end:

.section .data

#define KERNEL_CODE 0x9A
#define KERNEL_DATA 0x92
#define USER_CODE 0xFA
#define USER_DATA 0xF2
#define TSS 0xE9

#define LONG_MODE 0x20

gdt64:
    .quad 0
gdt64_code:
    // See Intel manual section 3.4.5 (Figure 3-8 'Segment Descriptor')

    .word 0            // segment limit (ignored)
    .word 0            // segment base (ignored)
    .byte 0            // segment base (ignored)
    .byte KERNEL_CODE
    .byte LONG_MODE
    .byte 0            // segment base (ignored)
gdt64_data:
    // A valid kernel data segment has to be at kernel_cs + 8 for syscall SS.

    .word 0            // segment limit (ignored)
    .word 0            // segment base (ignored)
    .byte 0            // segment base (ignored)
    .byte KERNEL_DATA
    .byte LONG_MODE
    .byte 0            // segment base (ignored)
gdt64_usrcode:
    .word 0            // segment limit (ignored)
    .word 0            // segment base (ignored)
    .byte 0            // segment base (ignored)
    .byte USER_CODE
    .byte LONG_MODE
    .byte 0            // segment base (ignored)
gdt64_usrdata:
    .word 0            // segment limit (ignored)
    .word 0            // segment base (ignored)
    .byte 0            // segment base (ignored)
    .byte USER_DATA
    .byte LONG_MODE
    .byte 0            // segment base (ignored)
gdt64_tss:
    // See Intel manual section 7.2.3 (Figure 7-4 'Format of TSS...')
    .word tss64_end - tss64 - 1
    .word 0
    .byte 0
    .byte TSS
    .byte LONG_MODE
    .byte 0
    .quad 0
gdt64_end:
.set GDTLEN, . - gdt64

.set CODEDESC, gdt64_code - gdt64
.set TSSDESC, gdt64_tss - gdt64

gdt64_pointer:
    .word GDTLEN
    .quad gdt64
