ENTRY(start)

SECTIONS
{

    kernel_start = 0x80000000;
    kernel_phy = 0x100000;
    kernel_vm_offset = kernel_start - kernel_phy;
    
	. = kernel_start;

    .text BLOCK(4K) : AT(ADDR(.text) - kernel_vm_offset)
	{
		*(.multiboot)
		*(.text)
	}
	.rodata BLOCK(4K) : AT(ADDR(.rodata) - kernel_vm_offset)
	{
		*(.rodata)
	}
	.data BLOCK(4K) : AT(ADDR(.data) - kernel_vm_offset)
	{
		*(.data)
	}
	.bss BLOCK(4K) : AT(ADDR(.bss) - kernel_vm_offset)
    {
		*(COMMON)
		*(.bss)
	}

    kernel_end = ALIGN(4K);

}
