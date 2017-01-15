/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/

#include "utils.h"


void kmain(void)
{
    //clear_screen(0x07);

    // move_cursor();

    kwrite_string(0, "Welcome to Project Nightingale", 0x2f);

    kwrite_string(160, "TEST", 0x07);

	return;
}


