#pragma once
#ifndef NG_TTY_H
#define NG_TTY_H

#include <basic.h>
#include <ng/fs.h>
#include <stdlib.h>
#include <sys/ttyctl.h>

struct tty {
    int initialized;
    int push_threshold;
    int buffer_index;
    pid_t controlling_pgrp;

    void (*print_fn)(const char *data, size_t len);

    char buffer[1024];

    int buffer_mode;
    int echo;

    struct ringbuf ring;
};

extern struct tty_file dev_serial;
extern struct tty_file dev_serial2;

int write_to_serial_tty(struct tty_file *tty_file, char c);

#endif // NG_TTY_H
