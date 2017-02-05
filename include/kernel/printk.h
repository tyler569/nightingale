
#pragma once

#include <kernel/serial.h>
#include <kernel/tty.h>

typedef struct _kernel_log_device {
    int (*write)(char *s, size_t len);
} kernel_log_device;

static kernel_log_device klog_tty = {
    .write = &terminal_write
};

static kernel_log_device klog_serial = {
    .write = NULL
};

__attribute__(( format(printf, 2, 3) ))
int dprintf(kernel_log_device dev, const char *format, ...);

__attribute__(( format(printf, 1, 2) ))
int printk(const char *format, ...);

