#include "drivers.h"
#include "file.h"

extern struct file_operations basic_char_dev_ops;
extern struct file_operations tty_ops;

struct file_operations char_drivers[256] = {
    [0] = &basic_char_dev_ops,
    [1] = &tty_ops,
};
