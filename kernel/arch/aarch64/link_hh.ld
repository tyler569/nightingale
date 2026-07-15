OUTPUT_FORMAT(elf64-littleaarch64)
OUTPUT_ARCH(aarch64)
ENTRY(start)

LMA = 0x100000;
VMA = 0xFFFFFFFF80000000;

PHDRS {
    text      PT_LOAD FLAGS(5); /* R-X */
    rodata    PT_LOAD FLAGS(4); /* R-- */
    data      PT_LOAD FLAGS(6); /* RW- */
}

SECTIONS {
    . = VMA;
    _kernel_phy_base = . - VMA;

    .text ALIGN(4K)   : { *(.text .text.*) } :text
    .rodata ALIGN(4K) : { *(.rodata .rodata.*) } :rodata
    .data ALIGN(4K)   : { *(.data .data.*) } :data
    .bss              : { *(COMMON) *(.bss .bss.*) } :data

    /DISCARD/ : { *(.eh_frame) *(.note .note.*) }

    _kernel_phy_top = . - VMA;
    _kernel_end = .;
}
