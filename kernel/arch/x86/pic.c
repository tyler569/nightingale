
#include <basic.h>
#include <panic.h>
#include <arch/x86/portio.h>
#include <arch/x86/pic.h>

#define MASTER_COMMAND  0x20
#define MASTER_DATA     0x21
#define SLAVE_COMMAND   0xA0
#define SLAVE_DATA      0xA1

#define END_OF_INTERRUPT 0x20

void send_end_of_interrupt(i32 irq) {
    if (irq >= 8) {
        outb(SLAVE_COMMAND, END_OF_INTERRUPT);
    }
    outb(MASTER_COMMAND, END_OF_INTERRUPT);
}

void remap_pic() {
    // Start initialization
    outb(MASTER_COMMAND, 0x11);
    outb(SLAVE_COMMAND, 0x11);

    // Master offset
    outb(MASTER_DATA, 0x20);
    // Slave offset
    outb(SLAVE_DATA, 0x28);

    outb(MASTER_DATA, 0x04);
    outb(SLAVE_DATA, 0x02);
    outb(MASTER_DATA, 0x01);
    outb(SLAVE_DATA, 0x01);
    outb(MASTER_DATA, 0x0);
    outb(SLAVE_DATA, 0x0);
}

void pic_irc_unmask(i32 irq) {
    u8 mask;

    assert(irq < 16, "IRQ numbers only go to 16\n");

    if (irq >= 8) {
        mask = inb(SLAVE_DATA);
        mask &= ~(1 << (irq - 8));
        outb(SLAVE_DATA, mask);
    } else {
        mask = inb(MASTER_DATA);
        mask &= ~(1 << (irq));
        outb(MASTER_DATA, mask);
    }
}

void pic_irc_mask(i32 irq) {
    u8 mask;

    if (irq > 15) panic("Unacceptable IRQ to mask: %d\n", irq);

    if (irq >= 8) {
        mask = inb(SLAVE_DATA);
        mask |= 1 << (irq - 8);
        outb(SLAVE_DATA, mask);
    } else {
        mask = inb(MASTER_DATA);
        mask |= 1 << (irq);
        outb(MASTER_DATA, mask);
    }
}


