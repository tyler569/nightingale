#include <ng/irq.h>
#include <nx/functional.h>
#include <nx/list.h>

struct irq_instance {
    nx::function<void(interrupt_frame *)> handler_func;
    nx::list_node node;

    irq_instance(void (*func)(interrupt_frame *, void *), void *data)
        : handler_func([=](interrupt_frame *r) { func(r, data); })
    {
    }

    explicit irq_instance(void (*func)(interrupt_frame *))
        : handler_func([=](interrupt_frame *r) { func(r); })
    {
    }

    explicit irq_instance(nx::function<void(interrupt_frame *)> &lambda)
        : handler_func(move(lambda))
    {
    }

    void handle(interrupt_frame *r) { handler_func(r); }
};

nx::list<irq_instance, &irq_instance::node> irq_handlers[NIRQS];

void irq_install(int irq, void (*fn)(interrupt_frame *, void *), void *impl)
{
    auto *handler = new irq_instance(fn, impl);
    irq_handlers[irq].push_back(*handler);
}

void irq_install(int irq, nx::function<void(interrupt_frame *)> &&fn)
{
    auto *handler = new irq_instance(fn);
    irq_handlers[irq].push_back(*handler);
}

void irq_handler(interrupt_frame *r)
{
    unsigned irq = r->interrupt_number - 32; // x86ism
    for (auto &h : irq_handlers[irq]) {
        h.handle(r);
    }
}
