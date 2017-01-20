
#include <system.h>
#include <screen.h>

void kmain(void)
{
    gdt_install();
    idt_install();
    irq_install();
    timer_install();
    irq_install_handler(1, keyboard_handler);
    __asm__ ("sti");

    init_screen();
    cls();
    kprintf("&20Project Nighingale\n&70");

    for (int i=10; i>0; i--) {
        kprintf("Here is 100 / %i: ", i);
        kprintf("%i\n", 100 / i);
    }

    for (;;) {
        __asm__ ("hlt");
    }



    return;
}


