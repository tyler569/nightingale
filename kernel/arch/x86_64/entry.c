#include <ng/arch.h>

extern void kernel_main();

void _start() {
	arch_init();
	kernel_main();
}
