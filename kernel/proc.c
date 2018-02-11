
#include <basic.h>
#include <cpu/interrupt.h>
#include "proc.h"

typedef struct proc {
    u32 id;
    interrupt_frame frame;
    struct proc *next;
    struct proc *parent;
} Proc;

Proc proc_zero = { 0, {}, NULL, NULL };

static void do_process_swap(Proc *current, Proc *next) {

}
