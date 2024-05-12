#include "limine.h"
#include "list.h"
#include "ng/arch-2.h"
#include "stdio.h"
#include "sys/slab.h"
#include "x86_64.h"

static struct limine_smp_request smpinfo = {
	.id = LIMINE_SMP_REQUEST,
};

static struct slab_cache per_cpu_cache;

static LIST_HEAD(cpus);

void ap_entry(struct limine_smp_info *) {
	per_cpu_t *cpu = slab_alloc(&per_cpu_cache);
	cpu->self = cpu;

	list_prepend(&cpus, &cpu->list);

	init_ap_idt();
	init_ap_gdt(cpu);

	init_int_stacks();

	printf("AP started\n");

	halt_forever();
}

void init_aps() {
	struct limine_smp_response *resp = volatile_get(smpinfo.response);

	if (!resp || resp->cpu_count <= 1)
		return;

	init_slab_cache(&per_cpu_cache, sizeof(per_cpu_t));

	for (size_t i = 0; i < resp->cpu_count; i++) {
		if (resp->cpus[i]->lapic_id == resp->bsp_lapic_id)
			continue;

		resp->cpus[i]->goto_address = ap_entry;
	}
}
