ENTRY(start)

SECTIONS {
    . = 0x100000;

    .boot : {
        *(.multiboot)
    }
    .text : {
        *(.text .text.*)
    }
    .data : {
        *(.data .data.*)
    }
    .bss : {
        *(.bss)
    }
}
