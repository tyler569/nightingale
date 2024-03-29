#pragma once
#ifndef NG_TTY_H
#define NG_TTY_H

#include <ng/ringbuf.h>
#include <ng/serial.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include <sys/ttyctl.h>
#include <sys/types.h>

BEGIN_DECLS

enum tty_buffering_mode {
    TTY_NO_BUFFERING,
    TTY_BUFFERING,
};

struct tty {
    int push_threshold;
    int buffer_index;
    pid_t controlling_pgrp;

    // void (*push_byte)(struct tty *tty, char byte);

    bool signal_eof;
    struct serial_device *serial_device;
    waitqueue_t read_queue;

    char buffer[1024];

    enum tty_buffering_mode buffer_mode;
    bool echo;

    struct ringbuf ring;
};

struct tty *new_tty(struct serial_device *dev, int id);
void tty_init(void);

extern struct tty *global_ttys[32];

int tty_push_byte(struct tty *tty, char c);

END_DECLS

#endif // NG_TTY_H
