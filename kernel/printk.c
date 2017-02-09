
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
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include <kernel/printk.h>

kernel_log_device *klog_default_device = &klog_tty;

typedef struct _print_format {
    int long_count;
    int padding;
    bool prefix;
    char type;
    char pad_char;
} print_format;

int dputchar(kernel_log_device *dev, int value) {
    char c = (char)value;
    dev->write(&c, sizeof(c));
    return value;
}

void _dprint_str(kernel_log_device *dev, char *buf) {
    size_t i = 0;
    while (buf[i]) {
        dputchar(dev, buf[i++]);
    }
}

/*
int _dprint_unsigned(kernel_log_device *dev, unsigned int value, int base, print_format fmt) {
    return _dprint_unsigned64(dev, (long long)value, base, fmt);
}

int _dprint_signed(kernel_log_device *dev, int value, print_format fmt) {
    return _dprint_signed64(dev, (long long)value, fmt);
}
*/

int _dprint_unsigned64(kernel_log_device *dev, unsigned long long value, int base, print_format fmt) {
    const char *charmap = "0123456789ABCDEF";
    if (fmt.type == 'x') {
        charmap = "0123456789abcdef";
    }
    char buf[64];
    buf[63] = '\0';
    size_t buf_ix = 62;

    do {
        buf[buf_ix--] = charmap[value % base];
        value /= base;
    } while (value > 0);
    if (base == 8 && fmt.prefix) {
        // fmt.padding -= 1;
        while (fmt.padding > 62 - buf_ix && fmt.pad_char != ' ') {
            buf[buf_ix--] = fmt.pad_char;
        }
        buf[buf_ix--] = '0';
    } else if (base == 16 && fmt.prefix) {
        // fmt.padding -= 2;
        while (fmt.padding > 62 - buf_ix && fmt.pad_char != ' ') {
            buf[buf_ix--] = fmt.pad_char;
        }
        buf[buf_ix--] = 'x';
        buf[buf_ix--] = '0';
    }
    while (fmt.padding > 62 - buf_ix) {
        buf[buf_ix--] = fmt.pad_char;
    }
    _dprint_str(dev, buf + buf_ix + 1);

    return 0;
}

int _dprint_signed64(kernel_log_device *dev, long long value, print_format fmt) {
    char buf[64];
    buf[63] = '\0';
    size_t buf_ix = 62;
    bool negative = false;

    if (value < 0) {
        value = -value;
        negative = true;
    }
    do {
        buf[buf_ix--] = value % 10 + '0';
        value /= 10;
    } while (value > 0);
    while (fmt.padding > 61 - buf_ix && fmt.pad_char != ' ') {
        buf[buf_ix--] = fmt.pad_char;
    }
    if (negative) {
        buf[buf_ix--] = '-';
    }
    while (fmt.padding > 62 - buf_ix && fmt.pad_char == ' ') {
        buf[buf_ix--] = fmt.pad_char;
    }
    _dprint_str(dev, buf + buf_ix + 1);

    return 0;
}

int _args_dprintk(kernel_log_device *dev, const char *format, va_list args) {
    size_t i;
    int c;
    long long j;
    char *s;


    size_t len = strlen(format);
    for (i=0; i < len; i++) {
        if (format[i] != '%') {
            dputchar(dev, format[i]);
        } else {
            print_format fmt = {0, 0, 0, 0, 0};
            if (format[i+1] == '#') {
                fmt.prefix = true;
                i += 1;
            }
            if (format[i+1] == '0') {
                fmt.pad_char = '0';
                i += 1;
            } else {
                fmt.pad_char = ' ';
            }
            if (format[i+1] > '0' && format[i+1] <= '9') {
                fmt.padding = format[i+1] - '0';
                i += 1;
                while (format[i+1] >= '0' && format[i+1] <= '9') {
                    fmt.padding *= 10;
                    fmt.padding += format[i+1] - '0';
                    i += 1;
                }
            }
            while (format[i+1] == 'l') {
                fmt.long_count += 1;
                i += 1;
            }
            fmt.type = format[i+1];
            if (fmt.long_count > 1) {
                switch (format[i+1]) {
                    case 'o':
                        j = va_arg(args, long long);
                        _dprint_unsigned64(dev, j, 8, fmt);
                        break;
                    case 'd':
                    case 'i':
                        j = va_arg(args, long long);
                        _dprint_signed64(dev, j, fmt);
                        break;
                    case 'u':
                        j = va_arg(args, long long);
                        _dprint_unsigned64(dev, j, 10, fmt);
                        break;
                    case 'X':
                    case 'x':
                        j = va_arg(args, long long);
                        _dprint_unsigned64(dev, j, 16, fmt);
                        break;
                }
            } else {
                switch (format[i+1]) {
                    case 'o':
                        c = va_arg(args, int);    //      v kludge to prevent sign extension
                        _dprint_unsigned64(dev, (long long)(unsigned)c, 8, fmt);
                        break;
                    case 'd':
                    case 'i':
                        c = va_arg(args, int);
                        _dprint_signed64(dev, (long long)(unsigned)c, fmt);
                        break;
                    case 'u':
                        c = va_arg(args, int);
                        _dprint_unsigned64(dev, (long long)(unsigned)c, 10, fmt);
                        break;
                    case 'X':
                    case 'x':
                        c = va_arg(args, int);
                        _dprint_unsigned64(dev, (long long)(unsigned)c, 16, fmt);
                        break;
                    case 'c':
                        c = va_arg(args, int);
                        dputchar(dev, c);
                        break;
                    case 's':
                        s = va_arg(args, char *);
                        _dprint_str(dev, s);
                        break;
                    case '%':
                        dputchar(dev, '%');
                        break;
                }
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

int klog(const char *format, ...) {
    int ret;
    va_list args;
    va_start(args, format);
    _dprint_str(klog_default_device, "KLOG: ");
    ret = _args_dprintk(klog_default_device, format, args);
    _dprint_str(klog_default_device, "\n");
    va_end(args);
    return ret;
}
