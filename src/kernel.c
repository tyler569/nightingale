
#include <system.h>
#include <screen.h>

void kmain(void)
{
    gdt_install();
    idt_install();

    init_screen();
    cls();
    kprintf("&20Project Nighingale\n&70");

    return;
}


