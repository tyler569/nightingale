
; Nightingale-64
; A 64 bit kernel for x86_64
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


; 32 bit bootstrap
section .text.low
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

	; Move PML4 to PML4[511]
	;mov eax, PML4 + (PAGE_PRESENT | PAGE_WRITEABLE)
	;mov dword [PML4 + (511 * 8)], eax ; Recursive map

	; Move PDPT to PML4[0]
	;mov eax, PDPT + (PAGE_PRESENT | PAGE_WRITEABLE)
	;mov dword [PML4], eax

	; Move PD to PDPT[0]
	;mov eax, PD + (PAGE_PRESENT | PAGE_WRITEABLE)
	;mov dword [PDPT], eax

    ; Move large_page(0) to PD[0]
    ;mov eax, 0 + (PAGE_PRESENT | PAGE_WRITEABLE | PAGE_ISHUGE)
    ;mov dword [PD], eax

    ; Move large_page(2M) to PD[1]
    ;mov eax, 0x200000 | (PAGE_PRESENT | PAGE_WRITEABLE | PAGE_ISHUGE)
    ;mov dword [PD + (1 * 8)], eax

	; Move PT0 to PD[0]
	;mov eax, PT0 + (PAGE_PRESENT | PAGE_WRITEABLE)
	;mov dword [PD], eax

	; Move PT1 to PD[1]
    ;mov eax, PT1 + (PAGE_PRESENT | PAGE_WRITEABLE)
	;mov dword [PD + 8], eax

	; Set PT for identitiy map of first 2MB (no map 0 page)
	;mov edi, PT0 + 8
	;mov eax, 0x1003
	;mov ecx, 511
.set_entry0:
	;mov dword [edi], eax
	;add eax, 0x1000
	;add edi, 8
	;loop .set_entry0

	;mov edi, PT1
	;mov eax, 0x200003
	;mov ecx, 512
;.set_entry1:
	;mov dword [edi], eax
	;add eax, 0x1000
	;add edi, 8
	;loop .set_entry1

set_paging:
	; And set up paging
	mov eax, PML4  ; PML4 pointer
	mov cr3, eax

	mov eax, cr4
	or eax, 3 << 4  ; Enable PAE and huge pages
	; or eax, 3 << 20 ; Enable SMEP and SMAP ; turns out this is not well supported
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

section .text
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

	mov edi, dword [rsp + 4]
	mov esi, dword [rsp]
	add rsp, 8

    push 0
    push 0
    push 0
    push 0

extern kernel_main
	call kernel_main

stop:
    hlt
	jmp stop


section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64 ; 8
    ; See Intel manual section 3.4.5 (Figure 3-8 'Segment Descriptor')

	dw 0            ; segment limit (ignored)
	dw 0            ; segment base (ignored)
	db 0            ; segment base (ignored)
	db 10011010b    ; present(1), dpl(2), type(5)
	db 00100000b    ; granularity(1), 32bit(1), 64bit(1), unused(1), limit(4)
	db 0            ; segment base (ignored)
.usrcode:
	dw 0            ; segment limit (ignored)
	dw 0            ; segment base (ignored)
	db 0            ; segment base (ignored)
	db 11111010b    ; present(1), dpl(2), type(5)
	db 00100000b    ; granularity(1), 32bit(1), 64bit(1), unused(1), limit(4)
	db 0            ; segment base (ignored)
.usrstack:
	dw 0            ; segment limit (ignored)
	dw 0            ; segment base (ignored)
	db 0            ; segment base (ignored)
	db 11110010b    ; present(1), dpl(2), type(4)
	db 00000000b    ; lots of stuff
	db 0            ; segment base (ignored)
.pointer:
    dw $ - gdt64 - 1
    dq gdt64


section .bss

; Boot kernel stack

align 0x10
stack:
    resb 0x10000
stack_top:


section .data
align 0x1000

; Initial paging structures

%define PAGE_PRESENT 0x01
%define PAGE_WRITEABLE 0x02
%define PAGE_USER 0x04
%define PAGE_ISHUGE 0x80

;; TESTING ONLY USER MODE KERNEL
%define PAGE_FLAGS (PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER)

PML4:
    dq PDPT + PAGE_FLAGS
    times 255 dq 0
    ; half
    dq PML4 + PAGE_FLAGS
    dq test_PML4 + PAGE_FLAGS
    times 253 dq 0
    dq 0
    ;dq PDPT + PAGE_FLAGS
PDPT:
    dq PD + PAGE_FLAGS
    times 511 dq 0
PD:
    dq PT0 + PAGE_FLAGS
    dq PT1 + PAGE_FLAGS
    times 510 dq 0

PT0:
    times 184 dq 0
    dq 0xb8000 + PAGE_FLAGS
    times 71 dq 0
%assign PAGE 0x100000 + PAGE_FLAGS
%rep 256
    dq PAGE
%assign PAGE PAGE + 0x1000
%endrep
    ; times 128 dq 0

PT1:
    times 512 dq 0
;%assign PAGE 0x200000 + PAGE_FLAGS
;%rep 512
;    dq PAGE
;%assign PAGE PAGE + 0x1000
;%endrep

; test second page table recureive structure
test_PML4:
    dq test_PDPT + PAGE_PRESENT
    times 255 dq 0
    dq test_PML4 ; can have, can't use for now
    dq test_PML4 + PAGE_PRESENT
    times 254 dq 0
test_PDPT:
    dq test_PD + PAGE_PRESENT
    times 511 dq 0
test_PD:
    dq test_PT + PAGE_PRESENT
    times 511 dq 0
test_PT:
    dq 0x12345000 + PAGE_PRESENT
    times 511 dq 0


