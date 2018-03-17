
extern c_interrupt_shim

; error code
; interrupt number
; all registers

section .text

interrupt_shim:
	push rax
	push rcx
	push rbx
	push rdx
	push rsi
	push rdi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	mov rdi, rsp
	mov rax, c_interrupt_shim
	call rax
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rbx
	pop rcx
	pop rax
	add rsp, 16
	iretq

%macro isrnoerr 1
    push 0
    push %1
    jmp interrupt_shim
%endmacro

%macro isrerr 1
    push %1
    jmp interrupt_shim
%endmacro

isr0: isrnoerr 0
isr1: isrnoerr 1
isr2: isrnoerr 2
isr3: isrnoerr 3
isr4: isrnoerr 4
isr5: isrnoerr 5
isr6: isrnoerr 6
isr7: isrnoerr 7
isr8: isrerr 8
isr9: isrnoerr 9
isr10: isrerr 10
isr11: isrerr 11
isr12: isrerr 12
isr13: isrerr 13
isr14: isrerr 14
isr15: isrnoerr 15
isr16: isrnoerr 16
isr17: isrerr 17
isr18: isrnoerr 18
isr19: isrnoerr 19
isr20: isrnoerr 20
isr21: isrnoerr 21
isr22: isrnoerr 22
isr23: isrnoerr 23
isr24: isrnoerr 24
isr25: isrnoerr 25
isr26: isrnoerr 26
isr27: isrnoerr 27
isr28: isrnoerr 28
isr29: isrnoerr 29
isr30: isrerr 30
isr31: isrnoerr 31
irq0: isrnoerr 32
irq1: isrnoerr 33
irq2: isrnoerr 34
irq3: isrnoerr 35
irq4: isrnoerr 36
irq5: isrnoerr 37
irq6: isrnoerr 38
irq7: isrnoerr 39
irq8: isrnoerr 40
irq9: isrnoerr 41
irq10: isrnoerr 42
irq11: isrnoerr 43
irq12: isrnoerr 44
irq13: isrnoerr 45
irq14: isrnoerr 46
irq15: isrnoerr 47
isr128: isrnoerr 128

;%assign isr_num 48
;%rep 208
;isr%[isr_num] isrnoerr %[isr_num]
;%assign isr_num isr_num+1
;%endrep

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

; Loop through exception ISRs and add them to the IDT
%assign isr_num 0
%rep 32
	mov rdi, isr_num
	mov rsi, isr%[isr_num]
	call load_idt_gate
%assign isr_num isr_num+1
%endrep

; Loop through IRQ ISRs and add them to the IDT
%assign irq_num 0
%rep 16
	mov rdi, irq_num + 32
	mov rsi, irq%[irq_num]
	call load_idt_gate
%assign irq_num irq_num+1
%endrep

    mov rdi, 128
    mov rsi, isr128
    call load_idt_gate

	lidt [idt_p]
	ret

