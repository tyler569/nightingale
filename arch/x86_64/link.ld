OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(kernel_entry)

KERNEL_START = 0xFFFFFFFF80000000;

SECTIONS {
    . = KERNEL_START;

    .text ALIGN(4K)   : { *(.text .text.*) }
    .rodata ALIGN(4K) : { *(.rodata .rodata.*) }
    .data ALIGN(4K)   : { *(.data .data.*) }
    .bss              : { *(COMMON) *(.bss .bss.*) }

    KERNEL_END = ALIGN(4K);
}
