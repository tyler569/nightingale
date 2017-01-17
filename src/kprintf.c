
/*
 * kformat -> kprintf
 *
 * letters  -- >  print letters
 * \char    -- >  print char
 * &colors  -- >  change console color to <colors>
 *  e.g. &70 -> change to LIGHT_GREY, BLACK (fg, bg)
 * %format  -- >  format in data
 *  e.g. %d  -> format int in from parameters (simulate printf)
 */

#include <stdarg.h>

#include <system.h>
#include <screen.h>

int kprintf(const char *format, ...) {

    int j;
    char *s;

    va_list args;
    va_start(args, format);

    for (int i=0; i < strlen(format); i++) {
        if (format[i] != '%' && format[i] != '&') {
            putchar(format[i]);
        } else if (format[i] == '&') {
            set_text_color((vga_color)format[i+2] - '0', (vga_color) format[i+1] - '0');
            i += 2;
        } else if (format[i] == '%') {
            switch (format[i+1]) {
                case 'c' : 
                    j = va_arg(args, int);
                    putchar(j);
                    break;
                case 'i' :
                    j = va_arg(args, int);
                    putint(j);
                    break;
                case 's':
                    s = va_arg(args, char *);
                    putstr(s);
                    break;
                case '&':
                    putchar('&');
                    break;
            }
            i++;
        }
    }

    return 0;
}


