
#ifndef _SYSTEM_H
#define _SYSTEM_H

extern char *memcpy(char *dest, const char *src, int count);
extern char *memset(char *dest, char val, int count);
extern short *memsetw(short *dest, short val, int count);
extern int strlen(const char *str);
extern char inportb (short _port);
extern void outportb (short _port, char _data);

#endif // _SYSTEM_H
