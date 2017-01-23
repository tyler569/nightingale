
#ifndef _ARCH_I386_GDT_H
#define _ARCH_I386_GDT_H

void gdt_set_gate(size_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void gdt_install();

#endif // _ARCH_I386_GDT_H

