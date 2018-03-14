ENTRY(start)

SECTIONS {
    . = 0x100000;

    _kernel_start = .;

    .rodata : {
        *(.rodata.multiboot)
    }
    .text : {
        *(.text.low)
    }
    .text ALIGN(4K) : AT(ADDR(.text) - 0xffffff8000100000) {
        *(.text .text.*)
    }
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xffffff8000100000) {
        *(.rodata .rodata.*)
    }
    .data ALIGN(4K) : AT(ADDR(.data) - 0xffffff8000100000) {
        *(.data .data.*)
    }
    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xffffff8000100000) {
        *(.bss .bss.*)
    }

    _kernel_end = . ;
}
