
; Nightingale-64
; A 64 bit kernel for x86_64
; Copyright (C) 2017, Tyler Philbrick

section .multiboot
header:
    dd 0xe85250d6
    dd 0x00000000 ; Flags
    dd (header_end - header)
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header)) ; Checksum

    ; Multiboot2 tags here

    ; end tag
    dd 0
    dd 0
    dd 0
header_end:


; 32 bit bootstrap

global start

section .text.init
bits 32

start:
    mov esp, stack_top

    ; Test for required cpuid function
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb no64

    ; Test for long mode
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz no64

    ; Initialize the init page tables
    mov eax, P3
    or eax, 0x3
    mov dword [P4], eax
    mov eax, P2
    or eax, 0x3
    mov dword [P3], eax
    mov eax, P1
    or eax, 0x3
    mov dword [P2], eax

    mov edi, P1
    mov ebx, 0x00000003
    mov ecx, 512
.set_entry:
    mov dword [edi], ebx
    add ebx, 0x1000
    add edi, 8
    loop .set_entry

    ; And set up paging
    mov eax, P4  ; P4 pointer
    mov cr3, eax

    mov eax, cr4 ; PAE
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080 ; long mode bit
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0 ; and enable paging
    or eax, 1 << 31
    mov cr0, eax

    mov dword [0xb8000], 0x2f4b2f4f

    hlt

no64:
    ; There is no long mode, print an error and halt
    mov dword [0xb8000], 0x4f6f4f6e
    mov dword [0xb8004], 0x4f344f36
    hlt


section .bss
align 0x1000
stack:
    resb 4096
stack_top:
P4: ;PML4
    resd 1024
P3: ;PDPT
    resd 1024
P2: ;PD
    resd 1024
P1: ;PT
    resd 1024

