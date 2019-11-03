
#pragma once
#ifndef NG_TTY_H
#define NG_TTY_H

#include <basic.h>
#include <nc/sys/ttyctl.h>

struct tty {
        int initialized;
        int push_threshold;
        int buffer_index;
        pid_t controlling_pgrp;
        struct fs_node *device_file;
        char buffer[1024];

        int buffer_mode;
        int echo;
};

extern struct tty serial_tty;

void init_serial_tty(void);
int write_to_serial_tty(char c);

#endif // NG_TTY_H

