
#pragma once

#include <kernel/serial.h>
#include <kernel/tty.h>

typedef struct _kernel_log_device {
    int (*write)(char *s, size_t len);
} kernel_log_device;

__attribute__((used))
static kernel_log_device klog_tty = {
    .write = &terminal_write
};

__attribute__((used))
static kernel_log_device klog_serial = {
    .write = &serial_write
};

__attribute__(( format(printf, 2, 3) ))
int dprintk(kernel_log_device *dev, const char *format, ...);

__attribute__(( format(printf, 1, 2) ))
int printk(const char *format, ...);

__attribute__(( format(printf, 1, 2) ))
int klog(const char *format, ...);
