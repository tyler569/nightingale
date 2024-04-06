#ifndef NG_LIMINE_H
#define NG_LIMINE_H

#include <limine.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

void limine_init(void);

void limine_memmap(void);
void *limine_module(void);
void *limine_rsdp(void);
int64_t limine_boot_time(void);
void *limine_kernel_file_ptr(void);
size_t limine_kernel_file_len(void);
char *limine_kernel_command_line(void);
phys_addr_t limine_kernel_physical_base(void);
virt_addr_t limine_kernel_virtual_base(void);
virt_addr_t limine_hhdm(void);
void limine_smp_init(limine_goto_address addr);
void limine_framebuffer(uint32_t *width, uint32_t *height, uint32_t *bpp,
	uint32_t *pitch, void **address);

END_DECLS

#endif // NG_LIMINE_H
