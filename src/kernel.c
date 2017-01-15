/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/

#include <system.h>
#include <screen.h>
#include <utils.h>

void kmain(void)
{
    //clear_screen(0x07);

    //kwrite_string(0, "Welcome to Project Nightingale", 0x02);
    //kwrite_string(160, "TEST", 0x07);
    
    init_screen();
    cls();
    set_text_color(COLOR_BLACK, COLOR_LIGHT_GREEN);
    putstr("Project Nightingale\n");
    set_text_color(COLOR_BLACK, COLOR_LIGHT_GREY);
    putstr("Hello World\n");

	return;
}


