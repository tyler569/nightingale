// Nightingale-64
// A 64 bit kernel for x86_64
// Copyright (C) 2017, Tyler Philbrick

.intel_syntax noprefix

.section .rodata.multiboot
header:
    .long 0xe85250d6
    .long 0 // Intel
    .long (header_end - header)
    .long 0x100000000 - (0xe85250d6 + 0 + (header_end - header)) // Checksum

    // Multiboot2 tags here
    // Framebuffer tag
    //    .word 5
    //    .word 0x0000
    //    .long 20
    //    .long 1024
    //    .long 768
    //    .long 24

    // Header location tag
    //    .word 10
    //    .word 0
    //    .long 24
    //    .long 0x00000000
    //    .long 0xffffffff
    //    .long 4096
    //    .long 0x150000

    // end tag
    .long 0, 0, 0
header_end:

// Kernel VM offset.  Used to create some low versions of symbols
// for bootstrap
#define VMA 0xFFFFFFFF80000000

// 32 bit bootstrap
.section .low.text
.code32

.global start
start:
    mov esp, stack_top

    push eax
    push ebx

.check_long_mode:
    // Test for required cpuid function
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb no64

    // Test for long mode
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz no64

.init_page_tables:
    // Used to be manual, removed in commit 160

.set_paging:
    // And set up paging
    mov eax, PML4  // PML4 pointer
    mov cr3, eax

    mov eax, cr4
    or eax, 3 << 4  // Enable PAE and huge pages
    // or eax, 3 << 20 // Enable SMEP and SMAP // turns out this is not well supported
    mov cr4, eax

    mov ecx, 0xC0000080 // IA32e_EFER MSR
    rdmsr
    or eax, (1 << 8) | (1 << 11) // IA-32e enable | NXE
    wrmsr

    mov eax, cr0
    or eax, 1 << 0  // PE
    or eax, 1 << 31 // PG
    or eax, 1 << 16 // WP
    mov cr0, eax

    mov eax, cr4
    or eax, 1 << 7  // PGE for global pages
    // or eax, 1 << 17 // PCIDE
    mov cr4, eax

.enable_fpu:
    fninit

.enable_sse:
    mov eax, cr0
    and eax, ~(1 << 2) // Clear CR0.EM
    or eax, 1 << 1     // Set CR0.MP
    mov cr0, eax

    mov eax, cr4
    or eax, 3 << 9     // Set CR4.OSFXSR and CR4.OSXMMEXCPT
    mov cr4, eax

.finish_init:
    lgdt [low_gdtp]

    mov dword ptr [0xb8002], 0x2f4b2f4f // OK

    push 8
    push start_64
    retf

no64:
    // There is no long mode, print an error and halt
    mov dword ptr [0xb8000], 0x4f6f4f6e // no
    mov dword ptr [0xb8004], 0x4f344f36 // 64
    hlt


.section .low.text
.code64
start_64:
    mov edi, [rsp + 4]
    mov esi, [rsp]
    add rsp, 8

    // Don't touch edi or esi again until kernel_main()

    mov rax, start_higher_half
    jmp rax

.section .text
start_higher_half:
    lgdt [gdt64_pointer]    // higher half gdt

    mov rax, 0xFFFFFFFF80000000
    add rax, boot_p4_mapping
    mov qword ptr [rax], 0

    mov rax, 0xFFFFFFFF80000000
    add rax, boot_p3_mapping
    mov qword ptr [rax], 0

    mov rsp, hhstack_top

    mov eax, 0
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

    mov rax, 0x5f345f365f345f36 // 6464
    mov [0xb8008], rax

// load tss
    mov rax, tss64
    mov [gdt64_tss + 2], ax
    shr rax, 16
    mov [gdt64_tss + 4], al
    shr rax, 8
    mov [gdt64_tss + 7], al
    shr rax, 8
    mov [gdt64_tss + 8], eax


    mov ax, TSSDESC
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

.global set_kernel_stack
set_kernel_stack:
    mov [gdt64_stack], rdi
    ret

.global read_ip
read_ip:
    mov rax, [rsp]
    ret

.section .low.bss
// Bootstrap low kernel stack
.align 0x10
stack:
    .skip 0x100
stack_top:

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

.section .data

tss64:
    .long 0              // reserved 0
.stack:
    .quad int_stack_top  // stack pl0
    .quad 0              // stack pl1
    .quad 0              // stack pl2
    .quad 0              // reserved 0
.ist0:
    .quad 0
.ist1:
    .quad 0
.ist2:
    .quad 0
.ist3:
    .quad 0
.ist4:
    .quad 0
.ist5:
    .quad 0
.ist6:
    .quad 0
.ist7:
    .quad 0
    .quad 0              // reserved 0
    .word 0              // reserved 0
.iomap:
    .word tss64.end - tss64
.end:

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
    .word tss64.end - tss64 - 1
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

.section .low.rodata
low_gdt64 = gdt64 - VMA
low_gdtp:
    .word GDTLEN
    .quad low_gdt64


.section .low.data
.align 0x1000

// Initial paging structures

#define PAGE_PRESENT 0x01
#define PAGE_WRITEABLE 0x02
#define PAGE_USER 0x04
#define PAGE_ISHUGE 0x80
#define PAGE_GLOBAL 0x100

#define PAGE_FLAGS (PAGE_PRESENT | PAGE_WRITEABLE)

.global boot_pt_root
boot_pt_root:
PML4:
boot_p4_mapping:
    .quad PDPT + PAGE_FLAGS
    .fill 255, 8
    // half
    .quad PHYS_PDPT + PAGE_FLAGS
    .fill 254, 8
    .quad PDPT + PAGE_FLAGS
PDPT:
boot_p3_mapping:
    .quad PD + PAGE_FLAGS
    .fill 509, 8
    .quad PD + PAGE_FLAGS
    .quad 0
PD:
    .quad PT0 + PAGE_FLAGS
    .quad PT1 + PAGE_FLAGS
    .fill 510, 8

PT0: // PT0 covers 000000 -> 200000
    .fill 184, 8
    .quad 0xb8000 + PAGE_FLAGS
    .fill 71, 8
.set PAGE, 0x100000 + PAGE_FLAGS
.rep 256
    .quad PAGE
.set PAGE, PAGE + 0x1000
.endr

PT1: // PT1 covers 200000 -> 400000
.set PAGE, 0x200000 + PAGE_FLAGS
.rep 512
    .quad PAGE
.set PAGE, PAGE + 0x1000
.endr

PHYS_PDPT:
    .quad 0 + PAGE_PRESENT | PAGE_WRITEABLE | PAGE_ISHUGE | PAGE_GLOBAL
    .fill 511, 8

