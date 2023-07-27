#ifndef NG_LIMINE_H
#define NG_LIMINE_H

#include <stdint.h>
#include <sys/types.h>
#include <limine.h>

void limine_init(void);

void limine_memmap(void);
void *limine_module(void);
void *limine_rsdp(void);
int64_t limine_boot_time(void);
void *limine_kernel_file_ptr(void);
size_t limine_kernel_file_len(void);
const char *limine_kernel_command_line(void);
phys_addr_t limine_kernel_physical_base(void);
virt_addr_t limine_kernel_virtual_base(void);
virt_addr_t limine_hhdm(void);
void limine_smp_init(int id, limine_goto_address addr);

#endif // NG_LIMINE_H
