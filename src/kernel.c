/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/

#include <system.h>
#include <utils.h>

void kmain(void)
{
    clear_screen(0x07);

    kwrite_string(0, "Welcome to Project Nightingale", 0x2f);

    memset((char *)0x500, 'g', 0x10);
    *(char *)0x510 = 0;

    kwrite_string(160, "TEST", 0x07);
    kwrite_string(240, (char *)0x500, 0x07);

	return;
}


