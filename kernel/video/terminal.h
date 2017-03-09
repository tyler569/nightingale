
struct abstract_terminal {
    int (*write)(char *buf, size_t len);
    // void (*color)() // TODO: how?
};

struct abstract_terminal boot_terminal_init();
