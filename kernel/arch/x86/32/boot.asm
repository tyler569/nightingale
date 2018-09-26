
; Nightingale-32
; A 32 bit kernel for x86_32
; Copyright (C) 2017, Tyler Philbrick
; vim: syntax=nasm

section .rodata.multiboot
header:
    dd 0xe85250d6
    dd 0 ; Intel
    dd (header_end - header)
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header)) ; Checksum

    ; Multiboot2 tags here
    ; Framebuffer tag
    ;    dw 5
    ;    dw 0x0000
    ;    dd 20
    ;    dd 1024
    ;    dd 768
    ;    dd 24

    ; Header location tag
    ;    dw 10
    ;    dw 0
    ;    dd 24
    ;    dd 0x00000000
    ;    dd 0xffffffff
    ;    dd 4096
    ;    dd 0x150000

    ; end tag
    dd 0
    dd 0
    dd 0
header_end:

; Kernel VM offset.  Used to create some low versions of symbols
; for bootstrap
%define VMA 0x80000000

; 32 bit bootstrap
section .low.text
bits 32

global start
start:
    mov esp, stack_top

    push eax
    push ebx

set_paging:
    ; And set up paging
    mov eax, PD
    mov cr3, eax

    mov eax, cr0
    or eax, 1 << 0  ; PE
    or eax, 1 << 31 ; PG
    or eax, 1 << 16 ; WP
    mov cr0, eax

    mov eax, cr4
    or eax, 1 << 7  ; PGE for global pages
    ; or eax, 1 << 17 ; PCIDE
    mov cr4, eax

enable_fpu:
    fninit

enable_sse:
    mov eax, cr0
    and eax, ~(1 << 2) ; Clear CR0.EM
    or eax, 1 << 1     ; Set CR0.MP
    mov cr0, eax

    mov eax, cr4
    or eax, 3 << 9     ; Set CR4.OSFXSR and CR4.OSXMMEXCPT
    mov cr4, eax

finish_init:
    lgdt [low_gdtp]
    mov dword [0xb8002], 0x2f4b2f4f ; OK

    jmp gdt32.codedesc:start_32


no32:
    ; There is no long mode, print an error and halt
    mov dword [0xb8000], 0x4f6f4f6e ; no
    mov dword [0xb8004], 0x4f344f36 ; 32
    hlt


section .low.text
bits 32
start_32:
    mov edi, dword [esp + 4]
    mov esi, dword [esp]
    add esp, 8

    ; Don't touch edi or esi again until kernel_main()

    mov eax, start_higher_half
    jmp eax

section .text
start_higher_half:
    lgdt [gdt32.pointer]    ; higher half gdt

    mov esp, hhstack_top

    mov eax, 0
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

    mov eax, 0x5f325f33 ; 32
    mov dword [0xb8008], eax

load_tss:
    mov eax, tss32
    mov word [gdt32.tss + 2], ax
    shr eax, 16
    mov byte [gdt32.tss + 4], al
    shr eax, 8
    mov byte [gdt32.tss + 7], al
    shr eax, 8
    mov dword [gdt32.tss + 8], eax


    mov ax, gdt32.tssdesc
    ltr ax


extern idt_ptr
    lidt [idt_ptr]

    push 0          ; rip = 0
    push 0          ; rbp = 0
    mov ebp, esp    ; set up root of backtrace

    ; rdi and rsi set above before jump to hh

extern kernel_main
    call kernel_main

stop:
    hlt
    jmp stop

section .low.bss
; Bootstrap low kernel stack
align 0x10
stack:
    resb 0x100
stack_top:


section .bss

align 0x1000
global boot_kernel_stack
boot_kernel_stack:
hhstack:
    resb 0x1000
hhstack_top:

align 0x10
global int_stack
int_stack:
    resb 0x1000
int_stack_top:

global tss32.stack
    
section .data
tss32:
    dd 0              ; reserved 0
.stack:
    dd int_stack_top  ; stack pl0
    dd 0              ; ss pl0
    dd 0              ; stack pl1
    dd 0              ; ss pl1 
    dd 0              ; stack pl2
    dd 0              ; ss pl2
    dd 0              ; reserved 0
%rep 104-32
    db 0              ; all the registers I don't care about
%endrep
.end:

section .rodata

%define KERNEL_CODE 0x9A
%define KERNEL_DATA 0x92
%define USER_CODE 0xFA
%define USER_DATA 0xF2
%define TSS 0xE9

%define NOT_LONG_MODE 0x00

gdt32:
    dq 0
.codedesc: equ $ - gdt32 ; 8
.code:
    ; See Intel manual section 3.4.5 (Figure 3-8 'Segment Descriptor')

    dw 0            ; segment limit (ignored)
    dw 0            ; segment base (ignored)
    db 0            ; segment base (ignored)
    db KERNEL_CODE
    db NOT_LONG_MODE
    db 0            ; segment base (ignored)
.usrcode:
    dw 0            ; segment limit (ignored)
    dw 0            ; segment base (ignored)
    db 0            ; segment base (ignored)
    db USER_CODE
    db NOT_LONG_MODE
    db 0            ; segment base (ignored)
.usrstack:
    dw 0            ; segment limit (ignored)
    dw 0            ; segment base (ignored)
    db 0            ; segment base (ignored)
    db USER_DATA
    db NOT_LONG_MODE
    db 0            ; segment base (ignored)
.tssdesc: equ $ - gdt32
.tss:
    ; See Intel manual section 7.2.3 (Figure 7-4 'Format of TSS...')
    dw tss32.end - tss32 - 1
    dw 0
    db 0
    db TSS
    db NOT_LONG_MODE
    db 0
.pointer:
    dw $ - gdt32 - 1
    dd gdt32

section .low.rodata
low_gdt32: equ gdt32 - VMA
low_gdtp:
    dw gdt32.pointer - gdt32 - 1
    dd low_gdt32


section .low.data
align 0x1000

; Initial paging structures

%define PAGE_PRESENT 0x01
%define PAGE_WRITEABLE 0x02
%define PAGE_USER 0x04
%define PAGE_ISHUGE 0x80
%define PAGE_GLOBAL 0x100

%define PAGE_FLAGS (PAGE_PRESENT | PAGE_WRITEABLE)

global boot_pt_root
boot_pt_root:
PD:
    dd PT0 + PAGE_FLAGS
    times 511 dd 0
    dd PT0 + PAGE_FLAGS
    times 510 dd 0
    dd PD + PAGE_FLAGS

PT0:
    times 184 dd 0
    dd 0xb8000 + PAGE_FLAGS
    times 71 dd 0
%assign PAGE 0x100000 + PAGE_FLAGS
%rep 384
%assign PAGE PAGE + 0x1000
%endrep

section .text
global read_ip
read_ip:
    mov eax, [esp]
    ret

