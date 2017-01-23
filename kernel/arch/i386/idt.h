
#ifndef _ARCH_I386_ISRS_H
#define _ARCH_I386_ISRS_H

/* This defines what the stack looks like after an ISR was running */
struct regs {
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_install();

#endif // _ARCH_I686_ISRS_H

