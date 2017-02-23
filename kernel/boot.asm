
; Nightingale-64
; A 64 bit kernel for x86_64
; Copyright (C) 2017, Tyler Philbrick

section .multiboot
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

    push eax
    push ebx

    call check_long_mode
    call init_page_tables
    call set_paging
    call enable_fpu
    call enable_sse

    lgdt [gdt64.pointer]

    mov dword [0xb8000], 0x2f4b2f4f


    jmp gdt64.code:start_64

    hlt

check_long_mode:
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

    ret

init_page_tables:
    ; Initialize the init page tables
    mov eax, P3
    or eax, 0x3
    mov dword [P4], eax
    mov eax, P4
    or eax, 0x3
    mov dword [P4 + 2040], eax ; Recursive map
    mov eax, P2
    or eax, 0x3
    mov dword [P3], eax
    mov eax, 0x83
    mov dword [P2], eax
    or eax, 1 << 21
    mov dword [P2 + 8], eax

    mov edi, P1
    mov ebx, 0x00000003
    mov ecx, 512
.set_entry:
    mov dword [edi], ebx
    add ebx, 0x1000
    add edi, 8
    loop .set_entry

    ret

set_paging:
    ; And set up paging
    mov eax, P4  ; P4 pointer
    mov cr3, eax

    mov eax, cr4 ; PAE & huge pages
    or eax, 3 << 4
    mov cr4, eax

    mov ecx, 0xC0000080 ; long mode bit
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0 ; and enable paging
    or eax, 1 << 31
    mov cr0, eax
   
    ret

no64:
    ; There is no long mode, print an error and halt
    mov dword [0xb8000], 0x4f6f4f6e
    mov dword [0xb8004], 0x4f344f36
    hlt

enable_fpu:
    fninit
    ret

enable_sse:
    mov eax, cr0
    and eax, ~(1 << 2) ; Clear CR0.EM
    or eax, 1 << 1     ; Set CR0.MP
    mov cr0, eax

    mov eax, cr4
    or eax, 3 << 9     ; Set CR4.OSFXSR and CR4.OSXMMEXCPT
    mov cr4, eax

    ret

bits 64
start_64:
    mov eax, 0
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

    mov rax, 0x5f345f365f345f36
    mov qword [0xb8008], rax

extern main
    call main

    hlt

section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; code segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

section .bss
align 0x1000
stack:
    resb 4096
stack_top:
P4: ;PML4
    resq 512
P3: ;PDPT
    resq 512
P2: ;PD
    resq 512
P1: ;PT
    resq 512

