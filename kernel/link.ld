ENTRY(start)

SECTIONS {

    . = 0x100000;

    .boot :
    {
        KEEP(*(.multiboot))
    }
    .text :
    {
        *(.text.init)
        *(.text)
    }
    .bss :
    {
        *(.bss)
    }
}
