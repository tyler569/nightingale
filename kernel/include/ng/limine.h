#pragma once

#include <limine.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

void limine_init();

void limine_memmap();
void *limine_module();
void *limine_rsdp();
int64_t limine_boot_time();
void *limine_kernel_file_ptr();
size_t limine_kernel_file_len();
char *limine_kernel_command_line();
virt_addr_t limine_hhdm();
void limine_framebuffer(uint32_t *width, uint32_t *height, uint32_t *bpp,
	uint32_t *pitch, void **address);

END_DECLS
