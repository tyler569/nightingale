#pragma once

#include "types.h"
#include <sys/cdefs.h>

BEGIN_DECLS

enum simple_char_devices {
	FS_DEV_NULL,
	FS_DEV_ZERO,
	FS_DEV_RANDOM,
	FS_DEV_INC,
};

extern struct file_ops *char_drivers[256];

END_DECLS
