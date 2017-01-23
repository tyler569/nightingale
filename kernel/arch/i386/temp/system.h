
#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* system.c */
void *memcpy(void *dest, const void *src, int32_t count);
void *memset(void *dest, uint8_t val, int32_t count);
// uint16_t *wmemset(uint16_t *dest, uint16_t val, int32_t count);
int32_t strlen(const void *str);
uint8_t inportb (uint16_t _port);
void outportb (uint16_t _port, uint8_t _data);

/* utils.c */
int32_t power(int32_t a, int32_t b);

/* kprintf.c */
int32_t kprintf(const char *format, ...);

/* gdt.c */
void gdt_install();

/* idt.c */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_install();

/* isr related */
/* This defines what the stack looks like after an ISR was running */
struct regs
{
    uint32_t gs, fs, es, ds;      /* pushed the segs last */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    uint32_t int_no, err_code;    /* our 'push byte #' and ecodes do this */
    uint32_t eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

/* isrs.c */
void isrs_install();

/* irq.c 8 */
void irq_install();
void irq_install_handler(size_t irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(size_t irq);

/* timer.c */
void timer_install();

/* keyboard.c */
void keyboard_handler(struct regs *r);



#endif // _SYSTEM_H
