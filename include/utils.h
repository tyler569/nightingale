
#ifndef _UTILS_H
#define _UTILS_H

//void update_cursor(int row, int col);
int power(int a, int b);
void clear_screen(int color);
void kwrite_string(int offset, const char *buf, int color);
void kwrite_int(int offset, int num, int color);
void kwrite_hex(int offset, int num, int color);
void move_cursor();

#endif //_UTILS_H
