
/*
 * printk
 *
 * letters  -- >  print letters
 * TBI: \char    -- >  print char
 * TBI: &colors  -- >  change console color to <colors>
 * %format  -- >  format in data
 *  some work
 *
 * printk prints to a kernel_log_device,
 *  currently a struct with a ->write(char *data, size_t len)
 *  tty: implemented
 *  serial: planned
 */

//#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include <kernel/printk.h>

kernel_log_device *klog_default_device = &klog_tty;

int dputchar(kernel_log_device *dev, int value) {
    char c = (char)value;
    dev->write(&c, sizeof(c));
    return value;
}

void _dprint_str(kernel_log_device *dev, char *buf) {
    size_t i = 0;
    while (buf[i])
        dputchar(dev, buf[i++]);
}

int _dprint_unsigned(kernel_log_device *dev, unsigned int value, int base) {
    const char *format = "0123456789ABCDEF";
    char buf[33];
    buf[32] = '\0';
    size_t buf_ix = 31;

    do {
        buf[buf_ix] = format[value % base];
        value /= base;
        buf_ix -= 1;
    } while (value > 0);
    _dprint_str(dev, buf + buf_ix + 1);

    return 0;
}

int _dprint_signed(kernel_log_device *dev, int value, int base) {
    const char *format = "0123456789ABCDEF";
    char buf[33];
    buf[32] = '\0';
    size_t buf_ix = 31;
    bool negative = false;

    if (value < 0) {
        value = -value;
        negative = true;
    }
    do {
        buf[buf_ix] = format[value % base];
        value /= base;
        buf_ix -= 1;
    } while (value > 0);
    if (negative) {
        *(buf + buf_ix) = '-';
        _dprint_str(dev, buf + buf_ix);
    } else {
        _dprint_str(dev, buf + buf_ix + 1);
    }
    return 0;
}

int _dprint_unsigned64(kernel_log_device *dev, unsigned long long value, int base) {
    const char *format = "0123456789ABCDEF";
    char buf[65];
    buf[65] = '\0';
    size_t buf_ix = 64;

    do {
        buf[buf_ix] = format[value % base];
        value /= base;
        buf_ix -= 1;
    } while (value > 0);
    _dprint_str(dev, buf + buf_ix + 1);

    return 0;
}

int _dprint_signed64(kernel_log_device *dev, long long value, int base) {
    const char *format = "0123456789ABCDEF";
    char buf[65];
    buf[65] = '\0';
    size_t buf_ix = 64;
    bool negative = false;

    if (value < 0) {
        value = -value;
        negative = true;
    }
    do {
        buf[buf_ix] = format[value % base];
        value /= base;
        buf_ix -= 1;
    } while (value > 0);
    if (negative) {
        *(buf + buf_ix) = '-';
        _dprint_str(dev, buf + buf_ix);
    } else {
        _dprint_str(dev, buf + buf_ix + 1);
    }
    return 0;
}

int _args_dprintk(kernel_log_device *dev, const char *format, va_list args) {
    size_t i;
    int32_t j;
    char *s;

    // va_start(args, format);

    size_t len = strlen(format);
    for (i=0; i < len; i++) {
        if (format[i] != '%' && format[i] != '&') {
            dputchar(dev, format[i]);
        /* } else if (format[i] == '&') {
            set_text_color((vga_color)format[i+2] - '0', (vga_color) format[i+1] - '0');
            i += 2;
        */
        } else if (format[i] == '%') {
            switch (format[i+1]) {
                case 'c' : 
                    j = va_arg(args, int);
                    dputchar(dev, j);
                    break;
                case 'o':
                    j = va_arg(args, int);
                    _dprint_unsigned(dev, j, 8);
                    break;
                case 'i' :
                    j = va_arg(args, int);
                    _dprint_signed(dev, j, 10);
                    break;
                case 'u':
                    j = va_arg(args, int);
                    _dprint_unsigned(dev, j, 10);
                    break;
                case 'x' :
                    j = va_arg(args, int);
                    _dprint_unsigned(dev, j, 16);
                    break;
                case 's':
                    s = va_arg(args, char *);
                    _dprint_str(dev, s);
                    break;
                case 'l':
                    switch (format[i+2]) {
                        case 'o':
                            j = va_arg(args, long long);
                            _dprint_unsigned64(dev, j, 8);
                            break;
                        case 'i' :
                            j = va_arg(args, long long);
                            _dprint_signed64(dev, j, 10);
                            break;
                        case 'u':
                            j = va_arg(args, long long);
                            _dprint_unsigned64(dev, j, 10);
                            break;
                        case 'x' :
                            j = va_arg(args, long long);
                            _dprint_unsigned64(dev, j, 16);
                            break;
                    }
                    i += 1;
                    break;
                case '%':
                    dputchar(dev, '%');
                    break;
            }
            i += 1;
        }
    }
    // va_end(args);
    return i;
}

int dprintk(kernel_log_device *dev, const char *format, ...) {
    int ret;
    va_list args;
    va_start(args, format);
    ret = _args_dprintk(dev, format, args);
    va_end(args);
    return ret;
}

int printk(const char *format, ...) {
    int ret;
    va_list args;
    va_start(args, format);
    ret = _args_dprintk(klog_default_device, format, args);
    va_end(args);
    return ret;
}

