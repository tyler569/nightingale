#pragma once

typedef void (*init_fn)();

#define define_init(fn, level) \
	init_fn __init_##fn __attribute__((section("init." #level), used)) = fn;

extern init_fn init_start[], init_end[];

static inline void do_init_calls() {
	for (init_fn *fn = init_start; fn < init_end; fn++) {
		(*fn)();
	}
}
