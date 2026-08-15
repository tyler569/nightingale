OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

VMA = 0xFFFFFFFF80000000;

PHDRS {
    text      PT_LOAD FLAGS(5);  /* R-X */
    rodata    PT_LOAD FLAGS(4);  /* R-- */
    data      PT_LOAD FLAGS(6);  /* RW- */
}

SECTIONS {
    . = VMA;
    kernel_start = .;

    .text ALIGN(4K)   : { *(.text .text.*) } :text
    .rodata ALIGN(4K) : { *(.rodata .rodata.*) } :rodata

    percpu ALIGN(16) : {
        percpu_template_start = .;
        QUAD(0)                  /* %gs:0 self-pointer reservation */
        *(percpu .percpu.*)
        percpu_template_end = .;
    } :rodata

    init_array ALIGN(8) : {
        init_start = .;
        KEEP(*(SORT(init.*)))
        init_end = .;
    } :rodata

    . = ALIGN(4K);

    .data             : { *(.data .data.*) } :data
    .bss              : { *(COMMON) *(.bss .bss.*) } :data

    kernel_end = .;
}
