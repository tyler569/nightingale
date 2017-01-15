
/*
 * Utilities - from http://www.osdever.net/bkerndev/Docs/creatingmain.htm
 */

char *memcpy(char *dest, const char *src, int count) {
    int i = 0;
    while (count--) {
        dest[i] = src[i];
        i++;
    }
    return dest;
}

char *memset(char *dest, char val, int count) {
    int i = 0;
    while (count--) {
        dest[i++] = val;
    }
    return dest;
}

short *wmemset(short *dest, short val, int count) {
    int i = 0;
    while (count--) {
        dest[i++] = val;
    }
    return dest;
}

int strlen(const char *str) {
    int i;
    for (i=0; str[i] != 0; i++);
    return i;
}

char inportb(short _port) {
    char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb(short _port, char _data) {
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

