#include <assert.h>
#include <limine.h>
#include <ng/arch.h>
#include <ng/limine.h>
#include <ng/pmm.h>
#include <stdio.h>

void limine_init() {
	printf("rsdp address: %p\n", limine_rsdp());
	printf("boot time: %li\n", limine_boot_time());

	printf("kernel command line: %s\n", limine_kernel_command_line());
}

__MUST_EMIT
static struct limine_kernel_file_request kernel_file_request = {
	.id = LIMINE_KERNEL_FILE_REQUEST,
	.revision = 0,
};

void *limine_kernel_file_ptr() {
	assert(kernel_file_request.response);

	return kernel_file_request.response->kernel_file->address;
}

size_t limine_kernel_file_len() {
	assert(kernel_file_request.response);

	return kernel_file_request.response->kernel_file->size;
}

char *limine_kernel_command_line() {
	assert(kernel_file_request.response);

	return kernel_file_request.response->kernel_file->cmdline;
}

__MUST_EMIT
static struct limine_rsdp_request rsdp_request = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0,
};

void *limine_rsdp() {
	assert(rsdp_request.response);

	return rsdp_request.response->address;
}

__MUST_EMIT
static struct limine_boot_time_request boot_time_request = {
	.id = LIMINE_BOOT_TIME_REQUEST,
	.revision = 0,
};

int64_t limine_boot_time() {
	assert(boot_time_request.response);

	return boot_time_request.response->boot_time;
}

__MUST_EMIT
static struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0,
};

void limine_framebuffer(uint32_t *width, uint32_t *height, uint32_t *bpp,
	uint32_t *pitch, void **address) {
	assert(framebuffer_request.response);
	assert(framebuffer_request.response->framebuffer_count == 1);

	struct limine_framebuffer *fb
		= framebuffer_request.response->framebuffers[0];
	*width = fb->width;
	*height = fb->height;
	*bpp = fb->bpp;
	*pitch = fb->pitch;
	*address = fb->address;
}
