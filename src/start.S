
#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x00000000

#define STACK_SIZE  0x4000

    .text
    .globl  start

multiboot_header:
    .align 4
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

start:
    movl $(stack + STACK_SIZE), %esp

    call kmain

    hlt

    .comm stack, STACK_SIZE
