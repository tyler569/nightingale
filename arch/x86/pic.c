
#include <ng/basic.h>
#include <ng/panic.h>
#include "portio.h"
#include "pic.h"

#define MASTER_COMMAND  0x20
#define MASTER_DATA     0x21
#define SLAVE_COMMAND   0xA0
#define SLAVE_DATA      0xA1

#define END_OF_INTERRUPT 0x20

void pic_send_eoi(int irq) {
    if (irq >= 8) {
        outb(SLAVE_COMMAND, END_OF_INTERRUPT);
    }
    outb(MASTER_COMMAND, END_OF_INTERRUPT);
}

void pic_init() {
    outb(MASTER_COMMAND, 0x11); // reset and program
    outb(MASTER_DATA, 0x20);    // starting at interrupt 0x20
    outb(MASTER_DATA, 0x04);    // slave at line 2
    outb(MASTER_DATA, 0x01);    // 8086 mode
    outb(MASTER_DATA, 0xFF);    // mask all interrupts

    outb(SLAVE_COMMAND, 0x11); // reset and program
    outb(SLAVE_DATA, 0x28);    // starting at interrupt 0x20
    outb(SLAVE_DATA, 0x02);    // (not 100% sure)
    outb(SLAVE_DATA, 0x01);    // 8086 mode
    outb(SLAVE_DATA, 0xFF);    // mask all interrupts

    pic_irq_unmask(2); // allow cascade
}

void pic_irq_unmask(int irq) {
    unsigned char mask;

    if (irq > 15 || irq < 0)  panic("pic: can't unmask irq %i\n", irq);

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

void pic_irq_mask(int irq) {
    unsigned char mask;

    if (irq > 15 || irq < 0)  panic("pic: can't mask irq %i\n", irq);

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


