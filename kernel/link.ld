ENTRY(start)

SECTIONS {
    . = 0x100000;

    _kernel_start = 0x100000;

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
        *(.bss .bss.*)
    }

    _kernel_end = . ;
}
