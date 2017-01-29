
#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x00000000

#define KERN_VIRT_BASE      0xC0000000
#define KERN_PAGE           (KERN_VIRT_BASE >> 22)

#define STACK_SIZE  0x4000

    .data
    .align 0x1000
init_pagedir:
// Identity map the first 4MB
    .long 0x00000083
    .fill (KERN_PAGE - 1), 4, 0
    .long 0x00000083
    .fill (1024 - KERNEL_PAGE - 1), 4, 0


    .text
    .global start

multiboot_header:
    .align 4
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

start:
    .long (_start - KERN_VIRT_BASE)

_start:
    movl $(init_pagedir - KERN_VIRT_BASE), %ecx
    movl %ecx, %cr3

    movl %cr4, %ecx
    or $0x00000010, %ecx // enable 4MB pages
    movl %ecx, %cr4

    movl %cr0, %ecx
    or $0x80000000, %ecx // enable paging
    movl %ecx, %cr0

    movl $(higherhalf_start), %ecx
    jmp *%ecx

higherhalf_start:
    // movl $0, (init_pagedir)
    movl %cr3, %ecx
    movl %ecx, %cr3

    mov $stack, %esp

    push %ebx            // multiboot.h

    call main
    
    cli
    hlt

/*
non-higherhalf code from the before time

_start:
    movl $(stack + STACK_SIZE), %esp
    pushl %ebx
    call main
done:
    hlt
    jmp done 
*/
    .comm stack, STACK_SIZE
