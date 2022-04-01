#pragma once
#include <sys/cdefs.h>
#include "types.h"

enum simple_char_devices {
    FS_DEV_NULL,
    FS_DEV_ZERO,
    FS_DEV_RANDOM,
    FS_DEV_INC,
};

extern struct file_operations *char_drivers[256];
