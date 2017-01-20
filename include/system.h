
#ifndef _SYSTEM_H
#define _SYSTEM_H

/* system.c */
void *memcpy(void *dest, const void *src, int count);
void *memset(void *dest, char val, int count);
short *wmemset(short *dest, short val, int count);
int strlen(const void *str);
char inportb (short _port);
void outportb (short _port, char _data);

/* utils.c */
int power(int a, int b);

/* kprintf.c */
int kprintf(const char *format, ...);

/* gdt.c */
void gdt_install();

/* idt.c */
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
void idt_install();

/* isr related */
/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

/* isrs.c */
void isrs_install();

/* irq.c 8 */
void irq_install();
void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);

/* timer.c */
void timer_install();

/* keyboard.c */
void keyboard_handler(struct regs *r);



#endif // _SYSTEM_H
