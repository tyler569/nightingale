#include <ng/x86/uart.h>

void serial_init() {
	x86_uart_init();
}

void serial_write(struct serial_device *dev, char c) {
	if (dev && dev->ops)
		dev->ops->write_byte(dev, c);
}

char serial_read(struct serial_device *dev) {
	if (dev && dev->ops)
		return dev->ops->read_byte(dev);
	else
		return 0;
}

void serial_write_str(struct serial_device *dev, const char *buf, size_t len) {
	if (dev && dev->ops)
		dev->ops->write_string(dev, buf, len);
}
