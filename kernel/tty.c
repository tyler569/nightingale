
#include <ng/basic.h>
#include <ds/ringbuf.h>
#include <ng/fs.h>
#include <ng/thread.h>
#include <ng/serial.h>
#include <ng/tty.h>

/*
 * TODO
 * - set push threshold:
 *    -> ng/sh needs to be able to ask for no line buffering
 */

struct tty serial_tty = {0};

static void init_serial_tty(void) {
        serial_tty.initialized = 1;
        serial_tty.push_threshold = 256;
        serial_tty.buffer_index = 0;
        serial_tty.device_file = fs_resolve_relative_path(NULL, "/dev/serial");
}

#include <ng/print.h>

int write_to_serial_tty(char c) {
//        printf("(%i)", c);
        if (!serial_tty.initialized) {
                init_serial_tty();
        }

        if (c == '\r') {
                serial_tty.buffer[serial_tty.buffer_index++] = '\n';

                ring_write(&serial_tty.device_file->extra.ring,
                           serial_tty.buffer, serial_tty.buffer_index);
                serial_write('\r');
                serial_write('\n');
                serial_tty.buffer_index = 0;

                wake_blocked_threads(&serial_tty.device_file->blocked_threads);
        /*
        } else if (c == ^C) {
                send_signal(serial_tty.controlling_pgrp, INT);
        */
        } else if (c >= ' ' && c <= '~') {
                serial_write(c);
                serial_tty.buffer[serial_tty.buffer_index++] = c;
        } else if (c < ' ') {
                serial_write('^');
                serial_write('@' + c);
        } else if (c == '\177') {
                if (serial_tty.buffer_index) {
                        serial_tty.buffer[serial_tty.buffer_index] = '\0';
                        serial_tty.buffer_index -= 1;
                        serial_write('\b');
                        serial_write(' ');
                        serial_write('\b');
                }
        } else {
                serial_write('?');
        }

        return 0;
}

