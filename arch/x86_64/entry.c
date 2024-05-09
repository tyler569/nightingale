#include "limine.h"
#include "ng/mem.h"
#include "sys/cdefs.h"
#include "x86_64.h"
#include <ng/arch-2.h>

LIMINE_BASE_REVISION(1)

void kernel_main();

// The kernel entrypoint, called by the bootloader.
// This function is set as the entry on the kernel ELF file.
USED void kernel_entry() {
	init_bsp_gdt();
	init_idt();
	init_syscall();
	init_page_mmap();
	init_kmem_alloc();
	init_int_stacks();
	init_aps();

	kernel_main();

	halt_forever();
}
