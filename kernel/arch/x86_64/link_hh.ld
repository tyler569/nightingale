OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

VMA = 0xFFFFFFFF80000000;

PHDRS {
    text      PT_LOAD FLAGS(5);
    rodata    PT_LOAD FLAGS(4);
    data      PT_LOAD FLAGS(6);
}

SECTIONS {
    . = VMA;


    .text ALIGN(4K)   : { *(.text .text.*) } :text
    .rodata ALIGN(4K) : { *(.rodata .rodata.*) } :rodata

	init : ALIGN(8) {
        init_start = .;
        KEEP(*(SORT(init.*)))
        init_end = .;
    } :rodata

	. = ALIGN(4K);

    .data             : { *(.data .data.*) } :data
    .bss              : { *(COMMON) *(.bss .bss.*) } :data

    /* /DISCARD/ : { *(.eh_frame* .note* .debug* .comment) } */

    _kernel_end = .;
}
