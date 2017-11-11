
; Nightingale-64
; A 64 bit kernel for x86_64
; Copyright (C) 2017, Tyler Philbrick

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

	; end tag
	dd 0
	dd 0
	dd 0
header_end:


; 32 bit bootstrap
section .text
bits 32

global start
start:
	mov esp, stack_top

	push eax
	push ebx

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

init_page_tables:
	; Initialize the init page tables
	; The init page tables map everything as writable
	; The kernel must remap itself based on information
	; obtained from multiboot to prevent this from compromising W^X.

	; Move PDPT to PML4[0]
	mov eax, PDPT
	or eax, 0x3
	mov dword [PML4], eax

	; Move PML4 to PML4[511]
	mov eax, PML4
	or eax, 0x3
	mov dword [PML4 + (511 * 8)], eax ; Recursive map

	; Move large_page(0) to PDPT[0]
	mov eax, 0
	or eax, 0x13
	mov dword [PDPT], eax

	; Move large_page(4M) to PDPT[0]
	mov eax, 0x100000
	or eax, 0x13
	mov dword [PDPT + (1 * 8)], eax

	; Move PD to PDPT[0]
	;mov eax, PD
	;or eax, 0x3
	;mov dword [PDPT], eax

	; Move PT to PD[0]
	;mov eax, PT
	;or eax, 0x3
	;mov dword [PD], eax

	; Set PT for identitiy map of first 2MB
	;mov edi, PT
	;mov eax, 0x3
	;mov ecx, 512
;.set_entry:
	;mov dword [edi], eax
	;add eax, 0x1000
	;add edi, 8
	;loop .set_entry

set_paging:
	; And set up paging
	mov eax, PML4  ; PML4 pointer
	mov cr3, eax

	mov eax, cr4
	or eax, 3 << 4  ; Enable PAE and huge pages
	or eax, 3 << 20 ; Enable SMEP and SMAP
	mov cr4, eax

	mov ecx, 0xC0000080 ; IA32e_EFER MSR
	rdmsr
	or eax, (1 << 8) | (1 << 11) ; IA-32e enable | NXE
	wrmsr

	mov eax, cr0
	or eax, 1 << 0  ; PE
	or eax, 1 << 31 ; PG
	or eax, 1 << 16 ; WP
	mov cr0, eax

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

	lgdt [gdt64.pointer]

	mov dword [0xb8002], 0x2f4b2f4f ; OK

	jmp gdt64.code:start_64


no64:
	; There is no long mode, print an error and halt
	mov dword [0xb8000], 0x4f6f4f6e ; no
	mov dword [0xb8004], 0x4f344f36 ; 64
	hlt

bits 64
start_64:
	mov eax, 0
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax

	mov rax, 0x5f345f365f345f36 ; 6464
	mov qword [0xb8008], rax

extern load_idt
	call load_idt

	mov edi, dword [rsp]
	mov esi, dword [rsp + 4]
	add rsp, 8

extern kernel_main
	call kernel_main

stop:
    hlt
	jmp stop


section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64 ; 8
	dw 0
	dw 0
	db 0
	db 10011010b
	db 00100000b
	db 0
.pointer:
    dw $ - gdt64 - 1
    dq gdt64


section .bss
align 0x1000

stack:
    resb 0x1000
stack_top:

initial_heap:
	resb 0x10000

PML4:
    resq 512
PDPT:
    resq 512
PD:
    resq 512
PT:
    resq 512

PDPT_high:
    resq 512
PD_high:
    resq 512

