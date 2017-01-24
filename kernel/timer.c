
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <kernel/cpu.h>

void timer_phase(int32_t hz)
{
    int32_t divisor = 1193180 / hz;   /* Calculate our divisor */
    outportb(0x43, 0x36);             /* Set our command byte 0x36 */
    outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

uint32_t timer_ticks = 0;

void timer_handler(struct regs *r)
{
    timer_ticks++;

    if (timer_ticks % 100 == 0){
        printf("One second has passed\n");
    }
}

void timer_install()
{
	timer_phase(100);
    /* Installs 'timer_handler' to IRQ0 */
    // irq_install_handler(0, timer_handler);
}
