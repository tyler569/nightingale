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

    percpu : ALIGN(64) {
        percpu_template_start = .;
        *(SORT_BY_ALIGNMENT(percpu))
        percpu_template_end = .;
    } :rodata

    init_array ALIGN(8) : {
        init_start = .;
        KEEP(*(SORT(init.*)))
        init_end = .;
    } :rodata

    proc_files ALIGN(8) : {
        proc_files_start = .;
        KEEP(*(proc_files))
        proc_files_end = .;
    } :rodata

    pci_drivers ALIGN(8) : {
        pci_drivers_start = .;
        KEEP(*(pci_drivers))
        pci_drivers_end = .;
    } :rodata

    . = ALIGN(4K);

    .data : {
        *(SORT_BY_ALIGNMENT(.data))
        *(SORT_BY_ALIGNMENT(.data.*))
    } :data

    .bss : {
        *(SORT_BY_ALIGNMENT(COMMON))
        *(SORT_BY_ALIGNMENT(.bss))
        *(SORT_BY_ALIGNMENT(.bss.*))
    } :data

    kernel_end = .;
}
