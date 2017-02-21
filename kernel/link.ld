ENTRY(start)

SECTIONS {

    . = 0x100000;

    .boot :
    {
        *(.multiboot)
    }
    .text :
    {
        *(.text.init)
    }
    .bss :
    {
        *(.bss)
    }

    . = 0xFFFF000000000000;
}
