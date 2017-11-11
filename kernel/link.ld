ENTRY(start)

SECTIONS {
    . = 0x400000;

    .rodata : ALIGN(4K) {
        *(.rodata.multiboot)
        *(.rodata .rodata.*)
    }
    .text : ALIGN(4K) {
        *(.text .text.*)
    }
    .data : ALIGN(4K) {
        *(.data .data.*)
    }
    .bss : ALIGN(4K) {
        *(.bss)
    }
    __kernel_end = ALIGN(4K);
}
