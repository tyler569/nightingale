#include <ng/irq.h>
#include <ng/serial.h>
#include <ng/tty.h>
#include <ng/x86/cpu.h>
#include <ng/x86/pic.h>
#include <ng/x86/uart.h>

#define COM1 (port_addr_t)0x3f8
#define COM2 (port_addr_t)0x2f8

#define UART_DATA 0
#define UART_INTERRUPT_ENABLE 1
#define UART_BAUD_LOW 0
#define UART_BAUD_HIGH 1
#define UART_FIFO_CTRL 2
#define UART_LINE_CTRL 3
#define UART_MODEM_CTRL 4
#define UART_LINE_STATUS 5
#define UART_MODEM_STATUS 6

struct serial_device *x86_com[2];

static bool is_transmit_empty(port_addr_t com);
static bool is_data_available(port_addr_t com);
static void wait_for_transmit_empty(port_addr_t com);
static void wait_for_data_available(port_addr_t com);

static void x86_uart_write_byte(struct serial_device *dev, char b)
{
    port_addr_t p = dev->port_number;
    wait_for_transmit_empty(p);
    outb(p + UART_DATA, b);
}

static void x86_uart_write(
    struct serial_device *dev, const char *buf, size_t len)
{
    port_addr_t p = dev->port_number;
    for (size_t i = 0; i < len; i++)
        x86_uart_write_byte(dev, buf[i]);
}

static char x86_uart_read_byte(struct serial_device *dev)
{
    port_addr_t p = dev->port_number;
    wait_for_data_available(p);
    return inb(p + UART_DATA);
}

static void x86_uart_enable_interrupt(struct serial_device *dev, bool enable)
{
    port_addr_t p = dev->port_number;
    char value = enable ? 0x9 : 0;
    outb(p + UART_INTERRUPT_ENABLE, value);
}

static struct serial_ops x86_uart_serial_ops = {
    .read_byte = x86_uart_read_byte,
    .write_byte = x86_uart_write_byte,
    .write_string = x86_uart_write,
    .enable = x86_uart_enable_interrupt,
};

struct serial_device *new_x86_uart(port_addr_t address)
{
    struct serial_device *dev = malloc(sizeof(struct serial_device));
    *dev = (struct serial_device) {
        .port_number = address,
        .ops = &x86_uart_serial_ops,
    };
    return dev;
}

static bool is_transmit_empty(port_addr_t com)
{
    return (inb(com + UART_LINE_STATUS) & 0x20) != 0;
}

static bool is_data_available(port_addr_t com)
{
    return (inb(com + UART_LINE_STATUS) & 0x01) != 0;
}

static void wait_for_transmit_empty(port_addr_t com)
{
    while (!is_transmit_empty(com)) { }
}

static void wait_for_data_available(port_addr_t com)
{
    while (!is_data_available(com)) { }
}

static void x86_uart_irq_handler(interrupt_frame *r, void *serial_device)
{
    struct serial_device *dev = serial_device;

    char c = dev->ops->read_byte(dev);
    if (dev->tty)
        tty_push_byte(dev->tty, c);
}

static void x86_uart_setup(port_addr_t p)
{
    // TODO: cleanup with registers above
    outb(p + 1, 0x00);
    outb(p + 3, 0x80);
    outb(p + 0, 0x03);
    outb(p + 1, 0x00);
    outb(p + 3, 0x03);
    outb(p + 2, 0xC7);
    outb(p + 4, 0x0B);
}

void x86_uart_init()
{
    x86_uart_setup(COM1);
    x86_uart_setup(COM2);

    x86_com[0] = new_x86_uart(COM1);
    x86_com[1] = new_x86_uart(COM2);

    x86_com[0]->ops->enable(x86_com[0], true);
    x86_com[1]->ops->enable(x86_com[1], true);

    irq_install(IRQ_SERIAL1, x86_uart_irq_handler, x86_com[0]);
    irq_install(IRQ_SERIAL2, x86_uart_irq_handler, x86_com[1]);
}
