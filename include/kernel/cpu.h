
#pragma once

#include <stddef.h>
#include <stdint.h>

void gdt_set_gate(size_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void gdt_install();
extern void gdt_flush();

/* This defines what the stack looks like after an ISR was running */
struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_install_handler(uint8_t int_num, void (*handler)(struct regs *r));
void idt_install();

void irq_install_handler(size_t irq, void (*handler)(struct regs *r));
void irq_install();

void timer_phase(int32_t hz);
void timer_install();

void setup_paging();

void keyboard_echo_handler(struct regs *r);

uint8_t inportb(uint16_t _port);
void outportb(uint16_t _port, uint8_t _data);

static inline void sti() {
    __asm__ ( "sti" );
}

static inline void cli() {
    __asm__ ( "cli" );
}

static inline void hlt() {
    __asm__ ( "hlt" );
}

