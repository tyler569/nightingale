
#ifndef _KERNEL_ARCH_SERIAL_H
#define _KERNEL_ARCH_SERIAL_H

void serial_initialize();
uint8_t serial_read();
void serial_write(uint8_t a);

#endif // _KERNEL_ARCH_SERIAL_H
