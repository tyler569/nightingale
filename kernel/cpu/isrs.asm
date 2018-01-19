
extern c_interrupt_shim
extern c_irq_shim

%macro pushaq 0
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

%macro popaq 0
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


%macro isrerr 2
	push %1
	pushaq
	mov rdi, rsp
	mov rax, %2
	call rax
	popaq
	add rsp, 16
	iretq
%endmacro

%macro isrnoerr 2
	push 0
	isrerr %1, %2
%endmacro


section .text

extern divide_by_zero_exception
extern generic_exception
extern panic_exception
extern syscall_handler
extern general_protection_exception
extern page_fault

extern timer_handler
extern uart_irq_handler
extern other_irq_handler

isr0: isrnoerr 0, divide_by_zero_exception
isr1: isrnoerr 1, generic_exception
isr2: isrnoerr 2, generic_exception
isr3: isrnoerr 3, generic_exception
isr4: isrnoerr 4, generic_exception
isr5: isrnoerr 5, generic_exception
isr6: isrnoerr 6, generic_exception
isr7: isrnoerr 7, generic_exception
isr8: isrerr 8, generic_exception
isr9: isrnoerr 9, generic_exception
isr10: isrerr 10, generic_exception
isr11: isrerr 11, generic_exception
isr12: isrerr 12, generic_exception
isr13: isrerr 13, generic_exception
isr14: isrerr 14, page_fault
isr15: isrnoerr 15, generic_exception
isr16: isrnoerr 16, generic_exception
isr17: isrerr 17, generic_exception
isr18: isrnoerr 18, generic_exception
isr19: isrnoerr 19, generic_exception
isr20: isrnoerr 20, generic_exception
isr21: isrnoerr 21, generic_exception
isr22: isrnoerr 22, generic_exception
isr23: isrnoerr 23, generic_exception
isr24: isrnoerr 24, generic_exception
isr25: isrnoerr 25, generic_exception
isr26: isrnoerr 26, generic_exception
isr27: isrnoerr 27, generic_exception
isr28: isrnoerr 28, generic_exception
isr29: isrnoerr 29, generic_exception
isr30: isrerr 30, generic_exception
isr31: isrnoerr 31, generic_exception
irq0: isrnoerr 32, timer_handler
irq1: isrnoerr 33, other_irq_handler
irq2: isrnoerr 34, other_irq_handler
irq3: isrnoerr 35, other_irq_handler
irq4: isrnoerr 36, uart_irq_handler
irq5: isrnoerr 37, other_irq_handler
irq6: isrnoerr 38, other_irq_handler
irq7: isrnoerr 39, other_irq_handler
irq8: isrnoerr 40, other_irq_handler
irq9: isrnoerr 41, other_irq_handler
irq10: isrnoerr 42, other_irq_handler
irq11: isrnoerr 43, other_irq_handler
irq12: isrnoerr 44, other_irq_handler
irq13: isrnoerr 45, other_irq_handler
irq14: isrnoerr 46, other_irq_handler
irq15: isrnoerr 47, other_irq_handler
isr127: isrnoerr 127, panic_exception
isr128: isrnoerr 128, syscall_handler


section .bss
idt:
	resq 512
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

    mov rdi, 127
    mov rsi, isr127
    call load_idt_gate
    mov rdi, 128
    mov rsi, isr128
    call load_idt_gate

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

