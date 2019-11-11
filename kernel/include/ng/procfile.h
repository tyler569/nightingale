
#pragma once
#ifndef NG_PROCFILE_H
#define NG_PROCFILE_H

#include <basic.h>
#include <ng/fs.h>
#include <nc/assert.h>
#include <nc/stdlib.h>

struct file *make_procfile(const char *name, int (*fn)(struct open_file *), void *data);

#endif // NG_PROCFILE_H

