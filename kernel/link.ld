ENTRY(start)

SECTIONS {

    . = 0x100000;

    .boot : {
        KEEP(*(.multiboot))
    }
    .text : {
        *(.text.init)
        *(.text .text.*)
    }
    .rodata : {
        *(.rodata .rodata.*)
    }
    .data.rel.ro : {
        *(.data.rel.ro.local*) *(.data.rel.ro .data.rel.ro.*)
    }
    .bss : {
        *(.bss)
    }
}
