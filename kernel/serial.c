#ifdef __x86_64__
#include <ng/x86/uart.h>
#else
#error "unimplementled"
#endif // X86

void serial_init() { x86_uart_init(); }

void serial_write(struct serial_device *dev, char c) {
	dev->ops->write_byte(dev, c);
}

char serial_read(struct serial_device *dev) { return dev->ops->read_byte(dev); }

void serial_write_str(struct serial_device *dev, const char *buf, size_t len) {
	dev->ops->write_string(dev, buf, len);
}
