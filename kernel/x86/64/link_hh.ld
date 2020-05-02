ENTRY(start)

LMA = 0x100000;
VMA = 0xFFFFFFFF80000000;

PROVIDE(kernel_stack = tss64.stack);

SECTIONS {
    . = LMA;

    _kernel_start = .;
    _mapped_kernel_start = . + VMA;

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
    _hh_kernel_start = .;

    _ro_begin = .;
    .text ALIGN(4K) : AT(ADDR(.text) - VMA) {
        *(.text)
    }
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
