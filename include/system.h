
#ifndef _SYSTEM_H
#define _SYSTEM_H

/* system.c */
void *memcpy(void *dest, const void *src, int count);
void *memset(void *dest, char val, int count);
short *memsetw(short *dest, short val, int count);
int strlen(const void *str);
char inportb (short _port);
void outportb (short _port, char _data);

/* utils.c */
int power(int a, int b);

/* kprintf.c */
int kprintf(const char *format, ...);

/* gdt.c */
void gdt_install();


#endif // _SYSTEM_H
