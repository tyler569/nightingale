
struct abstract_terminal {
    int (*write)(const char *buf, size_t len);
    // void (*color)() // TODO: how?
};

void term_init();

struct abstract_terminal term;

