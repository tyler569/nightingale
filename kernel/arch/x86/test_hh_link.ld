ENTRY(start)

LMA = 0x100000;
VMA = 0xFFFFFFFF80000000;

SECTIONS {
    . = LMA;

    _kernel_start = .;

    .low.rodata : {
        *(.rodata.multiboot .low.rodata)
    }
    .low.text : {
        *(.low.text)
    }
    .low.data : {
        *(.low.data)
    }
    .low.bss : {
        *(.low.bss)
    }

    . += VMA;
    _hh_start = .;

    .text ALIGN(4K) : AT(ADDR(.text) - VMA) {
        *(.text)
    }
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - VMA) {
        *(.rodata .rodata)
    }
    .data ALIGN(4K) : AT(ADDR(.data) - VMA) {
        *(.data .data.*)
    }
    .bss ALIGN(4K) : AT(ADDR(.bss) - VMA) {
        *(.bss .bss.*)
    }

    _kernel_end = . ;
}
