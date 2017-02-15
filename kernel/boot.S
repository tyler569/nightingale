
#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x00000002

#define KERN_PHYS_BASE      (0x100000)
#define KERN_VIRT_BASE      (0x80000000)
#define KERN_VIRT_OFFSET    (KERN_VIRT_BASE - KERN_PHYS_BASE)

#define KERN_PAGE           (KERN_VIRT_BASE >> 22)

#define PHY(addr)           (addr - KERN_VIRT_OFFSET)

#define STACK_SIZE          0x4000

    .section .data
    .align 0x1000
    .global pagedir

pt0:
    .fill 1024, 4, 0

pt_kern:
    .fill 1024, 4, 0

pagedir:
    .fill 1024, 4, 0


    .section .multiboot

multiboot_header:
    .align 4
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

    .section .text
    .global start
    .extern kernel_end

start:
    .long PHY(_start)

_start:
    cli

// Fill page directory with initial mappings
    movl $(PHY(pt0)), %eax
    orl $0x103, %eax
    movl %eax, (PHY(pagedir))

    movl $(PHY(pt_kern)), %eax
    orl $0x103, %eax
    movl %eax, (PHY(pagedir) + (KERN_PAGE * 4))

    movl $(PHY(pagedir)), %eax
    orl $0x103, %eax
    movl %eax, (PHY(pagedir) + 1023 * 4)

// Fill the page tables with initial mappings
    movl $0x103, %eax
    movl $PHY(pt0), %edi
    movl $257, %ecx
.nxt0:
    movl %eax, (%edi)
    addl $4, %edi
    addl $0x1000, %eax
    subl $1, %ecx
    jnz .nxt0

    movl $0x100103, %eax
    movl $PHY(pt_kern), %edi
    movl $kernel_end, %ecx    // Map only the kernel binary
    subl $KERN_VIRT_BASE, %ecx
    shrl $12, %ecx
.nxt1:
    movl %eax, (%edi)
    addl $4, %edi
    addl $0x1000, %eax
    subl $1, %ecx
    jnz .nxt1


    movl $(PHY(pagedir)), %ecx
    movl %ecx, %cr3
/*
    movl %cr4, %ecx
    orl $0x00000010, %ecx // enable 4MB pages
    movl %ecx, %cr4
*/
    movl %cr0, %ecx
    orl $0x80000000, %ecx // enable paging
    movl %ecx, %cr0

    movl $(higherhalf_start), %ecx
    jmp *%ecx

higherhalf_start:
    movl $0, %eax
    movl %eax, (pt0 + 257 * 4)
    
    movl %cr3, %ecx
    movl %ecx, %cr3

    movl $(stack + STACK_SIZE), %esp

    push %ebx            // multiboot.h

    call kmain
    
    cli
    hlt


    .comm stack, STACK_SIZE
