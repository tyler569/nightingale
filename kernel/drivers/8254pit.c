
#include "../llio/portio.h"
#include "8254pit.h"

#define CHANNEL_0_DATA      0x40
#define CHANNEL_1_DATA      0x41
#define CHANNEL_2_DATA      0x42
#define COMMAND_REGISTER    0x43


// TODO: genericize this.
void setup_interval_timer(int hz) {
    int32_t divisor = 1193180 / hz;   /* Calculate our divisor */
    outb(0x43, 0x36);             /* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outb(0x40, divisor >> 8);     /* Set high byte of divisor */
}



