ENTRY(start)

SECTIONS
{
	. = 0xC0100000;

    .text BLOCK(4K) : AT(ADDR(.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}
	.rodata BLOCK(4K) : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata)
	}
	.data BLOCK(4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}
	.bss BLOCK(4K) : AT(ADDR(.bss) - 0xC0000000)
    {
        _sbss = .;
		*(COMMON)
		*(.bss)
        _ebss = .;
	}

    kernel_end = ALIGN(0x4000)
}
