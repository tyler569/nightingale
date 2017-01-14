/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/

#include "utils.h"

#define ROW 80



void kmain(void)
{
	const char *str = "OK";

    clear_screen(0x07);

    // move_cursor();

    kwrite_string(1 * ROW, str, 0x2f);
    kwrite_int(3 * ROW, 0xFFFFFFFF, 0x07);
    kwrite_int(3 * ROW + 10, 0xFFFFFFF0, 0x07);
    kwrite_int(3 * ROW + 20, 0xFFFFFF00, 0x07);
    kwrite_hex(5 * ROW, 0x7FFF0000, 0x07);

	return;
}


