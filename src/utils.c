/* void update_cursor(int row, int col) 
 * by Dark Fiber
 */

/*
void update_cursor(int row, int col)
{
    unsigned short position=(row*80) + col;
 
    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
} */

int power(int a, int b) {
    int c = 1;
    while (b--) {
        c *= a;
    }
    return c;
}

void kwrite_string(int offset, const char *buf, int color) {
    volatile char *video = (volatile char *)(0xB8000 + offset * 2);
    while (*buf != 0) {
        *video++ = *buf++;
        *video++ = color;
    }
}

void kwrite_int(int offset, int num, int color) {
    volatile char *video = (volatile char *)(0xB8000 + offset * 2);
    if (num < 0) {
        *video++ = '-';
        *video++ = color;
        num = -num;
    }
    if (num == 0) {
        *video++ = '0';
        *video++ = color;
        return;
    }
    int printing = 0;
    int power10;
    for (int i = 9; i >= 0; i--) {
        power10 = power(10, i);
        if (num / power10 > 0 || printing) {
            *video++ = '0' + (num / power10);
            *video++ = color;
            num -= (num / power10) * power10;
            printing = 1;
        }
    }
}

void kwrite_hex(int offset, int num, int color) {
    volatile char *video = (volatile char *)(0xB8000 + offset * 2);
    video[0] = 'x';
    video[1] = color;
    for (int i=0; i<8; i++) {
        if ((num & 0xF) < 0xA) {
            video[16 - 2*i] = '0' + (num & 0xF);
        } else {
            video[16 - 2*i] = 'A' + ((num & 0xF) - 10);
        }
        video[17 - 2*i] = color;
        num >>= 4;
    }
}

void clear_screen(int color) {
    volatile char *video = (volatile char *)0xB8000;
    int screen_len = 80 * 25;
    for (int i=0; i<screen_len; i++) {
        video[2*i] = ' ';
        video[2*i + 1] = color;
    }
}


