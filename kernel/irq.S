
    .macro IRQ index
        .global irq\index
        irq\index:
            cli
            push $0
            push $(\index + 32)
            jmp irq_common_stub
    .endm

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

    .extern irq_handler

irq_common_stub:
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
    movl $irq_handler, %eax
    call *%eax       // A special call, preserves the 'eip' register
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp     // Cleans up the pushed error code and pushed ISR number
    iret           // pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!


