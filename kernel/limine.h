#ifndef NG_LIMINE_H
#define NG_LIMINE_H

#include <stdint.h>
#include <sys/types.h>

void limine_init(void);

void limine_memmap(void);
void *limine_module(void);
void *limine_rsdp(void);
int64_t limine_boot_time(void);
void *limine_kernel_file(void);
const char *limine_kernel_command_line(void);
phys_addr_t limine_kernel_physical_base(void);
virt_addr_t limine_kernel_virtual_base(void);
virt_addr_t limine_hhdm(void);

#endif // NG_LIMINE_H
