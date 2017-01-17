
#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x00000000

#define STACK_SIZE  0x4000

    .text
    .global start
    .global gdt_flush
    .extern gp 

multiboot_header:
    .align 4
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

start:
    movl $(stack + STACK_SIZE), %esp

    call kmain

    hlt

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



    .comm stack, STACK_SIZE
