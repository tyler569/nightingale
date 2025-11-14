OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

VMA = 0xFFFFFFFF80000000;

SECTIONS {
    . = VMA;

    .text ALIGN(4K)   : { *(.text .text.*) } :text
    .rodata ALIGN(4K) : { *(.rodata .rodata.*) } :rodata
    .data ALIGN(4K)   : { *(.data .data.*) } :data
    .bss              : { *(COMMON) *(.bss .bss.*) } :data

    /DISCARD/ : { *(.eh_frame) *(.note .note.*) }

    _kernel_end = .;
}
