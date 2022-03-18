#include <basic.h>
#include <ng/irq.h>
#include <stdlib.h>
#include <list.h>

bool handlers_init = false;
list irq_handlers[NIRQS];

static void init_handlers()
{
    for (int i = 0; i < NIRQS; i++) {
        list_init(&irq_handlers[i]);
    }
}

void irq_install(int irq, void (*fn)(interrupt_frame *, void *), void *impl)
{
    if (!handlers_init) {
        init_handlers();
        handlers_init = true;
    }
    struct irq_handler *handler = malloc(sizeof(struct irq_handler));
    handler->handler_func = fn;
    handler->impl = impl;
    list_init(&handler->node);
    list_append(&irq_handlers[irq], &handler->node);
}

void irq_handler(interrupt_frame *r)
{
    unsigned irq = r->interrupt_number - 32; // x86ism
    if (list_empty(&irq_handlers[irq]))
        return;

    list_for_each (struct irq_handler, h, &irq_handlers[irq], node) {
        h->handler_func(r, h->impl);
    }
}
