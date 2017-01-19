
#include <system.h>
#include <screen.h>

void kmain(void)
{
    gdt_install();
    idt_install();
    isrs_install();

    init_screen();
    cls();
    kprintf("&20P\n&70%i");
    putchar(1/0);
    
    return;
}


