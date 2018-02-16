
#include <basic.h>

#include "portio.h"

u8 inb(port p) {
    u8 ret;
    asm volatile ("");
    return ret;
}
void outb(port p, u8 v);

u16 inw(port p);
void outw(port p, u16 v);

u32 ind(port p);
void outd(port p, u32 v);

#endif
