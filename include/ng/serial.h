#pragma once
#ifndef NG_SERIAL_H
#define NG_SERIAL_H

#include <stdbool.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct serial_device;

struct serial_ops {
    char (*read_byte)(struct serial_device *);
    void (*write_byte)(struct serial_device *, char);
    void (*write_string)(struct serial_device *, const char *, size_t);
    void (*enable)(struct serial_device *dev, bool enable);
};

struct serial_device {
    int port_number; // TODO: machine independant;
    const struct serial_ops *ops;
    struct tty *tty;
};

void serial_init(void);

void serial_write(struct serial_device *, char c);
void serial_write_str(struct serial_device *, const char *buf, size_t len);
char serial_read(struct serial_device *);

END_DECLS

#endif // NG_SERIAL_H
