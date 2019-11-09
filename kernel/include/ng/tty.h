
#pragma once
#ifndef NG_TTY_H
#define NG_TTY_H

#include <basic.h>
#include <nc/stdlib.h>
#include <nc/sys/ttyctl.h>

struct tty {
        int initialized;
        int push_threshold;
        int buffer_index;
        pid_t controlling_pgrp;
        struct file *device_file;
        void (*print_fn)(const char *data, size_t len);
        char buffer[1024];

        int buffer_mode;
        int echo;
};

extern struct tty serial_tty;
extern struct tty serial_tty2;

void init_serial_ttys(void);
int write_to_serial_tty(struct tty *tty, char c);

#endif // NG_TTY_H

