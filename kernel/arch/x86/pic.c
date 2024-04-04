#include <ng/x86/cpu.h>
#include <ng/x86/pic.h>

#define PRIMARY_COMMAND 0x20
#define PRIMARY_DATA 0x21
#define SECONDARY_COMMAND 0xA0
#define SECONDARY_DATA 0xA1

#define END_OF_INTERRUPT 0x20

void pic_send_eoi(int irq) {
	if (irq >= 8)
		outb(SECONDARY_COMMAND, END_OF_INTERRUPT);
	outb(PRIMARY_COMMAND, END_OF_INTERRUPT);
}

void pic_init() {
	outb(PRIMARY_COMMAND, 0x11); // reset and program
	outb(PRIMARY_DATA, 0x20); // starting at interrupt 0x20
	outb(PRIMARY_DATA, 0x04); // secondary at line 2
	outb(PRIMARY_DATA, 0x01); // 8086 mode
	outb(PRIMARY_DATA, 0xFF); // mask all interrupts

	outb(SECONDARY_COMMAND, 0x11); // reset and program
	outb(SECONDARY_DATA, 0x28); // starting at interrupt 0x20
	outb(SECONDARY_DATA, 0x02); // (not 100% sure)
	outb(SECONDARY_DATA, 0x01); // 8086 mode
	outb(SECONDARY_DATA, 0xFF); // mask all interrupts

	pic_irq_unmask(2); // allow cascade
}

void pic_irq_unmask(int irq) {
	// unsigned char mask;

	// if (irq > 15 || irq < 0)
	//     panic("pic: can't unmask irq %i\n", irq);

	// if (irq >= 8) {
	//     mask = inb(SECONDARY_DATA);
	//     mask &= ~(1 << (irq - 8));
	//     outb(SECONDARY_DATA, mask);
	// } else {
	//     mask = inb(PRIMARY_DATA);
	//     mask &= ~(1 << (irq));
	//     outb(PRIMARY_DATA, mask);
	// }
}

void pic_irq_mask(int irq) {
	// unsigned char mask;

	// if (irq > 15 || irq < 0)
	//     panic("pic: can't mask irq %i\n", irq);

	// if (irq >= 8) {
	//     mask = inb(SECONDARY_DATA);
	//     mask |= 1 << (irq - 8);
	//     outb(SECONDARY_DATA, mask);
	// } else {
	//     mask = inb(PRIMARY_DATA);
	//     mask |= 1 << (irq);
	//     outb(PRIMARY_DATA, mask);
	// }
}
