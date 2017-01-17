/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/

#include <system.h>
#include <screen.h>

void kmain(void)
{
    gdt_install();

    
    init_screen();
    cls();
    kprintf("&20Project Nightingale\n&70");

    for (int i=0; i<16; i++) {
        kprintf("Hello number: %i\n", i);
    }


	return;
}


