
extern c_interrupt_shim
extern c_irq_shim

%macro pushallq 0
	push rax
	push rcx
	push rdx
	push rbx
	push rsi
	push rdi
	push rsp
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popallq 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rsp
	pop rdi
	pop rsi
	pop rbx
	pop rdx
	pop rcx
	pop rax
%endmacro


%macro isrerr 1
	push %1
	pushallq
	mov rdi, rsp
	mov rax, c_interrupt_shim
	call rax
	popallq
	add rsp, 16
	iretq
%endmacro

%macro isrnoerr 1
	push 0
	isrerr %1
%endmacro

%macro irq 1
	push 0
	push %1
	pushallq
	mov rdi, rsp
	mov rax, c_irq_shim
	call rax
	popallq
	add rsp, 16
	iretq
%endmacro


section .text

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
irq0: irq 32
irq1: irq 33
irq2: irq 34
irq3: irq 35
irq4: irq 36
irq5: irq 37
irq6: irq 38
irq7: irq 39
irq8: irq 40
irq9: irq 41
irq10: irq 42
irq11: irq 43
irq12: irq 44
irq13: irq 45
irq14: irq 46
irq15: irq 47


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

	lidt [idt_p]
	ret

global enable_irqs
enable_irqs:
	sti
	ret

global disable_irqs
disable_irqs:
	cli
	ret

