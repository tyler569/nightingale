
#include <stdint.h>

#include <kernel/cpu.h>
#include <kernel/serial.h>

#define COM1_PORT 0x3f8
 
void serial_initialise() {
    outportb(COM1_PORT + 1, 0x00);    // Disable all interrupts
    outportb(COM1_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outportb(COM1_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outportb(COM1_PORT + 1, 0x00);    //                  (hi byte)
    outportb(COM1_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outportb(COM1_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outportb(COM1_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int serial_received() {
    return inportb(COM1_PORT + 5) & 1;
}
 
uint8_t serial_read() {
    while (serial_received() == 0);
 
    return inportb(COM1_PORT);
}

int is_transmit_empty() {
    return inportb(COM1_PORT + 5) & 0x20;
}
 
void serial_write(uint8_t a) {
    while (is_transmit_empty() == 0);
 
    outportb(COM1_PORT,a);
}
