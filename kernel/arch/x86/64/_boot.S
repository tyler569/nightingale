
// Nightingale-64
// A 64 bit kernel for x86_64
// Copyright (C) 2017, 2018, Tyler Philbrick

    .section ".rodata.multiboot"

header:
    .long 0xe85250d6
    .long 0 // Intel
    .long (header_end - header)
    .long 0x100000000 - (0xe85250d6 + 0 + (header_end - header))

    // end tag
    .long 0
    .long 0
    .long 0
header_end:

// Kernel VM offset.  Used to create some low versions of symbols
// for bootstrap
#define VMA 0xFFFFFFFF80000000

// 32 bit bootstrap

    .section ".low.text"
    .code32

    .globl start
start:
    movl $stack_top, %esp

    push %eax
    push %ebx

    // check for long mode:
    // Test for required cpuid function
    movl $0x80000000, %eax
    cpuid
    cmp $0x80000001, %eax
    jb no64

    // Test for long mode
    movl eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz $no64

    // set_paging:
    movl PML4(%rip), %eax
    movl %eax, %cr3

    // PAE and huge pages
    movl %cr4, %eax
    or $(3 << 4), %eax
    movl %eax, %cr4

    movl $0xC0000080, %ecx // IA32e_EFER MSR
    rdmsr
    // IA-32e enable | NXE
    or $(1 << 8 | 1 << 11), %eax
    wrmsr

    movl %cr0, %eax
    orl $1        , %eax // PE
    orl $(1 << 31), %eax // PG
    orl $(1 << 16), %eax // WP
    movl %eax, %cr0

    movl %cr4, %eax
    or $(1 << 7), %eax // Huge pages enable
    movl %eax, %cr4

    // enable_fpu:
    fninit

    // enable_sse:
    // mov eax, cr0
    // and eax, ~(1 << 2) // Clear CR0.EM
    // or eax, 1 << 1     // Set CR0.MP
    // mov cr0, eax

    // mov eax, cr4
    // or eax, 3 << 9     // Set CR4.OSFXSR and CR4.OSXMMEXCPT
    // mov cr4, eax

    lgdt (low_gdtp)

    movl 0x2f4b2f4f, (0xb8002) // OK

    ljmp 8, start_64


no64:
    // There is no long mode, print an error and halt
    movl $0x4f6f4f6e, (0xb8000) // no
    movl $0x4f344f36, (0xb8004) // 64
    cli
    hlt


    .section ".low.text"
    .code64
start_64:
    movq 4(%rsp), %rdi
    movq (%rsp), %rdi
    add %rdi, $8

    // Don't touch edi or esi again until kernel_main()

    movq start_higher_half, %rax
    jmp %rax

section .text
start_higher_half:
    lgdt [gdtp64]    // higher half gdt

    mov rsp, hhstack_top

    mov eax, 0
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

    mov rax, 0x5f345f365f345f36 // 6464
    mov qword [0xb8008], rax

load_tss:
    mov rax, tss64
    mov word [gdt64.tss + 2], ax
    shr rax, 16
    mov byte [gdt64.tss + 4], al
    shr rax, 8
    mov byte [gdt64.tss + 7], al
    shr rax, 8
    mov dword [gdt64.tss + 8], eax


    mov ax, gdt64.tssdesc
    ltr ax


extern idt_ptr
    lidt [idt_ptr]

    push 0          // rip = 0
    push 0          // rbp = 0
    mov rbp, rsp    // set up root of backtrace

    // rdi and rsi set above before jump to hh

extern kernel_main
    call kernel_main

stop:
    hlt
    jmp stop

section .low.bss
// Bootstrap low kernel stack
align 0x10
stack:
    resb 0x100
stack_top:


section .bss

align 0x1000
global boot_kernel_stack
global hhstack_guard_page
hhstack_guard_page:
    resb 0x1000
boot_kernel_stack:
hhstack:
    resb 0x1000
hhstack_top:

align 0x10
global int_stack
int_stack:
    resb 0x1000
int_stack_top:

global tss64.stack
    
section .data
tss64:
    dd 0              // reserved 0
