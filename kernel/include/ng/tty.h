
#pragma once
#ifndef NIGHTINGALE_TTY_H
#define NIGHTINGALE_TTY_H

#include <ng/basic.h>
#include <ng/syscall_consts.h>

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

#endif
