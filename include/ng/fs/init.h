#pragma once
#ifndef NG_INITFS_H
#define NG_INITFS_H

#include "sys/cdefs.h"

BEGIN_DECLS

void fs_init(void *);
void load_initfs(void *);

END_DECLS

#endif // NG_INITFS_H
