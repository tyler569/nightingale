ENTRY(start)

LMA = 0x100000;
VMA = 0xFFFFFFFF80000000;

PROVIDE(kernel_stack = tss64.stack);

PHDRS {
    lowtext   PT_LOAD ;
    lowdata   PT_LOAD ;
    text      PT_LOAD ;
    data      PT_LOAD ;
}

SECTIONS {
    . = LMA;

    _kernel_start = .;
    _kernel_phy_base = .;
    _mapped_kernel_start = . + VMA;

    .multiboot  : { *(.rodata.multiboot) } :lowtext
    .low.rodata : { *(.low.rodata) } :lowtext
    .low.text   : { *(.low.text) }   :lowtext
    .low.data   : { *(.low.data) }   :lowdata
    .low.bss    : { *(.low.bss) }    :lowdata

    . += VMA;
    _hh_kernel_start = .;

    _ro_begin = .;
    .text ALIGN(4K) : AT(ADDR(.text) - VMA)   { *(.text .text.*) }     :text
    .rodata         : AT(ADDR(.rodata) - VMA) { *(.rodata .rodata.*) } :text
    _ro_end = .;

    .data ALIGN(4K) : AT(ADDR(.data) - VMA) { *(.data .data.*) } :data
    .bss            : AT(ADDR(.bss) - VMA)  { *(.bss .bss.*) }   :data

    _kernel_end = . ;
    _kernel_phy_top = . - VMA;
}
