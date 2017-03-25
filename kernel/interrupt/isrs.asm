
section .text
isr0:
	mov qword [0xb8000], 0x0000000000000000
	
	iretq


section .bss
idt:
	resq 64
idt_end:


section .rodata
idt_p:
	dw idt_end - idt - 1
	dq idt


section .text
global load_idt
load_idt:
	push rax

	lea rax, [isr0]
	mov word [idt], ax		; Offset 0..15
	mov word [idt + 2], 8   ; CS selector
	mov byte [idt + 4], 0	; IST..0
	mov byte [idt + 5], 10001111b	; P + TYPE
	shr rax, 16
	mov word [idt + 6], ax	; Offset 16..31
	shr rax, 16
	mov dword [idt + 8], eax ; Offset 32..63
	mov dword [idt + 12], 0  ; Reserved 0
	
	lidt [idt_p]

	pop rax
	ret
	
