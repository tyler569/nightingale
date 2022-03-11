#pragma once

#include "types.h"

void make_proc_file2(
    const char *name,
    void (*generate)(struct fs2_file *, void *arg),
    void *arg
);

struct inode *new_proc_file(
    int mode,
    void (*generate)(struct fs2_file *, void *arg),
    void *arg
);

void proc2_sprintf(struct fs2_file *, const char *fmt, ...) __PRINTF(2, 3);
