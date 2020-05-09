ENTRY(start)

LMA = 0x100000;
VMA = 0x80000000;

PROVIDE(kernel_stack = tss32.stack);

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

    _ro_base = .;
    _code_start = .;
    .text ALIGN(4K) : AT(ADDR(.text) - VMA) {
        *(.text)
    }
    _code_end = .;
    .rodata : AT(ADDR(.rodata) - VMA) {
        *(.rodata .rodata)
    }
    _ro_end = .;

    .data ALIGN(4K) : AT(ADDR(.data) - VMA) {
        *(.data .data.*)
    }
    .bss : AT(ADDR(.bss) - VMA) {
        *(.bss .bss.*)
    }

    _kernel_end = . ;
}
