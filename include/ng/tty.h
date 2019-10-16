
#pragma once
#ifndef NIGHTINGALE_TTY_H
#define NIGHTINGALE_TTY_H

#include <ng/basic.h>

struct tty {
        int initialized;
        int push_threshold;
        int buffer_index;
        pid_t controlling_pgrp;
        struct fs_node *device_file;
        char buffer[1024];
};

int write_to_serial_tty(char c);

#endif