.stack:
    dq int_stack_top  // stack pl0
    dq 0              // stack pl1
    dq 0              // stack pl2
    dq 0              // reserved 0
.ist0:
    dq 0
.ist1:
    dq 0
.ist2:
    dq 0
.ist3:
    dq 0
.ist4:
    dq 0
.ist5:
    dq 0
.ist6:
    dq 0
.ist7:
    dq 0
    dq 0              // reserved 0
    dw 0              // reserved 0
.iomap:
    dw tss64.end - tss64
.end:

section .data

%define KERNEL_CODE 0x9A
%define KERNEL_DATA 0x92
%define USER_CODE 0xFA
%define USER_DATA 0xF2
%define TSS 0xE9

%define LONG_MODE 0x20

gdt64:
    dq 0
.codedesc: equ $ - gdt64 // 8
.code:
    // See Intel manual section 3.4.5 (Figure 3-8 'Segment Descriptor')

    dw 0            // segment limit (ignored)
    dw 0            // segment base (ignored)
    db 0            // segment base (ignored)
    db KERNEL_CODE
    db LONG_MODE
    db 0            // segment base (ignored)
.usrcode:
    dw 0            // segment limit (ignored)
    dw 0            // segment base (ignored)
    db 0            // segment base (ignored)
    db USER_CODE
    db LONG_MODE
    db 0            // segment base (ignored)
.usrstack:
    dw 0            // segment limit (ignored)
    dw 0            // segment base (ignored)
    db 0            // segment base (ignored)
    db USER_DATA
    db LONG_MODE
    db 0            // segment base (ignored)
.tssdesc: equ $ - gdt64
.tss:
    // See Intel manual section 7.2.3 (Figure 7-4 'Format of TSS...')
    dw tss64.end - tss64 - 1
    dw 0
    db 0
    db TSS
    db LONG_MODE
    db 0
    dq 0
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

section .low.rodata
low_gdt64: equ gdt64 - VMA
low_gdtp:
    dw gdt64.pointer - gdt64 - 1
    dq low_gdt64


section .low.data
align 0x1000

// Initial paging structures

%define PAGE_PRESENT 0x01
%define PAGE_WRITEABLE 0x02
%define PAGE_USER 0x04
%define PAGE_ISHUGE 0x80
%define PAGE_GLOBAL 0x100

%define PAGE_FLAGS (PAGE_PRESENT | PAGE_WRITEABLE)

global boot_pt_root
boot_pt_root:
PML4:
    dq PDPT + PAGE_FLAGS
    times 255 dq 0
    // half
    dq PML4 + PAGE_FLAGS
    times 254 dq 0
    dq PDPT + PAGE_FLAGS
PDPT:
    dq PD + PAGE_FLAGS
    times 509 dq 0
    dq PD + PAGE_FLAGS
    dq 0
PD:
    dq PT0 + PAGE_FLAGS
    //dq PT1 + PAGE_FLAGS
    times 511 dq 0

PT0:
    times 184 dq 0
    dq 0xb8000 + PAGE_FLAGS
    times 71 dq 0
%assign PAGE 0x100000 + PAGE_FLAGS
%rep 256
    dq PAGE
%assign PAGE PAGE + 0x1000
%endrep
    // times 128 dq 0

//PT1:
//    times 512 dq 0
//%assign PAGE 0x200000 + PAGE_FLAGS
//%rep 512
//    dq PAGE
//%assign PAGE PAGE + 0x1000
//%endrep

// test second page table recureive structure
// test_PML4:
//     dq test_PDPT + PAGE_PRESENT
//     times 255 dq 0
//     dq test_PML4 // can have, can't use for now
//     dq test_PML4 + PAGE_PRESENT
//     times 254 dq 0
// test_PDPT:
//     dq test_PD + PAGE_PRESENT
//     times 511 dq 0
// test_PD:
//     dq test_PT + PAGE_PRESENT
//     times 511 dq 0
// test_PT:
//     dq 0x12345000 + PAGE_PRESENT
//     times 511 dq 0
// 

section .text
global read_ip
read_ip:
    mov rax, [rsp]
    ret
