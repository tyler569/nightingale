
#include <stdio.h>

enum ev_handler_outcome {
    EV_PEEK,
    EV_CONSUME,
};

struct evhandler {
    void* context;
    int (*handler)(void* context, void* event);
    int interest[];
};

struct evmgr {
    int (*processor)(struct evmgr* em, void* event);
    int handlers_count;
    struct evhandler handlers[16];
};

// int evmgr_register
// struct evhandler* evmgr_deregister
int evmgr_ingest(struct evmgr* em, void* event) {
    return em->processor(em, event);
}

int test_handler_func(void* context, void* event) {
    printf("%s\n", (char*)event);
    return EV_CONSUME;
}

struct evhandler test_handler = {
    .context = NULL,
    .handler = test_handler_func,
    .interest = { 'a' },
};

int test_processor(struct evmgr* em, void* event) {
    int consumed = 0;
    for (int i=0; i<em->handlers_count; i++) {
        if ((em->handlers[i].interest[0]) != ((char*)event)[0]) {
            // no interest
            continue;
        }
        int status = em->handlers[i].handler(em->handlers[i].context, event);
        if (status == EV_CONSUME) {
            consumed = 1;
            break;
        }
    }
    return consumed;
}

int main() {
    struct evmgr test_ev = {
        .processor = test_processor,
        .handlers_count = 1,
        .handlers = { test_handler },
    };

    evmgr_ingest(&test_ev, (void*)"Hello World");

    evmgr_ingest(&test_ev, (void*)"a big baloon");
};

