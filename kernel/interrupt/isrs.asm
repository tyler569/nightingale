
section .bss
idt:
	resq 256
idt_end:


section .rodata
idt_p:
	dw idt_end - idt - 1
	dq idt


section .text
global load_idt_gate
load_idt_gate:
	shl rdi, 4	; idt index * 16

	mov word [idt + rdi], si				; Offset 0..15
	mov word [idt + 2 + rdi], 8				; CS selector
	mov byte [idt + 4 + rdi], 0				; IST..0
	mov byte [idt + 5 + rdi], 10001110b		; P + TYPE
	shr rsi, 16
	mov word [idt + 6 + rdi], si			; Offset 16..31
	shr rsi, 16
	mov dword [idt + 8 + rdi], esi			; Offset 32..63
	mov dword [idt + 12 + rdi], 0			; Reserved 0
	
	ret

global load_idt
load_idt:

extern load_idt_gates
	call load_idt_gates

;	mov rdi, 0
;	lea rsi, [divide_by_zero_exception]
;	call load_idt_gate
;
;	mov rdi, 0xd
;	lea rsi, [general_protection_exception]
;	call load_idt_gate
;
;	mov rdi, 0x20
;	lea rsi, [int_20_handler]
;	call load_idt_gate

	lidt [idt_p]
	ret


