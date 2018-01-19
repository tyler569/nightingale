ENTRY(start)

SECTIONS {
    . = 0x100000;

    _kernel_start = .;

    /* _kernel_rodata = ALIGN(4K); */
    .rodata : ALIGN(4K) {
        *(.rodata.multiboot)
        *(.rodata .rodata.*)
    }
    /* _kernel_rodata_end = .; */

    /* _kernel_text = ALIGN(4K); */
    .text : ALIGN(4K) {
        *(.text .text.*)
    }
    /* _kernel_text_end = .; */

    /* _kernel_data = ALIGN(4K); */
    .data : ALIGN(4K) {
        *(.data .data.*)
    }
    /* _kernel_data_end = .; */

    /* _kernel_bss = ALIGN(4K); */
    .bss : ALIGN(4K) {
        *(.bss .bss.*)
    }
    /* _kernel_bss_end = .; */

    _kernel_end = ALIGN(4K);
}
