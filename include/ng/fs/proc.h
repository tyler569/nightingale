#pragma once

#include "sys/cdefs.h"
#include "types.h"

BEGIN_DECLS

void make_proc_file(
	const char *name, void (*generate)(struct file *, void *arg), void *arg);

struct vnode *new_proc_vnode(
	int mode, void (*generate)(struct file *, void *arg), void *arg);

void proc_sprintf(struct file *, const char *fmt, ...) __PRINTF(2, 3);

END_DECLS
