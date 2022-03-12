#pragma once
#include <basic.h>
#include "types.h"

enum simple_char_devices {
    FS2_DEV_NULL,
    FS2_DEV_ZERO,
    FS2_DEV_RANDOM,
    FS2_DEV_INC,
};

extern struct file_operations *char_drivers[256];
