
#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x00000000

#define STACK_SIZE  0x4000

    .section .text
    .code32
    .global _start

multiboot_header:
    .align 4
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

_start:
    cli

    // Check if CPUID is supported by attempting to flip the ID bit (bit 21) in
    // the FLAGS register. If we can flip it, CPUID is available.
 
    // Copy FLAGS in to EAX via stack
    pushfl
    pop %eax
 
    // Copy to ECX as well for comparing later on
    mov %eax, %ecx
 
    // Flip the ID bit
    xor $(1 << 21), %eax
 
    // Copy EAX to FLAGS via the stack
    push %eax
    popfl
 
    // Copy FLAGS back to EAX (with the flipped bit if CPUID is supported)
    pushfl
    pop %eax
 
    // Restore FLAGS from the old version stored in ECX (i.e. flipping the ID bit
    // back if it was ever flipped).
    push %ecx
    popfl
 
    // Compare EAX and ECX. If they are equal then that means the bit wasn't
    // flipped, and CPUID isn't supported.
    xor %ecx, %eax
    jz .NoCPUID

    mov $0x80000000, %eax  // Set the A-register to 0x80000000.
    cpuid                  // CPU identification.
    cmp $0x80000001, %eax  // Compare the A-register with 0x80000001.
    jb .NoLongMode         // It is less, there is no long mode.
	
    mov $0x80000001, %eax  // Set the A-register to 0x80000001.
    cpuid                  // CPU identification.
    test $(1 << 29), %edx  // Test if the LM-bit, which is bit 29, is set in the D-register.
    jz .NoLongMode         // They aren't, there is no long mode.

    mov $0x1000, %edi       // Set the destination index to 0x1000.
    mov %edi, %cr3          // Set control register 3 to the destination index.
    xor %eax, %eax          // Nullify the A-register.
    mov $4096, %ecx         // Set the C-register to 4096.
    rep stosl               // Clear the memory.
    mov %cr3, %edi          // Set the destination index to control register 3.
    
    movl $0x2003, (%edi)            // Set the uint32_t at the destination index to 0x2003.
    add $1000, %edi              // Add 0x1000 to the destination index.
    movl $0x3003, (%edi)      // Set the uint32_t at the destination index to 0x3003.
    add $1000, %edi              // Add 0x1000 to the destination index.
    movl $0x4003, (%edi)      // Set the uint32_t at the destination index to 0x4003.S
    add $1000, %edi              // Add 0x1000 to the destination index.

    mov $0x00000003, %ebx          // Set the B-register to 0x00000003.
    mov $512, %ecx                 // Set the C-register to 512.
 
.SetEntry:
    mov %ebx, (%edi)         // Set the uint32_t at the destination index to the B-register.
    add $0x1000, %ebx              // Add 0x1000 to the B-register.
    add $8, %edi                   // Add eight to the destination index.
    loop .SetEntry               // Set the next entry.

    mov %cr4, %eax                 // Set the A-register to control register 4.
    or $(1 << 5), %eax               // Set the PAE-bit, which is the 6th bit (bit 5).
    mov %eax, %cr4                 // Set control register 4 to the A-register.

    mov $0xC0000080, %ecx          // Set the C-register to 0xC0000080, which is the EFER MSR.
    rdmsr                        // Read from the model-specific register.
    or $(1 << 8), %eax               // Set the LM-bit which is the 9th bit (bit 8).
    wrmsr                        // Write to the model-specific register.

    mov %cr0, %eax                 // Set the A-register to control register 0.
    or $(1 << 31), %eax              // Set the PG-bit, which is the 32nd bit (bit 31).
    mov %eax, %cr0                 // Set control register 0 to the A-register.

    lgdt (.Pointer)         // Load the 64-bit global descriptor table.

    movl $(longmode_start), %ecx
    jmp *%ecx       // Set the code segment and enter 64-bit long mode.S
 

.NoLongMode:
.NoCPUID:
	cli
    hlt
    jmp .NoCPUID


   .code64
longmode_start:
    mov %esp, %esp                   // zero the top 32 bits of esp
    cli                           // Clear the interrupt flag.
    mov $0x10, %ax            // Set the A-register to the data descriptor.
    mov %ax, %ds                    // Set the data segment to the A-register.
    mov %ax, %es                    // Set the extra segment to the A-register.
    mov %ax, %fs                    // Set the F-segment to the A-register.
    mov %ax, %gs                    // Set the G-segment to the A-register.
    mov %ax, %ss                    // Set the stack segment to the A-register.
    mov $0xB8000, %edi              // Set the destination index to 0xB8000.
    mov $0x1F201F201F201F20, %rax   // Set the A-register to 0x1F201F201F201F20.
    mov $500, %ecx                  // Set the C-register to 500.
    rep stosq                     // Clear the screen.
    hlt                           // Halt the processor.
    



    .section .data
GDT64:                           // Global Descriptor Table (64-bit).
    .Null: 
    .quad (. - GDT64)         // The null descriptor.
    .long 0                         // Limit (low).
    .long 0                         // Base (low).
    .byte 0                         // Base (middle)
    .byte 0                         // Access.
    .byte 0                         // Granularity.
    .byte 0                         // Base (high).
    .Code: 
    .quad (. - GDT64)         // The code descriptor.
    .long 0                         // Limit (low).
    .long 0                         // Base (low).
    .byte 0                         // Base (middle)
    .byte 0b10011010                 // Access (exec/read/.
    .byte 0b00100000                 // Granularity.
    .byte 0                         // Base (high).
    .Data: 
    .quad (. - GDT64)         // The data descriptor.
    .long 0                         // Limit (low).
    .long 0                         // Base (low).
    .byte 0                         // Base (middle)
    .byte 0b10010010                 // Access (read/write).
    .byte 0b00000000                 // Granularity.
    .byte 0                         // Base (high).
    .Pointer:                    // The GDT-pointer.
    .long . - GDT64 - 1             // Limit.
    .quad GDT64                     // Base.

    .comm stack, STACK_SIZE


