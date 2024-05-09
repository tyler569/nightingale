#include "limine.h"
#include "stddef.h"
#include "sys/cdefs.h"

static struct limine_module_request moduleinfo = {
	.id = LIMINE_MODULE_REQUEST,
};

bool get_initrd_info(void **initrd_start, size_t *initrd_size) {
	struct limine_module_response *resp = volatile_get(moduleinfo.response);

	if (resp->module_count == 0)
		return false;

	*initrd_start = (void *)resp->modules[0]->address;
	*initrd_size = resp->modules[0]->size;

	return true;
}
