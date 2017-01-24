
#ifndef _KERNEL_KLOG_H
#define _KERNEL_KLOG_H

typedef enum _loglevel {
    LOG_CRIT = '\x10',
    LOG_WARN = '\x20',
    LOG_INFO = '\x30',
    LOG_DEBG = '\x40'
} loglevel;

int32_t printk(char *buf);

#endif // _KERNEL_KLOG_H
