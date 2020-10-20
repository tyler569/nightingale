#pragma once
#ifndef NG_PROCFILE_H
#define NG_PROCFILE_H

#include <basic.h>
#include <ng/fs.h>
#include <assert.h>
#include <stdlib.h>

struct file *make_procfile(const char *name,
                void (*fn)(struct open_file *), void *data);

void procfs_init(void);

#endif // NG_PROCFILE_H
