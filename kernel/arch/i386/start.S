
#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x00000000

#define STACK_SIZE  0x4000

    .text
    .global start
    .extern afterboot_init
    .extern kmain

multiboot_header:
    .align 4
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

start:
    movl $(stack + STACK_SIZE), %esp
    call afterboot_init
    call kmain
done:
    hlt
    jmp done 

    .global gdt_flush
    .extern gp 
gdt_flush:
    lgdt (gp)
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    ljmp $0x8, $flush2
flush2:
    ret

    .global idt_load
    .extern idtp
idt_load:
    lidt (idtp)
    ret

    .comm stack, STACK_SIZE
