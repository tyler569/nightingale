#include "limine.h"
#include "ng/mem.h"
#include "sys/cdefs.h"
#include "x86_64.h"
#include <assert.h>
#include <ng/arch-2.h>
#include <ng/limine.h>
#include <ng/x86/acpi.h>
#include <ng/x86/apic.h>
#include <ng/x86/pic.h>

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
	// init_aps();

	// ported, fixup
	{
		heap_init(__global_heap_ptr, early_malloc_pool, EARLY_MALLOC_POOL_LEN);
		serial_init();

		acpi_rsdp_t *rsdp = limine_rsdp();
		acpi_init(rsdp);
		void *madt = acpi_get_table("APIC");
		assert(madt);
		pic_init();
		ioapic_init(madt);
		lapic_init();

		tty_init();
		limine_init();
	}

	kernel_main();

	halt_forever();
}
