
    .macro ISR_NOERR index
        .global isr\index
        isr\index:
            cli
            push $0
            push $\index
            jmp isr_common_stub
    .endm

    .macro ISR_ERR index
        .global isr\index
        isr\index:
            cli
            push $\index
            jmp isr_common_stub
    .endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31 

// We call a C function in here. We need to let the assembler know
// that '_fault_handler' exists in another file
    .extern fault_handler

// This is our common ISR stub. It saves the processor state, sets
// up for kernel mode segments, calls the C-level fault handler,
// and finally restores the stack frame.
isr_common_stub:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    xor %eax, %eax
    movw $0x10, %ax   // Load the Kernel Data Segment descriptor!
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movl %esp, %eax   // Push us the stack
    push %eax
    movl $fault_handler, %eax
    call *%eax       // A special call, preserves the 'eip' register
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp     // Cleans up the pushed error code and pushed ISR number
    iret           // pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!


    .global idt_load
    .extern idtp
idt_load:
    lidt (idtp)
    ret


